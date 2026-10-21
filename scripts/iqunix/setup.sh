#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-2.0-or-later
set -euo pipefail

method=mise
version=${OPENRGB_VERSION:-1.1-iqunix-ev63.1}
while (( $# )); do
  case "$1" in
    --method) method=${2:?Expected mise or curl}; shift 2 ;;
    --version) version=${2:?Expected a release tag}; shift 2 ;;
    -h|--help) printf 'Usage: setup.sh [--method mise|curl] [--version TAG]\n'; exit 0 ;;
    *) printf 'Unknown argument: %s\n' "$1" >&2; exit 1 ;;
  esac
done
[[ $method == mise || $method == curl ]] || { echo 'Method must be mise or curl' >&2; exit 1; }
[[ $(uname -s) == Linux && $(uname -m) == x86_64 ]] || {
  echo 'This release supports Linux x86_64.' >&2; exit 1;
}
[[ $version =~ ^[A-Za-z0-9._-]+$ ]] || { echo 'Invalid release tag' >&2; exit 1; }
command -v curl >/dev/null

as_root() {
  if (( EUID == 0 )); then "$@"; else sudo "$@"; fi
}

if [[ $method == mise ]]; then
  command -v mise >/dev/null || { echo 'Install mise first: https://mise.jdx.dev/getting-started.html' >&2; exit 1; }
  tool="github:huacnlee/OpenRGB[asset_pattern=OpenRGB-iqunix-ev63-linux-x86_64.tar.gz,strip_components=1,filter_bins=openrgb]@$version"
  mise use --global "$tool"
  package_dir=$(mise where "$tool")
  "$package_dir/openrgb" --version
else
  command -v sha256sum >/dev/null
  command -v tar >/dev/null
  cache_root=${XDG_CACHE_HOME:-$HOME/.cache}/openrgb-iqunix
  mkdir -p "$cache_root"
  package_dir=$(mktemp -d "$cache_root/install.XXXXXX")
  trap 'rm -r -- "$package_dir"' EXIT
  base="https://github.com/huacnlee/OpenRGB/releases/download/$version"
  asset=OpenRGB-iqunix-ev63-linux-x86_64.tar.gz
  curl -fL --retry 3 "$base/$asset" -o "$package_dir/$asset"
  curl -fL --retry 3 "$base/SHA256SUMS" -o "$package_dir/SHA256SUMS"
  (cd "$package_dir" && sha256sum --check SHA256SUMS)
  tar -xzf "$package_dir/$asset" -C "$package_dir"
  package_dir="$package_dir/iqunix-ev63"
  # Keep the extraction root for cleanup after installation.
  trap 'rm -r -- "${package_dir%/iqunix-ev63}"' EXIT
  "$package_dir/openrgb" --version
  as_root install -Dm755 "$package_dir/openrgb" /usr/local/bin/openrgb
fi

as_root install -Dm644 "$package_dir/60-openrgb-iqunix.rules" /etc/udev/rules.d/60-openrgb-iqunix.rules
as_root udevadm control --reload-rules
as_root udevadm trigger --subsystem-match=hidraw
printf '\nInstalled OpenRGB %s using %s.\nRestart your OpenRGB server; on Omarchy: omarchy restart shell\n' "$version" "$method"
if [[ $method == mise ]]; then
  printf 'Use a mise-activated shell, then check: command -v openrgb\n'
fi
