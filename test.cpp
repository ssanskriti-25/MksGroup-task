#include <iostream>
#include <fstream>
#include <thread>
#include <queue>
#include <string>
#include <vector>
#include <algorithm>
#include <mutex>
#include <condition_variable>
#include <cctype>

std::queue<std::string> q1;
std::queue<std::string> q2;
std::mutex mtx1, mtx2;
std::condition_variable cv1, cv2;
bool Reading_Done = false;
bool Sorting_Done = false;

void readFile(const std::string &inputFilePath)
{
    std::ifstream inputFile(inputFilePath);
    if (!inputFile.is_open())
    {
        std::cerr << "Failed to open the input file!" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(inputFile, line))
    {
        size_t start = 0;
        size_t end;

        while ((end = line.find('.', start)) != std::string::npos)
        {
            std::string str = line.substr(start, end - start + 1);
            {
                std::unique_lock<std::mutex> lock(mtx1);
                q1.push(str);
            }
            cv1.notify_one();
            start = end + 1;
        }
    }

    Reading_Done = true;
    cv1.notify_all();
}


void sortLines()
{
    while (true)
    {
        std::string str;
        {
            std::unique_lock<std::mutex> lock(mtx1);
            cv1.wait(lock, []
                     { return !q1.empty() || Reading_Done; });

            if (q1.empty() && Reading_Done)
            {
                Sorting_Done = true;
                cv2.notify_all();
                return;
            }

            str = q1.front();
            q1.pop();
        }

        std::vector<std::string> words;
        std::string word;
        for (char c : str)
        {
            if (std::isspace(c) || c == '.')
            {
                if (!word.empty())
                {
                    words.push_back(word);
                    word.clear();
                }
            }
            else
            {
                word += c;
            }
        }
        if (!word.empty())
        {
            words.push_back(word);
        }

        std::sort(words.begin(), words.end(), [](const std::string &a, const std::string &b)
                  {
            std::string lowerA = a;
            std::string lowerB = b;
            std::transform(lowerA.begin(), lowerA.end(), lowerA.begin(), ::tolower);
            std::transform(lowerB.begin(), lowerB.end(), lowerB.begin(), ::tolower);
            return lowerA < lowerB; });

        std::string sortedSentence;
        for (const auto &w : words)
        {
            sortedSentence += w + " ";
        }
        if (!sortedSentence.empty())
        {
            sortedSentence.pop_back(); 
            sortedSentence += '.';
        }

        {
            std::unique_lock<std::mutex> lock(mtx2);
            q2.push(sortedSentence);
        }
        cv2.notify_one();
    }
}

void writeFile(const std::string &outputFilePath)
{
    std::ofstream outputFile(outputFilePath);
    if (!outputFile.is_open())
    {
        std::cerr << "Failed to open the output file!" << std::endl;
        return;
    }

    while (true)
    {
        std::string sentence;
        {
            std::unique_lock<std::mutex> lock(mtx2);
            cv2.wait(lock, []
                     { return !q2.empty() || Sorting_Done; });

            if (q2.empty() && Sorting_Done)
            {
                return;
            }

            sentence = q2.front();
            q2.pop();
        }

        outputFile << sentence << std::endl;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }

    std::string input = argv[1];
    std::string output = argv[2];

    std::thread reader(readFile, input);
    std::thread sorter(sortLines);
    std::thread writer(writeFile, output);

    reader.join();
    sorter.join();
    writer.join();

    return 0;
}
