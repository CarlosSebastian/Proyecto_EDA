#include "ClassifiedSequence.h"
#include "Dataset.h"
#include "../Utils/FileIterator.h"
#include "../Utils/OSUtils.h"
#include "../Utils/UCRInfo.h"
#include "../distance/BoundsID.h"
#include "../distance/Bounds.h"
#include "../distance/elastic/DTW.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <random>
#include <ctime>
#include <filesystem>

enum class ExperimentType {
    UNSORTED,
    SHUFFLED,
    SORTED,
    TEST_TIGHTNESS
};

struct BoundInfo {
    BoundsID bound;
    int k;
    
    BoundInfo(BoundsID b, int K) : bound(b), k(K) {}
};

struct Statistics {
    int errors = 0;
    int correct = 0;
    int pruned = 0;
    int prepruned = 0;
    
    void reset() {
        errors = 0;
        correct = 0;
        pruned = 0;
        prepruned = 0;
    }
};

class TSTester {
private:
    static const int verbosity = 1;
    static const bool debug = false;
    static constexpr double delta = 1e-8;

public:
    static void doNN(BoundsID bound, int k, Dataset& train, Dataset& test, 
                     int w, bool winPercent, ExperimentType sort,
                     std::ofstream& timesStream, std::ofstream& timeStdStream,
                     std::ofstream& accuracyStream, std::ofstream& prunedStream);

    static void do1NNunsorted(BoundsID bound, int k, Dataset& train, Dataset& test,
                             int w, bool winPercent, std::ofstream& timesStream,
                             std::ofstream& timeStdStream, std::ofstream& accuracyStream,
                             std::ofstream& prunedStream, std::vector<double>& uetest,
                             std::vector<double>& letest, std::vector<double>& luetest,
                             std::vector<double>& uletest, Statistics& stats);

    static void do1NNsorted(BoundsID bound, int k, Dataset& train, Dataset& test,
                           int w, bool winPercent, std::ofstream& timesStream,
                           std::ofstream& timeStdStream, std::ofstream& accuracyStream,
                           std::ofstream& prunedStream, std::vector<double>& uetest,
                           std::vector<double>& letest, std::vector<double>& luetest,
                           std::vector<double>& uletest, Statistics& stats);

    static void runNN(BoundsID bound, int k, int w, bool winPercent, 
                     ExperimentType sort, std::ofstream& timesStream,
                     std::ofstream& timeStdStream, std::ofstream& accuracyStream,
                     std::ofstream& prunedStream, const std::string& datasetDirectory);

    static void runNNd(BoundsID bound, int k, int w, bool winPercent,
                      const std::string& dataset, ExperimentType sort,
                      std::ofstream& timesStream, std::ofstream& timeStdStream,
                      std::ofstream& accuracyStream, std::ofstream& prunedStream,
                      const std::string& datasetDirectory);

    static void testNN(const std::string& baseDir, const std::string& outDir,
                      const std::string& suffix, std::vector<std::string>& datasets,
                      std::vector<BoundInfo>& bounds, int w, bool winPercent,
                      ExperimentType sort, const std::string& datasetDirectory);

    static void doTest(int window, bool winPercent, std::vector<std::string>& boundsStr,
                      std::vector<std::string>& datasets, ExperimentType experiment,
                      const std::string& outName, const std::string& datasetDirectory);

    static void doTT(BoundsID bound, int k, Dataset& train, Dataset& test,
                    int w, bool winPercent, std::ofstream& resultStream);

    static void runTestTightness(BoundsID bound, int k, int w, bool winPercent,
                                std::ofstream& resultStream, const std::string& datasetDirectory);

    static void runTestTightnessd(BoundsID bound, int k, int w, bool winPercent,
                                 const std::string& dataset, std::ofstream& resultStream,
                                 const std::string& datasetDirectory);

    static void doTestTightness(const std::string& baseDir, const std::string& outDir,
                               const std::string& suffix, std::vector<std::string>& datasets,
                               std::vector<BoundInfo>& bounds, int w, bool winPercent,
                               ExperimentType sort, const std::string& datasetDirectory);

    static void getStats(const std::string& datasetDirectory);
};

