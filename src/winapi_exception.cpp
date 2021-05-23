#include <winut/winapi_exception.hpp>
#include <winut/errors.hpp>


namespace winut {

    winapi_exception::winapi_exception(const DWORD last_error) : message(get_last_error_message(last_error)), last_error(last_error) {}

    winapi_exception::winapi_exception(const std::wstring& message_prefix, const DWORD last_error)
        : message(message_prefix + L". System code: " + std::to_wstring(last_error) + L". System message: " + get_last_error_message(last_error)), 
        last_error(last_error) 
    {}

    const std::wstring& winapi_exception::get_message() const noexcept {
        return this->message;
    }

    DWORD winapi_exception::get_error_code() const noexcept {
        return this->last_error;
    }

}