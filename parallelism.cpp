#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>

using namespace std;

const int MAX_THREADS = 100;
int numThreads = 1;

struct WordCount
{
    int loveCount;
    int hateCount;
};

// Função para processar um bloco de texto e contar ocorrências de "love" e "hate"
void processBlock(const vector<string>& block, WordCount& localCount)
{
    for (const auto& word : block) 
    {
        if (word == "love")
        {
            localCount.loveCount++;
        } 
        else if (word == "hate") 
        {
            localCount.hateCount++;
        }
    }
}

int main() 
{
    string filename = "shakespeare.txt";
    // Função para carregar o arquivo e dividir em blocos
    vector<vector<string>> blocks = loadAndSplitFile(filename);

    for (int i = 1; i <= MAX_THREADS; i++) 
    {
        numThreads = i;
        int blockSize = blocks.size() / numThreads;
        vector<thread> threads;
        vector<WordCount> localCounts(numThreads);

        auto startPrep = chrono::high_resolution_clock::now();

        for (int i = 0; i < numThreads; ++i) 
        {
            auto begin = blocks.begin() + i * blockSize;
            auto end = (i == numThreads - 1) ? blocks.end() : begin + blockSize;
            vector<string> block(begin, end);

            threads.emplace_back([&, i, block]() 
            {
                processBlock(block, localCounts[i]);
            });
        }

        for (auto& thread : threads) 
        {
            thread.join();
        }

        auto endPrep = chrono::high_resolution_clock::now();
        auto prepTime = chrono::duration_cast<chrono::milliseconds>(endPrep - startPrep);

        WordCount totalCount;
        for (const auto& count : localCounts) 
        {
            totalCount.loveCount += count.loveCount;
            totalCount.hateCount += count.hateCount;
        }

        auto startSearch = chrono::high_resolution_clock::now();

        // A partir daqui, você pode realizar mais processamento, se necessário.
       
        auto endSearch = chrono::high_resolution_clock::now();
        auto execTime = chrono::duration_cast<chrono::milliseconds>(endSearch - startSearch);

        auto totalTime = prepTime + execTime;

        cout << "Número de threads: " << numThreads << endl;
        cout << "Tamanho do bloco de cada thread: " << blocks.size() / numThreads << endl;
        cout << "Tempo de preparação: " << prepTime.count() << " ms" << endl;
        cout << "Tempo de execução da pesquisa: " << execTime.count() << " ms" << endl;
        cout << "Tempo total: " << totalTime.count() << " ms" << endl;
        cout << "Ocorrências de 'love': " << totalCount.loveCount << endl;
        cout << "Ocorrências de 'hate': " << totalCount.hateCount << endl;

        if (totalCount.loveCount > totalCount.hateCount) 
        {
            cout << "Palavra mais utilizada: 'love'" << endl;
        } 
        else if (totalCount.loveCount < totalCount.hateCount) 
        {
            cout << "Palavra mais utilizada: 'hate'" << endl;
        } 
        else 
        {
            cout << "Ambas as palavras foram utilizadas igualmente." << endl;
        }

        cout << "-----------------------------" << endl;
    }

    return 0;
}
