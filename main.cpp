#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>
#include <windows.h>
#include <vector>


bool RecursiveSearchFiles(const std::wstring& currentDir, const std::wstring& partialName, std::vector<std::wstring>& foundedPaths){
        std::wstring searchPath = currentDir + L"\\*";

        WIN32_FIND_DATAW FindData{};
        HANDLE hFind = FindFirstFileW(searchPath.c_str(), &FindData);

        if (hFind == INVALID_HANDLE_VALUE){
            std::cout << "[!1] Error: " << GetLastError() << std::endl;
            return false;
        }

        do {
            if (!(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
                std::wstring filename = FindData.cFileName;
                if (filename.find(partialName) != std::wstring::npos){
                    foundedPaths.push_back(currentDir + L"\\" + filename);
                    FindClose(hFind);
                    return true;
                }
            }
        } while (FindNextFileW(hFind, &FindData));
        
        FindClose(hFind);

        hFind = FindFirstFileW(searchPath.c_str(), &FindData);

        if (hFind == INVALID_HANDLE_VALUE){
            std::cout << "[!2] Error: " << GetLastError() << std::endl;
            return false;
        }

        do {
            if (FindData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY){
                std::wstring subDirName = FindData.cFileName;
                if (subDirName == L"." || subDirName == L"..") 
                    continue;
                if (FindData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) 
                    continue;

                std::wstring nextDir = currentDir + L"\\" + subDirName;

                if (RecursiveSearchFiles(nextDir, partialName, foundedPaths)){
                    FindClose(hFind);
                    return true;
                }
            }
        } while (FindNextFileW(hFind, &FindData));

        FindClose(hFind);
        return false;
    }

void ReadFiles(const wchar_t* filePath){
    HANDLE hFile = CreateFileW(
        filePath,
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (hFile == INVALID_HANDLE_VALUE)
        return;

    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize)){
        CloseHandle(hFile);
        return;
    }

    std::vector<char> buffer(static_cast<size_t>(fileSize.QuadPart) + 1, 0);
    DWORD bytesRead = 0;

    BOOL success = ReadFile(
        hFile,
        buffer.data(),
        static_cast<DWORD>(fileSize.QuadPart),
        &bytesRead,
        nullptr
    );

    if (!success){
        CloseHandle(hFile);
        return;
    }

    CloseHandle(hFile);
    std::cout << "Read " << bytesRead << " bytes:\n" << buffer.data() << "\n";
}

int main() {
    std::vector<std::wstring> filePartialName = { 
        L"SeedPhrase",
        //L"passwords"
    };
    std::wstring baseDir = L"D:";
    std::vector<std::wstring> foundPath;
    size_t fPT = filePartialName.size();
    
    for (int i = 0; i < fPT; i++){
        if (RecursiveSearchFiles(baseDir, filePartialName[i], foundPath)){
            std::wcout << foundPath[i] << std::endl; 
        } else {
            std::wcout << "[-] The file containing the {" 
            << filePartialName[i] 
            << "} section was not found!" << std::endl;
        }
    }

    for (int i = 0; i < fPT; i++) {
        const wchar_t* wcstr = foundPath[i].c_str();
        ReadFiles(wcstr);
    }    

    system("PAUSE");
    return 0;
}
