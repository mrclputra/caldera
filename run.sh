#!/bin/bash
set -e

BUILD_DIR="build"
VCPKG_PATH="${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
TARGET_NAME="caldera"

#rm -rf "$BUILD_DIR"

# use clang binaries
cmake -B "$BUILD_DIR" -S . \
  -G Ninja \
  -DCMAKE_C_COMPILER="clang" \
  -DCMAKE_CXX_COMPILER="clang++" \
  -DCMAKE_BUILD_TYPE=Release \
  -DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_PATH" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

cmake --build "$BUILD_DIR"

# for distribution
DIST_DIR="$BUILD_DIR/dist"
rm -rf "$DIST_DIR"
mkdir -p "$DIST_DIR"
cp "$BUILD_DIR/$TARGET_NAME.exe" "$BUILD_DIR"/*.dll "$DIST_DIR"/
cp -r "$BUILD_DIR/assets" "$DIST_DIR"/

# # run
# cd "$BUILD_DIR"
# ./"$TARGET_NAME".exe "C:\Users\Marcelino\Downloads\QueenFlat2.ply"
# # ./"$TARGET_NAME".exe "C:\Users\Marcelino\Desktop\tests\clouds\tree02.ply"