void TSTester::do1NNunsorted(BoundsID bound, int k, Dataset& train, Dataset& test,
                            int w, bool winPercent, std::ofstream& timesStream,
                            std::ofstream& timeStdStream, std::ofstream& accuracyStream,
                            std::ofstream& prunedStream, std::vector<double>& uetest,
                            std::vector<double>& letest, std::vector<double>& luetest,
                            std::vector<double>& uletest, Statistics& stats) {
    
    // DEBUG: Imprimir qué bound se está usando
    if (verbosity > 0) {
        std::string boundName = boundsIDToString(bound);
        if (bound == BoundsID::Enhanced || bound == BoundsID::EnhancedWebb) {
            boundName += "(" + std::to_string(k) + ")";
        }
        std::cout << "  Executing do1NNunsorted with bound: " << boundName << std::endl;
    }
    
    for (size_t i = 0; i < test.series.size(); i++) {
        ClassifiedSequence& testSeries = test.series[i];
        int l = testSeries.sequence.size();
        double nearestD = std::numeric_limits<double>::max();
        int nearestClass = 0;
        
        if (bound == BoundsID::Webb || bound == BoundsID::EnhancedWebb || bound == BoundsID::Petitjean) {
            if (uetest.empty()) {
                uetest.resize(l);
                letest.resize(l);
            } else if (uetest.size() < static_cast<size_t>(l)) {
                uetest.resize(l);
                letest.resize(l);
            }
            
            int thisW = winPercent ? l * w / 100 : w;
            if (debug)
                Bounds::simpleGetEnvelopes(testSeries.sequence, thisW, uetest, letest);
            else
                Bounds::lemireGetEnvelopes(testSeries.sequence, thisW, uetest, letest);
            
            if (bound == BoundsID::Webb || bound == BoundsID::EnhancedWebb) {
                if (luetest.empty()) {
                    luetest.resize(l);
                    uletest.resize(l);
                } else if (luetest.size() < static_cast<size_t>(l)) {
                    luetest.resize(l);
                    uletest.resize(l);
                }
                Bounds::lemireGetUpper(letest, thisW, uletest);
                Bounds::lemireGetLower(uetest, thisW, luetest);
            }
        }
        
        for (size_t j = 0; j < train.series.size(); j++) {
            double thisBound;
            int thisW = winPercent ? l * w / 100 : w;
            
            ClassifiedSequence& trainSeries = train.series[j];
            
            if (j == 0) {
                thisBound = 0;
            } else {
                switch (bound) {
                    case BoundsID::None:
                        thisBound = 0;
                        break;
                    
                    case BoundsID::Keogh:
                        thisBound = Bounds::lbKeogh(testSeries.sequence, trainSeries.upper,
                                                    trainSeries.lower, nearestD);
                        break;
                    
                    case BoundsID::Improved:
                        thisBound = Bounds::lbImproved(testSeries.sequence, trainSeries.sequence,
                                                       trainSeries.upper, trainSeries.lower,
                                                       thisW, nearestD);
                        break;
                    
                    case BoundsID::Enhanced:
                        thisBound = Bounds::lbEnhanced(testSeries.sequence, trainSeries.sequence,
                                                       trainSeries.upper, trainSeries.lower,
                                                       k, thisW, nearestD);
                        break;
                    
                    case BoundsID::Petitjean:
                        thisBound = Bounds::lbPetitjean(testSeries.sequence, uetest, letest,
                                                        trainSeries.sequence, trainSeries.upper,
                                                        trainSeries.lower, thisW, nearestD);
                        break;
                    
                    case BoundsID::Webb:
                        thisBound = Bounds::lbWebb(testSeries.sequence, uetest, letest,
                                                   luetest, uletest, trainSeries.sequence,
                                                   trainSeries.upper, trainSeries.lower,
                                                   trainSeries.lowerUpper, trainSeries.upperLower,
                                                   thisW, nearestD);
                        break;
                    
                    case BoundsID::EnhancedWebb:
                        thisBound = Bounds::enhancedLBWebb(testSeries.sequence, uetest, letest,
                                                           luetest, uletest, trainSeries.sequence,
                                                           trainSeries.upper, trainSeries.lower,
                                                           trainSeries.lowerUpper, trainSeries.upperLower,
                                                           k, thisW, nearestD);
                        break;
                    
                    default:
                        std::cerr << "Bound not handled!" << std::endl;
                        return;
                }
            }
            
            if (thisBound >= nearestD) {
                stats.pruned++;
            } else {
                double thisDist = DTW::distance(testSeries.sequence, trainSeries.sequence, thisW);
                
                if (thisDist < nearestD) {
                    nearestD = thisDist;
                    nearestClass = trainSeries.klass;
                }
            }
        }
        
        if (nearestClass == testSeries.klass) stats.correct++;
        else stats.errors++;
    }
}

