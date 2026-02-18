#include "src/file_generator.h"
#include <iostream>

int main()
{
    // 1. Генерация первого файла с равномерным распределением
    generate_uniform_file("files/file1.txt");

    // 2. Генерация второго файла с неравномерным распределением
    std::vector<double> probabilities = {0.5, 0.25, 0.15, 0.1}; // сумма = 1
    generate_nonuniform_file("files/file2.txt", probabilities);

    // 3. Обработка третьего файла (художественный текст на английском)
    process_english_text("files/english_text.txt", "files/file3.txt");

    std::cout << "Все файлы успешно созданы." << std::endl;
    return 0;
}
