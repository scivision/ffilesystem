#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <iostream>
#include <string>
#include <cstdlib>

#include <filesystem>


// Function to "touch" a file (update modification time or create if not exists)
// for paths potentially longer than MAX_PATH.
// std::ofstream does not work for Windows long paths for MSVC, MinGW, etc.

bool TouchFile(std::wstring_view path) {
    // Prepend the path with \\?\ to support long paths (or regular paths).

    HANDLE hFile = CreateFileW(
        path.data(),               // File name (long path)
        FILE_WRITE_ATTRIBUTES,          // Desired access: to change attributes (like timestamps)
                                        // Use GENERIC_WRITE if you also want to write content or ensure creation.
                                        // For just touching, FILE_WRITE_ATTRIBUTES is sufficient if the file exists.
                                        // To create if not existing, OPEN_ALWAYS needs some write access.
                                        // Let's use FILE_WRITE_ATTRIBUTES | GENERIC_WRITE for robustness in creation.
                                        // Update: For SetFileTime, FILE_WRITE_ATTRIBUTES is enough.
                                        // However, to ensure creation with OPEN_ALWAYS, some write access might be implicitly needed
                                        // by the system, or it might just create an empty file.
                                        // Let's stick to FILE_WRITE_ATTRIBUTES for the primary goal of updating timestamps.
                                        // If OPEN_ALWAYS creates the file, it will have default timestamps.
                                        // We then update them.

        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // Share mode
        nullptr,                           // Security attributes
        OPEN_ALWAYS,                    // Creation disposition:
                                        // Opens a file, always.
                                        // If the specified file exists, the function succeeds and the last-error code is set to ERROR_ALREADY_EXISTS (though this is not an error).
                                        // If the specified file does not exist and is a valid path to a writable location, the function creates the file and the last-error code is set to zero.
        FILE_ATTRIBUTE_NORMAL,          // Flags and attributes
        NULL                            // Template file
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Error opening or creating file: " << path << L" (Error code: " << GetLastError() << L")\n";
        return false;
    }
    CloseHandle(hFile);

    std::filesystem::last_write_time(path, std::filesystem::file_time_type::clock::now());

    return true;
}

int main() {
    // Construct a path longer than 260 characters.
    auto const baseDir = std::filesystem::temp_directory_path();
    std::filesystem::path longName;

    for (int i = 0; i < 15; ++i)
      longName /= L"a_very_long_name_component_" + std::to_wstring(i);

    auto const dir = (LR"(\\?\)" + baseDir.wstring()) / longName;
    std::wstring path = dir / L"over260charsTotal.txt";

    // create_directories also needs prepended \\?\ or it will fail
    std::filesystem::create_directories(dir);

    std::wcout << L"Touch file (length " << path.length() << L"): " << path << L"\n";

    if (!TouchFile(path)) {
        std::wcerr << L"Failed to touch file.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
