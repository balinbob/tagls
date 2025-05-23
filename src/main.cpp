#include "process.h"
#include "CaseInsensitiveTagMap.hpp"
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

/**
 * Print out the usage message for tagls.
 *
 * @param[in] argv command line arguments
 */
void printUsage(char* argv[]) {
    std::cout << "Usage:  " << argv[0] << " [ -e | -r | -c ]" << " <path/to/directory/containing/.flac/files>\n";
    std::cout << "-e  extended (nonstandard) tags\n";
    std::cout << "-r  recurse directories\n";
    std::cout << "-c  color output\n";
}

/**
 * The main entry point of the program.
 *
 * This function parses command line arguments and calls `processFlac` for each
 * FLAC file in the specified directories. It keeps track of the previous header
 * and extra tags to be able to print information only when it changes.
 *
 * @param argc the number of command line arguments
 * @param argv the command line arguments
 * @return 0 if successful, 1 if there were errors
 */
int main(int argc, char* argv[]) {
    int errors = 0;
    bool recurse = false;
    bool extended = false;
    std::vector<fs::path> inputDirs;
    std::vector<fs::path> allDirs;
    if (argc < 2) printUsage(argv);
    for (int argno = 1; argno < argc; argno++) {
        std::string arg = argv[argno];
        if (arg == "-r") {
            recurse = true;
        }
        else if (arg == "-e") {
            extended = true;
        }
        else if (arg == "-c") {
            useColor = true;
        }
        else if (fs::exists(arg) && fs::is_directory(arg)) {
            inputDirs.emplace_back(arg);
        }
        else {
            if (!errors) printUsage(argv);
            errors += 1;
            std::cout << "invalid argument or path not found: " << arg << "\n";
        }
    }

    bool first = true;
    Hdr prevHdr;
    TagMap prevExtra;
    for (const auto& dir : inputDirs) {
        if (recurse) {
            for (const auto& entry : fs::recursive_directory_iterator(dir)) {
                if (entry.is_regular_file() && toLower(entry.path().extension().string()) == ".flac") {
                    processFlac(entry.path(), prevHdr, prevExtra, first, extended);
                    first = false;
                }
            }
        }
        else {
            for (const auto& entry : fs::directory_iterator(dir)) {
                if (entry.is_regular_file() && toLower(entry.path().extension().string()) == ".flac") {
                    processFlac(entry.path(), prevHdr, prevExtra, first, extended);
                    first = false;
                }
            }
        }
    }
}
