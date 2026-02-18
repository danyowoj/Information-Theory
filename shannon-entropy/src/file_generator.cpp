#include "file_generator.h"
#include <fstream>
#include <random>
#include <cctype>
#include <cmath>
#include <iostream>

void generate_uniform_file(const std::string &filename)
{
    std::ofstream out(filename);
    if (!out)
    {
        std::cerr << "Ошибка открытия файла для записи: " << filename << std::endl;
        return;
    }

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

void generate_nonuniform_file(const std::string &filename, const std::vector<double> &probs)
{
    if (probs.size() != N_SYMBOLS)
    {
        std::cerr << "Ошибка: размер вектора вероятностей не совпадает с количеством символов." << std::endl;
        return;
    }

    double sum = 0.0;
    for (double p : probs)
    {
        sum += p;
    }
    if (std::fabs(sum - 1.0) > EPSILON)
    {
        std::cerr << "Предупреждение: сумма вероятностей (" << sum
                  << ") отличается от 1 больше допустимого (EPSILON=" << EPSILON
                  << "). Распределение будет автоматически нормализовано." << std::endl;
    }

    std::ofstream out(filename);
    if (!out)
    {
        std::cerr << "Ошибка открытия файла для записи: " << filename << std::endl;
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<int> dist(probs.begin(), probs.end());

    for (long long i = 0; i < SEQ_LENGTH; ++i)
    {
        int index = dist(gen);
        out << SYMBOLS[index];
    }

    out.close();
    std::cout << "Файл " << filename << " с неравномерным распределением создан." << std::endl;
}

void process_english_text(const std::string &input_file, const std::string &output_file)
{
    std::ifstream in(input_file);
    if (!in)
    {
        std::cerr << "Ошибка открытия входного файла: " << input_file << std::endl;
        std::cerr << "Убедитесь, что файл существует в директории программы." << std::endl;
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
            out << static_cast<char>(std::tolower(ch));
        }
        else if (std::isspace(ch))
        {
            out << ' ';
        }
    }

    in.close();
    out.close();
    std::cout << "Файл " << output_file << " обработан из " << input_file << std::endl;
}
