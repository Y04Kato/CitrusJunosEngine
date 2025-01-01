#pragma once


class SceneNumber {
public:
	static SceneNumber* GetInstance();

	void Initialize(const int nowSceneNo);

	void SetSceneNumber(const int nextSceneNo);

	const int GetSceneNumber() { return sceneNo_; }

private:
	SceneNumber() = default;
	~SceneNumber() = default;
	SceneNumber(const SceneNumber& obj) = default;
	SceneNumber& operator=(const SceneNumber& obj) = default;

	int sceneNo_;
};