#include "process.h"
#include "CaseInsensitiveTagMap.hpp"
#include <iostream>
#include <string>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <taglib/fileref.h>
#include <taglib/flacfile.h>
#include <taglib/tag.h>
#include <taglib/xiphcomment.h>
#include <taglib/tstringlist.h>

#define RESET   (useColor ? "\033[0m"  : "")
#define GRAY    (useColor ? "\033[90m" : "")
#define BROWN   (useColor ? "\033[33m" : "")
#define CYAN    (useColor ? "\033[36m" : "")
#define GREEN   (useColor ? "\033[32m" : "")

namespace fs = std::filesystem;


/**
 * Returns a copy of the input string with all characters converted to lower
 * case using the locale-insensitive std::tolower function.
 */
std::string toLower(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

/**
 * Moves the terminal cursor to a specified column.
 *
 * @param col The column number to move the cursor to, where column 1 is the
 *            first column of the line.
 */

void move_cursor_to_column(int col) {
    std::cout << "\033[" << col << "G";
}

/**
 * Compares two Hdr objects for equality.
 *
 * Two Hdr objects are equal if and only if all their corresponding StringList
 * fields are equal.
 */
bool operator==(const Hdr& a, const Hdr& b) {
    return stringListsEqual(a.artist, b.artist) &&
           stringListsEqual(a.album, b.album) &&
           stringListsEqual(a.genre, b.genre) &&
           stringListsEqual(a.date, b.date) &&
           stringListsEqual(a.discnumber, b.discnumber) &&
           stringListsEqual(a.comment, b.comment);
}

/**
 * Compares two Hdr objects for inequality.
 *
 * Two Hdr objects are unequal if and only if any of their corresponding
 * StringList fields are unequal.
 */
bool operator!=(const Hdr& a, const Hdr& b) {
    return !(a == b);
}

/**
 * Compares two TagLib::StringList objects for equality.
 *
 * Two TagLib::StringList objects are equal if and only if they have the same
 * size and all corresponding elements are equal.
 */
bool stringListsEqual(const TagLib::StringList& a, const TagLib::StringList& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i)
        if (a[i] != b[i]) return false;
    return true;
}

/**
 * Prints the header information for a FLAC file.
 *
 * This function displays various metadata fields of a FLAC file's header,
 * such as artist, album, disc number, date, genre, and comment. It compares
 * each field against the previous header and prints only the fields that 
 * differ, unless the `all` parameter is set to `true`, in which case all 
 * fields are printed regardless of changes.
 *
 * @param h The current header containing the metadata to be printed.
 * @param prevHdr The previous header used for comparison to determine 
 *                which fields have changed.
 * @param all A boolean flag indicating whether to print all fields or 
 *            only those that have changed since the previous header.
 */

/**
 * Prints the header information for a FLAC file.
 *
 * This function displays various metadata fields of a FLAC file's header,
 * such as artist, album, disc number, date, genre, and comment. It compares
 * each field against the previous header and prints only the fields that 
 * differ, unless the `all` parameter is set to `true`, in which case all 
 * fields are printed regardless of changes.
 *
 * @param h The current header containing the metadata to be printed.
 * @param prevHdr The previous header used for comparison to determine 
 *                which fields have changed.
 * @param all A boolean flag indicating whether to print all fields or 
 *            only those that have changed since the previous header.
 */

void printHeader(const Hdr& h, const Hdr& prevHdr, bool all) {
    move_cursor_to_column(0);

    std::string artist = h.artist.toString().to8Bit();
    std::string album = h.album.toString().to8Bit();
    std::string discnumber = h.discnumber.toString().to8Bit();
    std::string date = h.date.toString().to8Bit();
    std::string genre = h.genre.toString().to8Bit();
    std::string comment = h.comment.toString().to8Bit();
    
    std::cout << GREEN;

    if ((artist != "") && (all || (h.artist != prevHdr.artist))) {
        std::cout << "artist    : " << h.artist << std::endl;
    }
    if ((album != "") && (all || h.album != prevHdr.album)) {
        std::cout << "album     : " << h.album << std::endl;
    }
    if ((discnumber != "") && (all || h.discnumber != prevHdr.discnumber)) {
        std::cout << "discnumber: " << h.discnumber << std::endl;
    }
    if ((date != "") && (all || h.date != prevHdr.date)) {
        std::cout << "date      : " << h.date << std::endl;
    }
    if ((genre != "") && (all || (h.genre != prevHdr.genre))) {
        std::cout << "genre     : " << h.genre << std::endl;
    }
    if ((comment != "") && (all || h.comment != prevHdr.comment)) {
        std::cout << "comment   : " << h.comment << std::endl;
    }
    std::cout << RESET;
}

