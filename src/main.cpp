#include "process.h"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <string>
#include <algorithm>
#include <cctype>
#include <vector>

namespace fs = std::filesystem;

Hdr prevHdr;
bool first = false;

void printUsage(char* argv[]) {
    std::cout << argv[0] << " <path to directory containing .flac files>\n";
}

int main(int argc, char* argv[]) {
    bool recurse = false;
    std::vector<fs::path> inputDirs;
    std::vector<fs::path> allDirs;
    for (int argno = 1; argno < argc; argno++) {
        std::string arg = argv[argno];
        if (arg == "-r") {
            recurse = true;
        }
        else if (fs::exists(arg) && fs::is_directory(arg)) {
            inputDirs.emplace_back(arg);
        }
        else {
            printUsage(argv);
            std::cout << "invalid argument: " << arg << "\n";
        }
    }

    bool first = true;
    Hdr prevHdr;
    for (const auto& dir : inputDirs) {
        if (recurse) {
            for (const auto& entry : fs::recursive_directory_iterator(dir)) {
                if (entry.is_regular_file() && toLower(entry.path().extension().string()) == ".flac") {
                    processFlac(entry.path(), prevHdr, first);
                    first = false;
                }
            }
        }
        else {
            for (const auto& entry : fs::directory_iterator(dir)) {
                if (entry.is_regular_file() && toLower(entry.path().extension().string()) == ".flac") {
                    processFlac(entry.path(), prevHdr, first);
                    first = false;
                }
            }
        }
    }
}
