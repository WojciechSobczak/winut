#include <winut/process.hpp>
#include <winut/utils.hpp>

#include <windows.h>
#include <psapi.h>
#include <winut/winapi_exception.hpp>
#include <algorithm>
#include <cwctype>
#include <utility>

#undef max



std::vector<DWORD> winut::get_all_processes_pids(uint32_t process_ids_count_to_preallocate) {
    std::vector<DWORD> output(process_ids_count_to_preallocate);
    DWORD pids_bytes_returned = 0;
    if (!EnumProcesses(output.data(), output.size() * sizeof(DWORD), &pids_bytes_returned)) {
        throw winut::winapi_exception(L"EnumProcesses() failed");
    }
    output.resize(pids_bytes_returned / sizeof(DWORD));
    return output;
}

winut::process_details winut::get_process_details(DWORD process_id) {
    const HANDLE process_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id);
    if (process_handle == NULL) {
        throw winut::winapi_exception(L"OpenProcess() failed");
    }

    constexpr uint16_t modules_buffer_size = 1024;
    HMODULE modules_handles[modules_buffer_size];
    DWORD bytes_returned = 0;
    if (!EnumProcessModules(process_handle, modules_handles, modules_buffer_size * sizeof(HMODULE), &bytes_returned)) {
        throw winut::winapi_exception(L"EnumProcessModules() failed");
    }

    winut::process_details output;
    for (uint64_t i = 0; i < bytes_returned / sizeof(HMODULE); i++) {
        constexpr uint16_t base_name_buffer_size = 1024;
        WCHAR base_name_buffer[base_name_buffer_size];
        DWORD charactersReturned = GetModuleBaseNameW(
            process_handle,
            modules_handles[i],
            base_name_buffer,
            base_name_buffer_size
        );

        if (charactersReturned == 0) {
            throw winut::winapi_exception(L"GetModuleBaseNameW() failed");
        }

        std::wstring moduleBaseName(base_name_buffer, charactersReturned);
        std::wstring moduleBaseNameLower = moduleBaseName;
        std::transform(moduleBaseName.begin(), moduleBaseName.end(), moduleBaseNameLower.begin(), std::towlower);

        winut::process_module* current_module = nullptr;
        if (moduleBaseNameLower.ends_with(L".exe")) {
            current_module = &output.process_module;
        } 
        else if (moduleBaseNameLower.ends_with(L".dll")) {
            output.dlls_loaded.push_back({});
            current_module = &output.dlls_loaded[output.dlls_loaded.size() - 1];
        } 
        else {
            output.other_modules_loaded.push_back({});
            current_module = &output.other_modules_loaded[output.other_modules_loaded.size() - 1];
        }
        current_module->name = moduleBaseName;

        MODULEINFO module_info;
        BOOL success = GetModuleInformation(
            process_handle,
            modules_handles[i],
            &module_info,
            sizeof(MODULEINFO)
        );
        if (success == 0) {
            throw winut::winapi_exception(L"GetModuleInformation() failed");
        }
        current_module->entry_point = module_info.EntryPoint;
        current_module->load_address = module_info.lpBaseOfDll;
        current_module->size_in_bytes = module_info.SizeOfImage;
    }

    return output;
}

winut::thread_entry::thread_entry(const THREADENTRY32& thread_entry) {
    this->size = thread_entry.dwSize;
    this->thread_id = thread_entry.th32ThreadID;
    this->owner_process_id = thread_entry.th32OwnerProcessID;
    this->kernel_base_priority_level = thread_entry.tpBasePri;
}

void winut::iterate_through_all_system_threads(HANDLE snapshot_handle, std::function<void(winut::thread_entry&&)> on_thread_iteration) {
    THREADENTRY32 win_thread_entry;
    win_thread_entry.dwSize = sizeof(THREADENTRY32);
    bool next_thread_loaded = Thread32First(snapshot_handle, &win_thread_entry);
    DWORD possible_thread_32_first_error = GetLastError();
    if (possible_thread_32_first_error == ERROR_NO_MORE_FILES) {
        return;
    }

    while (next_thread_loaded) {
        on_thread_iteration(thread_entry{ win_thread_entry });
        next_thread_loaded = Thread32Next(snapshot_handle, &win_thread_entry);
        DWORD possible_thread_32_next_error = GetLastError();
        if (possible_thread_32_next_error == ERROR_NO_MORE_FILES) {
            return;
        }
    }
}

std::vector<winut::thread_entry> winut::get_snapshot_threads(HANDLE snapshot_handle) {
    std::vector<winut::thread_entry> output;
    winut::iterate_through_all_system_threads(snapshot_handle, [&output](winut::thread_entry&& thread_entry) {
        output.emplace_back(std::forward<winut::thread_entry>(thread_entry));
    });
    return output;
}


winut::thread_times winut::get_thread_times(HANDLE thread_handle) {
    FILETIME thread_filetimes[4] = { 0 };
    if (!GetThreadTimes(thread_handle, &thread_filetimes[0], &thread_filetimes[1], &thread_filetimes[2], &thread_filetimes[3])) {
        throw winut::winapi_exception(L"GetThreadTimes() failed");
    }
    return winut::thread_times{
        .creation_time = winut::filetime_to_uint64(thread_filetimes[0]),
        .exit_time = winut::filetime_to_uint64(thread_filetimes[1]),
        .kernel_time = winut::filetime_to_uint64(thread_filetimes[2]),
        .user_time = winut::filetime_to_uint64(thread_filetimes[3])
    };
}

std::optional<DWORD> winut::get_process_main_thread_id(DWORD process_id) {
    winut::handle_guard snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, process_id);
    if (snapshot_handle.is_invalid()) {
        throw winut::winapi_exception(L"CreateToolhelp32Snapshot() failed");
    }

    uint64_t first_thread_create_time = std::numeric_limits<uint64_t>::max();
    std::optional<DWORD> main_thread_id{};
    winut::iterate_through_all_system_threads(snapshot_handle.get(), [&first_thread_create_time, &main_thread_id, process_id](winut::thread_entry&& thread_entry) {
        if (thread_entry.owner_process_id != process_id) {
            return;
        }

        winut::handle_guard currently_checked_thread = OpenThread(THREAD_QUERY_INFORMATION, TRUE, thread_entry.thread_id);
        if (currently_checked_thread.is_null()) {
            throw winut::winapi_exception(L"OpenThread() failed");
        }

        winut::thread_times thread_times = winut::get_thread_times(currently_checked_thread.get());
        if (thread_times.creation_time != 0 && thread_times.creation_time < first_thread_create_time) {
            first_thread_create_time = thread_times.creation_time;
            main_thread_id = thread_entry.thread_id;
        }
    });

    return main_thread_id;
}

