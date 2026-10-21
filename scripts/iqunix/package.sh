#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/../.."
mkdir -p target/release/iqunix-ev63
install -m755 target/build/openrgb target/release/iqunix-ev63/openrgb
strip target/release/iqunix-ev63/openrgb
cp scripts/iqunix/install.sh scripts/iqunix/60-openrgb-iqunix.rules LICENSE Documentation/IQUNIX-EV63.md target/release/iqunix-ev63/
tar -C target/release -czf target/release/OpenRGB-iqunix-ev63-linux-x86_64.tar.gz iqunix-ev63
sha256sum target/release/OpenRGB-iqunix-ev63-linux-x86_64.tar.gz > target/release/SHA256SUMS
