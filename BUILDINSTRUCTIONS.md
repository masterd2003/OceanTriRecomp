# Building OceanTriRecomp from source

This document describes a complete Windows x86-64 build of OceanTriRecomp.

The repository does **not** include `generated/`. That directory contains the translated game code, is roughly 580 MB, and is derived from the game. It is ignored by Git and must be regenerated from a **PAL** Monster Hunter Tri disc image.

The translation pipeline currently requires Linux or WSL. The final C++ build runs natively on Windows.

## 1. What you need

### Game files

- A legally obtained **PAL** Monster Hunter Tri disc image.
- Supported image formats: ISO, RVZ or WIA.
- Expected game ID: **RMHP08**.
- NTSC-U and NTSC-J images are not supported.

### Windows build tools

Install:

- Windows 10 or Windows 11, 64-bit.
- Visual Studio 2022 Build Tools with:
  - **Desktop development with C++**
  - A Windows 10 or Windows 11 SDK
- LLVM/Clang for Windows, version 21 or newer recommended.
- CMake 3.25 or newer.
- Ninja.
- Git.
- Python 3 (only needed for the translation step).
- .NET 8 SDK (only needed for the translation step).
- WSL with a Linux distribution (recommended for the translation step).

The build uses Clang targeting the MSVC toolchain, so the Visual Studio Build Tools and Windows SDK are required even though the compiler is Clang.

### ANGLE

The Windows build expects ANGLE at:

```text
third_party/angle/
  include/
    EGL/egl.h
    GLES3/gl3.h
  lib/
    libGLESv2.lib
  bin/
    libEGL.dll
    libGLESv2.dll
    libGLESv1_CM.dll
    d3dcompiler_47.dll
```

If ANGLE is somewhere else, change the three ANGLE paths in the CMake command below.

SDL3 is downloaded automatically by Aurora during the first CMake configure. An internet connection is needed for that first configure.

## 2. Prepare the PAL game data

Extract the complete PAL disc to `Assets/DATA`, preserving the disc layout. The important files are:

```text
Assets/DATA/sys/main.dol
Assets/DATA/sys/boot.bin
Assets/DATA/sys/fst.bin
Assets/DATA/files/mh3.sel
Assets/DATA/files/01/*.rso
```

The translation scripts expect the layout produced by `nodtool`. The example used in `scripts/translate.py` is:

```bash
tools/nodtool extract -q "path/to/mh3-pal.iso" Assets/DATA
```

If `nodtool` is not included in your checkout, use an equivalent extractor and keep the same directory layout.

Do not commit `Assets/DATA`, the disc image, or any extracted game files.

## 3. Generate `generated/`

Run this step in Linux or WSL. Native Windows Python does not provide the `fcntl` module used by the translation lock.

From the repository root:

```bash
cd /path/to/OceanTriRecomp

python3 --version
python3 -c "import fcntl; print('fcntl available')"
dotnet --version
```

If `dotnet` is not at `tools/dotnet/dotnet`, point the script at your installation:

```bash
export DOTNET_BIN="$(command -v dotnet)"
```

Run the full translation:

```bash
python3 scripts/translate.py --force
```

The first run builds the C# translator, prepares the disc analysis, translates the main executable and the RSO overlays, and generates the dispatch shards. It can take several minutes and uses a few hundred MB of disk space. Logs are written to `logs/`.

Expected output includes:

```text
generated/translation.json
generated/functions/*.cpp
generated/rso_overlays/*.cpp
generated/build_shards/shards.cmake
```

On later runs, omit `--force` to rebuild only the steps whose inputs changed.

The Windows CMake build does not run translation automatically. If you change `symbols.txt`, `recomp.yml`, the site lists, translator sources, or runtime native ports, run the translation again before rebuilding.

## 4. Configure the Windows build

Open a Windows developer shell with Clang, CMake and Ninja in `PATH`. The **x64 Native Tools Command Prompt for VS 2022** works well.

From the repository root:

```powershell
$Repo = (Get-Location).Path

cmake -S runtime -B build -G Ninja `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_C_COMPILER="C:/Program Files/LLVM/bin/clang.exe" `
  -DCMAKE_CXX_COMPILER="C:/Program Files/LLVM/bin/clang++.exe" `
  -DMH3_ALLOW_UNSUPPORTED_HOST=ON `
  -DMH3_AURORA_ROOT="$Repo/aurora-main" `
  -DAURORA_ANGLE_VULKAN=ON `
  -DAURORA_EGL_INCLUDE_DIR="$Repo/third_party/angle/include" `
  -DAURORA_GLES3_INCLUDE_DIR="$Repo/third_party/angle/include" `
  -DAURORA_GLESV2_LIBRARY="$Repo/third_party/angle/lib/libGLESv2.lib" `
  -DMH3_BUILD_TESTS=OFF `
  -DMH3_BUILD_VITA_TEXTURE_PACKER=OFF
```

