/**
 * @file Field.h
 * @brief ステージ全般の機能を管理
 * @author KATO
 * @date 2025/04/18
 */

#include "Field.h"

void Field::Initialize(Model* model) {
	//Input
	input_ = Input::GetInstance();

	assert(model != nullptr);
	model_ = model;

	worldTransform_.Initialize();

	//各指定したブロックを生成
	for (int i = 0;i < verticalSize_;i++) {
		for (int j = 0;j < horizontalSize_;j++) {
			CreateBlocks(i, j);
		}
	}

	//Test
	block_.model.Initialize(model_->modelData_, model_->modelData_.textureIndex);
	block_.model.SetDirectionalLightFlag(true, 3);

	block_.world.Initialize();

	block_.world.translation_ = { 0.0f,blockSize_ * 2.0f + block_.world.scale_.num[1],0.0f };
	block_.world.rotation_ = { 0.0f,0.0f,0.0f };
	block_.world.scale_ = { 0.5f,0.5f,0.5f };

	block_.color = { 1.0f,0.0f,0.0f,1.0f };

	block_.massLocation = { -1,-1 };
}

void Field::Update() {
	ImGui::Begin("TestOperate");
	ImGui::DragFloat("BlockWidth", &blockWidth_);
	ImGui::DragFloat2("nowPos_", &nowPos_.num[0]);
	ImGui::Text("NowPosBlock%d:%d", GetBlockAt(nowPos_.num[0], nowPos_.num[1]).x, GetBlockAt(nowPos_.num[0], nowPos_.num[1]).y);
	ImGui::DragFloat("Radius", &radius_);
	ImGui::DragFloat("DeltaY", &deltaY_);
	ImGui::Text("TestRaiseBlocksAround:Ekey");
	ImGui::Text("TestRaiseBlocksAroundWithAttenuation:Rkey");
	ImGui::End();

	//現在のnowPos_の位置からradius_範囲をdeltaY_分下げる
	if (input_->TriggerKey(DIK_E)) {
		RaiseBlocksAround(GetBlockAt(nowPos_.num[0], nowPos_.num[1]), radius_, deltaY_);
	}
	if (input_->TriggerKey(DIK_R)) {
		RaiseBlocksAroundWithAttenuation(GetBlockAt(nowPos_.num[0], nowPos_.num[1]), radius_, deltaY_);
	}

	//各ブロックの高さに応じて色を変更
	ColorAdjustmentByHeight(highColor_, lowColor_, 0.0f, 2.0f);

	//現在のnowPos_に対応するブロックを赤くする
	VectorInt2 selected = GetBlockAt(nowPos_.num[0], nowPos_.num[1]);
	for (Block& block : blocks_) {
		if (block.massLocation.x == selected.x && block.massLocation.y == selected.y) {
			block.color = { 1.0f, 0.0f, 0.0f, 1.0f };//赤
		}
	}

	//ブロック間隔の更新
	if (blockWidth_ != prevBlockWidth_) {
		for (Block& block : blocks_) {
			//ステージの中央からのオフセットを計算
			float offsetX = (horizontalSize_ - 1) * 0.5f * blockWidth_;
			float offsetZ = (verticalSize_ - 1) * 0.5f * blockWidth_;

			//ブロックの配置座標を計算
			block.world.translation_ = {
				centerBlockPos_.num[0] + (block.massLocation.x * blockWidth_) - offsetX,
				centerBlockPos_.num[1],
				centerBlockPos_.num[2] + (block.massLocation.y * blockWidth_) - offsetZ
			};
		}

		//更新
		prevBlockWidth_ = blockWidth_;
	}

	//各ブロックの行列更新
	for (Block& block : blocks_) {
		block.world.UpdateMatrix();
	}


	block_.world.translation_ = { nowPos_.num[0],blockSize_ * 2.0f + block_.world.scale_.num[1], nowPos_.num[1] };
	block_.world.UpdateMatrix();
}

void Field::Draw(const ViewProjection& viewProjection) {
	for (Block& block : blocks_) {
		block.model.Draw(block.world, viewProjection, block.color);
	}

	block_.model.Draw(block_.world, viewProjection, block_.color);
}

void Field::Finalize() {
	blocks_.clear();
}

