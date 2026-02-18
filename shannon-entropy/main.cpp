#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include <cctype>

const int N_SYMBOLS = 4;                              // количество различных символов
const char SYMBOLS[N_SYMBOLS] = {'a', 'b', 'c', 'd'}; // алфавит
const long long SEQ_LENGTH = 18000;                   // длина последовательности (>10 КБ)
const double EPSILON = 1e-9;                          // допустимая погрешность при проверке суммы вероятностей

/**
 * Генерирует файл с равномерным распределением
 * символов из алфавита SYMBOLS.
 * @param filename имя выходного файла
 */
void generate_uniform_file(const std::string &filename)
{
    std::ofstream out(filename);
    if (!out)
    {
        std::cerr << "Ошибка открытия файла для записи: " << filename << std::endl;
        return;
    }

    // Генератор случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, N_SYMBOLS - 1);

    for (long long i = 0; i < SEQ_LENGTH; ++i)
    {
        int index = dist(gen);
        out << SYMBOLS[index];
    }

    out.close();
    std::cout << "Файл " << filename << " с равномерным распределением создан." << std::endl;
}

/**
 * Генерирует файл с заданным неравномерным
 * распределением вроятностей символов.
 * @param filename имя выходного файла
 * @param probs вектор вероятностей (должен иметь размер N_SYMBOLS и сумму 1)
 */
void generate_nonuniform_file(const std::string &filename, const std::vector<double> &probs)
{
    if (probs.size() != N_SYMBOLS)
    {
        std::cerr << "Ошибка: размер вектора вероятностей не совпадает с количеством символов." << std::endl;
        return;
    }

    double sum = 0.0;
    for (double p : probs)
        sum += p;
    if (std::fabs(sum - 1.0) > EPSILON)
        std::cerr << "Предупреждение: сумма вероятностей (" << sum
                  << ") отличается от 1 больше допустимого (EPSILON=" << EPSILON
                  << "). Распределение будет автоматически нормализовано." << std::endl;

    std::ofstream out(filename);
    if (!out)
    {
        std::cerr << "Ошибка открытия файла для записи: " << filename << std::endl;
        return;
    }

    // Генератор случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());
    // discrete_distribution использует веса для дискретного распределения
    // Даже если сумма не равна 1, она сама нормализует веса
    std::discrete_distribution<int> dist(probs.begin(), probs.end());

    for (long long i = 0; i < SEQ_LENGTH; ++i)
    {
        int index = dist(gen);
        out << SYMBOLS[index];
    }

    out.close();
    std::cout << "Файл " << filename << " с неравномерным распределением создан." << std::endl;
}

/**
 * Обрабатывает английский текст:
 * - приводит все буквы к нижнему регистру;
 * - удаляет знаки преринания и другие символы (кроме букв и пробелов);
 * - заменяет любые пробельные символы (табулиции, перевод строки) на пробел.
 * @param input_file имя исходного файла с художественным текстом
 * @param output_file имя выходного файла
 */
void process_english_text(const std::string &input_file, const std::string &output_file)
{
    std::fstream in(input_file);
    if (!in)
    {
        std::cerr << "Ошибка открытия входного файла: " << input_file << std::endl;
        return;
    }

    std::ofstream out(output_file);
    if (!out)
    {
        std::cerr << "Ошибка открытия выходного файла: " << output_file << std::endl;
        return;
    }

    char ch;
    while (in.get(ch))
    {
        if (std::isalpha(ch))
        {
            // Приводим буквы к нижнему регистру и записываем в файл
            out << static_cast<char>(std::tolower(ch));
        }
        else if (std::isspace(ch))
        {
            // Заменяем пробельный символ на пробел
            out << ' ';
        }
        // Другие символы игнорируем
    }

    in.close();
    out.close();
    std::cout << "Файл " << output_file << " обработан из " << input_file << '.' << std::endl;
}

int main()
{
    //* 1. Генерация первого файла с равномерным распределением
    generate_uniform_file("file1.txt");

    //* 2. Генерация второго файла с неравномерным распределением
    std::vector<double> probabilities = {0.5, 0.25, 0.15, 0.1};
    generate_nonuniform_file("file2.txt", probabilities);

    //* 3. Обработка третьего файла (художественный текст на английском языке)
    process_english_text("english_text.txt", "file3.txt");

    std::cout << "Все файлы успешно созданы." << std::endl;
    return 0;
}