Notes:

- Change the paths if Clang or ANGLE are installed elsewhere.
- `MH3_ALLOW_UNSUPPORTED_HOST=ON` is required for a Windows host build.
- `AURORA_ANGLE_VULKAN=ON` selects the ANGLE Vulkan path used by the Windows build.
- If `pgo/mh3-release.profdata.tar.zst` is missing, add `-DMH3_PGO_RELEASE_PROFILE=OFF`.

If configuration fails because `generated/translation.json` or `generated/build_shards/shards.cmake` is missing, return to step 3 and run the translation.

## 5. Build

```powershell
cmake --build build --target MH3 --parallel
```

The executable is:

```text
build/MH3.exe
```

## 6. Copy the runtime DLLs

Aurora copies `SDL3.dll` into the build directory automatically. Copy the ANGLE DLLs as well:

```powershell
Copy-Item third_party\angle\bin\*.dll build\ -Force
```

If `SDL3.dll` is missing, copy it manually:

```powershell
Copy-Item build\_deps\sdl3_prebuilt-src\bin\SDL3.dll build\
```

Before launching, the build directory should contain at least:

```text
MH3.exe
SDL3.dll
libEGL.dll
libGLESv2.dll
libGLESv1_CM.dll
d3dcompiler_47.dll
```

## 7. Run

The normal launcher flow is:

```powershell
.\build\MH3.exe
```

Select the PAL ISO/RVZ/WIA image. The launcher copies it beside `MH3.exe` and prepares the game data.

You can also pass the image directly:

```powershell
.\build\MH3.exe --rom "D:\Games\mh3-pal.iso"
```

For development, you can use already-extracted data and skip the launcher:

```powershell
$env:MH3_DISC_DIR = "$Repo\Assets\DATA"
.\build\MH3.exe
```

`MH3_DISC_DIR` must point to a prepared `Assets/DATA`-style directory.

## 8. Rebuilding

After changing C++ source files:

```powershell
cmake --build build --target MH3 --parallel
```

After changing translation inputs:

```bash
# Linux or WSL
python3 scripts/translate.py
```

Then rebuild on Windows:

```powershell
cmake --build build --target MH3 --parallel
```

For a clean rebuild, delete `build/` and run the configure and build commands again. Do not delete `generated/` unless you intend to rerun the translation.

## Troubleshooting

### `generated/translation.json` is missing

Run the translation pipeline first:

```bash
python3 scripts/translate.py --force
```

### Translation fails with `ModuleNotFoundError: No module named 'fcntl'`

You are running the translation with native Windows Python. Run it in WSL or Linux instead.

### Translation cannot find .NET

Install the .NET 8 SDK, then either create `tools/dotnet/dotnet` or run:

```bash
export DOTNET_BIN="$(command -v dotnet)"
```

### CMake cannot find `EGL/egl.h` or `GLES3/gl3.h`

Check that ANGLE is in `third_party/angle` and that the three ANGLE paths passed to CMake are correct.

### The executable starts but cannot find `libEGL.dll` or `libGLESv2.dll`

Copy the ANGLE DLLs beside `MH3.exe`:

```powershell
Copy-Item third_party\angle\bin\*.dll build\ -Force
```

### The game rejects the disc image

Confirm that it is the PAL release with game ID `RMHP08`. NTSC-U and NTSC-J images are not supported.

### `generated/build_shards/shards.cmake` contains paths from another computer

Regenerate `generated/` on this checkout:

```bash
python3 scripts/translate.py --force
```

### CMake cannot find the Windows SDK or MSVC headers

Install Visual Studio 2022 Build Tools with the Desktop C++ workload and a Windows SDK, then reconfigure from a developer command prompt.

## What not to commit

Do not commit:

- `generated/`
- `build/`
- `Assets/DATA/`
- disc images (`.iso`, `.rvz`, `.wia`)
- `Saves/`
- runtime settings such as `MH3UI.*`
- local logs or backups

The repository `.gitignore` already excludes build trees and `generated/`.
