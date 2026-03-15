#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include <string>
#include <cmath>
#include <functional>

using namespace std;

// Структура узла дерева Хаффмана
struct Node
{
    string block; // блок (для листа)
    int freq;     // частота
    Node *left, *right;

    Node(string b, int f) : block(b), freq(f), left(nullptr), right(nullptr) {}
    Node(int f, Node *l, Node *r) : block(""), freq(f), left(l), right(r) {}
};

// Рекурсивное удаление дерева
void deleteTree(Node *node)
{
    if (!node)
        return;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

// Рекурсивный обход для сбора длин кодов
void collectLengths(Node *node, int depth, map<string, int> &lenMap)
{
    if (!node)
        return;
    if (!node->left && !node->right)
    {
        lenMap[node->block] = depth;
    }
    else
    {
        collectLengths(node->left, depth + 1, lenMap);
        collectLengths(node->right, depth + 1, lenMap);
    }
}

int main()
{
    // 1. Чтение файла
    ifstream in("file.txt");
    if (!in)
    {
        cerr << "Не удалось открыть файл file.txt" << endl;
        return 1;
    }

    string text;
    char ch;
    while (in.get(ch))
    {
        text.push_back(ch);
    }
    in.close();

    int totalChars = text.size();
    if (totalChars == 0)
    {
        cerr << "Файл пуст" << endl;
        return 1;
    }

    // 2. Подсчёт частот символов и энтропии H
    map<char, int> charFreq;
    for (char c : text)
    {
        charFreq[c]++;
    }

    double H = 0.0;
    for (auto &p : charFreq)
    {
        double p_i = static_cast<double>(p.second) / totalChars;
        H -= p_i * log2(p_i);
    }

    cout << "Энтропия источника (на символ): " << H << " бит\n\n";

    // 3. Обработка для n = 1..4
    cout << "Оценка избыточности кодирования на один символ:\n";
    cout << "-------------------------------------------------------------\n";
    cout << "n=1\t\tn=2\t\tn=3\t\tn=4\n";
    cout << "-------------------------------------------------------------\n";

    for (int n = 1; n <= 4; ++n)
    {
        int numBlocks = totalChars / n; // количество полных блоков
        if (numBlocks == 0)
        {
            cout << "-\t\t";
            continue;
        }

        // Подсчёт частот блоков
        map<string, int> blockFreq;
        for (int i = 0; i < numBlocks * n; i += n)
        {
            string block = text.substr(i, n);
            blockFreq[block]++;
        }

        // Построение дерева Хаффмана
        auto cmp = [](Node *a, Node *b)
        { return a->freq > b->freq; };
        priority_queue<Node *, vector<Node *>, decltype(cmp)> pq(cmp);

        for (auto &p : blockFreq)
        {
            pq.push(new Node(p.first, p.second));
        }

        // Специальный случай: если только один блок
        if (pq.size() == 1)
        {
            Node *only = pq.top();
            map<string, int> codeLen;
            codeLen[only->block] = 1; // код длины 1
            double avgLenBlock = only->freq * 1.0 / numBlocks;
            double avgLenPerChar = avgLenBlock / n;
            double redundancy = avgLenPerChar - H;
            cout << redundancy << "\t";
            delete only;
            continue;
        }

        while (pq.size() > 1)
        {
            Node *left = pq.top();
            pq.pop();
            Node *right = pq.top();
            pq.pop();
            Node *parent = new Node(left->freq + right->freq, left, right);
            pq.push(parent);
        }
        Node *root = pq.top();

        // Сбор длин кодов
        map<string, int> codeLen;
        collectLengths(root, 0, codeLen);

        // Вычисление средней длины кода на блок
        double totalBits = 0.0;
        for (auto &p : blockFreq)
        {
            totalBits += p.second * codeLen[p.first];
        }
        double avgLenBlock = totalBits / numBlocks;
        double avgLenPerChar = avgLenBlock / n;
        double redundancy = avgLenPerChar - H;

        cout << redundancy << "\t";

        // Очистка памяти
        deleteTree(root);
    }

    cout << "\n-------------------------------------------------------------\n";
    return 0;
}
