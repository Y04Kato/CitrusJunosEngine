/**
 * @file Transition.h
 * @brief トランジションを行うクラス
 * @author KATO
 * @date 2024/10/12
 */

#pragma once
#include "components/2d/CreateSprite.h"

class Transition {
public:
	static Transition* GetInstance();

	void Initialize();

	void Update();

	void Draw();

	//シーン明け用
	void SceneStart();

	//シーン終わり用
	void SceneEnd();

	bool GetIsSceneStart_() { return isSceneStart_; }
	bool GetIsSceneEnd_() { return isSceneEnd_; }

	Transition(const Transition& obj) = delete;
	Transition& operator=(const Transition& obj) = delete;
private:
	Transition() = default;
	~Transition() = default;

	TextureManager* textureManager_;

	//各種テクスチャ
	uint32_t koma_;
	uint32_t box_;
	uint32_t circle_;

	static const int transitionSpriteMaxNum_ = 6;//独楽と円の個数は同じである必要がある為これは両方に適用する

	std::unique_ptr <CreateSprite> komaSprite_[transitionSpriteMaxNum_];
	EulerTransform komaTransform_[transitionSpriteMaxNum_];

	std::unique_ptr <CreateSprite> boxSprite_[transitionSpriteMaxNum_];
	EulerTransform boxTransform_[transitionSpriteMaxNum_];

	std::unique_ptr <CreateSprite> circleSprite_[transitionSpriteMaxNum_];
	EulerTransform circleTransform_[transitionSpriteMaxNum_];
	
	Vector4 pairMaterial_[transitionSpriteMaxNum_];

	EulerTransform spriteUVTransform_;
	EulerTransform allSpriteTransform_;//全てのspriteを同時に動かす用

	//シーン開始/終了フラグ
	bool isSceneStart_ = false;
	bool isSceneEnd_ = false;

	//遷移スピード
	const float transitionSpeed_ = 20.0f;

	//何処まで行ったら各遷移が終了するか
	const float startTransitionEndPoint_ = 3000.0f;
	const float endTransitionEndPoint_ = 1430.0f;

};