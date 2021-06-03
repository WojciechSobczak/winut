#include <winut/utils.hpp>

namespace winut {

    handle_guard::handle_guard(HANDLE handle) : handle(handle) {};

    handle_guard::~handle_guard() {
        if (this->is_valid()) {
            CloseHandle(this->handle);
        }
    }

    HANDLE handle_guard::get() {
        return this->handle;
    };

    bool handle_guard::is_invalid() {
        return this->handle == INVALID_HANDLE_VALUE;
    }

    bool handle_guard::is_valid() {
        return this->handle != INVALID_HANDLE_VALUE;
    }

    bool handle_guard::is_null() {
        return this->handle == NULL;
    }

    bool handle_guard::is_not_null() {
        return this->handle != NULL;
    }
}
