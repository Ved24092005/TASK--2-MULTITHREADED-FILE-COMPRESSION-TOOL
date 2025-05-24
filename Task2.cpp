#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <chrono>
#include <string>
using namespace std;
using namespace chrono;

// Run-Length Encoding
string rleCompress(const string& input) {
    string output = "";
    int n = input.length();
    for (int i = 0; i < n;) {
        int count = 1;
        while (i + count < n && input[i] == input[i + count]) count++;
        output += input[i] + to_string(count);
        i += count;
    }
    return output;
}

// RLE Decompression
string rleDecompress(const string& input) {
    string output = "";
    for (size_t i = 0; i < input.length(); ++i) {
        char ch = input[i];
        string num = "";
        while (++i < input.length() && isdigit(input[i])) {
            num += input[i];
        }
        --i;
        if (!num.empty())
            output.append(stoi(num), ch);
    }
    return output;
}

// Multithreaded Compression
void compressChunk(const string& input, string& output, int start, int end) {
    output = rleCompress(input.substr(start, end - start));
}

void processFile(bool compressMode, int numThreads) {
    ifstream inFile("input.txt");
    if (!inFile) {
        cerr << "Cannot open input.txt\n";
        return;
    }
    string content((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();

    auto start_time = high_resolution_clock::now();

    string result;
    if (compressMode) {
        int chunkSize = content.size() / numThreads;
        vector<string> outputs(numThreads);
        vector<thread> threads;

        for (int i = 0; i < numThreads; ++i) {
            int start = i * chunkSize;
            int end = (i == numThreads - 1) ? content.size() : (i + 1) * chunkSize;
            threads.emplace_back(compressChunk, ref(content), ref(outputs[i]), start, end);
        }

        for (auto& t : threads) t.join();

        for (const auto& s : outputs) result += s;
    } else {
        result = rleDecompress(content);  // SINGLE threaded decompression (safe)
    }

    auto end_time = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end_time - start_time);

    if (compressMode) {
        ofstream outFile("compressed.rle");
        outFile << result;
        outFile.close();
        cout << "Compressed to compressed.rle in " << duration.count() << " ms\n";
    } else {
        ofstream outFile("decompressed.txt");
        outFile << result;
        outFile.close();
        cout << "Decompressed to decompressed.txt in " << duration.count() << " ms\n";
    }
}

int main() {
    int choice, threads;
    cout << "=== Multithreaded File Compression Tool ===\n";
    cout << "1. Compress File\n";
    cout << "2. Decompress File\n";
    cout << "Enter choice: ";
    cin >> choice;

    if (choice == 1) {
        cout << "Enter number of threads to use: ";
        cin >> threads;
        processFile(true, threads);
    } else if (choice == 2) {
        processFile(false, 1); // Decompression is SINGLE THREADED
    } else {
        cout << "Invalid choice.\n";
    }

    return 0;
}
