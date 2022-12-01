#pragma once

#include <stdio.h>
#include <string.h>

template<int Size>
class CString
{
	char String[Size] = { 0 };
public:
	CString()
	{
		memset(String, 0, Size);
	}

	void SetString(char* InString)
	{
		memset(String, 0, Size);
		strcpy(String, InString);
	}

	char* GetString()
	{
		return String;
	}
};
