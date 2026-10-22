# OceanTriRecomp

A native PC port of **Monster Hunter Tri** made by statically recompiling the game's PowerPC code. It runs as native x86-64 code with a modern renderer built on SDL3 and ANGLE, not as an emulator.

> **Source status:** This version is not up to date with the latest OceanTriRecomp source. The upstream OceanTriRecomp repository does not currently publish its most recent source code, so this tree can lag behind the newest fixes and features. Use the latest available release when you only want to play.

> **PAL only:** This build targets the European release, game ID **RMHP08**. It does **not** support NTSC-U or NTSC-J disc images. You must provide your own legally obtained PAL copy of the game (ISO, RVZ or WIA). No game data is included.

## Features

- Static recompilation of the PAL game code for native Windows x86-64.
- SDL3 windowing with ANGLE rendering over **Vulkan** or **OpenGL ES**.
- Selectable internal resolution: 540p, 720p, 900p, 1080p, 1440p and 2160p.
- FPS limit of 30, 45, 60, 90 or 120, with optional V-Sync and an on-screen FPS counter.
- 16:9 aspect-ratio enforcement and borderless fullscreen.
- In-game settings menu with Graphics, Appearance, System, Controls and Save Manager pages.
- 11 UI themes and 5 selectable fonts.
- Game language selection: Spanish, English, French, German and Italian.
- Interface language selection: Spanish and English.
- Keyboard/mouse and controller input, including adjustable mouse-look sensitivity.
- Save manager with backup, import and export support.
- Portable configuration stored next to `MH3.exe` in `MH3UI.graphics`, `MH3UI.theme.v3`, `MH3UI.language` and `MH3UI.uilanguage`.

## Requirements

- Windows 10 or Windows 11, 64-bit.
- A GPU and driver supporting Vulkan or OpenGL ES 3.0 through ANGLE.
- A PAL Monster Hunter Tri disc image: ISO, RVZ or WIA.
- A writable folder for the game. Avoid `Program Files`, because settings and saves are stored beside the executable.

## Installation

1. Download the latest Windows build from the **[Releases](https://github.com/masterd2003/OceanTriRecomp/releases)** page.
2. Extract the archive into a writable folder, for example `D:\Games\OceanTriRecomp`.
3. Keep all included DLLs next to `MH3.exe`.
4. Run `MH3.exe`.
5. On first launch, select your **PAL** Monster Hunter Tri disc image. The launcher copies the image beside `MH3.exe` and prepares the game data. The first preparation can take a while and needs some free disk space.
6. After preparation, the game starts automatically.

Settings are saved beside the executable. Game saves are stored in the `Saves` folder beside the executable. Back up that folder before updating the port.

You can also pass an image directly:

```powershell
.\MH3.exe --rom "D:\Games\mh3-pal.iso"
```

## Building from source


A Windows source build needs Clang, CMake, Ninja, Visual Studio Build Tools with the Windows SDK, and ANGLE. The translation step currently requires Linux or WSL because it uses `fcntl`.

Full prerequisites, disc extraction, translation, CMake configuration, DLL setup and troubleshooting are in **[BUILDINSTRUCTIONS.md](BUILDINSTRUCTIONS.md)**.

## Known limitations

- Only the PAL release is supported.
- Game data is not distributed. A compatible PAL disc image is mandatory.
- The newest upstream source is not publicly available, so this repository may lag behind.
- Online play and some optional features may depend on the specific build and are not guaranteed to match the latest upstream release.

## License

The project is distributed under the GNU General Public License v3.0. See [LICENSE](LICENSE).
