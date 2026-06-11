CC=g++
CFLAGS=-c -Wall -g -std=c++17 -Iinclude
LFLAGS=$(shell pkg-config --libs sfml-graphics)
SFML_FLAGS=$(shell pkg-config --cflags sfml-graphics)

all: cryptocalc
	rm -f *.o

cryptocalc: main app ui math_utils rsa dh benchmark
	$(CC) main.o app.o ui.o math_utils.o rsa.o dh.o benchmark.o -o cryptocalc $(LFLAGS)

main: src/main.cpp include/app.h
	$(CC) $(CFLAGS) $(SFML_FLAGS) src/main.cpp -o main.o

app: src/app.cpp include/app.h include/ui.h include/rsa.h include/dh.h include/math_utils.h include/benchmark.h
	$(CC) $(CFLAGS) $(SFML_FLAGS) src/app.cpp -o app.o

ui: src/ui.cpp include/ui.h
	$(CC) $(CFLAGS) $(SFML_FLAGS) src/ui.cpp -o ui.o

math_utils: src/math_utils.cpp include/math_utils.h
	$(CC) $(CFLAGS) src/math_utils.cpp -o math_utils.o

rsa: src/rsa.cpp include/rsa.h include/math_utils.h
	$(CC) $(CFLAGS) src/rsa.cpp -o rsa.o

dh: src/dh.cpp include/dh.h include/math_utils.h
	$(CC) $(CFLAGS) src/dh.cpp -o dh.o

benchmark: src/benchmark.cpp include/benchmark.h include/rsa.h include/math_utils.h
	$(CC) $(CFLAGS) src/benchmark.cpp -o benchmark.o

clean:
	rm -f *.o cryptocalc