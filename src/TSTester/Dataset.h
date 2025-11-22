#ifndef DATASET_H
#define DATASET_H

#include "ClassifiedSequence.h"
#include <vector>
#include <string>
#include <random>

class Dataset {
public:
    std::vector<ClassifiedSequence> series;
    int count;

    Dataset() : count(0), verbosity(1) {}

    void load(const std::string& filename);
    void shuffle(int seed);
    void print() const;

private:
    int maxLength;
    std::vector<double> buffer;
    int verbosity;
    std::mt19937 rnd;
};

#endif // DATASET_H