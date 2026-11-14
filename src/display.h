#ifndef AIRPORT_DISPLAY_H
#define AIRPORT_DISPLAY_H

#ifdef _WIN32
#include "winbgitypes.h"
#endif

namespace airport {
// Borderless fullscreen keeps the fixed BGI drawing canvas. The presentation
// path scales the completed frame to the client area with letterboxing.
class Display {
#ifdef _WIN32
    HWND window;
    LONG_PTR savedStyle = 0;
    WINDOWPLACEMENT savedPlacement{};
    bool fullscreen = false;
#endif
public:
    Display() {
#ifdef _WIN32
        window = BGI__GetWindowDataPtr()->hWnd;
        savedPlacement.length = sizeof(savedPlacement);
#endif
    }
    void toggleFullscreen() {
#ifdef _WIN32
        if (!fullscreen) {
            MONITORINFO monitor{};
            monitor.cbSize = sizeof(monitor);
            if (!GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &monitor)
                || !GetWindowPlacement(window, &savedPlacement)) return;
            savedStyle = GetWindowLongPtr(window, GWL_STYLE);
            SetWindowLongPtr(window, GWL_STYLE, savedStyle & ~WS_OVERLAPPEDWINDOW);
            const RECT& r = monitor.rcMonitor;
            SetWindowPos(window, HWND_TOP, r.left, r.top, r.right-r.left, r.bottom-r.top,
                         SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
            fullscreen = true;
        } else {
            SetWindowLongPtr(window, GWL_STYLE, savedStyle);
            SetWindowPlacement(window, &savedPlacement);
            SetWindowPos(window, NULL, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            fullscreen = false;
        }
        InvalidateRect(window, NULL, FALSE);
#endif
    }
};
} // namespace airport
#endif
