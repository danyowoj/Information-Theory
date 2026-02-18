#ifndef FILE_GENERATOR_H
#define FILE_GENERATOR_H

#include <string>
#include <vector>

// Константы для первых двух файлов
const int N_SYMBOLS = 4;                              // количество различных символов (может быть 3,4,5)
const char SYMBOLS[N_SYMBOLS] = {'a', 'b', 'c', 'd'}; // алфавит
const long long SEQ_LENGTH = 18000;                   // длина последовательности (>10 КБ)
const double EPSILON = 1e-9;                          // допустимая погрешность при проверке суммы вероятностей

/**
 * Генерирует файл с равномерным распределением символов из алфавита SYMBOLS.
 * @param filename имя выходного файла
 */
void generate_uniform_file(const std::string &filename);

/**
 * Генерирует файл с заданным неравномерным распределением вероятностей символов.
 * @param filename имя выходного файла
 * @param probs вектор вероятностей (должен иметь размер N_SYMBOLS и сумму 1)
 */
void generate_nonuniform_file(const std::string &filename, const std::vector<double> &probs);

/**
 * Обрабатывает английский текст:
 * - приводит все буквы к нижнему регистру,
 * - удаляет знаки препинания и другие символы (кроме букв и пробелов),
 * - заменяет любые пробельные символы (табуляции, переводы строк) на обычный пробел.
 * @param input_file имя исходного файла с художественным текстом
 * @param output_file имя выходного файла
 */
void process_english_text(const std::string &input_file, const std::string &output_file);

#endif // FILE_GENERATOR_H
