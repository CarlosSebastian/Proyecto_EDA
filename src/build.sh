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

if [ -z "$CPP_FILES" ]; then
    echo "ERROR: No se encontraron archivos .cpp"
    exit 1
fi

echo "Archivos encontrados:"
echo "$CPP_FILES"
echo ""

echo "Compilando"
echo "..."

# Detectar sistema operativo para el nombre del ejecutable
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" ]]; then
    # Windows (Git Bash, MSYS2, Cygwin)
    OUTPUT="$BIN_DIR/TSTester.exe"
    EXECUTABLE=".\bin\TSTester.exe"
else
    # Linux/Mac
    OUTPUT="$BIN_DIR/TSTester"
    EXECUTABLE="./bin/TSTester"
fi

g++ -std=c++17 -O3 $CPP_FILES -o "$OUTPUT"

echo ""
echo "============================================"
echo "  Compilación completa!"
echo "  Ejecutable generado: $OUTPUT"
echo "============================================"
echo ""
echo "Ejemplo de ejecución:"
echo "  $EXECUTABLE -bkeogh,improved -W -t -d50words"
