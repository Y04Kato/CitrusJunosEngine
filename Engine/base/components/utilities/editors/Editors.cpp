#include "Editors.h"

Editors* Editors::GetInstance() {
	static Editors instance;

	return &instance;
}

void Editors::Initialize() {
	input_ = Input::GetInstance();
	textureManager_ = TextureManager::GetInstance();

	worldTransform_.Initialize();

	Model model;
	ObjModelData_ = model.LoadModelFile("project/gamedata/resources/block", "block.obj");
	ObjTexture_ = textureManager_->Load(ObjModelData_.material.textureFilePath);

	for (int i = 0; i < objCountMax_; i++) {
		objNameHolder_[i] = "test" + std::to_string(i);
	}
}

void Editors::Update() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	ApplyGlobalVariables();

	//Group名の設定
	ImGui::InputText("GroupName", groupName_, sizeof(groupName_));
	if (ImGui::Button("SetGroupName")) {
		decisionGroupName_ = groupName_;
		GlobalVariables::GetInstance()->CreateGroup(decisionGroupName_);

		globalVariables->AddItem(decisionGroupName_, "ObjCount", objCount_);
	}

	//配置するブロック名の設定
	ImGui::InputText("BlockName", objName_, sizeof(objName_));
	//ブロックの配置
	if (ImGui::Button("SpawnBlock")) {
		SetObject(EulerTransform{ { 1.0f,1.0f,1.0f }, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} }, objName_);
		objCount_++;
		globalVariables->SetValue(decisionGroupName_, "ObjCount", objCount_);
		for (Obj& obj : objects_) {
			globalVariables->AddItem(decisionGroupName_, obj.name, (std::string)objName_);
			globalVariables->AddItem(decisionGroupName_, obj.name + "Translate", obj.world.translation_);
			//globalVariables->AddItem(decisionGroupName_,obj.name + "Rotate", obj.world.rotation_);
			globalVariables->AddItem(decisionGroupName_, obj.name + "Scale", obj.world.scale_);
		}
	}
	//ブロックの削除
	if (ImGui::Button("DeleteBlock")) {
		SetObject(EulerTransform{ { 1.0f,1.0f,1.0f }, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} }, objName_);
		for (auto it = objects_.begin(); it != objects_.end();) {
			if (it->name == objName_) {
				globalVariables->RemoveItem(decisionGroupName_, (std::string)objName_ + "Translate");
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
			SetObject(EulerTransform{ { 1.0f,1.0f,1.0f }, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} }, objNameHolder_[i]);
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
		if (objName_ == obj.name) {
			obj.world = Editor(view, obj.world);
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
		//obj.world.rotation_ = globalVariables->GetVector3Value(decisionGroupName_,  obj.name + "Rotate");
		obj.world.scale_ = globalVariables->GetVector3Value(decisionGroupName_, obj.name + "Scale");
	}
}

void Editors::SetObject(EulerTransform transform, const std::string& name) {
	Obj obj;
	obj.model.Initialize(ObjModelData_, ObjTexture_);
	obj.model.SetDirectionalLightFlag(true, 3);

	obj.world.Initialize();
	obj.world.translation_ = transform.translate;
	obj.world.rotation_ = transform.rotate;
	obj.world.scale_ = transform.scale;

	obj.material = { 1.0f,1.0f,1.0f,1.0f };

	obj.name = name;
	objects_.push_back(obj);
}

WorldTransform Editors::Editor(ViewProjection& view, WorldTransform world) {
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

	worldTransform_ = world;

	if (input_->TriggerKey(DIK_1)) {
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}
	if (input_->TriggerKey(DIK_2)) {
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	}
	if (input_->TriggerKey(DIK_3)) {
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	}

	Matrix4x4 gizmoMatrix = world.matWorld_;
	ImGuizmo::Manipulate(&view.matView.m[0][0], &view.matProjection.m[0][0], mCurrentGizmoOperation, ImGuizmo::WORLD, &gizmoMatrix.m[0][0]);

	EulerTransform transform;
	ImGuizmo::DecomposeMatrixToComponents(&gizmoMatrix.m[0][0], &transform.translate.num[0], &transform.rotate.num[0], &transform.scale.num[0]);

	worldTransform_.translation_ = transform.translate;
	worldTransform_.rotation_ = transform.rotate;
	worldTransform_.scale_ = transform.scale;

	worldTransform_.UpdateMatrix();

	return worldTransform_;
}

EulerTransform Editors::Editor(ViewProjection& view, EulerTransform world) {
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

	if (input_->TriggerKey(DIK_1)) {
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}
	if (input_->TriggerKey(DIK_2)) {
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	}
	if (input_->TriggerKey(DIK_3)) {
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