/**
 * @file Iscene.h
 * @brief 各種シーンをまとめる
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "components/audio/Audio.h"
#include "components/input/Input.h"
#include "components/3d/WorldTransform.h"
#include "components/3d/ViewProjection.h"
#include "TextureManager.h"
#include "components/2d/CreateTriangle.h"
#include "components/2d/CreateSprite.h"
#include "components/2d/CreateParticle.h"
#include "components/3d/CreateSphere.h"
#include "components/3d/CreateSkyBox.h"
#include "components/3d/Model.h"
#include "components/debugcamera/DebugCamera.h"
#include "components/utilities/followCamera/FollowCamera.h"
#include "components/utilities/collisionManager/CollisionManager.h"
#include "components/utilities/collisionManager/CollisionConfig.h"
#include "postEffect/PostEffect.h"
#include "editors/Editors.h"
#include "SceneNumber.h"

//シーン一覧、シーン追加時はここに追加する
enum SCENE {
	TITLE_SCENE,
	GAME_SCENE,
	CLEAR_SCENE,
	OVER_SCENE,
	DEBUG_SCENE,
	SCENE_MAX
};

class Iscene{
public:
	virtual void Initialize() = 0;

	virtual void Update() = 0;

	virtual void Draw() = 0;

	virtual void DrawUI() = 0;

	virtual void DrawPostEffect() = 0;

	virtual void Finalize() = 0;

	virtual ~Iscene() {};
};
