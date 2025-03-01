/**
 * @file CreateSprite.cpp
 * @brief 2DSpriteの初期化及び描画を行う
 * @author KATO
 * @date 未記録
 */

#include "CreateSprite.h"

void CreateSprite::Initialize(Vector2 size, uint32_t textureIndex) {
	dxCommon_ = DirectXCommon::GetInstance();
	CJEngine_ = CitrusJunosEngine::GetInstance();
	textureManager_ = TextureManager::GetInstance();

	index_ = textureIndex;
	AdjustTextureSize();
	size_ = size;

	SettingVertex();
	SettingColor();
	SettingTransform();
}

void CreateSprite::Draw(const EulerTransform& transform, const EulerTransform& uvTransform, const Vector4& material) {
	Matrix4x4 uvtransformMtrix = MakeScaleMatrix(uvTransform.scale);
	uvtransformMtrix = Multiply(uvtransformMtrix, MakeRotateZMatrix(uvTransform.rotate.num[2]));
	uvtransformMtrix = Multiply(uvtransformMtrix, MakeTranslateMatrix(uvTransform.translate));

	*materialData_ = { material,false };
	materialData_->uvTransform = uvtransformMtrix;

	//Sprite用のworldViewProjectionMatrixを作る
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionmatrix = MakeOrthographicMatrix(0.0f, 0.0f, (float)dxCommon_->GetWin()->GetClientWidth(), (float)dxCommon_->GetWin()->GetClientHeight(), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionmatrix));
	*wvpData_ = worldViewProjectionMatrix;

	float left = (0.0f - anchor_.num[0]) * size_.num[0];
	float right = (1.0f - anchor_.num[0]) * size_.num[0];
	float top = (0.0f - anchor_.num[1]) * size_.num[1];
	float bottom = (1.0f - anchor_.num[1]) * size_.num[1];

	if (isFlipX_) {
		left = -left;
		right = -right;
	}
	if (isFlipY_) {
		top = -top;
		bottom = -bottom;
	}

	//座標の設定
	vertexData_[0].position = { left,bottom,0.0f,1.0f };
	vertexData_[1].position = { left,top,0.0f,1.0f };
	vertexData_[2].position = { right,bottom,0.0f,1.0f };
	vertexData_[3].position = { right,top,0.0f,1.0f };

	Microsoft::WRL::ComPtr <ID3D12Resource> textureBuffer = textureManager_->GetTextureBuffer(index_);

	if (textureBuffer) {
		D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();

		float tex_left = textureLeftTop_.num[0] / resDesc.Width;
		float tex_right = (textureLeftTop_.num[0] + textureSize_.num[0]) / resDesc.Width;
		float tex_top = textureLeftTop_.num[1] / resDesc.Height;
		float tex_bottom = (textureLeftTop_.num[1] + textureSize_.num[1]) / resDesc.Height;

		//Texcoordの設定
		vertexData_[0].texcoord = { tex_left,tex_bottom };
		vertexData_[1].texcoord = { tex_left,tex_top };
		vertexData_[2].texcoord = { tex_right,tex_bottom };
		vertexData_[3].texcoord = { tex_right,tex_top };
	}

	//描画
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite_);

	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(index_));

	dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void CreateSprite::Finalize() {

}

std::unique_ptr <CreateSprite> CreateSprite::CreateSpriteFromTexture(Vector2 size, uint32_t textureIndex) {
	std::unique_ptr <CreateSprite> sprite = std::make_unique <CreateSprite>();
	sprite->Initialize(size, textureIndex);
	return sprite;
}

