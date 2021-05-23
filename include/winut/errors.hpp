#pragma once
#include <windows.h>
#include <string>

namespace winut {

    std::wstring get_last_error_message(const DWORD last_error_code);
    std::wstring get_last_error_message();

}