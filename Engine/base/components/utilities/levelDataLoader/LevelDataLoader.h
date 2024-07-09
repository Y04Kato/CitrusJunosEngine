#pragma once
#include "Input.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "LevelData.h"

#include <string.h>
#include <fstream>
#include <sstream>
#include <json.hpp>

class LevelDataLoader {
public:
	static LevelDataLoader* GetInstance();

	void Initialize(const std::string& directoryPath, const std::string& filename);

	LevelData* GetLevelData() { return levelData_; }

	LevelDataLoader(const LevelDataLoader& obj) = delete;
	LevelDataLoader& operator=(const LevelDataLoader& obj) = delete;
private:
	LevelDataLoader() = default;
	~LevelDataLoader() = default;

	LevelData* levelData_;

	//JSONファイルのロードとデータの確認
	static bool FileLoad(const std::string fullpath, nlohmann::json& deserialized);

	//親ノードのロード
	static LevelData* SearchObjects(nlohmann::json& deserialized);

	//子ノードのロード
	static void SearchChildren(LevelData* levelData, nlohmann::json& parent, EulerTransform& parentTransform);

	//Transformのロード
	static EulerTransform TransformLoad(nlohmann::json& object);
};