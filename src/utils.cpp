#include <winut/utils.hpp>

winut::handle_guard::handle_guard(HANDLE handle) noexcept : handle(handle) {};

winut::handle_guard::handle_guard(winut::handle_guard&& other) noexcept : handle(other.handle) {
    other.handle = NULL;
}

winut::handle_guard::~handle_guard() noexcept {
    if (this->is_valid()) {
        CloseHandle(this->handle);
    }
}

HANDLE winut::handle_guard::get() const noexcept {
    return this->handle;
};

bool winut::handle_guard::is_invalid() const noexcept {
    return this->handle == INVALID_HANDLE_VALUE;
}

bool winut::handle_guard::is_valid() const noexcept {
    return this->handle != INVALID_HANDLE_VALUE;
}

bool winut::handle_guard::is_null() const noexcept {
    return this->handle == NULL;
}

bool winut::handle_guard::is_not_null() const noexcept {
    return this->handle != NULL;
}
