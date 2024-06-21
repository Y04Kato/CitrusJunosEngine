#include "ConvertString.h"

std::wstring ConvertString(const std::string& str){
	if (str.empty()){
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0){
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

std::string ConvertString(const std::wstring& str){
	if (str.empty()){
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0){
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}

const char ConvertStringC(const std::string& str) {
	auto&& stringPath = ConvertString(str);
	size_t len = stringPath.length() + 1;
	std::vector<char> vectorStr;
	vectorStr.resize(len);

	std::size_t convertedChars = 0;
	wcstombs_s(&convertedChars, vectorStr.data(), len, stringPath.c_str(), _TRUNCATE);
	const std::string& result = vectorStr.data();

	return *result.c_str();
}

std::string ConvertStringC(const char& c) {
	return &c;
}

void Log(const std::string& message) {
	OutputDebugStringA(message.c_str());
}