#ifndef BOUNDS_H
#define BOUNDS_H

#include <vector>
#include <deque>
#include <algorithm>
#include <limits>
#include <cmath>

class Bounds {
public:
    static double lbKeogh(const std::vector<double>& q, 
                         const std::vector<double>& u, 
                         const std::vector<double>& l, 
                         double bsf);
    
    static double lbEnhanced(const std::vector<double>& q, 
                            const std::vector<double>& t,
                            const std::vector<double>& u, 
                            const std::vector<double>& l, 
                            int k, int w, double bsf);
    
    static double lbImproved(const std::vector<double>& q, 
                            const std::vector<double>& t,
                            const std::vector<double>& ut, 
                            const std::vector<double>& lt, 
                            int w, double bsf);
    
    static double lbPetitjean(const std::vector<double>& q, 
                             const std::vector<double>& uq,
                             const std::vector<double>& lq, 
                             const std::vector<double>& t,
                             const std::vector<double>& ut, 
                             const std::vector<double>& lt, 
                             int w, double bsf);
    
    static double lbWebb(const std::vector<double>& q, 
                        const std::vector<double>& uq,
                        const std::vector<double>& lq, 
                        const std::vector<double>& luq,
                        const std::vector<double>& ulq, 
                        const std::vector<double>& t,
                        const std::vector<double>& ut, 
                        const std::vector<double>& lt,
                        const std::vector<double>& lut, 
                        const std::vector<double>& ult,
                        int window, double bsf);
    
    static double enhancedLBWebb(const std::vector<double>& q, 
                                const std::vector<double>& uq,
                                const std::vector<double>& lq, 
                                const std::vector<double>& luq,
                                const std::vector<double>& ulq, 
                                const std::vector<double>& t,
                                const std::vector<double>& ut, 
                                const std::vector<double>& lt,
                                const std::vector<double>& lut, 
                                const std::vector<double>& ult,
                                int k, int window, double bsf);
    
    static void lemireGetEnvelopes(const std::vector<double>& t, int w,
                                  std::vector<double>& u, std::vector<double>& l);
    
    static void lemireGetUpper(const std::vector<double>& t, int w,
                              std::vector<double>& u);
    
    static void lemireGetLower(const std::vector<double>& t, int w,
                              std::vector<double>& l);
    
    static void simpleGetEnvelopes(const std::vector<double>& t, int w,
                                  std::vector<double>& u, std::vector<double>& l);
    
    static inline double dist(double v1, double v2) {
        double d = v1 - v2;
        return d * d;
    }

private:
    static std::deque<int> du;
    static std::deque<int> dl;
    static std::vector<double> proj;
    static std::vector<double> up;
    static std::vector<double> lp;
    static const int defaultAllocSize = 3000;
};

#endif // BOUNDS_H