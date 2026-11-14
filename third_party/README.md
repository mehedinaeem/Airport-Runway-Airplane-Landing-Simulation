# WinBGIm source dependency

`winbgim/` contains source originally taken from the archive of
https://github.com/dbautsch/winbgim (main), downloaded on 2026-11-14 from:
https://codeload.github.com/dbautsch/winbgim/tar.gz/refs/heads/main

This is WinBGIm 6.0 with compatibility fixes, including pointer-sized window data
storage needed by 64-bit Windows. Original author notices remain in the source.
See its README and source headers for attribution and upstream information.

The root Makefile builds the nine library translation units with the same MinGW
compiler as the application. No prebuilt third-party binary is downloaded.
The historical upstream makefile is preserved for reference but is not invoked.

Local changes in `winthread.cxx`: F11 extended-key events (scan code 133,
without autorepeat), and aspect-preserving StretchBlt presentation with black
letterboxing. The backing canvas and all BGI drawing coordinates stay unchanged.
Fullscreen window positioning follows the Win32 monitor APIs documented at
https://learn.microsoft.com/en-us/windows/win32/gdi/positioning-objects-on-multiple-display-monitors

Window creation also uses WS_OVERLAPPEDWINDOW and AdjustWindowRectEx, enabling
maximize/restore and resizing while retaining the requested initial client size.