void Field::CreateBlocks(const int x, const int z) {
	Block block;

	//モデルの設定
	block.model.Initialize(model_->modelData_, model_->modelData_.textureIndex);
	block.model.SetDirectionalLightFlag(true, 3);

	block.world.Initialize();

	//ステージの中央からのオフセットを計算
	float offsetX = (horizontalSize_ - 1) * 0.5f * blockWidth_;
	float offsetZ = (verticalSize_ - 1) * 0.5f * blockWidth_;

	//ブロックの配置座標を計算
	block.world.translation_ = {
		centerBlockPos_.num[0] + (x * blockWidth_) - offsetX,
		centerBlockPos_.num[1],
		centerBlockPos_.num[2] + (z * blockWidth_) - offsetZ
	};

	block.world.rotation_ = { 0.0f,0.0f,0.0f };
	block.world.scale_ = { 1.0f,1.0f,1.0f };

	block.color = { 1.0f,1.0f,1.0f,1.0f };

	//マス位置xとzを保存
	block.massLocation = { x,z };

	blocks_.push_back(block);
}

VectorInt2 Field::GetBlockAt(float x, float z) {
	for (Block& block : blocks_) {
		const Vector3& blockPos = block.world.translation_;

		//各軸面上でブロックの中心からBlockSize_の範囲か確認
		if (std::abs(blockPos.num[0] - x) <= blockSize_ &&
			std::abs(blockPos.num[2] - z) <= blockSize_) {
			return block.massLocation;
		}
	}

	return VectorInt2{ -1,-1 };//見つからなければ適当な値
}

void Field::RaiseBlocksAround(const VectorInt2& center, float radius, float deltaY) {
	//基準となる中心ブロックのワールド座標を取得
	Vector3 centerPos{};
	bool found = false;

	for (const Block& block : blocks_) {
		if (block.massLocation.x == center.x && block.massLocation.y == center.y) {
			centerPos = block.world.translation_;
			found = true;
			break;
		}
	}

	//中心ブロックが見つからない場合は何もしない
	if (!found) {
		return;
	}

	//半径範囲内にあるブロックを下げる
	for (Block& block : blocks_) {
		Vector3 pos = block.world.translation_;

		//中心座標からの距離を計算（XZ平面）
		float dx = pos.num[0] - centerPos.num[0];
		float dz = pos.num[2] - centerPos.num[2];
		float distance = std::sqrt(dx * dx + dz * dz);

		if (distance <= radius) {
			block.world.translation_.num[1] += deltaY;
		}
	}
}

void Field::RaiseBlocksAroundWithAttenuation(const VectorInt2& center, float radius, float deltaY) {
	//基準となる中心ブロックのワールド座標を取得
	Vector3 centerPos{};
	bool found = false;

	for (const Block& block : blocks_) {
		if (block.massLocation.x == center.x && block.massLocation.y == center.y) {
			centerPos = block.world.translation_;
			found = true;
			break;
		}
	}

	//中心ブロックが見つからない場合は何もしない
	if (!found) {
		return;
	}

	//半径範囲内にあるブロックを下げる（中心に近いほど多く動かす）
	for (Block& block : blocks_) {
		Vector3 pos = block.world.translation_;

		// 中心座標からの距離を計算（XZ平面）
		float dx = pos.num[0] - centerPos.num[0];
		float dz = pos.num[2] - centerPos.num[2];
		float distance = std::sqrt(dx * dx + dz * dz);

		if (distance <= radius) {
			//線形減衰（距離が大きいほど減少量は少なくなる）
			float attenuation = 1.0f - (distance / radius);//1.0〜0.0f
			float adjustedDeltaY = deltaY * attenuation;
			block.world.translation_.num[1] += adjustedDeltaY;
		}
	}
}

void Field::ColorAdjustmentByHeight(const Vector4& highColor, const Vector4& lowColor, float centerY, float rangeY) {
	for (Block& block : blocks_) {
		float y = block.world.translation_.num[1];

		//上方向
		if (y > centerY + rangeY) {
			block.color = highColor;
		}
		else if (y > centerY) {
			float t = (y - centerY) / rangeY;//0.0～1.0
			block.color = {
				1.0f * (1.0f - t) + highColor.num[0] * t,
				1.0f * (1.0f - t) + highColor.num[1] * t,
				1.0f * (1.0f - t) + highColor.num[2] * t,
				1.0f * (1.0f - t) + highColor.num[3] * t
			};
		}

		//下方向
		else if (y < centerY - rangeY) {
			block.color = lowColor;
		}
		else if (y < centerY) {
			float t = (centerY - y) / rangeY;//0.0～1.0
			block.color = {
				1.0f * (1.0f - t) + lowColor.num[0] * t,
				1.0f * (1.0f - t) + lowColor.num[1] * t,
				1.0f * (1.0f - t) + lowColor.num[2] * t,
				1.0f * (1.0f - t) + lowColor.num[3] * t
			};
		}

		//中心値：白
		else {
			block.color = { 1.0f, 1.0f, 1.0f, 1.0f };
		}
	}
}