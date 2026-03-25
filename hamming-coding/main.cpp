#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

// Чтение строки из файла, содержащей только '0' и '1'
string readBitsFromFile(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Не удалось открыть файл " << filename << endl;
        exit(1);
    }
    string content, line;
    while (getline(file, line))
    {
        content += line;
    }
    file.close();

    // Удаляем все символы, кроме '0' и '1'
    string bits;
    for (char c : content)
    {
        if (c == '0' || c == '1')
            bits.push_back(c);
    }
    return bits;
}

// Сохранение строки битов в файл
void saveBitsToFile(const string &filename, const string &bits)
{
    ofstream file(filename);
    if (!file.is_open())
    {
        cerr << "Не удалось создать файл " << filename << endl;
        exit(1);
    }
    file << bits;
    file.close();
}

// Кодирование кодом Хэмминга (7,4)
string encodeHamming(const string &bits)
{
    const int k = 4; // информационных бит
    const int n = 7; // длина кодового слова
    string encoded;

    // Добавляем padding, чтобы длина была кратна k
    size_t pad = (k - (bits.size() % k)) % k;
    string paddedBits = bits + string(pad, '0');
    size_t blocks = paddedBits.size() / k;

    for (size_t block = 0; block < blocks; ++block)
    {
        // Информационные биты: d1 d2 d3 d4 в порядке позиций 3,5,6,7
        char d1 = paddedBits[block * k + 0];
        char d2 = paddedBits[block * k + 1];
        char d3 = paddedBits[block * k + 2];
        char d4 = paddedBits[block * k + 3];

        // Вычисление контрольных битов (XOR)
        char p1 = ((d1 - '0') ^ (d2 - '0') ^ (d4 - '0')) + '0';
        char p2 = ((d1 - '0') ^ (d3 - '0') ^ (d4 - '0')) + '0';
        char p3 = ((d2 - '0') ^ (d3 - '0') ^ (d4 - '0')) + '0';

        // Формирование кодового слова: позиции 1..7: p1, p2, d1, p3, d2, d3, d4
        string word = {p1, p2, d1, p3, d2, d3, d4};
        encoded += word;
    }
    // Сохраняем количество добавленных нулей для последующего удаления при декодировании
    // (можно вернуть вместе с encoded, но проще передать как параметр в decode)
    return encoded;
}

// Внесение ошибок с вероятностью p
string addErrors(const string &bits, double p)
{
    string result = bits;
    for (char &b : result)
    {
        double r = (double)rand() / RAND_MAX;
        if (r < p)
        {
            b = (b == '0') ? '1' : '0';
        }
    }
    return result;
}

