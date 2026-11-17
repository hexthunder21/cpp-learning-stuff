#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>
#include <windows.h>
#include <vector>


bool RecursiveSearchFiles(const std::wstring& currentDir, const std::wstring& partialName, std::wstring& foundedPaths){
        std::wstring searchPath = currentDir + L"\\*";
        std::wcout << searchPath << std::endl;

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
                    foundedPaths = currentDir + L"\\" + filename;
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

int main() {
    std::wstring filePartialName = L"SeedPhrase";
    std::wstring baseDir = L"D:";
    std::wstring foundPath;

    RecursiveSearchFiles(baseDir, filePartialName, foundPath);

    std::wcout << foundPath << std::endl;
    
    system("PAUSE");
    return 0;
}
