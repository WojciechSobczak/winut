#include <winut/winapi_exception.hpp>
#include <winut/errors.hpp>


namespace winut {

    winapi_exception::winapi_exception() {}

    winapi_exception::winapi_exception(const std::wstring& message) : message(message) {}

    winapi_exception::winapi_exception(const DWORD last_error) : message(get_last_error_message(last_error)) {}

    winapi_exception::winapi_exception(const DWORD last_error, const std::wstring& message_prefix)
        : message(message_prefix + L". System message: " + get_last_error_message(last_error)) 
    {}


}