#include "LevelDataLoader.h"

LevelDataLoader* LevelDataLoader::GetInstance() {
	static LevelDataLoader instance;

	return &instance;
}

void LevelDataLoader::Initialize(const std::string& directoryPath, const std::string& filename) {
	//連結してフルパスを得る
	const std::string fullpath = directoryPath + "/" + filename;
	//ファイルストリーム
	std::ifstream file;

	//ファイルを開く
	file.open(fullpath);
	//ファイルオープン可否をチェック
	if (file.fail()) {
		assert(0);
	}

	//JSON文字列から冷凍したデータ
	nlohmann::json deserialized;
	//解凍
	file >> deserialized;

	//正しいレベルデータファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("NodeName"));
	assert(deserialized["NodeName"].is_string());

	//"NodeName"を文字列として取得
	std::string name =
		deserialized["NodeName"].get<std::string>();
	//正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);

	//レベルデータ格納用インスタンスを生成
	LevelData* levelData = new LevelData();

	//全オブジェクトを走査
	for (nlohmann::json& object : deserialized["objects"]) {
		//assert(object.contains("type"));
		////種別を取得
		//std::string type = object["type"].get<std::string>();
	}
}

void LevelDataLoader::Update() {
	
}