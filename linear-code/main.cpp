#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

// Генерация файла с систематической порождающей матрицей [I | D]
// n - число строк (размерность), m - число столбцов (длина кода), m > n
void generateMatrixFile(const string &filename, int n, int m)
{
    ofstream fout(filename);
    if (!fout)
    {
        cerr << "Ошибка создания файла " << filename << endl;
        return;
    }

    // Запись размеров
    fout << n << " " << m << endl;

    // Генерация случайной подматрицы D (n строк, m-n столбцов)
    // и запись строк матрицы G = [I | D]
    for (int i = 0; i < n; ++i)
    {
        // Единичная подматрица: в i-й строке единица на i-й позиции
        for (int j = 0; j < n; ++j)
        {
            fout << (j == i ? 1 : 0) << " ";
        }
        // Случайная подматрица D
        for (int j = 0; j < m - n; ++j)
        {
            fout << (rand() % 2) << (j < m - n - 1 ? " " : "");
        }
        fout << endl;
    }

    fout.close();
    cout << "Сгенерирован файл " << filename << " с размерами " << n << "x" << m << endl;
}

// Обработка одного файла: возвращает кортеж (длина кода, размерность, кол-во слов, мин. расстояние)
tuple<int, int, unsigned long long, int> processFile(const string &filename)
{
    ifstream fin(filename);
    if (!fin)
    {
        cerr << "Не удалось открыть файл " << filename << endl;
        return {0, 0, 0, 0};
    }

    int n, m;
    fin >> n >> m;

    // Проверка условия n < m
    if (n >= m)
    {
        cerr << "Ошибка: n должно быть меньше m в файле " << filename << endl;
        return {0, 0, 0, 0};
    }

    // Чтение матрицы G
    vector<vector<int>> G(n, vector<int>(m));
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < m; ++j)
        {
            fin >> G[i][j];
        }
    }
    fin.close();

    // Длина кода = m
    int length = m;
    // Размерность = n
    int dimension = n;
    // Количество кодовых слов = 2^n
    unsigned long long numWords = 1ULL << n;

    // Поиск минимального расстояния (веса ненулевых кодовых слов)
    int minDistance = m + 1; // начальное значение больше максимально возможного веса

    // Перебор всех ненулевых информационных векторов (от 1 до 2^n - 1)
    for (unsigned long long info = 1; info < (1ULL << n); ++info)
    {
        // Вычисление кодового слова v = info * G (по модулю 2)
        vector<int> codeword(m, 0);
        for (int j = 0; j < m; ++j)
        {
            int bit = 0;
            for (int i = 0; i < n; ++i)
            {
                // Проверяем, установлен ли i-й бит в info
                if ((info >> i) & 1)
                {
                    bit ^= G[i][j];
                }
            }
            codeword[j] = bit;
        }

        // Подсчёт веса Хэмминга
        int weight = 0;
        for (int j = 0; j < m; ++j)
        {
            if (codeword[j])
                weight++;
        }

        if (weight < minDistance)
        {
            minDistance = weight;
            // Если достигли минимально возможного веса 1, можно прервать (но для надёжности продолжим)
            if (minDistance == 1)
                break;
        }
    }

    return {length, dimension, numWords, minDistance};
}

int main()
{
    // Инициализация генератора случайных чисел
    srand(static_cast<unsigned>(time(nullptr)));

    // Задаём пять различных размеров для файлов (n < m)
    vector<pair<int, int>> sizes = {
        {3, 8}, // код 1
        {4, 6}, // код 2
        {5, 7}, // код 3
        {6, 9}, // код 4
        {7, 10} // код 5
    };

    // Имена файлов
    vector<string> filenames = {
        "code1.txt",
        "code2.txt",
        "code3.txt",
        "code4.txt",
        "code5.txt"};

    // Генерируем файлы с систематическими матрицами
    cout << "Генерация тестовых файлов:\n";
    for (size_t i = 0; i < sizes.size(); ++i)
    {
        int n = sizes[i].first;
        int m = sizes[i].second;
        generateMatrixFile(filenames[i], n, m);
    }

    cout << "\nОбработка файлов и вычисление характеристик:\n";
    // Таблица результатов
    cout << left << setw(10) << "Код"
         << "| " << setw(14) << "Длина кода (m)"
         << "| " << setw(15) << "Размерность (n)"
         << "| " << setw(19) << "Кол-во кодовых слов"
         << "| " << setw(15) << "Мин. расстояние" << endl;
    cout << string(90, '-') << endl;

    for (size_t i = 0; i < filenames.size(); ++i)
    {
        auto [length, dim, numWords, minDist] = processFile(filenames[i]);

        cout << left << setw(10) << ("Код " + to_string(i + 1))
             << "| " << setw(14) << length
             << "| " << setw(15) << dim
             << "| " << setw(19) << numWords
             << "| " << setw(15) << minDist << endl;
    }

    return 0;
}
