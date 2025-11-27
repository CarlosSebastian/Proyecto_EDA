#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00  // Windows 10
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "httplib.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <chrono>
#include <thread>
#include <process.h>

namespace fs = std::filesystem;

// Función para obtener ruta absoluta relativa al ejecutable
std::string getExecutablePath() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string path(buffer);
    std::string::size_type lastSlash = path.find_last_of("\\/");
    if (lastSlash != std::string::npos) {
        path = path.substr(0, lastSlash);
    }
    // Subir un nivel (backend -> raíz)
    std::string::size_type parentSlash = path.find_last_of("\\/");
    if (parentSlash != std::string::npos) {
        path = path.substr(0, parentSlash);
    }
    return path;
}

// Rutas de configuración (se calculan dinámicamente)
std::string getTSTesterPath() {
    std::string base = getExecutablePath();
    return base + "\\src\\bin\\TSTester.exe";
}

std::string getDatasetDir() {
    std::string base = getExecutablePath();
    return base + "\\src\\UCR_TS_Archive_2015";
}

std::string getResultsDir() {
    std::string base = getExecutablePath();
    return base + "\\src\\resultados_api";
}

// Estructura temporal para resultados
struct ExperimentResult {
    std::string dataset;
    std::map<std::string, std::map<std::string, double>> bounds;
    int window;
    std::string experimentType;
};

// Función para ejecutar comando y capturar output
std::string executeCommand(const std::string& command) {
    std::string result;
    std::cout << "Ejecutando comando: " << command << "\n";
    
    // En Windows, _popen necesita que el comando esté en un formato específico
    // Usar cmd /c para ejecutar el comando correctamente
    std::string fullCommand = "cmd /c \"" + command + "\"";
    std::cout << "Comando completo: " << fullCommand << "\n";
    
    FILE* pipe = _popen(fullCommand.c_str(), "r");
    if (!pipe) {
        std::cerr << "ERROR: No se pudo abrir el pipe para ejecutar el comando\n";
        return "Error ejecutando comando";
    }
    
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    
    int exitCode = _pclose(pipe);
    std::cout << "Código de salida: " << exitCode << "\n";
    std::cout << "Output capturado: " << (result.empty() ? "(vacío)" : result.substr(0, 200)) << "\n";
    
    return result;
}

// Función para construir comando de TSTester
std::string buildTSTesterCommand(
    const std::vector<std::string>& bounds,
    const std::vector<std::string>& datasets,
    const std::string& windowType,
    int windowValue,
    const std::vector<int>& windowPercentages,
    const std::string& experimentType,
    const std::string& outputDir
) {
    std::ostringstream cmd;
    // Envolver rutas con espacios en comillas
    std::string testerPath = getTSTesterPath();
    if (testerPath.find(" ") != std::string::npos) {
        cmd << "\"" << testerPath << "\"";
    } else {
        cmd << testerPath;
    }
    
    // Bounds
    cmd << " -b";
    for (size_t i = 0; i < bounds.size(); i++) {
        if (i > 0) cmd << ",";
        cmd << bounds[i];
    }
    
    // Window
    if (windowType == "optimal") {
        cmd << " -W";
    } else if (windowType == "fixed") {
        cmd << " -w" << windowValue;
    } else if (windowType == "percentage") {
        if (windowPercentages.size() > 0) {
            cmd << " -g" << windowPercentages[0];
        }
    }
    
    // Datasets
    cmd << " -d";
    for (size_t i = 0; i < datasets.size(); i++) {
        if (i > 0) cmd << ",";
        cmd << datasets[i];
    }
    
    // Experiment type
    if (experimentType == "tightness") {
        cmd << " -t";
    } else if (experimentType == "nnSorted") {
        cmd << " -s";
    }
    // nnUnsorted es el default, no necesita flag
    
    // Output directory (con comillas si tiene espacios)
    std::string datasetDir = getDatasetDir();
    cmd << " -D ";
    if (datasetDir.find(" ") != std::string::npos) {
        cmd << "\"" << datasetDir << "\"";
    } else {
        cmd << datasetDir;
    }
    
    cmd << " -n ";
    if (outputDir.find(" ") != std::string::npos) {
        cmd << "\"" << outputDir << "\"";
    } else {
        cmd << outputDir;
    }
    
    return cmd.str();
}

