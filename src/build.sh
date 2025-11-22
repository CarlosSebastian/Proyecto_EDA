#!/bin/bash

set -e  # stop on error

echo "Compilando Proyecto EDA (DTW Bounds)"
echo "..."
# Entrar a la carpeta raíz del proyecto
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

# Directorio de salida
BIN_DIR="$SCRIPT_DIR/bin"
BUILD_DIR="$SCRIPT_DIR/build"

mkdir -p "$BIN_DIR"
mkdir -p "$BUILD_DIR"

echo "Buscando archivos fuente"
echo "..."
CPP_FILES=$(find . -name "*.cpp")

echo "Archivos encontrados:"
echo "$CPP_FILES"
echo ""

echo "Compilando"
echo "..."
g++ -std=c++17 -O3 $CPP_FILES -o "$BIN_DIR/TSTester"

echo "Compilación completa!"
echo "Ejecutable generado: $BIN_DIR/TSTester"
echo ""
echo "Ejemplo de ejecución:"
echo "  ./bin/TSTester -bkeogh,improved -W -t -d50words"
