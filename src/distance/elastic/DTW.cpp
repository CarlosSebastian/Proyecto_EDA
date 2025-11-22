#include "DTW.h"

double DTW::distance(const std::vector<double>& series1, 
                     const std::vector<double>& series2, 
                     int windowSize) {
    if (windowSize == -1) {
        windowSize = series1.size();
    }

    int length1 = series1.size();
    int length2 = series2.size();
    int maxLength = std::max(length1, length2);
    
    std::vector<std::vector<double>> matrix(maxLength, 
        std::vector<double>(maxLength, 0.0));

    int i, j;
    matrix[0][0] = squaredDistance(series1[0], series2[0]);
    
    for (i = 1; i < std::min(length1, 1 + windowSize); i++) {
        matrix[i][0] = matrix[i - 1][0] + squaredDistance(series1[i], series2[0]);
    }
    
    if (i < length1)
        matrix[i][0] = std::numeric_limits<double>::infinity();

    for (j = 1; j < std::min(length2, 1 + windowSize); j++) {
        matrix[0][j] = matrix[0][j - 1] + squaredDistance(series1[0], series2[j]);
    }
    
    if (j < length2)
        matrix[0][j] = std::numeric_limits<double>::infinity();

    for (i = 1; i < length1; i++) {
        int jStart = (i - windowSize < 1) ? 1 : i - windowSize;
        int jStop = (i + windowSize + 1 > length2) ? length2 : i + windowSize + 1;

        if (jStart > 1) 
            matrix[i][jStart - 1] = std::numeric_limits<double>::infinity();
            
        for (j = jStart; j < jStop; j++) {
            matrix[i][j] = min(matrix[i - 1][j - 1], matrix[i][j - 1], matrix[i - 1][j])
                          + squaredDistance(series1[i], series2[j]);
        }
        
        if (jStop < length2)
            matrix[i][jStop] = std::numeric_limits<double>::infinity();
    }
    
    return matrix[length1 - 1][length2 - 1];
}