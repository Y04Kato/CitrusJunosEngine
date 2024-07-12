#pragma once
#include "Input.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "ImGuiManager.h"
#include "Model.h"
#include "TextureManager.h"

#include "components/utilities/globalVariables/GlobalVariables.h"

struct Obj {
	Model model;
	WorldTransform world;
	Vector4 material;
	std::string name;
};

class Editors {
public:
	static Editors* GetInstance();

	void Initialize();

	void Update();

	void Draw(ViewProjection view);

	void Finalize();

	void ApplyGlobalVariables();

	void SetObject(EulerTransform trans, const std::string& name);

	WorldTransform Editor(ViewProjection& view, WorldTransform world);

	EulerTransform Editor(ViewProjection& view, EulerTransform world);

private:
	Editors() = default;
	~Editors() = default;
	Editors(const Editors& obj) = default;
	Editors& operator=(const Editors& obj) = default;

	Input* input_;
	TextureManager* textureManager_;

	WorldTransform worldTransform_;

	//ステージエディター擬き、名前をtest0~始め、それを記録する
	std::list<Obj> objects_;
	ModelData ObjModelData_;
	uint32_t ObjTexture_;
	static const int objCountMax_ = 100;
	int objCount_ = 0;
	std::string objNameHolder_[objCountMax_];

	char* objName_;
	char* groupName_;
	char* decisionGroupName_;
};