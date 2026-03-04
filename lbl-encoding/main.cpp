#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <string>

using namespace std;

// Структура для узла дерева Хаффмана
struct HuffmanNode
{
    char ch;
    double prob;
    HuffmanNode *left, *right;
    HuffmanNode(char c, double p) : ch(c), prob(p), left(nullptr), right(nullptr) {}
    HuffmanNode(double p, HuffmanNode *l, HuffmanNode *r) : ch(0), prob(p), left(l), right(r) {}
};

// Рекурсивное удаление дерева Хаффмана
void deleteHuffmanTree(HuffmanNode *node)
{
    if (!node)
        return;
    deleteHuffmanTree(node->left);
    deleteHuffmanTree(node->right);
    delete node;
}

// Компаратор для очереди с приоритетом (меньшая вероятность - выше приоритет)
struct Compare
{
    bool operator()(HuffmanNode *a, HuffmanNode *b)
    {
        return a->prob > b->prob;
    }
};

// Рекурсивное построение кодов Хаффмана
void buildHuffmanCodes(HuffmanNode *node, string code, map<char, string> &codes)
{
    if (!node->left && !node->right)
    {
        codes[node->ch] = code;
        return;
    }
    if (node->left)
        buildHuffmanCodes(node->left, code + "0", codes);
    if (node->right)
        buildHuffmanCodes(node->right, code + "1", codes);
}

// Функция построения кодов Хаффмана по частотам символов
map<char, string> huffman(const vector<pair<char, long long>> &freq, long long total)
{
    priority_queue<HuffmanNode *, vector<HuffmanNode *>, Compare> pq;
    for (auto &p : freq)
    {
        pq.push(new HuffmanNode(p.first, (double)p.second / total));
    }
    while (pq.size() > 1)
    {
        HuffmanNode *left = pq.top();
        pq.pop();
        HuffmanNode *right = pq.top();
        pq.pop();
        HuffmanNode *parent = new HuffmanNode(left->prob + right->prob, left, right);
        pq.push(parent);
    }
    HuffmanNode *root = pq.top();
    map<char, string> codes;
    buildHuffmanCodes(root, "", codes);
    // Освобождаем память, занятую деревом
    deleteHuffmanTree(root);
    return codes;
}

// Рекурсивное построение кодов Шеннона-Фано
void shannonFano(vector<pair<char, double>> &probs, string code, map<char, string> &codes)
{
    if (probs.size() == 1)
    {
        codes[probs[0].first] = code;
        return;
    }
    double total = 0.0;
    for (auto &p : probs)
        total += p.second;
    double half = total / 2.0;
    double sum = 0.0;
    int split = 0;
    double bestDiff = total;
    // Ищем точку разделения, чтобы сумма вероятностей в левой части была как можно ближе к half
    for (size_t i = 0; i < probs.size(); ++i)
    {
        sum += probs[i].second;
        double diff = fabs(sum - half);
        if (diff < bestDiff)
        {
            bestDiff = diff;
            split = i;
        }
    }
    vector<pair<char, double>> left(probs.begin(), probs.begin() + split + 1);
    vector<pair<char, double>> right(probs.begin() + split + 1, probs.end());
    shannonFano(left, code + "0", codes);
    shannonFano(right, code + "1", codes);
}

// Функция построения кодов Шеннона-Фано по частотам
map<char, string> shannon(const vector<pair<char, long long>> &freq, long long total)
{
    vector<pair<char, double>> probs;
    for (auto &p : freq)
    {
        probs.push_back({p.first, (double)p.second / total});
    }
    // Сортируем по убыванию вероятности
    sort(probs.begin(), probs.end(),
         [](const pair<char, double> &a, const pair<char, double> &b)
         {
             return a.second > b.second;
         });
    map<char, string> codes;
    shannonFano(probs, "", codes);
    return codes;
}

// Структура для хранения статистики кодирования
struct EncodingStats
{
    double H1, H2, H3;   // энтропии битовой последовательности
    long long totalBits; // общее количество бит
};

