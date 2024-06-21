#include "RTVManager.h"

RTVManager* RTVManager::GetInstance(){
	static RTVManager instance;
	return &instance;
}

D3D12_CPU_DESCRIPTOR_HANDLE RTVManager::GetDescriptorHandle(){
	D3D12_CPU_DESCRIPTOR_HANDLE result;
	result = GetCPUDescriptorHandle();
	RTVValue += 1;
	return result;
}

D3D12_CPU_DESCRIPTOR_HANDLE RTVManager::GetCPUDescriptorHandle(){
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = DirectXCommon::GetInstance()->GetSrvDescriptiorHeap()->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSizeRTV * RTVValue);
	return handleCPU;
}
