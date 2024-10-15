/**
 * @file Editors.h
 * @brief エンジン内でレベルエディットを行い、.jsonとして保存の出来るレベルエディター機能
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "ImGuiManager.h"
#include "Model.h"
#include <random>

#include "components/utilities/globalVariables/GlobalVariables.h"

struct Obj {
	Model model;
	WorldTransform world;
	Vector4 material;
	std::string name;
	std::string type;
	int durability;
};

class Editors {
public:
	static Editors* GetInstance();

	void Initialize();

	//そのグループで描画するモデルを割り当てる関数
	void SetModels(ModelData ObjModelData, uint32_t ObjTexture);

	void Update();

	void Draw(ViewProjection view);

	void Finalize();

	void ApplyGlobalVariables();
	void SetGlobalVariables();

	//オブジェクトを一つ生成する関数
	void SetObject(EulerTransform trans, const std::string& name, const std::string& type);

	//グループの追加
	void AddGroupName(char* groupName);

	//グループを選択
	void SetGroupName(char* groupName);

	//現在のグループ内のオブジェクトが接触した際の処理
	void Hitobj(Obj o);

	//現在のグループのオブジェクト数を得る
	int GetObjCount() { return objCount_; }

	//現在のグループにオブジェクトデータを割り当てる
	void SetObj(std::list<Obj> obj) { objects_ = obj; }

	//現在のグループのオブジェクトデータを得る
	std::list<Obj> GetObj() { return objects_; }

	//Key1でTRANSLATE、key2でROTATE、key3でSCALE
	WorldTransform Guizmo(ViewProjection& view, WorldTransform world);

	//Key1でTRANSLATE、key2でROTATE、key3でSCALE
	EulerTransform Guizmo(ViewProjection& view, EulerTransform world);

private:
	Editors() = default;
	~Editors() = default;
	Editors(const Editors& obj) = default;
	Editors& operator=(const Editors& obj) = default;

	//ステージエディター擬き、名前をobj0~始め、それを記録する
	std::list<Obj> objects_;
	ModelData ObjModelData_;
	uint32_t ObjTexture_;
	static const int objCountMax_ = 100;
	int objCount_ = 0;
	std::string objNameHolder_[objCountMax_];

	char objName_[64];
	char objType_[64];
	char groupName_[64];
	char* decisionGroupName_;

	//trueでguiに直接数字を打ち込めるようにする
	bool isDirectInputMode_ = false;

	//Other
	std::random_device seedGenerator;
};