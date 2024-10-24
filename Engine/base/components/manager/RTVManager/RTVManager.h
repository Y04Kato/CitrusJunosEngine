/**
 * @file RTVManager.h
 * @brief RTVの割り当て、その後の管理を行う
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "DirectXCommon.h"
#include <DirectXTex/DirectXTex.h>

class RTVManager{
public:
	static RTVManager* GetInstance();

	//空いているRTVのディスクリプターハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle();

	uint32_t descriptorSizeRTV;
private:
	RTVManager() {
		descriptorSizeRTV = DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	~RTVManager() = default;
	RTVManager(const RTVManager& obj) = delete;
	RTVManager& operator=(const RTVManager& obj) = delete;

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle();

	uint32_t RTVValue = 0;
};