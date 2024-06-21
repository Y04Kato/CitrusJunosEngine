#pragma once
#include "DirectXCommon.h"

class ShaderCompiler{
public:

	static ShaderCompiler* GetInstance();

	void Initialize();
	IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile);

private:
	ShaderCompiler() = default;
	~ShaderCompiler() = default;
	ShaderCompiler(const ShaderCompiler& obj) = delete;
	ShaderCompiler& operator=(const ShaderCompiler& obj) = delete;

	HRESULT hr_;

	IDxcUtils* dxcUtils_ = nullptr;
	IDxcCompiler3* dxcCompiler_ = nullptr;
	IDxcIncludeHandler* includeHandler_ = nullptr;

	IDxcBlob* shaderBlob_ = nullptr;
	IDxcBlobUtf8* shaderError_ = nullptr;

private:
	void InitializeDxcCompiler();
};