#include "entropy_estimator.h"
#include <fstream>
#include <cmath>
#include <map>

double shannon_entropy(const std::vector<double> &probs)
{
    double H = 0.0;
    for (double p : probs)
        if (p > 0.0)
            H -= p * std::log2(p);
    return H;
}

double estimate_entropy_single(const std::string &filename)
{
    std::ifstream in(filename);
    if (!in)
        return -1.0;

    std::map<char, long long> freq;
    char ch;
    long long total = 0;

    while (in.get(ch))
    {
        freq[ch]++;
        total++;
    }
    in.close();

    if (total == 0)
        return 0.0;

    std::vector<double> probs;
    for (const auto &p : freq)
        probs.push_back(static_cast<double>(p.second) / total);

    return shannon_entropy(probs);
}

double estimate_entropy_pairs(const std::string &filename)
{
    std::ifstream in(filename);
    if (!in)
        return -1.0;

    std::string text;
    char ch;
    while (in.get(ch))
        text.push_back(ch);
    in.close();

    if (text.size() < 2)
        return 0.0;

    std::map<std::string, long long> pair_freq;
    long long total_pairs = 0;

    for (size_t i = 0; i < text.size() - 1; ++i)
    {
        std::string pair = text.substr(i, 2);
        pair_freq[pair]++;
        total_pairs++;
    }

    std::vector<double> probs;
    for (const auto &p : pair_freq)

        probs.push_back(static_cast<double>(p.second) / total_pairs);

    double H_pairs = shannon_entropy(probs);
    return H_pairs / 2.0;
}

double estimate_entropy_triples(const std::string &filename)
{
    std::ifstream in(filename);
    if (!in)
        return -1.0;

    std::string text;
    char ch;
    while (in.get(ch))
        text.push_back(ch);

    in.close();

    if (text.size() < 3)
        return 0.0;

    std::map<std::string, long long> triple_freq;
    long long total_triples = 0;

    for (size_t i = 0; i < text.size() - 2; ++i)
    {
        std::string triple = text.substr(i, 3);
        triple_freq[triple]++;
        total_triples++;
    }

    std::vector<double> probs;
    for (const auto &p : triple_freq)
        probs.push_back(static_cast<double>(p.second) / total_triples);

    double H_triples = shannon_entropy(probs);
    return H_triples / 3.0;
}