// Функция кодирования файла и подсчёта энтропии битовой последовательности
EncodingStats encodeAndStats(const string &filename, const map<char, string> &codes)
{
    ifstream file(filename, ios::binary);
    if (!file.is_open())
    {
        cerr << "Не удалось открыть файл " << filename << endl;
        return {0, 0, 0, 0};
    }

    long long count0 = 0, count1 = 0;
    long long count2[2][2] = {0};
    long long count3[2][2][2] = {0};
    int prev = -1, prev1 = -1, prev2 = -1; // для хранения предыдущих битов

    char ch;
    while (file.get(ch))
    {
        auto it = codes.find(ch);
        if (it == codes.end())
        {
            cerr << "Символ '" << ch << "' не найден в кодах!" << endl;
            continue;
        }
        const string &bits = it->second;
        for (char b : bits)
        {
            int bit = (b == '0' ? 0 : 1);
            // Счёт битов
            if (bit == 0)
                count0++;
            else
                count1++;
            // Пары
            if (prev != -1)
            {
                count2[prev][bit]++;
            }
            // Тройки
            if (prev1 != -1 && prev2 != -1)
            {
                count3[prev2][prev1][bit]++;
            }
            // Сдвиг
            prev2 = prev1;
            prev1 = prev;
            prev = bit;
        }
    }
    file.close();

    long long totalBits = count0 + count1;
    EncodingStats stats;
    stats.totalBits = totalBits;

    // H1
    double p0 = (totalBits > 0) ? (double)count0 / totalBits : 0;
    double p1 = (totalBits > 0) ? (double)count1 / totalBits : 0;
    stats.H1 = 0;
    if (p0 > 0)
        stats.H1 -= p0 * log2(p0);
    if (p1 > 0)
        stats.H1 -= p1 * log2(p1);

    // H2
    long long totalPairs = totalBits - 1;
    stats.H2 = 0;
    if (totalPairs > 0)
    {
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                if (count2[i][j] > 0)
                {
                    double p = (double)count2[i][j] / totalPairs;
                    stats.H2 -= p * log2(p);
                }
            }
        }
    }

    // H3
    long long totalTriples = totalBits - 2;
    stats.H3 = 0;
    if (totalTriples > 0)
    {
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                for (int k = 0; k < 2; ++k)
                {
                    if (count3[i][j][k] > 0)
                    {
                        double p = (double)count3[i][j][k] / totalTriples;
                        stats.H3 -= p * log2(p);
                    }
                }
            }
        }
    }
    return stats;
}

int main()
{
    // Список файлов для обработки
    vector<string> filenames = {"./files/file1.txt", "./files/file2.txt", "./files/file3.txt"};

    // Заголовок таблицы
    cout << left << setw(20) << "Метод кодирования"
         << setw(20) << "Название текста"
         << setw(30) << "Оценка избыточности кодирования"
         << setw(10) << "H1"
         << setw(10) << "H2"
         << setw(10) << "H3" << endl;
    cout << string(100, '-') << endl;

    for (const string &fname : filenames)
    {
        // Подсчёт частот символов в файле
        ifstream file(fname, ios::binary);
        if (!file.is_open())
        {
            cerr << "Не удалось открыть " << fname << endl;
            continue;
        }
        map<char, long long> freqMap;
        char ch;
        long long totalChars = 0;
        while (file.get(ch))
        {
            freqMap[ch]++;
            totalChars++;
        }
        file.close();

        // Преобразуем в вектор для удобства
        vector<pair<char, long long>> freqVec(freqMap.begin(), freqMap.end());

        // Вычисляем энтропию исходного текста H_sym
        double H_sym = 0.0;
        for (auto &p : freqVec)
        {
            double prob = (double)p.second / totalChars;
            H_sym -= prob * log2(prob);
        }

        // Построение кодов для двух методов
        map<char, string> codes_huff = huffman(freqVec, totalChars);
        map<char, string> codes_shan = shannon(freqVec, totalChars);

        // Обработка каждого метода
        vector<pair<string, map<char, string>>> methods = {{"Хаффман", codes_huff}, {"Шеннон", codes_shan}};

        for (auto &method : methods)
        {
            string methodName = method.first;
            map<char, string> &codes = method.second;

            // Вычисляем среднюю длину кода L_cp
            double L_cp = 0.0;
            for (auto &p : freqVec)
            {
                double prob = (double)p.second / totalChars;
                L_cp += prob * codes[p.first].size();
            }

            // Кодируем файл и получаем статистику по битам
            EncodingStats stats = encodeAndStats(fname, codes);

            // Избыточность
            double redundancy = L_cp - H_sym;

            // Вывод строки таблицы
            cout << left << setw(20) << methodName
                 << setw(20) << fname
                 << setw(30) << fixed << setprecision(4) << redundancy
                 << setw(10) << stats.H1
                 << setw(10) << stats.H2
                 << setw(10) << stats.H3 << endl;
        }
        cout << endl;
    }

    return 0;
}