void TSTester::do1NNsorted(BoundsID bound, int k, Dataset& train, Dataset& test,
                          int w, bool winPercent, std::ofstream& timesStream,
                          std::ofstream& timeStdStream, std::ofstream& accuracyStream,
                          std::ofstream& prunedStream, std::vector<double>& uetest,
                          std::vector<double>& letest, std::vector<double>& luetest,
                          std::vector<double>& uletest, Statistics& stats) {
    
    for (size_t i = 0; i < test.series.size(); i++) {
        ClassifiedSequence& testSeries = test.series[i];
        int l = testSeries.sequence.size();
        int thisW = winPercent ? l * w / 100 : w;
        int nearestClass = 0;
        
        if (bound == BoundsID::Petitjean || bound == BoundsID::Webb || bound == BoundsID::EnhancedWebb) {
            if (uetest.empty()) {
                uetest.resize(l);
                letest.resize(l);
            } else if (uetest.size() < static_cast<size_t>(l)) {
                uetest.resize(l);
                letest.resize(l);
            }
            
            if (debug)
                Bounds::simpleGetEnvelopes(testSeries.sequence, thisW, uetest, letest);
            else
                Bounds::lemireGetEnvelopes(testSeries.sequence, thisW, uetest, letest);
            
            if (bound == BoundsID::Webb || bound == BoundsID::EnhancedWebb) {
                if (luetest.empty()) {
                    luetest.resize(l);
                    uletest.resize(l);
                } else if (luetest.size() < static_cast<size_t>(l)) {
                    luetest.resize(l);
                    uletest.resize(l);
                }
                Bounds::lemireGetUpper(letest, thisW, uletest);
                Bounds::lemireGetLower(uetest, thisW, luetest);
            }
        }
        
        // Calcular bounds para todas las series de entrenamiento
        for (size_t j = 0; j < train.series.size(); j++) {
            ClassifiedSequence& trainSeries = train.series[j];
            double thisBound = 0.0;
            
            switch (bound) {
                case BoundsID::None:
                    thisBound = 0;
                    break;
                
                case BoundsID::Keogh:
                    thisBound = Bounds::lbKeogh(testSeries.sequence, trainSeries.upper,
                                                trainSeries.lower, std::numeric_limits<double>::max());
                    break;
                
                case BoundsID::Improved:
                    thisBound = Bounds::lbImproved(testSeries.sequence, trainSeries.sequence,
                                                   trainSeries.upper, trainSeries.lower,
                                                   thisW, std::numeric_limits<double>::max());
                    break;
                
                case BoundsID::Enhanced:
                    thisBound = Bounds::lbEnhanced(testSeries.sequence, trainSeries.sequence,
                                                   trainSeries.upper, trainSeries.lower,
                                                   k, thisW, std::numeric_limits<double>::max());
                    break;
                
                case BoundsID::Petitjean:
                    thisBound = Bounds::lbPetitjean(testSeries.sequence, uetest, letest,
                                                    trainSeries.sequence, trainSeries.upper,
                                                    trainSeries.lower, thisW,
                                                    std::numeric_limits<double>::max());
                    break;
                
                case BoundsID::Webb:
                    thisBound = Bounds::lbWebb(testSeries.sequence, uetest, letest,
                                               luetest, uletest, trainSeries.sequence,
                                               trainSeries.upper, trainSeries.lower,
                                               trainSeries.lowerUpper, trainSeries.upperLower,
                                               thisW, std::numeric_limits<double>::max());
                    break;
                
                case BoundsID::EnhancedWebb:
                    thisBound = Bounds::enhancedLBWebb(testSeries.sequence, uetest, letest,
                                                       luetest, uletest, trainSeries.sequence,
                                                       trainSeries.upper, trainSeries.lower,
                                                       trainSeries.lowerUpper, trainSeries.upperLower,
                                                       k, thisW, std::numeric_limits<double>::max());
                    break;
                
                default:
                    std::cerr << "Bound not handled!" << std::endl;
                    return;
            }
            
            trainSeries.lb = thisBound;
            
            if (debug && thisBound > DTW::distance(testSeries.sequence, trainSeries.sequence, thisW)) {
                std::cerr << "Lower bound exceeds true distance: " << thisBound << " - "
                         << DTW::distance(testSeries.sequence, trainSeries.sequence, thisW) << std::endl;
                testSeries.print(std::cerr);
                trainSeries.print(std::cerr);
                return;
            }
        }
        
        // Ordenar por lower bound
        std::sort(train.series.begin(), train.series.end());
        
        double nearestD = DTW::distance(testSeries.sequence, train.series[0].sequence, thisW);
        nearestClass = train.series[0].klass;
        
        for (size_t j = 1; j < train.series.size(); j++) {
            double thisBound = train.series[j].lb;
            
            if (thisBound >= nearestD) {
                stats.pruned += train.series.size() - j;
                break;
            } else {
                double thisDist = DTW::distance(testSeries.sequence, train.series[j].sequence, thisW);
                
                if (thisDist < nearestD) {
                    nearestD = thisDist;
                    nearestClass = train.series[j].klass;
                }
            }
        }
        
        if (nearestClass == testSeries.klass) stats.correct++;
        else stats.errors++;
    }
}

