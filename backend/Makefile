# Makefile para Backend API Server (Windows)

TARGET = server.exe
CXX = g++
CXXFLAGS = -std=c++17 -O2 -I. -I./httplib

# Descargar httplib si no existe
HTTPLIB = httplib/httplib.h

$(HTTPLIB):
	@echo "Descargando cpp-httplib..."
	@if not exist httplib mkdir httplib
	@powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h' -OutFile 'httplib/httplib.h'"

# Compilar
$(TARGET): $(HTTPLIB) server.cpp
	$(CXX) $(CXXFLAGS) server.cpp -o $(TARGET) -lws2_32 -lwsock32

all: $(TARGET)

clean:
	@if exist $(TARGET) del $(TARGET)
	@if exist *.o del *.o

run: $(TARGET)
	$(TARGET)

.PHONY: all clean run

