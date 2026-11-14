# Airport Runway & Airplane Landing Simulation

A complete, asset-free **C++11 / graphics.h / WinBGIm** Computer Graphics lab project. Opens a 1200 × 700 window and draws the airport and aircraft entirely with polygons, lines, ellipses, bars and pixels. No game engine, OpenGL, SDL or SFML is used.

Press **Enter** on the title screen. The aircraft approaches from the left, descends, extends its gear, flares to level flight, touches down, brakes smoothly and parks near terminal gate A4. A complete landing takes approximately 37 seconds at 1×. Clouds, radar, vehicles and lights continue moving after landing. The telemetry is illustrative rather than a physical flight model.

## Files

| File | Purpose |
| --- | --- |
| `src/main.cpp` | WinBGIm drawing helpers, airport, transformed airplane, UI, keyboard input and double-buffered frame loop |
| `src/display.h` | Borderless fullscreen toggle and restoration of the previous window |
| `src/simulation.h` | Graphics-independent animation state, transformations, telemetry and landing dynamics |
| `tests/simulation_test.cpp` | Tests of the complete landing sequence and controls' state operations |
| `tests/support/graphics.h` | Declarations for a Linux **syntax-only** check; not a graphics implementation |
| `Makefile` | Builds bundled WinBGIm, links the application, launches Wine, and runs model checks |
| `third_party/winbgim/` | Real WinBGIm source, preserved with original author notices |
| `docs/LAB_NOTES.md` | Function guide, coordinate system, transformations and demonstration notes |

## Linux: build and run (recommended for this workspace)

The real WinBGIm source is now included in `third_party/winbgim`. No manual
library download or placeholder path is needed. WinBGIm remains a Windows API
library; Linux uses MinGW to create a Windows executable and Wine to display it.
No SDL or other game library is used.

On Debian/Ubuntu, install the tools once:

```sh
sudo apt install g++-mingw-w64-x86-64 wine make
```

From a terminal in this project folder:

```sh
make run
```

This compiles WinBGIm locally, links `build/airport.exe`, and launches it only
if compilation succeeds. Click the graphics window and press Enter.

To build and launch separately:

```sh
make windows
wine build/airport.exe
```

The executable statically links the MinGW C++ runtime, so compiler runtime DLLs
do not have to be copied alongside it. Windows system DLLs are provided by Wine.
A working graphical desktop and Wine installation are required.

`BGI_ROOT=/path/to/winbgim` from the original instructions is no longer used.
The included dependency builds with the same 64-bit compiler as the application.

## Windows: compile and run

Install a C++11-capable MinGW toolchain and GNU Make. Open its MSYS/MinGW shell
in the project directory:

```sh
make windows WINCXX=g++ WINAR=ar
./build/airport.exe
```

The Makefile detects `OS=Windows_NT` and defaults to native MinGW tools on Windows.
For another toolchain, specify `WINCXX` and `WINAR`. Run the application from the
graphics window; the console is not used for input.

## Troubleshooting

- **`graphics.h` missing:** use the current `make windows` command. It includes
  `third_party/winbgim`, which contains the real graphics header and implementation.
- **Wine cannot open `build/airport.exe`:** first run `make windows` and resolve
  any build error. `make run` never starts Wine after a failed build.
- **`/tmp/hybrid-attendance-uv/env` missing at terminal startup:** this comes from
  stale shell initialization unrelated to this project. In `~/.bashrc` and
  `~/.profile`, remove the `. "/tmp/hybrid-attendance-uv/env"` line or replace it
  with `[ ! -f /tmp/hybrid-attendance-uv/env ] || . /tmp/hybrid-attendance-uv/env`.
- **Wine diagnostic messages:** the messages alone do not establish whether the
  application failed. Check whether the graphics window opens. If it does not,
  capture the output from `make run` now that an executable exists.

## Keyboard controls

The title bar includes minimize, maximize/restore and close controls. You can also double-click the title bar to maximize/restore, or drag the window edges to resize. Click the graphics window to give it keyboard focus. Press **F11** (or **F**) to toggle borderless fullscreen on the current monitor. The complete scene scales proportionally to the display; black margins preserve its aspect ratio. Switching modes keeps the landing in progress.

| Key | Action |
| --- | --- |
| Enter | Start landing from the title screen |
| P | Pause/resume the entire animation |
| F11 or F | Toggle fullscreen; press again to restore the previous window |
| R | Reset to the title screen, preserving mode and speed setting |
| + or = | Increase speed, up to 4× |
| - or _ | Decrease speed, down to 0.25× |
| D | Day mode |
| N | Night mode |
| Esc | Close the application |

Day/night and speed can be changed on the title screen or during flight. Restart clears pause. Enter during flight or after landing has no effect; use R then Enter for another landing. Closing the window also exits through WinBGIm's default close behavior.

## Verification

With a native C++11 compiler and GNU Make, no graphics library is needed for:

```sh
make check
```

This compiles and runs the model tests and checks the rendering translation unit using **declaration-only** BGI signatures. Tests cover transformed points, all landing stages at 0.25×/1×/4× with varying frame times, monotonic descent, wheels never going below the runway, contact during rollout, decreasing braking speed, exact parking position, pause, restart and speed limits.

The model checks and syntax checks passed with GCC using `-Wall -Wextra -Wpedantic`. The bundled WinBGIm source and application also compiled and linked successfully with the installed 64-bit MinGW toolchain. Interactive Wine validation requires access to the graphical desktop.

For the demonstration, check Enter → touchdown → parked; press P during descent; resume and switch N/D; try both speed limits; restart while paused; and exit with Esc. Confirm that both graphics pages display cleanly and keyboard focus is on the graphics window.

The program uses the documented [active/visual page buffering mechanism](https://winbgim.codecutter.org/V6_0/doc/swapbuffers.html) and [1200 × 700 `initwindow`](https://winbgim.codecutter.org/V6_0/doc/initwindow.html). It draws each complete frame to the hidden page before displaying it.
# Airport-Runway-Airplane-Landing-Simulation