// FUNCIÓN CORREGIDA - Este es el cambio principal
void TSTester::doNN(BoundsID bound, int k, Dataset& train, Dataset& test,
                   int w, bool winPercent, ExperimentType sort,
                   std::ofstream& timesStream, std::ofstream& timeStdStream,
                   std::ofstream& accuracyStream, std::ofstream& prunedStream) {
    
    std::vector<double> uetest, letest, luetest, uletest;
    
    // Calcular envelopes para todas las series de entrenamiento
    if (bound != BoundsID::None) {
        if (verbosity > 0) {
            std::cout << "  Computing envelopes for " << train.series.size() 
                     << " training sequences" << std::endl;
        }
        
        for (size_t i = 0; i < train.series.size(); i++) {
            ClassifiedSequence& seq = train.series[i];
            int l = seq.sequence.size();
            int thisW = winPercent ? l * w / 100 : w;
            
            if (i == 0 && verbosity > 0) {
                std::cout << "  Sequence length: " << l << ", Window: " << thisW << std::endl;
            }
            
            seq.lower.resize(l);
            seq.upper.resize(l);
            
            if (debug)
                Bounds::simpleGetEnvelopes(seq.sequence, thisW, seq.upper, seq.lower);
            else
                Bounds::lemireGetEnvelopes(seq.sequence, thisW, seq.upper, seq.lower);
            
            if (bound == BoundsID::Webb || bound == BoundsID::EnhancedWebb) {
                seq.lowerUpper.resize(l);
                seq.upperLower.resize(l);
                Bounds::lemireGetUpper(seq.lower, thisW, seq.upperLower);
                Bounds::lemireGetLower(seq.upper, thisW, seq.lowerUpper);
            }
        }
    }
    
    double meanTime = 0;
    double varianceTime = 0;
    double meanAccuracy = 0;
    double meanPruned = 0;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Determinar número de iteraciones según el tipo de experimento
    int numIterations = (sort == ExperimentType::SORTED) ? 1 : 10;
    
    for (int t = 1; t <= numIterations; t++) {
        Statistics stats;
        stats.reset();
        
        switch (sort) {
            case ExperimentType::UNSORTED:
                do1NNunsorted(bound, k, train, test, w, winPercent, timesStream,
                            timeStdStream, accuracyStream, prunedStream,
                            uetest, letest, luetest, uletest, stats);
                break;
            
            case ExperimentType::SHUFFLED:
                train.shuffle(t);
                do1NNunsorted(bound, k, train, test, w, winPercent, timesStream,
                            timeStdStream, accuracyStream, prunedStream,
                            uetest, letest, luetest, uletest, stats);
                break;
            
            case ExperimentType::SORTED:
                do1NNsorted(bound, k, train, test, w, winPercent, timesStream,
                          timeStdStream, accuracyStream, prunedStream,
                          uetest, letest, luetest, uletest, stats);
                break;
            
            default:
                break;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double thisTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        double thisAccuracy = stats.correct / static_cast<double>(stats.correct + stats.errors);
        double thisPruned = static_cast<double>(stats.pruned);
        
        double oldMeanTime = meanTime;
        meanTime += (thisTime - meanTime) / t;
        varianceTime += (thisTime - meanTime) * (thisTime - oldMeanTime);
        
        meanAccuracy += (thisAccuracy - meanAccuracy) / t;
        meanPruned += (thisPruned - meanPruned) / t;
        
        startTime = endTime;
    }
    
    // Ajustar desviación estándar según número de iteraciones
    double stdDev = (numIterations > 1) ? std::sqrt(varianceTime / numIterations) : 0.0;
    
    timesStream << "," << meanTime;
    timeStdStream << "," << stdDev;
    accuracyStream << "," << meanAccuracy;
    prunedStream << "," << meanPruned;
}

// Resto de funciones sin cambios...
void TSTester::runNNd(BoundsID bound, int k, int w, bool winPercent,
                     const std::string& dataset, ExperimentType sort,
                     std::ofstream& timesStream, std::ofstream& timeStdStream,
                     std::ofstream& accuracyStream, std::ofstream& prunedStream,
                     const std::string& datasetDirectory) {
    
    FileIterator files(verbosity, datasetDirectory);
    
    std::string boundDesc = boundsIDToString(bound);
    if (bound == BoundsID::Enhanced || bound == BoundsID::EnhancedWebb) {
        boundDesc += "(" + std::to_string(k) + ")";
    }
    
    if (!files.getByName(dataset)) {
        std::cerr << "Cannot open dataset " << dataset << std::endl;
        return;
    }
    
    std::string trainFile = files.getTrain();
    std::string testFile = files.getTest();
    
    std::cout << "Processing: " << files.getName() << " with " << boundDesc << std::endl;
    
    Dataset train, test;
    train.load(trainFile);
    test.load(testFile);
    
    int thisW = w;
    if (w == -2) {
        thisW = UCRInfo::getWindow(files.getName());
    }
    
    doNN(bound, k, train, test, thisW, winPercent, sort, timesStream,
         timeStdStream, accuracyStream, prunedStream);
    
    timesStream.flush();
    timeStdStream.flush();
    accuracyStream.flush();
    prunedStream.flush();
}

void TSTester::runNN(BoundsID bound, int k, int w, bool winPercent,
                    ExperimentType sort, std::ofstream& timesStream,
                    std::ofstream& timeStdStream, std::ofstream& accuracyStream,
                    std::ofstream& prunedStream, const std::string& datasetDirectory) {
    
    FileIterator files(verbosity, datasetDirectory);
    
    std::string boundDesc = boundsIDToString(bound);
    if (bound == BoundsID::Enhanced || bound == BoundsID::EnhancedWebb) {
        boundDesc += "(" + std::to_string(k) + ")";
    }
    
    timesStream << boundDesc;
    timeStdStream << boundDesc;
    accuracyStream << boundDesc;
    prunedStream << boundDesc;
    
    files.getNext();
    if (w == -2) {
        while (!files.atEnd() && UCRInfo::getWindow(files.getName()) == 0) {
            files.getNext();
        }
    }
    
    std::string trainFile = files.getTrain();
    std::string testFile = files.getTest();
    
    while (!trainFile.empty()) {
        std::cout << "Processing: " << files.getName() << " with " << boundDesc << std::endl;
        
        Dataset train, test;
        train.load(trainFile);
        test.load(testFile);
        
        int thisW = w;
        if (w == -2) {
            thisW = UCRInfo::getWindow(files.getName());
        }
        
        doNN(bound, k, train, test, thisW, winPercent, sort, timesStream,
             timeStdStream, accuracyStream, prunedStream);
        
        files.getNext();
        if (w == -2) {
            while (!files.atEnd() && UCRInfo::getWindow(files.getName()) == 0) {
                files.getNext();
            }
        }
        trainFile = files.getTrain();
        testFile = files.getTest();
    }
    
    timesStream << "\n";
    timeStdStream << "\n";
    accuracyStream << "\n";
    prunedStream << "\n";
}

void TSTester::testNN(const std::string& baseDir, const std::string& outDir,
                     const std::string& suffix, std::vector<std::string>& datasets,
                     std::vector<BoundInfo>& bounds, int w, bool winPercent,
                     ExperimentType sort, const std::string& datasetDirectory) {
    
    std::string basePath = baseDir + OSUtils::directorySep() + outDir + OSUtils::directorySep();
    
    std::ofstream timesStream(basePath + "times-" + suffix);
    std::ofstream timeStdStream(basePath + "time-dev-" + suffix);
    std::ofstream accuracyStream(basePath + "accuracy-" + suffix);
    std::ofstream prunedStream(basePath + "pruned-" + suffix);
    
    if (!timesStream || !timeStdStream || !accuracyStream || !prunedStream) {
        std::cerr << "Could not open output files!" << std::endl;
        return;
    }
    
    if (datasets.empty()) {
        for (auto& bound : bounds) {
            runNN(bound.bound, bound.k, w, winPercent, sort, timesStream,
                  timeStdStream, accuracyStream, prunedStream, datasetDirectory);
        }
    } else {
        for (auto& d : datasets) {
            timesStream << d;
            timeStdStream << d;
            accuracyStream << d;
            prunedStream << d;
            
            for (auto& bound : bounds) {
                runNNd(bound.bound, bound.k, w, winPercent, d, sort, timesStream,
                       timeStdStream, accuracyStream, prunedStream, datasetDirectory);
            }
            
            timesStream << "\n";
            timeStdStream << "\n";
            accuracyStream << "\n";
            prunedStream << "\n";
        }
    }
    
    timesStream.close();
    timeStdStream.close();
    accuracyStream.close();
    prunedStream.close();
}

// [Continúa con el resto del código - doTT, runTestTightness, etc. sin cambios]
// Por brevedad no lo repito aquí, pero el resto del código permanece igual

void TSTester::doTest(int window, bool winPercent, std::vector<std::string>& boundsStr,
                     std::vector<std::string>& datasets, ExperimentType experiment,
                     const std::string& outName, const std::string& datasetDirectory) {
    
    std::vector<BoundInfo> bounds;
    std::time_t now = std::time(nullptr);
    std::tm* ltm = std::localtime(&now);
    
    std::ostringstream dateStream;
    dateStream << std::setfill('0') 
               << std::setw(2) << (ltm->tm_year % 100)
               << std::setw(2) << (ltm->tm_mon + 1)
               << std::setw(2) << ltm->tm_mday
               << std::setw(2) << ltm->tm_hour
               << std::setw(2) << ltm->tm_min
               << std::setw(2) << ltm->tm_sec;
    
    std::string date = dateStream.str();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 999999);
    
    std::string fullDate = date + "-" + std::to_string(dis(gen));
    
    // Crear directorio base si no existe
    std::filesystem::create_directories(outName);
    
    std::string outDir = outName + OSUtils::directorySep() + fullDate;
    
    // Crear directorio de salida
    while (std::filesystem::exists(outDir)) {
        fullDate = date + "-" + std::to_string(dis(gen));
        outDir = outName + OSUtils::directorySep() + fullDate;
    }
    
    std::filesystem::create_directories(outDir);
    std::cout << "Output directory: " << fullDate << std::endl;
    
    int startW, endW;
    
    if (window == -2) {
        startW = endW = -2;
    } else if (window < 0) {
        startW = 1;
        endW = 10;
    } else {
        startW = endW = window;
    }
    
    // Parsear los bounds
    for (auto& b : boundsStr) {
        int k = 0;
        BoundsID bID = BoundsID::None;
        std::string bStr = b;
        
        // Convertir a minúsculas para comparación
        std::transform(bStr.begin(), bStr.end(), bStr.begin(), ::tolower);
        
        if (bStr.find("enhancedwebb") == 0) {
            bStr = bStr.substr(12);
            
            while (!bStr.empty() && std::isdigit(bStr[0])) {
                k = k * 10 + (bStr[0] - '0');
                bStr = bStr.substr(1);
            }
            
            bID = BoundsID::EnhancedWebb;
        } else if (bStr.find("enhanced") == 0) {
            bStr = bStr.substr(8);
            
            while (!bStr.empty() && std::isdigit(bStr[0])) {
                k = k * 10 + (bStr[0] - '0');
                bStr = bStr.substr(1);
            }
            
            bID = BoundsID::Enhanced;
        } else {
            bID = stringToBoundsID(b);
        }
        
        bounds.emplace_back(bID, k);
    }
    
    for (int w = startW; w <= endW; w++) {
        std::string suffix;
        
        if (w == -2) {
            suffix = "W";
        } else {
            suffix = winPercent ? "g" : "w";
            suffix += std::to_string(w);
        }
        
        if (experiment == ExperimentType::SORTED) {
            suffix += "-sort";
        } else if (experiment == ExperimentType::UNSORTED) {
            suffix += "-nosort";
        }
        
        suffix += ".csv";
        
        if (experiment == ExperimentType::TEST_TIGHTNESS) {
            doTestTightness(outName, fullDate, suffix, datasets, bounds, w, winPercent,
                          experiment, datasetDirectory);
        } else {
            testNN(outName, fullDate, suffix, datasets, bounds, w, winPercent,
                  experiment, datasetDirectory);
        }
    }
}

