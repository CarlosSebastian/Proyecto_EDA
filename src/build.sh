#!/bin/bash
# Script de compilación rápida para DTW Bounds

set -e  # Salir si hay error

echo "🔨 Compilando DTW Bounds..."

# Crear directorios
mkdir -p build/distance/elastic build/TSTester build/Utils bin

# Compilar
echo "📦 Compilando archivos fuente..."
g++ -std=c++17 -O3 -I. -c main.cpp -o build/main.o
g++ -std=c++17 -O3 -I. -c distance/elastic/DTW.cpp -o build/distance/elastic/DTW.o
g++ -std=c++17 -O3 -I. -c distance/Bounds.cpp -o build/distance/Bounds.o
g++ -std=c++17 -O3 -I. -c TSTester/Dataset.cpp -o build/TSTester/Dataset.o
g++ -std=c++17 -O3 -I. -c TSTester/TSTester.cpp -o build/TSTester/TSTester.o
g++ -std=c++17 -O3 -I. -c Utils/FileIterator.cpp -o build/Utils/FileIterator.o

# Enlazar
echo "🔗 Enlazando ejecutable..."
g++ -std=c++17 -O3 \
    build/main.o \
    build/distance/elastic/DTW.o \
    build/distance/Bounds.o \
    build/TSTester/Dataset.o \
    build/TSTester/TSTester.o \
    build/Utils/FileIterator.o \
    -o bin/TSTester -lstdc++fs

echo "✅ Compilación exitosa!"
echo "📍 Ejecutable: bin/TSTester"
echo ""
echo "Ejecuta con: ./bin/TSTester -h"