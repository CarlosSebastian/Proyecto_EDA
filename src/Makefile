# Nombre del ejecutable
TARGET = TSTester

# Directorios
SRC_DIR = .
BIN_DIR = $(SRC_DIR)/bin
BUILD_DIR = $(SRC_DIR)/build

# Compilador y flags
CXX = g++
CXXFLAGS = -std=c++17 -O3 -I.

# Buscar automáticamente todos los .cpp
SRC := $(shell find $(SRC_DIR) -name "*.cpp")

# Convertir cada archivo fuente a un objeto en build/
OBJ := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRC))

# Separador automático de directorios
MKDIR_P = mkdir -p

all: $(BIN_DIR)/$(TARGET)

# Compilar el ejecutable final
$(BIN_DIR)/$(TARGET): $(OBJ)
	@$(MKDIR_P) $(BIN_DIR)
	$(CXX) $(OBJ) -o $(BIN_DIR)/$(TARGET)
	@echo "============================================"
	@echo "  Compilación completa"
	@echo "  Ejecutable: $(BIN_DIR)/$(TARGET)"
	@echo "============================================"

# Regla genérica para compilar cada archivo .cpp a .o
$(BUILD_DIR)/%.o: %.cpp
	@$(MKDIR_P) $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	@echo "Ejecutando:"
	./bin/$(TARGET)

debug: CXXFLAGS = -std=c++17 -g -O0 -I.
debug: clean all
	@echo "Compilado en modo debug."

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Limpieza completada."

rebuild: clean all

# Evitar que make interprete estos nombres como archivos
.PHONY: all run debug clean rebuild
