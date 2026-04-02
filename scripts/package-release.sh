#!/usr/bin/env bash
set -euo pipefail

VERSION="${1:-}"
if [ -z "$VERSION" ]; then
    echo "Usage: $0 <version>"
    exit 1
fi

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build/cross/none/release"
ASSETS_DIR="$ROOT_DIR/romfs"
RELEASE_DIR="$ROOT_DIR/release"
STAGE_DIR="$RELEASE_DIR/stage"
SDMC_DIR="$STAGE_DIR/sdmc"

DAEMON_NSP="$BUILD_DIR/switchu-daemon.nsp"
MENU_NSP="$BUILD_DIR/switchu-menu.nsp"

for file in "$DAEMON_NSP" "$MENU_NSP"; do
    if [ ! -f "$file" ]; then
        echo "Missing required build artifact: $file"
        exit 1
    fi
done

if [ ! -d "$ASSETS_DIR" ]; then
    echo "Missing required assets directory: $ASSETS_DIR"
    exit 1
fi

rm -rf "$STAGE_DIR"
mkdir -p "$SDMC_DIR"
mkdir -p "$RELEASE_DIR"

install -Dm644 "$DAEMON_NSP" "$SDMC_DIR/atmosphere/contents/0100000000001000/exefs.nsp"
install -Dm644 "$MENU_NSP" "$SDMC_DIR/atmosphere/contents/010000000000100B/exefs.nsp"
install -Dm644 /dev/null "$SDMC_DIR/atmosphere/contents/0100000000001000/flags/boot2.flag"

mkdir -p "$SDMC_DIR/switch/SwitchU"
cp -a "$ASSETS_DIR"/. "$SDMC_DIR/switch/SwitchU/"

cp -f "$DAEMON_NSP" "$RELEASE_DIR/SwitchU-${VERSION}-switchu-daemon.nsp"
cp -f "$MENU_NSP" "$RELEASE_DIR/SwitchU-${VERSION}-switchu-menu.nsp"

ZIP_PATH="$RELEASE_DIR/SwitchU-${VERSION}-sdmc.zip"
rm -f "$ZIP_PATH"
if command -v zip > /dev/null 2>&1; then
    (
        cd "$SDMC_DIR"
        zip -r "$ZIP_PATH" atmosphere switch > /dev/null
    )
elif command -v bsdtar > /dev/null 2>&1; then
    (
        cd "$SDMC_DIR"
        bsdtar -a -cf "$ZIP_PATH" atmosphere switch
    )
else
    echo "Neither zip nor bsdtar is available to create $ZIP_PATH"
    exit 1
fi

CHECKSUM_FILE="$RELEASE_DIR/SwitchU-${VERSION}-sha256.txt"
(
    cd "$RELEASE_DIR"
    sha256sum \
        "SwitchU-${VERSION}-sdmc.zip" \
        "SwitchU-${VERSION}-switchu-daemon.nsp" \
        "SwitchU-${VERSION}-switchu-menu.nsp" \
        > "$CHECKSUM_FILE"
)

echo "Release artifacts generated in $RELEASE_DIR:"
ls -lh "$RELEASE_DIR"/SwitchU-"$VERSION"-* "$CHECKSUM_FILE"
