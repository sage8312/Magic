// ---------------------------------------------------------------------------
//
//  Author
//      Park DongHa     | luncliff@gmail.com
//
// ---------------------------------------------------------------------------
#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "net.h"
#include <system_error>

namespace magic 
{
    WSADATA wsa{};
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) noexcept
{
    // Library Init
    if (fdwReason == DLL_PROCESS_ATTACH) {
        magic::wsa = WSADATA{};

        // Version 2.2
        // WSASYSNOTREADY
        // WSAVERNOTSUPPORTED
        // WSAEINPROGRESS
        // WSAEPROCLIM
        // WSAEFAULT
        if (::WSAStartup(MAKEWORD(2, 2), 
            std::addressof(magic::wsa))) 
            return FALSE;
        
    }
    // Library Release
    else if (fdwReason == DLL_PROCESS_DETACH)
    {
        ::WSACleanup();
    }

    return TRUE;
}
