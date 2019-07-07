#include "filesystem.h"

namespace filesystem {

std::tuple<std::string, std::string> splitFileDirectory(const std::string & fullpath)
{
    auto split_at = fullpath.find_last_of("/");
    if(split_at == fullpath.npos) {
        return std::make_tuple(".", fullpath);
    }
    return std::make_tuple(fullpath.substr(0, split_at),
                           fullpath.substr(split_at + 1));
}

}; // filesystem
