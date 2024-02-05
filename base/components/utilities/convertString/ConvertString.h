#pragma once
#include <string>
#include <format>
#include <Windows.h>
#include <vector>

//string → wstring
std::wstring ConvertString(const std::string& str);

//wstring → string
std::string ConvertString(const std::wstring& str);

//string → char
const char ConvertStringC(const std::string& str);

//char → string
std::string ConvertStringC(const char& c);

void Log(const std::string& message);