void TSTester::doTT(BoundsID bound, int k, Dataset& train, Dataset& test,
                   int w, bool winPercent, std::ofstream& resultStream) {
    
    std::vector<double> uetest, letest, luetest, uletest;
    double avTightness = 0.0;
    long count = 0;
    
    if (bound != BoundsID::None) {
        for (size_t i = 0; i < train.series.size(); i++) {
            ClassifiedSequence& seq = train.series[i];
            int l = seq.sequence.size();
            int thisW = winPercent ? l * w / 100 : w;
            
            seq.lower.resize(l);
            seq.upper.resize(l);
            
            if (debug)
                Bounds::simpleGetEnvelopes(seq.sequence, thisW, seq.upper, seq.lower);
            else
                Bounds::lemireGetEnvelopes(seq.sequence, thisW, seq.upper, seq.lower);
            
            if (bound == BoundsID::Webb || bound == BoundsID::EnhancedWebb) {
                seq.lowerUpper.resize(l);
                seq.upperLower.resize(l);
                Bounds::lemireGetUpper(seq.lower, thisW, seq.upperLower);
                Bounds::lemireGetLower(seq.upper, thisW, seq.lowerUpper);
            }
        }
    }
    
    for (size_t i = 0; i < test.series.size(); i++) {
        ClassifiedSequence& testSeries = test.series[i];
        int l = testSeries.sequence.size();
        
        if (bound == BoundsID::Webb || bound == BoundsID::Petitjean || bound == BoundsID::EnhancedWebb) {
            if (uetest.empty()) {
                uetest.resize(l);
                letest.resize(l);
            } else if (uetest.size() < static_cast<size_t>(l)) {
                uetest.resize(l);
                letest.resize(l);
            }
            
            int thisW = winPercent ? l * w / 100 : w;
            
            if (debug)
                Bounds::simpleGetEnvelopes(testSeries.sequence, thisW, uetest, letest);
            else
                Bounds::lemireGetEnvelopes(testSeries.sequence, thisW, uetest, letest);
            
            if (bound == BoundsID::Webb || bound == BoundsID::EnhancedWebb) {
                if (luetest.empty()) {
                    luetest.resize(l);
                    uletest.resize(l);
                } else if (luetest.size() < static_cast<size_t>(l)) {
                    luetest.resize(l);
                    uletest.resize(l);
                }
                Bounds::lemireGetUpper(letest, thisW, uletest);
                Bounds::lemireGetLower(uetest, thisW, luetest);
            }
        }
        
        for (size_t j = 0; j < train.series.size(); j++) {
            double thisBound;
            int thisW = winPercent ? l * w / 100 : w;
            
            ClassifiedSequence& trainSeries = train.series[j];
            
            switch (bound) {
                case BoundsID::None:
                    thisBound = 0;
                    break;
                
                case BoundsID::Keogh:
                    thisBound = Bounds::lbKeogh(testSeries.sequence, trainSeries.upper,
                                                trainSeries.lower, std::numeric_limits<double>::max());
                    break;
                
                case BoundsID::Improved:
                    thisBound = Bounds::lbImproved(testSeries.sequence, trainSeries.sequence,
                                                   trainSeries.upper, trainSeries.lower,
                                                   thisW, std::numeric_limits<double>::max());
                    break;
                
                case BoundsID::Enhanced:
                    thisBound = Bounds::lbEnhanced(testSeries.sequence, trainSeries.sequence,
                                                   trainSeries.upper, trainSeries.lower,
                                                   k, thisW, std::numeric_limits<double>::max());
                    break;
                
                case BoundsID::Petitjean:
                    thisBound = Bounds::lbPetitjean(testSeries.sequence, uetest, letest,
                                                    trainSeries.sequence, trainSeries.upper,
                                                    trainSeries.lower, thisW,
                                                    std::numeric_limits<double>::max());
                    break;
                
                case BoundsID::Webb:
                    thisBound = Bounds::lbWebb(testSeries.sequence, uetest, letest,
                                               luetest, uletest, trainSeries.sequence,
                                               trainSeries.upper, trainSeries.lower,
                                               trainSeries.lowerUpper, trainSeries.upperLower,
                                               thisW, std::numeric_limits<double>::max());
                    break;
                
                case BoundsID::EnhancedWebb:
                    thisBound = Bounds::enhancedLBWebb(testSeries.sequence, uetest, letest,
                                                       luetest, uletest, trainSeries.sequence,
                                                       trainSeries.upper, trainSeries.lower,
                                                       trainSeries.lowerUpper, trainSeries.upperLower,
                                                       k, thisW, std::numeric_limits<double>::max());
                    break;
                
                default:
                    std::cerr << "Bound not handled!" << std::endl;
                    return;
            }
            
            double thisDist = DTW::distance(testSeries.sequence, trainSeries.sequence, thisW);
            
            if (thisBound > thisDist + delta) {
                std::string boundDesc = boundsIDToString(bound);
                if (bound == BoundsID::Enhanced || bound == BoundsID::EnhancedWebb) {
                    boundDesc += "(" + std::to_string(k) + ")";
                }
                
                std::cerr << "Lower bound " << thisBound << " exceeds distance " << thisDist
                         << " for bound " << boundDesc << "!" << std::endl;
                trainSeries.print(std::cerr);
                testSeries.print(std::cerr);
                return;
            }
            
            if (thisDist > 0.0) {
                double thisTightness = thisBound / thisDist;
                avTightness += (thisTightness - avTightness) / ++count;
            }
        }
    }
    
    resultStream << "," << avTightness;
}

