#pragma once
#include <windows.h>
#include <vector>
#include <string>

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

    std::vector<DWORD> get_all_processes_pids(uint32_t pids_count_to_preallocate = 2048);

    process_details get_process_details(DWORD pid);


}