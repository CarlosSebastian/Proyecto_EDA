# Makefile para DTW Bounds en C++

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -march=native
INCLUDES = -I./src/TSTester -I./src/Utils -I./src/distance -I./src/distance/elastic
LDFLAGS = -lstdc++fs

# Directorios
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Archivos fuente
SOURCES = $(SRC_DIR)/TSTester/Dataset.cpp \
          $(SRC_DIR)/TSTester/TSTester.cpp \
          $(SRC_DIR)/Utils/FileIterator.cpp \
          $(SRC_DIR)/distance/Bounds.cpp \
          $(SRC_DIR)/distance/elastic/DTW.cpp

# Archivos objeto
OBJECTS = $(BUILD_DIR)/TSTester/Dataset.o \
          $(BUILD_DIR)/TSTester/TSTester.o \
          $(BUILD_DIR)/Utils/FileIterator.o \
          $(BUILD_DIR)/distance/Bounds.o \
          $(BUILD_DIR)/distance/elastic/DTW.o

# Ejecutable
TARGET = $(BIN_DIR)/TSTester

# Regla principal
all: directories $(TARGET)

# Crear directorios necesarios
directories:
	@mkdir -p $(BUILD_DIR)/TSTester
	@mkdir -p $(BUILD_DIR)/Utils
	@mkdir -p $(BUILD_DIR)/distance
	@mkdir -p $(BUILD_DIR)/distance/elastic
	@mkdir -p $(BIN_DIR)

# Enlazar ejecutable
$(TARGET): $(OBJECTS)
	@echo "Linking..."
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compilar Dataset.cpp
$(BUILD_DIR)/TSTester/Dataset.o: $(SRC_DIR)/TSTester/Dataset.cpp $(SRC_DIR)/TSTester/Dataset.h $(SRC_DIR)/TSTester/ClassifiedSequence.h
	@echo "Compiling Dataset.cpp..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compilar TSTester.cpp
$(BUILD_DIR)/TSTester/TSTester.o: $(SRC_DIR)/TSTester/TSTester.cpp
	@echo "Compiling TSTester.cpp..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compilar FileIterator.cpp
$(BUILD_DIR)/Utils/FileIterator.o: $(SRC_DIR)/Utils/FileIterator.cpp $(SRC_DIR)/Utils/FileIterator.h
	@echo "Compiling FileIterator.cpp..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compilar Bounds.cpp
$(BUILD_DIR)/distance/Bounds.o: $(SRC_DIR)/distance/Bounds.cpp $(SRC_DIR)/distance/Bounds.h $(SRC_DIR)/distance/BoundsID.h
	@echo "Compiling Bounds.cpp..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compilar DTW.cpp
$(BUILD_DIR)/distance/elastic/DTW.o: $(SRC_DIR)/distance/elastic/DTW.cpp $(SRC_DIR)/distance/elastic/DTW.h
	@echo "Compiling DTW.cpp..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Limpiar archivos compilados
clean:
	@echo "Cleaning..."
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Clean complete"

# Limpiar y recompilar
rebuild: clean all

# Ejecutar prueba simple
test: all
	@echo "Running test..."
	./$(TARGET) -bkeogh,improved -w5

# Ayuda
help:
	@echo "Makefile para DTW Bounds"
	@echo ""
	@echo "Uso:"
	@echo "  make          - Compilar el proyecto"
	@echo "  make clean    - Limpiar archivos compilados"
	@echo "  make rebuild  - Limpiar y recompilar"
	@echo "  make test     - Compilar y ejecutar prueba simple"
	@echo "  make help     - Mostrar esta ayuda"
	@echo ""
	@echo "Ejemplos de ejecución:"
	@echo "  ./bin/TSTester -bkeogh,improved,petitjean,webb -W -t"
	@echo "  ./bin/TSTester -bkeogh,improved,petitjean,webb -W -s"
	@echo "  ./bin/TSTester -bkeogh,improved -w5 -dCoffee,Gun_Point"

.PHONY: all clean rebuild test help directories