void TSTester::runTestTightnessd(BoundsID bound, int k, int w, bool winPercent,
                                const std::string& dataset, std::ofstream& resultStream,
                                const std::string& datasetDirectory) {
    
    FileIterator files(verbosity, datasetDirectory);
    
    std::string boundDesc = boundsIDToString(bound);
    if (bound == BoundsID::Enhanced || bound == BoundsID::EnhancedWebb) {
        boundDesc += "(" + std::to_string(k) + ")";
    }
    
    if (!files.getByName(dataset)) {
        std::cerr << "Cannot open dataset " << dataset << std::endl;
        return;
    }
    
    std::string trainFile = files.getTrain();
    std::string testFile = files.getTest();
    
    std::cout << "Processing: " << files.getName() << " with " << boundDesc << std::endl;
    
    Dataset train, test;
    train.load(trainFile);
    test.load(testFile);
    
    int thisW = w;
    if (w == -2) {
        thisW = UCRInfo::getWindow(files.getName());
    }
    
    doTT(bound, k, train, test, thisW, winPercent, resultStream);
    resultStream.flush();
}

void TSTester::runTestTightness(BoundsID bound, int k, int w, bool winPercent,
                               std::ofstream& resultStream, const std::string& datasetDirectory) {
    
    FileIterator files(verbosity, datasetDirectory);
    
    std::string boundDesc = boundsIDToString(bound);
    if (bound == BoundsID::Enhanced || bound == BoundsID::EnhancedWebb) {
        boundDesc += "(" + std::to_string(k) + ")";
    }
    
    resultStream << boundDesc;
    
    files.getNext();
    if (w == -2) {
        while (!files.atEnd() && UCRInfo::getWindow(files.getName()) == 0) {
            files.getNext();
        }
    }
    
    std::string trainFile = files.getTrain();
    std::string testFile = files.getTest();
    
    while (!trainFile.empty()) {
        std::cout << "Processing: " << files.getName() << " with " << boundDesc << std::endl;
        
        Dataset train, test;
        train.load(trainFile);
        test.load(testFile);
        
        int thisW = w;
        if (w == -2) {
            thisW = UCRInfo::getWindow(files.getName());
        }
        
        doTT(bound, k, train, test, thisW, winPercent, resultStream);
        
        files.getNext();
        if (w == -2) {
            while (!files.atEnd() && UCRInfo::getWindow(files.getName()) == 0) {
                files.getNext();
            }
        }
        trainFile = files.getTrain();
        testFile = files.getTest();
    }
    
    resultStream << "\n";
}

