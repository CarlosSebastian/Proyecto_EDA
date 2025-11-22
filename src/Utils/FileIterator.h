#ifndef FILEITERATOR_H
#define FILEITERATOR_H

#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class FileIterator {
public:
    FileIterator(int verbosity, const std::string& datasetDirectory);
    
    void reset();
    bool atEnd() const;
    bool getByName(const std::string& fileName);
    void getNext();
    
    std::string getTrain() const { return train; }
    std::string getTest() const { return test; }
    std::string getName() const { return name; }

private:
    std::string train;
    std::string test;
    std::string name;
    std::string datasetDirectory;
    std::vector<fs::path> listData;
    size_t nextIndex;
    int verbosity;
};

#endif // FILEITERATOR_H