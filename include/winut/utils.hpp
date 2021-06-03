#pragma once
#include <inttypes.h>
#include <Windows.h>

namespace winut {
    class handle_guard {
    private:
        HANDLE handle;
    public:
        handle_guard(HANDLE handle);
        ~handle_guard();
        HANDLE get();
        bool is_invalid();
        bool is_valid();
        bool is_null();
        bool is_not_null();
    };

    constexpr uint64_t filetime_to_uint64(const FILETIME& filetime) noexcept {
        ULARGE_INTEGER buffer;
        buffer.LowPart = filetime.dwLowDateTime;
        buffer.HighPart = filetime.dwHighDateTime;
        return buffer.QuadPart;
    }

};

