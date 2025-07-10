/**
 * @file DebugScene.h
 * @brief ゲームの動作テストを行うシーンを管理
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "Iscene.h"

#include "dataReceipt/DataReceipt.h"

#include "Receipt3D.h"

class DebugScene :public Iscene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawUI() override;
	void DrawPostEffect() override;
	void Finalize() override;

private:
	//DataReceipt
	DataReceipt datareceipt_;
	std::unique_ptr<Receipt3D> receipt3D_;
};