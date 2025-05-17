#include <iostream>
#include <string>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <string>
#include <algorithm>
#include <cctype>
#include <taglib/fileref.h>
#include <taglib/flacfile.h>
#include <taglib/tag.h>
#include <taglib/xiphcomment.h>

namespace fs = std::filesystem;

struct hdr {
    std::string artist;
    std::string album;
    std::string date;
    std::string genre;
    std::string comment;
};


std::string toLower(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}
void move_cursor_to_column(int col) {
    std::cout << "\033[" << col << "G";
}

int main(int argc, char* argv[]) {
    for (int argno = 1; argno < argc; argno++) {
        std::string arg = argv[argno];
        std::cout << arg << "\n";
        fs::path dir = arg;
        if (fs::exists(dir) && fs::is_directory(dir)) {
            for (const auto& entry : fs::directory_iterator(dir)) {
                if (entry.is_regular_file() && toLower(entry.path().extension().string()) == ".flac") {
                    int fieldLen = 16;
                    std::string fname = entry.path().filename().stem().string();
                    fname = (fname.length() > fieldLen) ? fname.substr(0, fieldLen) : fname;
                    std::cout << fname << "..flac ";
                    TagLib::FLAC::File file(entry.path().string().c_str());
                    // TagLib::Tag* tag = file.tag();
                    TagLib::FLAC::File* flacFile = dynamic_cast<TagLib::FLAC::File*>(&file);
                    if (flacFile != nullptr && flacFile->isValid()) {
                        if (TagLib::Ogg::XiphComment* xiph = file.xiphComment()) {
                            auto fields = xiph->fieldListMap();
                            for (const auto& [key, values] : fields) {
                                int cursorColumn = fieldLen + 8;
                                move_cursor_to_column(cursorColumn);
                                std::cout << key << ":";
                                for (const auto& val : values) {
                                    move_cursor_to_column(cursorColumn + 16);    
                                    std::cout << val << std::endl;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}