#include "LockOn.h"
#include "baseCharacter/enemy/Enemy.h"

void LockOn::Initialize() {
	textureManager_ = TextureManager::GetInstance();
	tex_ = textureManager_->Load("project/gamedata/resources/lockon.png");
	spriteMaterial_ = { 1.0f,1.0f,1.0f,1.0f };
	spriteTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	SpriteuvTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	sprite_ = std::make_unique <CreateSprite>();
	sprite_->Initialize(Vector2{ 100.0f,100.0f }, tex_, false, false);
	sprite_->SetAnchor(Vector2{ 0.5f,0.5f });
	sprite_->SetTextureInitialSize();

	isLockOn_ = false;
	count_ = 0;
}

void LockOn::Update(const std::list<Enemy*>& enemys, const ViewProjection& viewProjection){
	XINPUT_STATE joystate;
	count_++;
	if (!Input::GetInstance()->GetJoystickState(0, joystate)) {
		return;
	}
	if (joystate.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		if (!(preInputPad.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
			if (!target_) {
				isLockOn_ = true;
				Search(enemys, viewProjection);
				Target();
			}
			else {
				isLockOn_ = false;
			}

		}
	}
	if (joystate.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {
		if (!(preInputPad.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)) {
			if (isAuto) {
				isAuto = false;
				Search(enemys, viewProjection);

			}
			else {
				isAuto = true;
				Search(enemys, viewProjection);

			}
		}
	}

	if (joystate.Gamepad.wButtons & XINPUT_GAMEPAD_Y) {
		if (!(preInputPad.Gamepad.wButtons & XINPUT_GAMEPAD_Y)) {
			isLockOn_ = true;
			if (iteratornum > 0) {

				iteratornum--;
				Search(enemys, viewProjection);
				Target();
			}
			else {
				iteratornum = max;
				Search(enemys, viewProjection);
				Target();
			}
		}
	}

	if (!isLockOn_) {
		Reset();

	}

	if (target_) {
		if (!target_->GetisAlive()) {
		}
		else {
			Reset();
			return;
		}
		Vector3 positionWorld = target_->GetWorldTransform().GetCenter();

		Vector3 pos = WorldToScreen(positionWorld, viewProjection);
		spriteTransform_.translate.num[0] = pos.num[0];
		spriteTransform_.translate.num[1] = pos.num[1];
		if (isRangeOut(viewProjection)) {
			Reset();
			Search(enemys, viewProjection);
		}
	}
	preInputPad = joystate;
	ImGui::Begin("LockOn");
	ImGui::Checkbox("IsAuto", &isAuto);
	ImGui::Text("%d", iteratornum);
	ImGui::End();
	count_++;
}

void LockOn::Draw() {
	if (target_) {
		sprite_->Draw(spriteTransform_, SpriteuvTransform_, spriteMaterial_);
	}
}

void LockOn::Target() {
	target_ = nullptr;

	if (!targets.empty()) {
		targets.sort([](auto& pair1, auto& pair2) {return pair1.first > pair2.first; });
		max = (int)targets.size();
		if (isAuto == true) { target_ = targets.front().second; }
		else {
			auto it = targets.begin(); // イテレータをリストの先頭に設定する
			if (iteratornum >= targets.size()) {
				iteratornum = (int)targets.size();
			}

			std::advance(it, iteratornum);
			if (it != targets.end()) {
				std::pair<float, Enemy*>element = *it;
				target_ = element.second;
			}
		}
	}
}



void LockOn::Search(const std::list<Enemy*>& enemys, const ViewProjection& viewProjection) {
	if (count_ > 60) {
		target_ = nullptr;
		targets.clear();
		for (Enemy* enemy : enemys) {
			Vector3 positionWorld = enemy->GetWorldTransform().GetCenter();
			Vector3 positionView = TransformN(positionWorld, viewProjection.matView);
			if (minDistance_ <= positionView.num[2] && positionView.num[2] <= maxDistance_) {
				Vector3 viewXZ = Normalize(Vector3{ positionView.num[0],0.0f,positionView.num[2] });
				Vector3 viewZ = Normalize(Vector3{ 0.0f,0.0f,positionView.num[2] });
				float cos = Length(Cross(viewXZ, viewZ));

				if (std::abs(cos) <= angleRange_) {

					targets.emplace_back(std::make_pair(positionView.num[2], enemy));
				}
			}

		}

		count_ = 0;
	}
}

void LockOn::Reset() {
	if (target_) {
		target_ = nullptr;
		isLockOn_ = false;
		iteratornum = 0;
	}
}

Vector3 LockOn::GetTargetPos() const {
	{
		if (target_) {
			return target_->GetWorldTransform().GetCenter();
		}
		return Vector3();
	}
}

bool LockOn::isTarget(){
	if (target_) {
		return true;
	}
	return false;
}

bool LockOn::isRangeOut(const ViewProjection& viewProjection){
	Vector3 positionWorld = target_->GetWorldTransform().GetCenter();
	Vector3 positionView = TransformN(positionWorld, viewProjection.matView);
	if (minDistance_ <= positionView.num[2] && positionView.num[2] <= maxDistance_) {
		Vector3 viewXZ = Normalize(Vector3{ positionView.num[0],0.0f,positionView.num[2]});
		Vector3 viewZ = Normalize(Vector3{ 0.0f,0.0f,positionView.num[2]});

		float cos = Length(Cross(viewXZ, viewZ));
		if (std::abs(cos) <= angleRange_) {
			//範囲内
			return false;
		}
	}
	//範囲外
	return true;
}

Vector3 LockOn::WorldToScreen(Vector3 world, const ViewProjection& viewProjection){
	Matrix4x4 matViewport = { 0.0f, 0.0f, WinApp::kClientWidth, WinApp::kClientHeight, 0.0f, 1.0f };
	Matrix4x4 matViewProjectionViewport =
		Multiply(viewProjection.matView, Multiply(viewProjection.matProjection, matViewport));
	return TransformN(world, matViewProjectionViewport);
}