// Función para parsear CSV simple
std::vector<std::vector<std::string>> parseCSV(const std::string& filepath) {
    std::vector<std::vector<std::string>> result;
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        return result;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;
        
        while (std::getline(ss, cell, ',')) {
            // Eliminar espacios
            cell.erase(0, cell.find_first_not_of(" \t"));
            cell.erase(cell.find_last_not_of(" \t") + 1);
            row.push_back(cell);
        }
        
        if (!row.empty()) {
            result.push_back(row);
        }
    }
    
    file.close();
    return result;
}

// Función para convertir resultados a JSON
std::string resultsToJSON(const std::vector<std::vector<std::vector<std::string>>>& csvData,
                          const std::vector<std::string>& datasets,
                          const std::vector<std::string>& bounds,
                          const std::string& experimentType,
                          int window) {
    std::ostringstream json;
    json << "{\n";
    json << "  \"experimentId\": \"" << std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() << "\",\n";
    json << "  \"status\": \"completed\",\n";
    json << "  \"results\": [\n";
    
    for (size_t d = 0; d < datasets.size(); d++) {
        if (d > 0) json << ",\n";
        json << "    {\n";
        json << "      \"dataset\": \"" << datasets[d] << "\",\n";
        json << "      \"bounds\": {\n";
        
        // Procesar cada CSV (times, pruned, accuracy, tightness, timeDev)
        // CSV 0: times, CSV 1: pruned, CSV 2: accuracy, CSV 3: tightness (opcional), CSV 4: timeDev
        
        for (size_t b = 0; b < bounds.size(); b++) {
            if (b > 0) json << ",\n";
            json << "        \"" << bounds[b] << "\": {\n";
            
            // Buscar la fila del dataset en cada CSV
            double time = 0.0, timeStdDev = 0.0, pruned = 0.0, accuracy = 0.0;
            double tightness = 0.0;
            bool hasTightness = false;
            
            // Times
            if (csvData.size() > 0 && d < csvData[0].size()) {
                const auto& row = csvData[0][d];
                if (b + 1 < row.size()) {
                    time = std::stod(row[b + 1]);
                }
            }
            
            // Time Dev
            if (csvData.size() > 4 && d < csvData[4].size()) {
                const auto& row = csvData[4][d];
                if (b + 1 < row.size()) {
                    timeStdDev = std::stod(row[b + 1]);
                }
            }
            
            // Pruned
            if (csvData.size() > 1 && d < csvData[1].size()) {
                const auto& row = csvData[1][d];
                if (b + 1 < row.size()) {
                    pruned = std::stod(row[b + 1]);
                }
            }
            
            // Accuracy
            if (csvData.size() > 2 && d < csvData[2].size()) {
                const auto& row = csvData[2][d];
                if (b + 1 < row.size()) {
                    accuracy = std::stod(row[b + 1]);
                }
            }
            
            // Tightness (opcional)
            if (csvData.size() > 3 && d < csvData[3].size()) {
                const auto& row = csvData[3][d];
                if (b + 1 < row.size()) {
                    tightness = std::stod(row[b + 1]);
                    hasTightness = true;
                }
            }
            
            json << "          \"time\": " << time << ",\n";
            json << "          \"timeStdDev\": " << timeStdDev << ",\n";
            json << "          \"pruned\": " << pruned << ",\n";
            json << "          \"accuracy\": " << accuracy;
            
            if (hasTightness) {
                json << ",\n          \"tightness\": " << tightness;
            }
            
            json << "\n        }";
        }
        
        json << "\n      },\n";
        json << "      \"window\": " << window << ",\n";
        json << "      \"experimentType\": \"" << experimentType << "\",\n";
        json << "      \"timestamp\": \"" << std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count() << "\"\n";
        json << "    }";
    }
    
    json << "\n  ]\n";
    json << "}";
    
    return json.str();
}

