#include <stdint.h>
#include <Windows.h>


wchar_t *get_error_str(DWORD error) {
    wchar_t *err_msg;
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                       FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, error,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&err_msg,
                   0, NULL);
    return err_msg;
}