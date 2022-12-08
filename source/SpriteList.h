#pragma once

#include <vector>
#include "CString.h"

struct SpriteList
{
    char* Magic = "SPLS";
    unsigned int SpriteCount = 0;
    std::vector<CString<64>> NameList;
};
