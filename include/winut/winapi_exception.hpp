#pragma once
#include <Windows.h>
#include <string>

namespace winut {

    class winapi_exception {
    private:
        std::wstring message;
    public:
        winapi_exception();
        winapi_exception(const std::wstring& message);
        winapi_exception(const DWORD last_error);
        winapi_exception(const DWORD last_error, const std::wstring& message_prefix);
    };

}