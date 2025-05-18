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
#include <taglib/tstringlist.h>


namespace fs = std::filesystem;

struct Hdr {
    TagLib::StringList artist;
    TagLib::StringList album;
    TagLib::StringList date;
    TagLib::StringList genre;
    TagLib::StringList comment;
    TagLib::StringList discnumber;
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

bool stringListsEqual(const TagLib::StringList& a, const TagLib::StringList& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i)
        if (a[i] != b[i]) return false;
    return true;
}

bool operator==(const Hdr& a, const Hdr& b) {
    return stringListsEqual(a.artist, b.artist) &&
           stringListsEqual(a.album, b.album) &&
           stringListsEqual(a.genre, b.genre) &&
           stringListsEqual(a.date, b.date) &&
           stringListsEqual(a.discnumber, b.discnumber) &&
           stringListsEqual(a.comment, b.comment);
}

bool operator!=(const Hdr& a, const Hdr& b) {
    return !(a == b);
}

void printHeader(const Hdr& h, const Hdr& prevHdr, bool all = false) {
    move_cursor_to_column(0);
    std::string artist = h.artist.toString().to8Bit();
    std::string album = h.album.toString().to8Bit();
    std::string discnumber = h.discnumber.toString().to8Bit();
    std::string date = h.date.toString().to8Bit();
    std::string genre = h.genre.toString().to8Bit();
    std::string comment = h.comment.toString().to8Bit();
    
    if ((artist != "") && (h.artist != prevHdr.artist)) {
        std::cout << h.artist << std::endl;
    }
    if ((album != "") && (all || h.album != prevHdr.album)) {
        std::cout << h.album << std::endl;
    }
    if ((discnumber != "") && (all || h.discnumber != prevHdr.discnumber)) {
        std::cout << h.discnumber << std::endl;
    }
    if ((date != "") && (all || h.date != prevHdr.date)) {
        std::cout << h.date << std::endl;
    }
    if ((genre != "") && all || (h.genre != prevHdr.genre)) {
        std::cout << h.genre << std::endl;
    }
    if ((comment != "") && (all || h.comment != prevHdr.comment)) {
        std::cout << h.comment << std::endl;
    }
}


int main(int argc, char* argv[]) {
    for (int argno = 1; argno < argc; argno++) {
        std::string arg = argv[argno];
//        std::cout << arg << "\n";
        fs::path dir = arg;
        Hdr prevHdr;
        bool first = true;
        std::cout << std::endl;   // separate command from output
        if (fs::exists(dir) && fs::is_directory(dir)) {
            for (const auto& entry : fs::directory_iterator(dir)) {
                if (entry.is_regular_file() && toLower(entry.path().extension().string()) == ".flac") {
                    int fieldLen = 22;
                    int cursorColumn = fieldLen + 8;
                    
                    std::string fname = entry.path().filename().stem().string();
                    fname = (fname.length() > fieldLen) ? fname.substr(0, fieldLen) : fname;
                //    std::cout << fname << "..flac ";
                    TagLib::FLAC::File file(entry.path().string().c_str());
                    TagLib::FLAC::File* flacFile = dynamic_cast<TagLib::FLAC::File*>(&file);
                    if (flacFile != nullptr && flacFile->isValid()) {
                        if (TagLib::Ogg::XiphComment* xiph = file.xiphComment()) {
                            Hdr h;
                            std::string tracknumber;
                            std::string title; 
                            auto fields = xiph->fieldListMap();
                            for (const auto& [key, values] : fields) {
                                std::string skey = key.to8Bit(true);
                            //    move_cursor_to_column(cursorColumn);
                                // std::cout << key << ":";
                                for (const auto& val : values) {
                                    // move_cursor_to_column(cursorColumn + 16);    
                                //     std::cout << val << std::endl;
                                    if (toLower(skey) == "tracknumber") {
                                        tracknumber = val.to8Bit(true);
                                        continue;
                                    }
                                    else if (toLower(skey) == "title") {
                                        title = val.to8Bit(true);
                                        continue;
                                    }

                                    if (toLower(skey) == "artist") {
                                        h.artist = values;
                                    }
                                    else if (toLower(skey) == "album") {
                                        h.album = values;
                                    }
                                    else if (toLower(skey) == "date") {
                                        h.date = values;
                                    }
                                    else if (toLower(skey) == "genre") {
                                        h.genre = values;
                                    }
                                    else if (toLower(skey) == "comment") {
                                        h.comment = values;
                                    }
                                    else if (toLower(skey) == "discnumber") {
                                        h.discnumber = values;
                                    }
                                }
                            }
                            if (first) {
                                first = false;
                                printHeader(h, prevHdr, true);
                            }
                            else if (h != prevHdr) {
                                std::cout << std::endl;
                                printHeader(h, prevHdr);
                                // std::cout << "CHANGED!!!!!!!!!!!\n";
                            }
                            move_cursor_to_column(0);
                            std::cout << fname;
                            move_cursor_to_column(fieldLen);
                            std::cout << "  .flac";

                            move_cursor_to_column(cursorColumn);
                            std::cout << tracknumber << "  " << title << std::endl;
                            prevHdr = h;
                        }
                    }
                }
            }
        }
    }
    return 0;
}