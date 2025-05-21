#ifndef CASE_INSENSITIVE_TAG_MAP_HPP
#define CASE_INSENSITIVE_TAG_MAP_HPP

#include <string>
#include <unordered_map>
#include <taglib/tstringlist.h>

struct CaseInsensitiveHash {
    std::size_t operator()(const std::string& s) const {
        std::string lowered;
        lowered.reserve(s.size());
        for (char c : s) lowered += std::tolower(c);
        return std::hash<std::string>()(lowered);
    }
};

struct CaseInsensitiveEqual {
    bool operator()(const std::string& a, const std::string& b) const {
        return std::equal(a.begin(), a.end(), b.begin(), b.end(),
            [](char ac, char bc) { return std::tolower(ac) == std::tolower(bc); });
    }
};

// TagMap: case-insensitive key map for tag name → value list
using TagMap = std::unordered_map<
    std::string,
    TagLib::StringList,
    CaseInsensitiveHash,
    CaseInsensitiveEqual
>;

#endif // CASE_INSENSITIVE_TAG_MAP_HPP
