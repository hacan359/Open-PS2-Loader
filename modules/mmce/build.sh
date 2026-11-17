#!/bin/sh
# Rebuilds mmceman.irx next to this script. Run inside the ps2dev
# container. The SDK's own copy is v2.1.1, older than the sio2man hook
# fix (cccc366), and freezes the menu on sustained transfers.
set -eu
PIN=db3e93f0fdbcf882f88da110cbd9b7db188ec17a
HERE="$(cd "$(dirname "$0")" && pwd)"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
git clone --quiet https://github.com/ps2-mmce/mmceman "$WORK/m"
git -C "$WORK/m" checkout --quiet "$PIN"
for p in "$HERE"/patches/*.patch; do git -C "$WORK/m" apply "$p"; done
mkdir -p "$WORK/m/mmceman/obj"
make -s -C "$WORK/m/mmceman"
cp "$WORK/m/mmceman/irx/mmceman.irx" "$HERE/mmceman.irx"
cp "$WORK/m/LICENSE" "$HERE/LICENSE"
