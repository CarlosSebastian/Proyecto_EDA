#ifndef OSUTILS_H
#define OSUTILS_H

#include <string>

#ifdef _WIN32
    #define PATH_SEPARATOR "\\"
#else
    #define PATH_SEPARATOR "/"
#endif

class OSUtils {
public:
    static std::string directorySep() {
        return PATH_SEPARATOR;
    }
    
    static bool isWindows() {
#ifdef _WIN32
        return true;
#else
        return false;
#endif
    }
};

#endif // OSUTILS_H