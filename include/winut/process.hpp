#pragma once
#include <windows.h>
#include <Tlhelp32.h>
#include <vector>
#include <string>
#include <functional>
#include <optional>

namespace winut {

    struct process_module {
        std::wstring name = L"<NO NAME>";
        uint64_t size_in_bytes = 0;
        void* load_address = nullptr;
        void* entry_point = nullptr;
    };

    struct dll_module : public process_module {};
    struct other_process_module : public process_module {};

    struct process_details {
        process_module process_module{};
        std::vector<dll_module> dlls_loaded{};
        std::vector<other_process_module> other_modules_loaded{};
    };

    std::vector<DWORD> get_all_processes_pids(uint32_t process_ids_count_to_preallocate = 2048);

    process_details get_process_details(DWORD process_id);

    struct thread_entry {
        DWORD size;
        DWORD thread_id;
        DWORD owner_process_id;
        LONG kernel_base_priority_level;

        explicit thread_entry(const THREADENTRY32& thread_entry);
    };

    void iterate_through_all_system_threads(HANDLE snapshot_handle, std::function<void(thread_entry&&)> on_thread_iteration);

    std::vector<thread_entry> get_snapshot_threads(HANDLE snapshot_handle);

    struct thread_times {
        uint64_t creation_time = 0;
        uint64_t exit_time = 0;
        uint64_t kernel_time = 0;
        uint64_t user_time = 0;
    };

    thread_times get_thread_times(HANDLE thread_handle);

    std::optional<DWORD> get_process_main_thread_id(DWORD process_id);

}