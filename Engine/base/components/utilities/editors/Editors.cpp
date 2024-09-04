#include "Editors.h"

Editors* Editors::GetInstance() {
	static Editors instance;

	return &instance;
}

void Editors::Initialize() {
	//とりあえず初期設定
	decisionGroupName_ = (char*)"None";
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	globalVariables->AddItem("None", "ObjCount", objCount_);
	ApplyGlobalVariables();

	for (int i = 0; i < objCountMax_; i++) {
		objNameHolder_[i] = "obj" + std::to_string(i);
	}
}

void Editors::SetModels(ModelData ObjModelData, uint32_t ObjTexture) {
	ObjModelData_ = ObjModelData;
	ObjTexture_ = ObjTexture;
}

void Editors::Update() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	//Group名の設定
	ImGui::InputText("GroupName", groupName_, sizeof(groupName_));
	//Groupの生成
	if (ImGui::Button("SetGroupName")) {
		decisionGroupName_ = (char*)groupName_;
		GlobalVariables::GetInstance()->CreateGroup((char*)groupName_);

		globalVariables->AddItem(decisionGroupName_, "ObjCount", objCount_);

		ApplyGlobalVariables();
	}

	//配置するブロック名とタイプの設定
	ImGui::InputText("BlockName", objName_, sizeof(objName_));
	ImGui::InputText("BlockType", objType_, sizeof(objType_));
	//ブロックの配置
	if (ImGui::Button("SpawnBlock")) {
		bool isSpawn = false;
		for (Obj& obj : objects_) {
			if (obj.name == objName_) {
				isSpawn = true;
			}
			else {
				isSpawn = false;
			}
		}

		if (isSpawn == false) {
			SetObject(EulerTransform{ { 1.0f,1.0f,1.0f }, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} }, objName_, objType_);

			objCount_++;
			globalVariables->SetValue(decisionGroupName_, "ObjCount", objCount_);
			for (Obj& obj : objects_) {
				globalVariables->AddItem(decisionGroupName_, obj.name, (std::string)objName_);
				globalVariables->AddItem(decisionGroupName_, obj.name + "Type", (std::string)objType_);
				globalVariables->AddItem(decisionGroupName_, obj.name + "Translate", obj.world.translation_);
				globalVariables->AddItem(decisionGroupName_, obj.name + "Rotate", obj.world.rotation_);
				globalVariables->AddItem(decisionGroupName_, obj.name + "Scale", obj.world.scale_);
			}
		}
	}
	//ブロックの削除
	if (ImGui::Button("DeleteBlock")) {
		SetObject(EulerTransform{ { 1.0f,1.0f,1.0f }, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} }, objName_, "None");
		for (auto it = objects_.begin(); it != objects_.end();) {
			if (it->name == objName_) {
				globalVariables->RemoveItem(decisionGroupName_, (std::string)objName_);
				globalVariables->RemoveItem(decisionGroupName_, (std::string)objName_ + "Type");
				globalVariables->RemoveItem(decisionGroupName_, (std::string)objName_ + "Translate");
				globalVariables->RemoveItem(decisionGroupName_, (std::string)objName_ + "Rotate");
				globalVariables->RemoveItem(decisionGroupName_, (std::string)objName_ + "Scale");
				objCount_--;
				globalVariables->SetValue(decisionGroupName_, "ObjCount", objCount_);
				it = objects_.erase(it);
			}
			else {
				++it;
			}
		}
	}
	//Jsonから配置のロード
	if (ImGui::Button("StartSetBlock")) {
		for (int i = 0; i < objCount_; i++) {
			SetObject(EulerTransform{ { 1.0f,1.0f,1.0f }, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} }, objNameHolder_[i], "None");
		}
	}

	//データ入力方法切り替え
	ImGui::Checkbox("DataDirectInputMode", &isDirectInputMode_);

	if (globalVariables->GroupNameSearch(decisionGroupName_) == true) {
		if (isDirectInputMode_ == true) {
			ApplyGlobalVariables();
		}
		else {
			SetGlobalVariables();
		}
	}

	//WorldTransform更新
	for (Obj& obj : objects_) {
		obj.world.UpdateMatrix();
	}
}

void Editors::Draw(ViewProjection view) {
	for (Obj& obj : objects_) {
		obj.model.Draw(obj.world, view, obj.material);

		//Guizmo操作
		if (isDirectInputMode_ == false) {
			if (objName_ == obj.name) {
				obj.world = Guizmo(view, obj.world);
			}
		}
	}
}

