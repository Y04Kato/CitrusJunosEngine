#pragma once
#include "Input.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

#include <string.h>
#include <fstream>
#include <sstream>
#include <json.hpp>

struct obejcts {
	Vector3 Translate;
	Vector3 Rotate;
	Vector3 Scale;
};

struct LevelData {
	std::string name;
	std::vector<obejcts> obejcts;
};

class LevelDataLoader {
public:
	static LevelDataLoader* GetInstance();

	void Initialize(const std::string& directoryPath, const std::string& filename);

	void Update();

	LevelDataLoader(const LevelDataLoader& obj) = delete;
	LevelDataLoader& operator=(const LevelDataLoader& obj) = delete;
private:
	LevelDataLoader() = default;
	~LevelDataLoader() = default;
};