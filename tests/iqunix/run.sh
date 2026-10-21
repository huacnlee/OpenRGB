#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/../.."
mkdir -p target/tests/iqunix
"${CXX:-c++}" -std=c++17 -Wall -Wextra -Werror \
  -I. -IRGBController -IControllers/IQUNIXController -Idependencies/json \
  $(pkg-config --cflags hidapi-hidraw) \
  tests/iqunix/protocol.cpp Controllers/IQUNIXController/IQUNIXController.cpp \
  -o target/tests/iqunix/protocol
target/tests/iqunix/protocol
printf 'IQUNIX protocol tests passed\n'