void TSTester::doTestTightness(const std::string& baseDir, const std::string& outDir,
                              const std::string& suffix, std::vector<std::string>& datasets,
                              std::vector<BoundInfo>& bounds, int w, bool winPercent,
                              ExperimentType sort, const std::string& datasetDirectory) {
    
    std::string basePath = baseDir + OSUtils::directorySep() + outDir + OSUtils::directorySep();
    std::ofstream resultStream(basePath + "tightness-" + suffix);
    
    if (!resultStream) {
        std::cerr << "Could not open output files!" << std::endl;
        return;
    }
    
    if (datasets.empty()) {
        for (auto& bound : bounds) {
            runTestTightness(bound.bound, bound.k, w, winPercent, resultStream, datasetDirectory);
        }
    } else {
        for (auto& d : datasets) {
            resultStream << d;
            
            for (auto& bound : bounds) {
                runTestTightnessd(bound.bound, bound.k, w, winPercent, d, resultStream, datasetDirectory);
            }
            
            resultStream << "\n";
        }
    }
    
    resultStream.close();
}

void TSTester::getStats(const std::string& datasetDirectory) {
    std::time_t now = std::time(nullptr);
    std::tm* ltm = std::localtime(&now);
    
    std::ostringstream dateStream;
    dateStream << std::setfill('0') 
               << std::setw(2) << (ltm->tm_year % 100)
               << std::setw(2) << (ltm->tm_mon + 1)
               << std::setw(2) << ltm->tm_mday
               << std::setw(2) << ltm->tm_hour
               << std::setw(2) << ltm->tm_min
               << std::setw(2) << ltm->tm_sec;
    
    std::string date = dateStream.str();
    
    while (std::filesystem::exists(date)) {
        date += "0";
    }
    
    std::filesystem::create_directories(date);
    
    std::ofstream statsStream(date + OSUtils::directorySep() + "stats.csv");
    if (!statsStream) {
        std::cerr << "Failed to create stats file!" << std::endl;
        return;
    }
    
    FileIterator files(verbosity, datasetDirectory);
    
    files.getNext();
    while (!files.atEnd()) {
        statsStream << "," << files.getName();
        files.getNext();
    }
    statsStream << "\n";
    
    files.reset();
    files.getNext();
    while (!files.atEnd()) {
        Dataset train;
        train.load(files.getTrain());
        statsStream << "," << train.series.size();
        files.getNext();
    }
    statsStream << "\n";
    
    files.reset();
    files.getNext();
    while (!files.atEnd()) {
        Dataset test;
        test.load(files.getTest());
        statsStream << "," << test.series.size();
        files.getNext();
    }
    statsStream << "\n";
    
    statsStream.close();
    std::cout << "Stats saved to: " << date << "/stats.csv" << std::endl;
}

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        token.erase(0, token.find_first_not_of(" \t\n\r\f\v"));
        token.erase(token.find_last_not_of(" \t\n\r\f\v") + 1);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

