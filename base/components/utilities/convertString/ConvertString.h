#pragma once
#include <string>
#include <format>
#include <Windows.h>
#include <vector>

std::wstring ConvertString(const std::string& str);

std::string ConvertString(const std::wstring& str);

const char ConvertStringC(const std::string& str);

std::string ConvertStringC(const char& c);

void Log(const std::string& message);