void Editors::Finalize() {
	objects_.clear();
}

void Editors::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	objCount_ = globalVariables->GetIntValue(decisionGroupName_, "ObjCount");

	for (Obj& obj : objects_) {
		obj.world.translation_ = globalVariables->GetVector3Value(decisionGroupName_, obj.name + "Translate");
		obj.world.rotation_ = globalVariables->GetVector3Value(decisionGroupName_, obj.name + "Rotate");
		obj.world.scale_ = globalVariables->GetVector3Value(decisionGroupName_, obj.name + "Scale");
		obj.type = globalVariables->GetStringValue(decisionGroupName_, obj.name + "Type");
	}
}

void Editors::SetGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->SetValue(decisionGroupName_, "ObjCount", objCount_);

	for (Obj& obj : objects_) {
		globalVariables->SetValue(decisionGroupName_, obj.name + "Translate", obj.world.translation_);
		globalVariables->SetValue(decisionGroupName_, obj.name + "Rotate", obj.world.rotation_);
		globalVariables->SetValue(decisionGroupName_, obj.name + "Scale", obj.world.scale_);
		globalVariables->SetValue(decisionGroupName_, obj.name + "Type", obj.type);
	}
}

void Editors::SetObject(EulerTransform transform, const std::string& name, const std::string& type) {
	Obj obj;
	obj.model.Initialize(ObjModelData_, ObjTexture_);
	obj.model.SetDirectionalLightFlag(true, 3);

	obj.world.Initialize();
	obj.world.translation_ = transform.translate;
	obj.world.rotation_ = transform.rotate;
	obj.world.scale_ = transform.scale;

	obj.material = { 1.0f,1.0f,1.0f,1.0f };

	obj.name = name;

	obj.type = type;

	objects_.push_back(obj);
}

void Editors::AddGroupName(char* groupName) {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	decisionGroupName_ = groupName;
	GlobalVariables::GetInstance()->CreateGroup(decisionGroupName_);

	objCount_ = 0;

	globalVariables->AddItem(decisionGroupName_, "ObjCount", objCount_);
}

void Editors::SetGroupName(char* groupName) {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	decisionGroupName_ = groupName;
	objCount_ = 0;

	ApplyGlobalVariables();

	for (int i = 0; i < objCount_; i++) {
		SetObject(EulerTransform{ { 0.0f,0.0f,0.0f }, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} }, objNameHolder_[i], "None");
	}

	ApplyGlobalVariables();
}

WorldTransform Editors::Guizmo(ViewProjection& view, WorldTransform world) {
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

	WorldTransform worldTransform_ = world;

	if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_RightShift) && ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_T)) {
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}
	if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_RightShift) && ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_R)) {
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	}
	if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_RightShift) && ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S)) {
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	}

	Matrix4x4 gizmoMatrix = world.matWorld_;
	ImGuizmo::Manipulate(&view.matView.m[0][0], &view.matProjection.m[0][0], mCurrentGizmoOperation, ImGuizmo::WORLD, &gizmoMatrix.m[0][0]);

	EulerTransform transform;
	ImGuizmo::DecomposeMatrixToComponents(&gizmoMatrix.m[0][0], &transform.translate.num[0], &transform.rotate.num[0], &transform.scale.num[0]);

	worldTransform_.translation_ = transform.translate;
	//worldTransform_.rotation_ = transform.rotate;
	worldTransform_.scale_ = transform.scale;

	worldTransform_.UpdateMatrix();

	return worldTransform_;
}

EulerTransform Editors::Guizmo(ViewProjection& view, EulerTransform world) {
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

	if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_RightShift) && ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_T)) {
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}
	if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_RightShift) && ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_R)) {
		//mCurrentGizmoOperation = ImGuizmo::ROTATE;
	}
	if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_RightShift) && ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S)) {
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	}

	Matrix4x4 gizmoMatrix = MakeIdentity4x4();
	Matrix4x4 AffineMatrix = MakeAffineMatrix(world.scale, world.rotate, world.translate);
	gizmoMatrix = AffineMatrix;
	ImGuizmo::Manipulate(&view.matView.m[0][0], &view.matProjection.m[0][0], mCurrentGizmoOperation, ImGuizmo::WORLD, &gizmoMatrix.m[0][0]);

	EulerTransform result;
	ImGuizmo::DecomposeMatrixToComponents(&gizmoMatrix.m[0][0], &result.translate.num[0], &result.rotate.num[0], &result.scale.num[0]);

	return result;
}