#include "src/file_generator.h"
#include "src/entropy_estimator.h"
#include <iostream>
#include <iomanip>

int main()
{
    std::cout << "\n=============== Генерация файлов ===============\n\n";
    generate_uniform_file("files/file1.txt");
    std::vector<double> probabilities = {0.5, 0.25, 0.15, 0.1};
    generate_nonuniform_file("files/file2.txt", probabilities);
    process_english_text("files/english_text.txt", "files/file3.txt");

    std::cout << "\n=============== Оценка энтропии ===============\n";

    std::cout << "\nФайл file1.txt (равномерное распределение):\n";
    double H1_single = estimate_entropy_single("files/file1.txt");
    double H1_pairs = estimate_entropy_pairs("files/file1.txt");
    double H1_triples = estimate_entropy_triples("files/file1.txt");
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "  Оценка по однобуквенным частотам: " << H1_single << " бит/символ\n";
    std::cout << "  Оценка по парам (с перекрытием):   " << H1_pairs << " бит/символ\n";
    std::cout << "  Оценка по тройкам (с перекрытием): " << H1_triples << " бит/символ\n";

    std::cout << "\nФайл file2.txt (неравномерное распределение):\n";
    double H2_single = estimate_entropy_single("files/file2.txt");
    double H2_pairs = estimate_entropy_pairs("files/file2.txt");
    double H2_triples = estimate_entropy_triples("files/file2.txt");
    std::cout << "  Оценка по однобуквенным частотам: " << H2_single << " бит/символ\n";
    std::cout << "  Оценка по парам (с перекрытием):   " << H2_pairs << " бит/символ\n";
    std::cout << "  Оценка по тройкам (с перекрытием): " << H2_triples << " бит/символ\n";

    std::cout << "\nФайл file3.txt (обработанный английский текст):\n";
    double H3_single = estimate_entropy_single("files/file3.txt");
    double H3_pairs = estimate_entropy_pairs("files/file3.txt");
    double H3_triples = estimate_entropy_triples("files/file3.txt");
    std::cout << "  Оценка по однобуквенным частотам: " << H3_single << " бит/символ\n";
    std::cout << "  Оценка по парам (с перекрытием):   " << H3_pairs << " бит/символ\n";
    std::cout << "  Оценка по тройкам (с перекрытием): " << H3_triples << " бит/символ\n";

    return 0;
}
