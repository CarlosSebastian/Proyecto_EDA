#ifndef CLASSIFIED_SEQUENCE_H
#define CLASSIFIED_SEQUENCE_H

#include <vector>
#include <iostream>

class ClassifiedSequence {
public:
    std::vector<double> sequence;
    std::vector<double> upper;
    std::vector<double> lower;
    std::vector<double> lowerUpper;
    std::vector<double> upperLower;
    int klass;
    double lb;

    ClassifiedSequence() : klass(0), lb(0.0) {}

    // Operador para comparación (usado en sort)
    bool operator<(const ClassifiedSequence& other) const {
        return lb < other.lb;
    }

    void print(std::ostream& out) const {
        out << klass;
        for (size_t i = 0; i < sequence.size(); i++) {
            out << "," << sequence[i];
        }
        out << std::endl;
    }
};

#endif // CLASSIFIED_SEQUENCE_H