void CreateSprite::SettingVertex() {
	//Sprite用のリソースを作る
	vertexResourceSprite_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * 6);

	//頂点バッファービューを作成し、先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResourceSprite_->GetGPUVirtualAddress();

	//使用するリソースサイズは頂点6つ分
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6;

	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	vertexResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	indexResourceSprite_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(uint32_t) * 6);

	indexBufferViewSprite_.BufferLocation = indexResourceSprite_->GetGPUVirtualAddress();

	indexBufferViewSprite_.SizeInBytes = sizeof(uint32_t) * 6;

	indexBufferViewSprite_.Format = DXGI_FORMAT_R32_UINT;

	indexResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite_));

	//Texcoordの設定
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[3].texcoord = { 1.0f,0.0f };

	indexDataSprite_[0] = 0;
	indexDataSprite_[1] = 1;
	indexDataSprite_[2] = 2;
	indexDataSprite_[3] = 1;
	indexDataSprite_[4] = 3;
	indexDataSprite_[5] = 2;

	for (int i = 0; i < 6; i++) {
		vertexData_[i].normal = { 0.0f,0.0f,-1.0f };
	}
}

void CreateSprite::SettingColor() {
	materialResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material));

	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->uvTransform = MakeIdentity4x4();
}

void CreateSprite::SettingTransform() {
	wvpResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice().Get(), sizeof(Matrix4x4));
	wvpResource_->Map(0, NULL, reinterpret_cast<void**>(&wvpData_));
	*wvpData_ = MakeIdentity4x4();
}

void CreateSprite::SetSize(Vector2 size) {
	size_ = size;
}

void CreateSprite::SetAnchor(Vector2 anchor) {
	anchor_ = anchor;
}

void CreateSprite::SetFlip(bool isFlipX, bool isFlipY) {
	isFlipX_ = isFlipX;
	isFlipY_ = isFlipY;
}

void CreateSprite::SetTextureLTSize(Vector2 textureLeftTop, Vector2 textureSize) {
	textureLeftTop_ = textureLeftTop;
	textureSize_ = textureSize;
}

void CreateSprite::SetTextureInitialSize() {
	AdjustTextureSize();
	size_ = textureSize_;
}

void CreateSprite::AdjustTextureSize() {
	Microsoft::WRL::ComPtr <ID3D12Resource> textureBuffer = textureManager_->GetTextureBuffer(index_);
	assert(textureBuffer);

	D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();

	textureSize_.num[0] = static_cast<float>(resDesc.Width);
	textureSize_.num[1] = static_cast<float>(resDesc.Height);
}

void CreateSprite::SetTextureIndex(uint32_t textureIndex) {
	index_ = textureIndex;
	AdjustTextureSize();
}

Vector2 CreateSprite::GetLocalPosition(const Vector2& localPoint, const EulerTransform& transform) {
	// スプライトのアンカーを基準に、ローカル座標の範囲を計算
	float left = (0.0f - anchor_.num[0]) * size_.num[0];
	float right = (1.0f - anchor_.num[0]) * size_.num[0];
	float top = (0.0f - anchor_.num[1]) * size_.num[1];
	float bottom = (1.0f - anchor_.num[1]) * size_.num[1];

	// X軸とY軸の反転を考慮
	if (isFlipX_) {
		std::swap(left, right);
	}
	if (isFlipY_) {
		std::swap(top, bottom);
	}

	// 指定されたローカル座標に基づいて座標を計算
	float x = left + (right - left) * localPoint.num[0];
	float y = top + (bottom - top) * localPoint.num[1];

	// ローカル座標をベクトルに変換
	Vector3 localPos = { x, y, 0.0f };

	// スケールを適用
	localPos.num[0] *= transform.scale.num[0];
	localPos.num[1] *= transform.scale.num[1];

	// Z軸回転を適用
	float rotationZ = transform.rotate.num[2];
	float cosTheta = cosf(rotationZ);
	float sinTheta = sinf(rotationZ);
	float rotatedX = localPos.num[0] * cosTheta - localPos.num[1] * sinTheta;
	float rotatedY = localPos.num[0] * sinTheta + localPos.num[1] * cosTheta;
	localPos.num[0] = rotatedX;
	localPos.num[1] = rotatedY;

	// 平行移動を適用
	localPos.num[0] += transform.translate.num[0];
	localPos.num[1] += transform.translate.num[1];

	// 2D座標として返す
	return { localPos.num[0], localPos.num[1] };
}

