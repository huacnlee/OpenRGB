#!/usr/bin/env bash
set -euo pipefail
# Run from the repository root.
mkdir -p build
c++ -std=c++17 -Wall -Wextra -Werror \
    -I. -IRGBController -Idependencies/json \
    -IControllers/EVisionKeyboardController/EVisionV2KeyboardController \
    $(pkg-config --cflags hidapi-hidraw) \
    tests/rainy75/protocol.cpp \
    Controllers/EVisionKeyboardController/EVisionV2KeyboardController/EVisionV2KeyboardController.cpp \
    RGBController/RGBControllerKeyNames.cpp -o build/rainy75-protocol-tests
./build/rainy75-protocol-tests
python3 -B -m unittest discover -s tests/rainy75 -v
