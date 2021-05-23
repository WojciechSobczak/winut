#include <winut/process.hpp>

#include <windows.h>
#include <psapi.h>
#include <winut/winapi_exception.hpp>
#include <algorithm>
#include <cwctype>

namespace winut {
    std::vector<DWORD> get_all_processes_pids(uint32_t pids_count_to_preallocate) {
        std::vector<DWORD> output(pids_count_to_preallocate);
        DWORD pids_bytes_returned = 0;
        if (!EnumProcesses(output.data(), output.size() * sizeof(DWORD), &pids_bytes_returned)) {
            throw winapi_exception(L"EnumProcesses() failed");
        }
        output.resize(pids_bytes_returned / sizeof(DWORD));
        return output;
    }
    process_details get_process_details(DWORD pid) {
        const HANDLE process_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
        if (process_handle == NULL) {
            throw winapi_exception(L"OpenProcess() failed");
        }

        constexpr uint16_t modules_buffer_size = 1024;
        HMODULE modules_handles[modules_buffer_size];
        DWORD bytes_returned = 0;
        if (!EnumProcessModules(process_handle, modules_handles, modules_buffer_size * sizeof(HMODULE), &bytes_returned)) {
            throw winapi_exception(L"EnumProcessModules() failed");
        }

        process_details output;
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
                throw winapi_exception(L"GetModuleBaseNameW() failed");
            }

            std::wstring moduleBaseName(base_name_buffer, charactersReturned);
            std::wstring moduleBaseNameLower = moduleBaseName;
            std::transform(moduleBaseName.begin(), moduleBaseName.end(), moduleBaseNameLower.begin(), std::towlower);

            process_module* current_module = nullptr;
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
                throw winapi_exception(L"GetModuleInformation() failed");
            }
            current_module->entry_point = module_info.EntryPoint;
            current_module->load_address = module_info.lpBaseOfDll;
            current_module->size_in_bytes = module_info.SizeOfImage;
        }

        return output;
    }

}