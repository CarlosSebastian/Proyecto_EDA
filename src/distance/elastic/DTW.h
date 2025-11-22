#ifndef DTW_H
#define DTW_H

#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>

class DTW {
public:
    static double distance(const std::vector<double>& series1, 
                          const std::vector<double>& series2, 
                          int windowSize);
    
    static inline double min(double A, double B, double C) {
        if (A < B) {
            return (A < C) ? A : C;
        } else {
            return (B < C) ? B : C;
        }
    }
    
    static inline double squaredDistance(double A, double B) {
        double x = A - B;
        return x * x;
    }
};

#endif // DTW_H