// Декодирование кода Хэмминга (7,4) и обрезание до исходной длины
string decodeHamming(const string &encodedBits, size_t originalLen)
{
    const int n = 7;
    const int k = 4;
    string decoded;

    size_t blocks = encodedBits.size() / n;
    for (size_t block = 0; block < blocks; ++block)
    {
        // Извлекаем символы кодового слова
        char p1 = encodedBits[block * n + 0];
        char p2 = encodedBits[block * n + 1];
        char d1 = encodedBits[block * n + 2];
        char p3 = encodedBits[block * n + 3];
        char d2 = encodedBits[block * n + 4];
        char d3 = encodedBits[block * n + 5];
        char d4 = encodedBits[block * n + 6];

        // Вычисление синдрома
        int s1 = (p1 - '0') ^ (d1 - '0') ^ (d2 - '0') ^ (d4 - '0');
        int s2 = (p2 - '0') ^ (d1 - '0') ^ (d3 - '0') ^ (d4 - '0');
        int s3 = (p3 - '0') ^ (d2 - '0') ^ (d3 - '0') ^ (d4 - '0');

        int syndrome = (s1 << 2) | (s2 << 1) | s3; // s1 s2 s3 как число от 0 до 7
        // Если синдром не 0, исправляем соответствующий бит
        if (syndrome != 0)
        {
            // Номер бита в кодовом слове (1-7) соответствует синдрому
            int pos = syndrome; // по свойству кода Хэмминга
            // Корректируем бит (меняем символ)
            if (pos >= 1 && pos <= 7)
            {
                char &bit = const_cast<char &>(encodedBits[block * n + (pos - 1)]);
                bit = (bit == '0') ? '1' : '0';
                // Обновляем переменные для извлечения информационных битов после исправления
                // Но проще после исправления заново извлечь из изменённой строки
                // Однако мы уже имеем ссылку на bit, и изменение будет видно в encodedBits
                // Поэтому обновим значения для извлечения:
                switch (pos)
                {
                case 1:
                    p1 = bit;
                    break;
                case 2:
                    p2 = bit;
                    break;
                case 3:
                    d1 = bit;
                    break;
                case 4:
                    p3 = bit;
                    break;
                case 5:
                    d2 = bit;
                    break;
                case 6:
                    d3 = bit;
                    break;
                case 7:
                    d4 = bit;
                    break;
                }
            }
        }
        // Извлекаем информационные биты (d1,d2,d3,d4)
        decoded.push_back(d1);
        decoded.push_back(d2);
        decoded.push_back(d3);
        decoded.push_back(d4);
    }
    // Обрезаем до исходной длины (убираем padding)
    if (decoded.size() > originalLen)
        decoded = decoded.substr(0, originalLen);
    return decoded;
}

// Подсчёт количества ошибок между двумя строками битов
int countErrors(const string &original, const string &decoded)
{
    if (original.size() != decoded.size())
    {
        cerr << "Размеры строк не совпадают при подсчёте ошибок!" << endl;
        return -1;
    }
    int errors = 0;
    for (size_t i = 0; i < original.size(); ++i)
    {
        if (original[i] != decoded[i])
            ++errors;
    }
    return errors;
}

int main()
{
    srand(static_cast<unsigned>(time(nullptr))); // инициализация ГПСЧ

    string inputFile = "./files/input.txt";     // файл с исходными битами
    string encodedFile = "./files/encoded.txt"; // закодированный файл (до ошибок)
    string noisyFile = "./files/noisy.txt";     // файл с ошибками
    string decodedFile = "./files/decoded.txt"; // декодированный файл

    // 1. Чтение исходных битов
    string originalBits = readBitsFromFile(inputFile);
    if (originalBits.empty())
    {
        cerr << "Исходный файл пуст или не содержит битов." << endl;
        return 1;
    }
    size_t originalLen = originalBits.size();

    // 2. Кодирование
    string encodedBits = encodeHamming(originalBits);
    saveBitsToFile(encodedFile, encodedBits);
    cout << "Кодирование завершено. Размер: " << encodedBits.size() << " бит." << endl;

    // 3. Для различных вероятностей ошибок
    vector<double> probs = {0.0001, 0.001, 0.01, 0.1};
    cout << "\nРезультаты:\n";
    cout << left << setw(20) << "Вероятность ошибки p"
         << setw(20) << "Количество ошибок" << endl;
    cout << string(40, '-') << endl;

    for (double p : probs)
    {
        // Вносим ошибки в закодированную последовательность
        string noisyBits = addErrors(encodedBits, p);
        saveBitsToFile(noisyFile, noisyBits);

        // Декодируем искажённую последовательность
        string decodedBits = decodeHamming(noisyBits, originalLen);
        saveBitsToFile(decodedFile, decodedBits);

        // Сравниваем с исходной
        int errors = countErrors(originalBits, decodedBits);
        cout << fixed << setprecision(4) << left << setw(20) << p
             << setw(20) << errors << endl;
    }

    cout << "\nФайлы сохранены: " << encodedFile << ", " << noisyFile << ", " << decodedFile << endl;
    return 0;
}
