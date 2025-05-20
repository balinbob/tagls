#include <filesystem>
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

std::string toLower(const std::string& input);
void move_cursor_to_column(int col);
bool operator==(const Hdr& a, const Hdr& b);
bool operator!=(const Hdr& a, const Hdr& b);
bool stringListsEqual(const TagLib::StringList& a, const TagLib::StringList& b);
void printHeader(const Hdr& h, const Hdr& prevHdr, bool all = false);
void processFlac(const fs::path& dir, Hdr& prevHdr, bool first);