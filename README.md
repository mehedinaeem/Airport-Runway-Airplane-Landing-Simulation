# Airport Runway & Airplane Landing Simulation

A C++11 computer graphics project using `graphics.h` and WinBGIm. It draws an airport and animates an airplane approaching, descending, extending its landing gear, touching down, braking, and parking near gate A4.

The scene includes day/night modes, moving clouds and vehicles, runway lights, a rotating radar, telemetry, and fullscreen support. All graphics are drawn with primitives; no image assets are required. The telemetry is illustrative, not a real flight model.

## 1. Clone the repository

Install Git first, then open a terminal and run:

```bash
git clone https://github.com/mehedinaeem/Airport-Runway-Airplane-Landing-Simulation.git
cd Airport-Runway-Airplane-Landing-Simulation
```

Run all remaining project commands from this directory. WinBGIm source is included in `third_party/winbgim/`; no separate graphics library download is needed.

## 2. Install dependencies

### Ubuntu / Debian Linux

The application uses the Windows API. On Linux, MinGW builds the Windows executable and Wine runs its graphics window.

```bash
sudo apt update
sudo apt install git make g++ g++-mingw-w64-x86-64 wine
```

A graphical desktop is required to see the simulation. A terminal-only server cannot display it without a configured graphical environment.

### Windows

Install MSYS2, open its **UCRT64** terminal, and install the tools:

```bash
pacman -Syu
```

If prompted to close the terminal, reopen the UCRT64 terminal and finish the update before continuing:

```bash
pacman -S --needed git make mingw-w64-ucrt-x86_64-gcc
```

Use the UCRT64 terminal for the clone, build, and run commands. The Makefile uses Unix shell commands such as `mkdir -p` and `rm -rf`, so use this shell instead of Command Prompt.

## 3. Build and run

On either platform, from the project directory:

```bash
make run
```

This builds the bundled WinBGIm library, compiles the application to `build/airport.exe`, and launches it. Linux launches through Wine; Windows launches the executable directly.

Click the graphics window, then press **Enter** to start the landing. At normal speed, the complete landing takes approximately 37 seconds. Press **Esc** to exit.

### Build and launch separately

Build only:

```bash
make windows
```

Then launch on Linux:

```bash
wine build/airport.exe
```

Or launch on Windows from the UCRT64 terminal:

```bash
./build/airport.exe
```

The default `make` command also builds `build/airport.exe`. The executable statically links the MinGW C++ runtime.

## 4. Test and clean

Run the simulation tests and check the rendering code's syntax:

```bash
make check
```

These checks use the native C++ compiler and do not open a graphics window or require Wine. They cover landing stages, wheel contact, braking, parking, pause, restart, transformations, and speed limits. The rendering syntax check uses a declaration-only graphics header; it does not verify visual output.

Delete generated build files:

```bash
make clean
```

Rebuild and run afterward:

```bash
make run
```

### Command reference

| Command | Purpose |
| --- | --- |
| `make` | Build the Windows application |
| `make windows` | Build `build/airport.exe` without launching |
| `make run` | Build and launch the application |
| `make check` | Run model tests and the rendering syntax check |
| `make clean` | Delete the `build/` directory |

## Keyboard controls

| Key | Action |
| --- | --- |
| Enter | Start from the title screen |
| P | Pause or resume the entire animation |
| R | Reset to the title screen; press Enter to start again |
| F11 or F | Toggle borderless fullscreen |
| + or = | Increase speed, up to 4× |
| - or _ | Decrease speed, down to 0.25× |
| D | Switch to day mode |
| N | Switch to night mode |
| Esc | Exit |

Restart preserves the lighting mode and speed and clears pause. After parking, the airport background continues animating. The window can also be resized or maximized; the scene keeps its aspect ratio.

## Project structure

```text
src/
  main.cpp                 Drawing, input, and frame loop
  display.h                Window scaling and fullscreen support
  simulation.h             Landing state, timing, and transformations
tests/
  simulation_test.cpp      Simulation tests
  support/graphics.h       Declarations for the rendering syntax check
third_party/winbgim/        Bundled WinBGIm source and author notices
docs/LAB_NOTES.md           Implementation and lab demonstration notes
Makefile                   Build, run, test, and cleanup commands
build/                     Generated library, executable, and tests
```

See [the lab notes](docs/LAB_NOTES.md) for the coordinate system, graphics primitives, transformations, landing state machine, and buffering implementation.

## Troubleshooting

- **`make: command not found`:** install GNU Make. On Windows, run the commands inside the MSYS2 UCRT64 terminal.
- **`x86_64-w64-mingw32-g++: command not found`:** on Ubuntu/Debian, install `g++-mingw-w64-x86-64`.
- **`g++: command not found`:** install `g++` on Linux or the UCRT64 GCC package shown above on Windows.
- **`wine: command not found`:** install Wine on Linux, then retry `make run`.
- **`graphics.h` missing:** build with `make windows` or `make run` from the repository root. These commands include and compile the bundled graphics library.
- **Wine cannot open the executable:** run `make windows` and fix any compilation errors first. The expected output is `build/airport.exe`.
- **No graphics window opens:** check that a graphical desktop is available and Wine is configured correctly. Wine may perform initial setup on its first run.
- **Keys have no effect:** click the graphics window to give it keyboard focus. Input is handled in that window.
- **Using a different MinGW toolchain:** override the compiler and archiver, for example `make windows WINCXX=g++ WINAR=ar`. After changing toolchains, run `make clean` before rebuilding.

WinBGIm is a Windows graphics library. The application is not a native Linux graphics executable; Linux runs the generated Windows application through Wine.
