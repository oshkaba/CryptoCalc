# CryptoCalc

CryptoCalc is a C++ SFML application for cryptography calculations. It includes primality testing, RSA, Diffie-Hellman, modular arithmetic tools, benchmarks, and result saving.

## Requirements

CryptoCalc requires:

* Ubuntu or WSL Ubuntu
* g++ with C++17 support
* make
* pkg-config
* SFML 3.x

The project folder must contain:

```text
src/
include/
fonts/
makefile
```

Do not move the `fonts/` folder. The app loads fonts from that folder.

## Install Dependencies

Run:

```bash
sudo apt update
sudo apt install build-essential make pkg-config cmake git
```

## Install SFML 3

First, try:

```bash
sudo apt install libsfml-dev
```

Check the installed version:

```bash
pkg-config --modversion sfml-graphics
```

If it prints `3.x.x`, continue to the build step.

If it prints `2.x.x`, install SFML 3 from source:

```bash
sudo apt remove libsfml-dev
sudo apt install libxrandr-dev libxcursor-dev libxi-dev libudev-dev libfreetype-dev libharfbuzz-dev libflac-dev libvorbis-dev libgl1-mesa-dev libegl1-mesa-dev libmbedtls-dev libssh2-1-dev

cd ~
git clone --branch 3.1.0 https://github.com/SFML/SFML.git SFML-3
cd SFML-3

cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_SHARED_LIBS=ON
cmake --build build
sudo cmake --install build
sudo ldconfig
```

Check SFML again:

```bash
pkg-config --modversion sfml-graphics
```

It should print:

```text
3.x.x
```

## Build and Run

Go into the project folder:

```bash
cd CryptoCalc
```

Build the project:

```bash
make clean
make
```

Run CryptoCalc:

```bash
./cryptocalc
```

## Clean

To remove the executable and build files:

```bash
make clean
```
