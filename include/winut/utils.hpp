#pragma once
#include <inttypes.h>
#include <Windows.h>

namespace winut {
    class handle_guard {
    private:
        HANDLE handle;
    public:
        handle_guard(HANDLE handle) noexcept;
        handle_guard(handle_guard&& handle) noexcept;
        ~handle_guard() noexcept;
        HANDLE get() const noexcept;
        bool is_invalid() const noexcept;
        bool is_valid() const noexcept;
        bool is_null() const noexcept;
        bool is_not_null() const noexcept;
    };

    constexpr uint64_t filetime_to_uint64(const FILETIME& filetime) noexcept {
        ULARGE_INTEGER buffer;
        buffer.LowPart = filetime.dwLowDateTime;
        buffer.HighPart = filetime.dwHighDateTime;
        return buffer.QuadPart;
    }
};