/**
 * Updates the header fields of a Hdr object with new values.
 *
 * The function takes a Hdr object, a TagLib::StringList containing the new
 * values, and a string key identifying which header field to update. It
 * compares the key to the names of the header fields in a case-insensitive
 * manner and updates the corresponding field in the Hdr object.
 *
 * @param h The Hdr object whose header fields are to be updated.
 * @param values The TagLib::StringList containing the new values for the
 *               header field.
 * @param skey The string key identifying which header field to update.
 */
void collectHeaderTags(Hdr& h, const TagLib::StringList& values, const std::string& skey) {
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

/**
 * Prints FLAC tags in a readable format.
 *
 * This function takes a path to a FLAC file, the previous header tags, the
 * previous extra tags, a boolean indicating whether this is the first file
 * being processed, and a boolean indicating whether or not to print extended
 * information. It prints the tags from the FLAC file in a readable format and
 * keeps track of the previous header and extra tags to be able to print
 * information only when it changes.
 *
 * @param path The path to the FLAC file being processed.
 * @param prevHdr The previous header tags.
 * @param prevExtra The previous extra tags.
 * @param first A boolean indicating whether this is the first file being
 *              processed.
 * @param extended A boolean indicating whether or not to print extended
 *                 information.
 */
void processFlac(const fs::path& path, Hdr& prevHdr, TagMap& prevExtra, bool first, bool extended) {        
    std::cout << std::endl;   // separate command from output
    int fieldLen = 28;
    int cursorColumn = fieldLen + 8;
                    
    std::string fname = path.filename().stem().string();
    fname = (fname.length() > fieldLen) ? fname.substr(0, fieldLen) : fname;
    TagLib::FLAC::File file(path.string().c_str());
    TagLib::FLAC::File* flacFile = dynamic_cast<TagLib::FLAC::File*>(&file);
    if (flacFile != nullptr && flacFile->isValid()) {
        if (TagLib::Ogg::XiphComment* xiph = file.xiphComment()) {
            Hdr h;
            std::string tracknumber;
            std::string title; 
            auto fields = xiph->fieldListMap();
            for (const auto& [key, values] : fields) {
                std::string skey = key.to8Bit(true);
                for (const auto& val : values) {
                    if (toLower(skey) == "tracknumber") {
                        tracknumber = val.to8Bit(true);
                        continue;
                    }
                    else if (toLower(skey) == "title") {
                        title = val.to8Bit(true);
                        continue;
                    }

                    collectHeaderTags(h, values, skey);

                }
            }

            TagMap extra;
            for (const auto& [key, valueList] : fields) {
                std::string skey = toLower(key.to8Bit());
            
                if (standardKeys.find(skey) == standardKeys.end()) {
                    extra[skey] = valueList;
                }
            }


            if (first) {
                printHeader(h, prevHdr, true);
            }
            else if (h != prevHdr) {
                std::cout << std::endl;
                printHeader(h, prevHdr);
            }

            if (extended) {
                for (const auto& [key, values] : extra) {
                    const auto it = prevExtra.find(key);
                    const bool changed = it == prevExtra.end() || it->second != values;

                    if (first || changed) {
                        for (const auto& val : values) {
                            std::cout << "[+] " << key << ": " << val.to8Bit(true) << "\n";
                        }
                    }
                }
                prevExtra = extra;
            }

            if (h.album != prevHdr.album) {
                std::cout << GRAY;
                move_cursor_to_column(0);
                std::cout << "filename";
                move_cursor_to_column(cursorColumn);
                std::cout << "t#";
                move_cursor_to_column(cursorColumn + 4);
                std::cout << "title\n";
                std::cout << RESET;
            }
            move_cursor_to_column(0);
            std::cout << CYAN;
            std::cout << fname;
            move_cursor_to_column(fieldLen);
            std::cout << "  .flac";

            move_cursor_to_column(cursorColumn);
            std::cout << BROWN << tracknumber << "  " << CYAN << title << RESET;
            prevHdr = h;
        }
    }
}