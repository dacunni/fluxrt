#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include <string>
#include <tuple>

namespace filesystem {

std::tuple<std::string, std::string> splitFileDirectory(const std::string & fullpath);

bool hasExtension(const std::string & filename, const std::string & ending);

}; // filesystem

#endif
