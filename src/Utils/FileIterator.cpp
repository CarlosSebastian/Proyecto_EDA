#include "FileIterator.h"
#include <iostream>
#include <algorithm>

FileIterator::FileIterator(int verbosity, const std::string& datasetDirectory)
    : datasetDirectory(datasetDirectory), nextIndex(0), verbosity(verbosity) {
    
    if (verbosity > 1) std::cout << "Opened files folder" << std::endl;
    
    try {
        for (const auto& entry : fs::directory_iterator(datasetDirectory)) {
            if (entry.is_directory()) {
                listData.push_back(entry.path());
                if (verbosity > 1) {
                    std::cout << entry.path().filename().string() << " is a directory" << std::endl;
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error accessing directory: " << e.what() << std::endl;
    }
    
    std::sort(listData.begin(), listData.end());
    std::cout << "found " << listData.size() << " folders" << std::endl;
}

void FileIterator::reset() {
    train = "";
    test = "";
    name = "";
    nextIndex = 0;
}

bool FileIterator::atEnd() const {
    return nextIndex >= listData.size();
}

bool FileIterator::getByName(const std::string& fileName) {
    train = datasetDirectory + "/" + fileName + "/" + fileName + "_TRAIN";
    test = datasetDirectory + "/" + fileName + "/" + fileName + "_TEST";
    
    if (fs::exists(train) && fs::exists(test)) {
        name = fileName;
        return true;
    }
    return false;
}

void FileIterator::getNext() {
    test = "";
    bool finished = false;
    
    while (!finished) {
        if (nextIndex >= listData.size()) {
            train = "";
            test = "";
            name = "";
            finished = true;
        } else {
            fs::path dataRep = listData[nextIndex++];
            finished = getByName(dataRep.filename().string());
        }
    }
}