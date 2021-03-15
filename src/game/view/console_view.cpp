#if !defined(NDEBUG)

#include "console_view.h"

#if defined(_WIN64)
# include <windows.h>

namespace urus
{
    void ConsoleView::updateViewPositionAndBounds() const
    {
        HWND handleWnd = GetConsoleWindow();
        MoveWindow(handleWnd, mX, mY, mWidth, mHeight, TRUE /* repaint */);
    }
} // namespace urus
#endif // _WIN64

#if defined(__APPLE__)
namespace urus
{
    void ConsoleView::updateViewPositionAndBounds() const
    {
        // noop
    }
} // namespace urus
#endif // __APPLE__

#endif // NDEBUG