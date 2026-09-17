# caldera

This is a command line point cloud renderer in OpenGL. Load a `.ply` file and view it.

## Build

Requires/uses CMake, and vcpkg

```bash
cmake -B build -S . -G Ninja \
  -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
cmake --build build
```

Or you can just run `./run.sh`, which does the same and launches with a sample path (edit the path inside first).

## Usage

```bash
./caldera.exe path/to/cloud.ply
```

Only `.ply` files are supported (ASCII or binary)

### notes

TODO:
- PDAL integration
- Monte-Carlo point rendering
- transforms and basic operations
