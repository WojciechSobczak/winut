#include <Windows.h>
#include <winut/errors.hpp>
#include <memory>
#include <iostream>
#include <cpput/defer.hpp>
#include <cpput/dummy_var_macro.hpp>

namespace winut {


    std::wstring get_last_error_message() {
        return get_last_error_message(GetLastError());
    }

    std::wstring get_last_error_message(const DWORD last_error_code) {
        WCHAR* returned_message_buffer = nullptr;
        auto _ = cpput::defer([returned_message_buffer]() noexcept {
            LocalFree(returned_message_buffer);
        });

        DWORD returned_message_buffer_size = FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            last_error_code,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)&returned_message_buffer,
            0,
            NULL
        );

        if (returned_message_buffer_size == 0) {
            std::cerr << "winut: FormatMessage() failed. Error code: " << GetLastError() << std::endl;
            throw std::exception("winut: FormatMessage() failed");
        }

        return std::wstring(returned_message_buffer, returned_message_buffer_size);
    }


}