// Función para encontrar el directorio de resultados más reciente
std::string findLatestResultsDir(const std::string& baseDir) {
    if (!fs::exists(baseDir)) {
        std::cerr << "Directorio base no existe: " << baseDir << "\n";
        return "";
    }
    
    std::string latest;
    auto latestTime = std::chrono::system_clock::time_point::min();
    int dirCount = 0;
    
    for (const auto& entry : fs::directory_iterator(baseDir)) {
        if (entry.is_directory()) {
            dirCount++;
            auto writeTime = fs::last_write_time(entry.path());
            auto timePoint = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                writeTime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
            
            if (timePoint > latestTime) {
                latestTime = timePoint;
                latest = entry.path().filename().string();
            }
        }
    }
    
    std::cout << "Directorios encontrados en " << baseDir << ": " << dirCount << "\n";
    std::cout << "Directorio más reciente: " << (latest.empty() ? "(ninguno)" : latest) << "\n";
    
    return latest;
}

int main() {
    httplib::Server svr;
    
    // Habilitar CORS
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });
    
    // Manejar OPTIONS para CORS
    svr.Options(".*", [](const httplib::Request&, httplib::Response& res) {
        return;
    });
    
    // Función auxiliar para extraer array de strings del JSON
    auto extractStringArray = [](const std::string& json, const std::string& key) -> std::vector<std::string> {
        std::vector<std::string> result;
        std::string searchKey = "\"" + key + "\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return result;
        
        pos = json.find("[", pos);
        if (pos == std::string::npos) return result;
        
        size_t end = json.find("]", pos);
        if (end == std::string::npos) return result;
        
        std::string arrayContent = json.substr(pos + 1, end - pos - 1);
        std::stringstream ss(arrayContent);
        std::string item;
        
        while (std::getline(ss, item, ',')) {
            // Limpiar comillas y espacios
            item.erase(0, item.find_first_not_of(" \t\""));
            item.erase(item.find_last_not_of(" \t\"") + 1);
            if (!item.empty()) {
                result.push_back(item);
            }
        }
        
        return result;
    };
    
    // Función auxiliar para extraer booleano del JSON
    auto extractBool = [](const std::string& json, const std::string& key) -> bool {
        std::string searchKey = "\"" + key + "\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return false;
        
        pos = json.find(":", pos);
        if (pos == std::string::npos) return false;
        
        pos = json.find_first_not_of(" \t", pos + 1);
        if (pos == std::string::npos) return false;
        
        return json.substr(pos, 4) == "true";
    };
    
    // Función auxiliar para extraer string del JSON
    auto extractString = [](const std::string& json, const std::string& key) -> std::string {
        std::string searchKey = "\"" + key + "\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return "";
        
        pos = json.find(":", pos);
        if (pos == std::string::npos) return "";
        
        pos = json.find("\"", pos);
        if (pos == std::string::npos) return "";
        pos++;
        
        size_t end = json.find("\"", pos);
        if (end == std::string::npos) return "";
        
        return json.substr(pos, end - pos);
    };
    
    // Endpoint: Ejecutar experimento
    svr.Post("/api/experiment/run", [&](const httplib::Request& req, httplib::Response& res) {
        std::cout << "Recibida petición de experimento\n";
        
        try {
            std::string body = req.body;
            std::cout << "Body recibido: " << body << "\n";
            std::cout << "Body length: " << body.length() << "\n";
            
            // Extraer configuración del JSON (formato anidado del frontend)
            std::vector<std::string> bounds = extractStringArray(body, "bounds");
            std::vector<std::string> datasets = extractStringArray(body, "datasets");
            
            std::cout << "Bounds extraídos: ";
            for (const auto& b : bounds) std::cout << b << " ";
            std::cout << "\n";
            std::cout << "Datasets extraídos: ";
            for (const auto& d : datasets) std::cout << d << " ";
            std::cout << "\n";
            
            // Buscar experimentTypes (anidado)
            bool tightness = false, nnUnsorted = false, nnSorted = false;
            size_t expTypesPos = body.find("\"experimentTypes\"");
            if (expTypesPos != std::string::npos) {
                std::cout << "Encontrado experimentTypes en posición: " << expTypesPos << "\n";
                size_t expTypesEnd = body.find("}", expTypesPos);
                if (expTypesEnd != std::string::npos) {
                    std::string expTypesSection = body.substr(expTypesPos, expTypesEnd - expTypesPos);
                    tightness = extractBool(expTypesSection, "tightness");
                    nnUnsorted = extractBool(expTypesSection, "nnUnsorted");
                    nnSorted = extractBool(expTypesSection, "nnSorted");
                    std::cout << "Tightness: " << tightness << ", nnUnsorted: " << nnUnsorted << ", nnSorted: " << nnSorted << "\n";
                }
            } else {
                std::cout << "experimentTypes no encontrado, buscando directamente\n";
                // Fallback: buscar directamente (formato plano)
                tightness = extractBool(body, "tightness");
                nnUnsorted = extractBool(body, "nnUnsorted");
                nnSorted = extractBool(body, "nnSorted");
            }
            
            // Buscar windowConfig (anidado)
            std::string windowType = "optimal";
            int windowValue = 0;
            std::vector<int> windowPercentages;
            size_t windowConfigPos = body.find("\"windowConfig\"");
            if (windowConfigPos != std::string::npos) {
                std::cout << "Encontrado windowConfig en posición: " << windowConfigPos << "\n";
                size_t windowConfigEnd = body.find("}", windowConfigPos);
                if (windowConfigEnd != std::string::npos) {
                    std::string windowConfigSection = body.substr(windowConfigPos, windowConfigEnd - windowConfigPos);
                    windowType = extractString(windowConfigSection, "type");
                    if (windowType.empty()) windowType = "optimal";
                    std::cout << "Window type: " << windowType << "\n";
                    
                    // Extraer windowValue si existe
                    size_t valuePos = windowConfigSection.find("\"value\"");
                    if (valuePos != std::string::npos) {
                        valuePos = windowConfigSection.find(":", valuePos);
                        if (valuePos != std::string::npos) {
                            size_t numStart = windowConfigSection.find_first_of("0123456789", valuePos);
                            if (numStart != std::string::npos) {
                                size_t numEnd = windowConfigSection.find_first_not_of("0123456789", numStart);
                                if (numEnd != std::string::npos) {
                                    windowValue = std::stoi(windowConfigSection.substr(numStart, numEnd - numStart));
                                }
                            }
                        }
                    }
                }
            } else {
                std::cout << "windowConfig no encontrado, usando optimal por defecto\n";
                windowType = "optimal";
            }
            
            // Extraer windowValue si existe
            size_t valuePos = body.find("\"value\"");
            if (valuePos != std::string::npos) {
                valuePos = body.find(":", valuePos);
                if (valuePos != std::string::npos) {
                    size_t numStart = body.find_first_of("0123456789", valuePos);
                    if (numStart != std::string::npos) {
                        size_t numEnd = body.find_first_not_of("0123456789", numStart);
                        windowValue = std::stoi(body.substr(numStart, numEnd - numStart));
                    }
                }
            }
            
            // Validar
            if (bounds.empty() || datasets.empty()) {
                res.set_content("{\"error\": \"Bounds y datasets son requeridos\"}", "application/json");
                res.status = 400;
                return;
            }
            
            if (!tightness && !nnUnsorted && !nnSorted) {
                res.set_content("{\"error\": \"Al menos un tipo de experimento debe estar activo\"}", "application/json");
                res.status = 400;
                return;
            }
            
            // Crear directorio de resultados
            std::string resultsDir = getResultsDir();
            if (!fs::exists(resultsDir)) {
                fs::create_directories(resultsDir);
            }
            
            std::string outputDir = resultsDir;
            
            // Ejecutar experimentos y recopilar resultados
            std::vector<std::vector<std::vector<std::string>>> allCSVData;
            std::string experimentType = "nnUnsorted";
            int window = -2; // Óptima por defecto
            
            if (nnUnsorted) {
                std::cout << "Ejecutando 1-NN UNSORTED...\n";
                std::string cmd = buildTSTesterCommand(bounds, datasets, windowType, windowValue,
                                                       windowPercentages, "nnUnsorted", outputDir);
                std::cout << "Comando: " << cmd << "\n";
                std::string output = executeCommand(cmd);
                std::cout << "Output: " << output << "\n";
                
                // Esperar a que se generen los archivos (aumentar tiempo para datasets grandes)
                std::this_thread::sleep_for(std::chrono::seconds(5));
                
                // Encontrar directorio más reciente
                std::string latestDir = findLatestResultsDir(outputDir);
                std::cout << "Directorio más reciente encontrado: " << (latestDir.empty() ? "(ninguno)" : latestDir) << "\n";
                
                if (!latestDir.empty()) {
                    std::string basePath = outputDir + "\\" + latestDir + "\\";
                    std::cout << "Buscando CSVs en: " << basePath << "\n";
                    
                    // Leer CSVs
                    std::vector<std::vector<std::string>> timesCSV = parseCSV(basePath + "times-W-nosort.csv");
                    std::vector<std::vector<std::string>> prunedCSV = parseCSV(basePath + "pruned-W-nosort.csv");
                    std::vector<std::vector<std::string>> accuracyCSV = parseCSV(basePath + "accuracy-W-nosort.csv");
                    std::vector<std::vector<std::string>> timeDevCSV = parseCSV(basePath + "time-dev-W-nosort.csv");
                    std::vector<std::vector<std::string>> tightnessCSV = parseCSV(basePath + "tightness-W.csv");
                    
                    std::cout << "CSVs leídos - times: " << timesCSV.size() << " filas, pruned: " << prunedCSV.size() 
                              << " filas, accuracy: " << accuracyCSV.size() << " filas\n";
                    
                    if (timesCSV.empty() && prunedCSV.empty() && accuracyCSV.empty()) {
                        std::cerr << "ADVERTENCIA: Todos los CSVs están vacíos. TSTester.exe puede no haber generado resultados.\n";
                    }
                    
                    allCSVData.push_back(timesCSV);
                    allCSVData.push_back(prunedCSV);
                    allCSVData.push_back(accuracyCSV);
                    allCSVData.push_back(tightnessCSV);
                    allCSVData.push_back(timeDevCSV);
                    
                    experimentType = "nnUnsorted";
                } else {
                    std::cerr << "ERROR: No se encontró directorio de resultados. Verifica que TSTester.exe se ejecutó correctamente.\n";
                }
            }
            
            if (nnSorted) {
                std::cout << "Ejecutando 1-NN SORTED...\n";
                std::string cmd = buildTSTesterCommand(bounds, datasets, windowType, windowValue,
                                                       windowPercentages, "nnSorted", outputDir);
                std::cout << "Comando: " << cmd << "\n";
                std::string output = executeCommand(cmd);
                std::cout << "Output: " << output << "\n";
                
                std::this_thread::sleep_for(std::chrono::seconds(3));
                
                std::string latestDir = findLatestResultsDir(outputDir);
                if (!latestDir.empty()) {
                    std::string basePath = outputDir + "\\" + latestDir + "\\";
                    
                    std::vector<std::vector<std::string>> timesCSV = parseCSV(basePath + "times-W-sort.csv");
                    std::vector<std::vector<std::string>> prunedCSV = parseCSV(basePath + "pruned-W-sort.csv");
                    std::vector<std::vector<std::string>> accuracyCSV = parseCSV(basePath + "accuracy-W-sort.csv");
                    std::vector<std::vector<std::string>> timeDevCSV = parseCSV(basePath + "time-dev-W-sort.csv");
                    
                    allCSVData.push_back(timesCSV);
                    allCSVData.push_back(prunedCSV);
                    allCSVData.push_back(accuracyCSV);
                    allCSVData.push_back({}); // No tightness para sorted
                    allCSVData.push_back(timeDevCSV);
                    
                    experimentType = "nnSorted";
                }
            }
            
            if (tightness) {
                std::cout << "Ejecutando tightness test...\n";
                std::string cmd = buildTSTesterCommand(bounds, datasets, windowType, windowValue,
                                                       windowPercentages, "tightness", outputDir);
                std::cout << "Comando: " << cmd << "\n";
                std::string output = executeCommand(cmd);
                std::cout << "Output: " << output << "\n";
                
                // Esperar más tiempo para tightness test (puede tardar mucho)
                std::this_thread::sleep_for(std::chrono::seconds(10));
                
                // Buscar el directorio más reciente (puede ser diferente al de nnUnsorted)
                std::string latestDir = findLatestResultsDir(outputDir);
                std::cout << "Directorio tightness encontrado: " << (latestDir.empty() ? "(ninguno)" : latestDir) << "\n";
                
                if (!latestDir.empty()) {
                    std::string basePath = outputDir + "\\" + latestDir + "\\";
                    std::cout << "Buscando tightness-W.csv en: " << basePath << "\n";
                    std::vector<std::vector<std::string>> tightnessCSV = parseCSV(basePath + "tightness-W.csv");
                    std::cout << "Tightness CSV leído: " << tightnessCSV.size() << " filas\n";
                    
                    // Si no hay otros datos, usar solo tightness
                    if (allCSVData.empty()) {
                        allCSVData.push_back({}); // times
                        allCSVData.push_back({}); // pruned
                        allCSVData.push_back({}); // accuracy
                        allCSVData.push_back(tightnessCSV);
                        allCSVData.push_back({}); // timeDev
                        experimentType = "tightness";
                    } else {
                        // Agregar tightness a datos existentes
                        if (allCSVData.size() > 3) {
                            allCSVData[3] = tightnessCSV;
                        }
                    }
                }
            }
            
            // Convertir a JSON
            if (!allCSVData.empty()) {
                std::string jsonResult = resultsToJSON(allCSVData, datasets, bounds, experimentType, window);
                std::cout << "JSON generado (primeros 500 chars): " << jsonResult.substr(0, 500) << "\n";
                res.set_content(jsonResult, "application/json");
                res.status = 200;
            } else {
                std::cerr << "ERROR: allCSVData está vacío. No se pudieron leer los resultados.\n";
                std::cerr << "Verifica que TSTester.exe se ejecutó correctamente y generó los CSVs.\n";
                res.set_content("{\"status\": \"error\", \"error\": \"No se pudieron leer los resultados. Verifica que TSTester.exe se ejecutó correctamente.\"}", "application/json");
                res.status = 500;
            }
            
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
            res.set_content("{\"error\": \"" + std::string(e.what()) + "\"}", "application/json");
            res.status = 500;
        }
    });
    
    // Endpoint: Obtener datasets disponibles
    svr.Get("/api/datasets", [](const httplib::Request&, httplib::Response& res) {
        std::vector<std::string> datasets;
        
        std::string datasetDir = getDatasetDir();
        if (fs::exists(datasetDir)) {
            for (const auto& entry : fs::directory_iterator(datasetDir)) {
                if (entry.is_directory()) {
                    std::string name = entry.path().filename().string();
                    // Verificar que tenga archivos TRAIN y TEST
                    if (fs::exists(entry.path() / (name + "_TRAIN")) &&
                        fs::exists(entry.path() / (name + "_TEST"))) {
                        datasets.push_back(name);
                    }
                }
            }
        }
        
        std::ostringstream json;
        json << "[";
        for (size_t i = 0; i < datasets.size(); i++) {
            if (i > 0) json << ",";
            json << "\"" << datasets[i] << "\"";
        }
        json << "]";
        
        res.set_content(json.str(), "application/json");
    });
    
    // Endpoint: Health check
    svr.Get("/api/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("{\"status\": \"ok\"}", "application/json");
    });
    
    std::cout << "Servidor iniciado en http://localhost:8081\n";
    std::cout << "Endpoints disponibles:\n";
    std::cout << "  POST /api/experiment/run\n";
    std::cout << "  GET  /api/datasets\n";
    std::cout << "  GET  /api/health\n";
    
    svr.listen("localhost", 8081);
    
    return 0;
}

