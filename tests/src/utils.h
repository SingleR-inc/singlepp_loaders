#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <string>

inline void quick_dump(std::string path, std::string contents) {
    std::ofstream out (path, std::ofstream::out);
    out << contents;
}

#endif
