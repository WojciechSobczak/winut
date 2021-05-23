#pragma once
#include <Windows.h>
#include <string>

namespace winut {

    class winapi_exception {
    private:
        std::wstring message;
        DWORD last_error;

    public:
        winapi_exception(const DWORD last_error);
        winapi_exception(const std::wstring& message_prefix, const DWORD last_error = GetLastError());

        const std::wstring& get_message() const noexcept;
        DWORD get_error_code() const noexcept;
    };

}