#if __cplusplus >= 201703L

#include "CreateSpriteList.h"
#include "SpriteList.h"
#include <filesystem>
#include <vector>
#include <fstream>
#include <iostream>
#include <ostream>
namespace fs = std::filesystem;

bool CreateSpriteList(std::string out_name, std::string path)
{
    if (argc == 3)
    {
        SpriteList List;
        for (const auto& entry : fs::directory_iterator(path))
        {
            std::string base_filename = entry.path().string().substr(entry.path().string().find_last_of("/\\") + 1);
            std::string::size_type const p(base_filename.find_last_of('.'));
            if(base_filename.substr(base_filename.find_last_of(".") + 1) != "png") continue;

            std::string file_without_extension = base_filename.substr(0, p);
            CString<64> FileName;
            FileName.SetString(const_cast<char*>(file_without_extension.c_str()));
            List.NameList.push_back(FileName);
            ++List.SpriteCount;
        }

        std::cout << "Total sprite count: " << List.SpriteCount << std::endl;

        std::ofstream OutFile(out_name, std::ios::out | std::ios::binary);
        if (!OutFile)
        {
            std::cout << "Cannot write to file " << out_name << std::endl;
        }
        OutFile.write(List.Magic, 4);
        OutFile.write((char*)&List.SpriteCount, 4);
        for (CString<64> Name : List.NameList)
        {
            OutFile.write(Name.GetString(), 64);
        }
        OutFile.close();

        std::cout << "Wrote sprite list to file " << out_name << std::endl;

        return true;
    }

    return false;
}
#endif