int main(int argc, char* argv[]) {
    int window = -1;
    ExperimentType experiment = ExperimentType::UNSORTED;
    bool winPercent = false;
    std::vector<std::string> bounds = {"none", "keogh", "improved", "enhanced5", "petitjean", "webb"};
    std::vector<std::string> datasets;
    std::string outName = ".";
    std::string datasetDirectory = "UCR_TS_Archive_2015";
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg[0] == '-' && arg.length() > 1) {
            switch (arg[1]) {
                case 'b':
                    if (arg.length() > 2) {
                        bounds = split(arg.substr(2), ',');
                    }
                    std::cout << "Using bounds:";
                    for (const auto& b : bounds) std::cout << " " << b;
                    std::cout << std::endl;
                    break;
                
                case 'd':
                    if (arg.length() > 2) {
                        datasets = split(arg.substr(2), ',');
                    }
                    std::cout << "Using datasets:";
                    for (const auto& d : datasets) std::cout << " " << d;
                    std::cout << std::endl;
                    break;
                
                case 'D':
                    if (arg.length() > 2) {
                        datasetDirectory = arg.substr(2);
                    } else if (i + 1 < argc) {
                        datasetDirectory = argv[++i];
                    } else {
                        std::cerr << "Directory name not specified!" << std::endl;
                        return 1;
                    }
                    std::cout << "Dataset directory: " << datasetDirectory << std::endl;
                    break;
                
                case 'g':
                    winPercent = true;
                    if (arg.length() > 2) {
                        window = std::stoi(arg.substr(2));
                    } else if (i + 1 < argc) {
                        window = std::stoi(argv[++i]);
                    } else {
                        std::cerr << "Window percentage not specified!" << std::endl;
                        return 1;
                    }
                    std::cout << "Window percentage: " << window << "%" << std::endl;
                    break;
                
                case 'n':
                    if (arg.length() > 2) {
                        outName = arg.substr(2);
                    } else if (i + 1 < argc) {
                        outName = argv[++i];
                    } else {
                        std::cerr << "Output name not specified!" << std::endl;
                        return 1;
                    }
                    std::cout << "Output directory name: " << outName << std::endl;
                    break;
                
                case 'o':
                    experiment = ExperimentType::SHUFFLED;
                    std::cout << "Experiment type: SHUFFLED" << std::endl;
                    break;
                
                case 's':
                    experiment = ExperimentType::SORTED;
                    std::cout << "Experiment type: SORTED" << std::endl;
                    break;
                
                case 't':
                    experiment = ExperimentType::TEST_TIGHTNESS;
                    std::cout << "Experiment type: TEST_TIGHTNESS" << std::endl;
                    break;
                
                case 'w':
                    if (arg.length() > 2) {
                        window = std::stoi(arg.substr(2));
                    } else if (i + 1 < argc) {
                        window = std::stoi(argv[++i]);
                    } else {
                        std::cerr << "Window size not specified!" << std::endl;
                        return 1;
                    }
                    std::cout << "Window size: " << window << std::endl;
                    break;
                
                case 'W':
                    window = -2;
                    std::cout << "Using optimal window for each dataset" << std::endl;
                    break;
                
                case 'x':
                    TSTester::getStats(datasetDirectory);
                    return 0;
                
                default:
                    std::cerr << "Unknown option: " << arg << std::endl;
                    return 1;
            }
        }
    }
    
    std::cout << "\nStarting DTW Bounds Testing..." << std::endl;
    std::cout << "========================================" << std::endl;
    
    TSTester::doTest(window, winPercent, bounds, datasets, experiment, outName, datasetDirectory);
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "Testing completed successfully!" << std::endl;
    
    return 0;
}