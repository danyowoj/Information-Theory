#ifndef ENTROPY_ESTIMATOR_H
#define ENTROPY_ESTIMATOR_H

#include <string>
#include <vector>

/**
 * Вычисляет энтропию по формуле Шеннона:
 * H = -sum(p_i ( log2(p_i))
 * @param probs вектор вероятностей символов
 * @return значение энтропии (в битах)
 */
double shannon_entropy(const std::vector<double> &probs);

/**
 * Оценка жнтропии по первым оценкам вероятностей.
 * @param filename имя файла
 * @return оценка энтропии (в битах на символ)
 */
double estimate_entropy_single(const std::string &filename);

/**
 * Оценка энтропии по частотам последовательных пар символов (с перекрытием)
 * @param filename имя файла
 * @return оценка энтропии (в битах на символ) = H(пары) / 2
 */
double estimate_entropy_pairs(const std::string &filename);

/**
 * Оценка энтропии по частотам последовательных троек символов (с перекрытием)
 * @param filename имя файла
 * @return оценка энтропии (в битах на символ) = H(тройки) / 3
 */
double estimate_entropy_triples(const std::string &filename);

#endif ENTROPY_ESTIMATOR_H // ENTROPY_ESTIMATOR_H
