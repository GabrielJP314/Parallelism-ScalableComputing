#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>

using namespace std;

const int MAX_THREADS = 100; // Maximum number of threads to use
int numThreads = 1; // Number of threads to use

struct WordCount
{
    int loveCount = 0;
    int hateCount = 0;
};

vector<vector<string>> loadAndSplitFile(const string& filename)
{
    ifstream file(filename);
    string line;
    vector<string> block;
    vector<vector<string>> blocks;
    while (getline(file, line)) 
    {
        if (line.empty()) 
        {
            continue;
        }
        block.push_back(line);
        if (block.size() >= 1000) // We will split the file into blocks of 1000 lines each
        {
            // If the block is full, we add it to the list of blocks and clear it for the next block
            blocks.push_back(block);
            block.clear();
        }
    }
    // If there are any remaining lines aka the file size is not a multiple of 1000
    if (!block.empty()) 
    {
        blocks.push_back(block);
    }
    return blocks;
}

void processBlock(const vector<vector<string>>& blocks, int start, int end, WordCount& localCount)
{
    for (int i = start; i < end; ++i) 
    {
        for (const auto& line : blocks[i]) 
        {
            // This simplistic approach does not handle punctuation or case differences
            if (line.find("love") != string::npos) // Checks if the substring "love" is present
            {
                localCount.loveCount++;
            }
            if (line.find("hate") != string::npos) // Checks if the substring "hate" is present
            {
                localCount.hateCount++;
            }
        }
    }
}

int main() 
{
    // Loop through 
    for (int i = 1; i <= MAX_THREADS; i++)
    {

    string filename = "shakespeare.txt";

    // Load and split the file into blocks, and measure the time it takes
    auto startPrep = chrono::high_resolution_clock::now();
    vector<vector<string>> blocks = loadAndSplitFile(filename);
    auto endPrep = chrono::high_resolution_clock::now();
    auto prepTime = chrono::duration_cast<chrono::milliseconds>(endPrep - startPrep);

    if(blocks.empty()) {
        cout << "No data loaded or file is empty.\n";
        return 1;
    }

    /*
    // After removing the for change this i to MAX_THREADS
    ###############
    ###############
    ###############
    ###############
    ###############
    ###############
    ###############
    ###############
    ###############
    ###############
    ###############
    ###############
    */
    numThreads = min(i, static_cast<int>(blocks.size()));
    vector<thread> threads;
    vector<WordCount> localCounts(numThreads);
    int blockSize = blocks.size() / numThreads;

    
    auto startSearch = chrono::high_resolution_clock::now();
    for (int i = 0; i < numThreads; ++i) 
    {   
        int start = i * blockSize;
        int end = (i == numThreads - 1) ? blocks.size() : (i + 1) * blockSize;
        threads.emplace_back(processBlock, std::cref(blocks), start, end, std::ref(localCounts[i]));
    }

    for (auto& thread : threads) 
    {
        thread.join();
    }
    auto endSearch = chrono::high_resolution_clock::now();
    auto execTime = chrono::duration_cast<chrono::milliseconds>(endSearch - startSearch);


    WordCount totalCount;
    for (const auto& count : localCounts) 
    {
        totalCount.loveCount += count.loveCount;
        totalCount.hateCount += count.hateCount;
    }
        
        // A partir daqui, você pode realizar mais processamento, se necessário.


        auto totalTime = prepTime + execTime;

        
        cout << "Número de threads: " << numThreads << endl;
        cout << "Tamanho do bloco de cada thread: " << blocks.size() / numThreads << endl;
        cout << "Tempo de preparação: " << prepTime.count() << " ms" << endl;
        cout << "Tempo de execução da pesquisa: " << execTime.count() << " ms" << endl;
        cout << "Tempo total: " << totalTime.count() << " ms" << endl;
        cout << "Ocorrências de 'love': " << totalCount.loveCount << endl;
        cout << "Ocorrências de 'hate': " << totalCount.hateCount << endl;

        int most_used;

        if (totalCount.loveCount > totalCount.hateCount) 
        {
            cout << "Palavra mais utilizada: 'love'" << endl;
            most_used = 1;
        } 
        else if (totalCount.loveCount < totalCount.hateCount) 
        {
            cout << "Palavra mais utilizada: 'hate'" << endl;
            most_used = 0;
        } 
        else 
        {
            cout << "Ambas as palavras foram utilizadas igualmente." << endl;
            most_used = 2;
        }

        cout << "-----------------------------" << endl;


        // Write the results to a file
        ofstream outfile;
        outfile.open("saida.txt", ios::app);
        outfile << numThreads << ";" <<  blocks.size() / numThreads << ";" << prepTime.count() << ";" << execTime.count() << ";" << totalTime.count() << ";" << totalCount.loveCount << ";" << totalCount.hateCount << ";"<< most_used << endl;
        outfile.close();

        }
        return 0;
    }

