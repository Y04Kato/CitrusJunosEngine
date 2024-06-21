#pragma once

//シーン一覧、シーン追加時はここに追加する
enum SCENE {
	TITLE_SCENE,
	GAME_SCENE,
	CLEAR_SCENE,
	DEMO_SCENE,
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

	static int sceneNo;

	virtual ~Iscene() {};
};
