/**
 * @file SRVManager.h
 * @brief SRVの割り当て、その後の管理を行う
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "DirectXCommon.h"
#include <DirectXTex/DirectXTex.h>

struct DESCRIPTERHANDLE {
	CD3DX12_CPU_DESCRIPTOR_HANDLE CPU;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPU;
};

class SRVManager{
public:
	static SRVManager* GetInstance();

	//空いているSRVのディスクリプターハンドルを取得
	DESCRIPTERHANDLE GetDescriptorHandle();
	
	//現在のSRVの数を取得
	uint32_t GetSRVValue();

	uint32_t descriptorSizeSRV;

private:
	SRVManager() {
		descriptorSizeSRV = DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	~SRVManager() = default;
	SRVManager(const SRVManager& obj) = delete;
	SRVManager& operator=(const SRVManager& obj) = delete;

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle();
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle();

	uint32_t SRVValue = 1;
};