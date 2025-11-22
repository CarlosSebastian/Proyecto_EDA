#include "Dataset.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

void Dataset::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "FileNotFoundException: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (verbosity > 1) std::cout << "Processing line: " << line << std::endl;

        buffer.clear();
        std::stringstream ss(line);
        std::string token;

        // Obtener la clase
        if (!std::getline(ss, token, ',')) continue;
        int thisKlass = std::stoi(token);

        if (verbosity > 1) std::cout << "Class = " << thisKlass << std::endl;

        // Obtener la secuencia
        while (std::getline(ss, token, ',')) {
            try {
                // Eliminar espacios en blanco
                token.erase(0, token.find_first_not_of(" \t\n\r\f\v"));
                token.erase(token.find_last_not_of(" \t\n\r\f\v") + 1);
                
                if (!token.empty()) {
                    double value = std::stod(token);
                    buffer.push_back(value);
                    if (verbosity > 1) std::cout << "Value = " << value << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error parsing value: " << token << std::endl;
            }
        }

        ClassifiedSequence cs;
        cs.sequence = buffer;
        cs.klass = thisKlass;

        series.push_back(cs);
        if (verbosity > 1) std::cout << "series.add(cs)" << std::endl;
    }

    file.close();
    count = series.size();
}

void Dataset::shuffle(int seed) {
    rnd.seed(seed);
    std::shuffle(series.begin(), series.end(), rnd);
}

void Dataset::print() const {
    for (const auto& cs : series) {
        std::cout << cs.klass;
        for (size_t i = 0; i < cs.sequence.size(); i++) {
            std::cout << ", " << cs.sequence[i];
        }
        std::cout << std::endl;
    }
}