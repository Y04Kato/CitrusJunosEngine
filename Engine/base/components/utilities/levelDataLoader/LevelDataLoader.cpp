#include "LevelDataLoader.h"

LevelDataLoader* LevelDataLoader::GetInstance() {
	static LevelDataLoader instance;

	return &instance;
}

void LevelDataLoader::Initialize(const std::string& directoryPath, const std::string& filename) {
	//ファイルパス連結
	const std::string fullpath = directoryPath + "/" + filename;

	//JSON文字列から冷凍したデータ
	nlohmann::json deserialized;

	//JSONデータが正しい物か確認
	FileLoad(fullpath, deserialized);

	//データのロード
	levelData_ = SearchObjects(deserialized);
}

bool LevelDataLoader::FileLoad(const std::string fullpath, nlohmann::json& deserialized){
	//ファイルストリーム
	std::ifstream file;

	//ファイルを開く
	file.open(fullpath);
	//ファイルオープン可否をチェック
	if (file.fail()) {
		assert(0);
	}

	//解凍
	file >> deserialized;

	//正しいレベルデータファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	//"name"を文字列として取得
	std::string name = deserialized["name"].get<std::string>();
	//正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);

	//成功
	return true;
}

LevelData* LevelDataLoader::SearchObjects(nlohmann::json& deserialized){
	//レベルデータ格納用インスタンスを生成
	LevelData* levelData = new LevelData();

	//子ノードに送るデータ記録用
	std::string parentName;
	EulerTransform parentTransform;

	//全オブジェクトを走査
	for (nlohmann::json& object : deserialized["objects"]) {
		assert(object.contains("Type"));
		//種別を取得
		std::string type = object["Type"].get<std::string>();

		parentName = "NULL";
		parentTransform.translate = { 0.0f,0.0f,0.0f };
		parentTransform.rotate = { 0.0f,0.0f,0.0f };
		parentTransform.scale = { 0.0f,0.0f,0.0f };

		//geoノード
		if (type.compare("geo") == 0) {
			//要素追加
			levelData->objectsData_.emplace_back(LevelData::MeshData{});
			//今追加した要素の参照を得る
			LevelData::MeshData& objectData = levelData->objectsData_.back();

			//オブジェクト名
			objectData.name = object["Name"];
			parentName = objectData.name;

			if (object.contains("file_name")) {
				//ファイル名
				objectData.flieName = object["file_name"];
			}

			//トランスフォーム
			objectData.transform = TransformLoad(object);
			objectData.transform.translate = objectData.transform.translate;
			parentTransform = objectData.transform;

			//Houdiniでは親ノードはそもそも描画しない
			objectData.isParent = true;

		}

		//子ノード
		if (object.contains("Children")) {
			SearchChildren(levelData, object, parentTransform);
		}
	}

	return levelData;
}

void LevelDataLoader::SearchChildren(LevelData* levelData, nlohmann::json& parent, EulerTransform& parentTransform){
	EulerTransform parentChildrenTransform;
	
	//全オブジェクトを走査
	for (nlohmann::json& object : parent["Children"]) {
		assert(object.contains("Type"));
		//種別を取得
		std::string type = object["Type"].get<std::string>();

		parentChildrenTransform.translate = { 0.0f,0.0f,0.0f };
		parentChildrenTransform.rotate = { 0.0f,0.0f,0.0f };
		parentChildrenTransform.scale = { 0.0f,0.0f,0.0f };

		//box
		if (type.compare("box") == 0) {
			//要素追加
			levelData->objectsData_.emplace_back(LevelData::MeshData{});
			//今追加した要素の参照を得る
			LevelData::MeshData& objectData = levelData->objectsData_.back();

			//オブジェクト名
			objectData.name = object["Name"];
			objectData.name = "-" + objectData.name;

			//ファイル名
			if (object.contains("file_name")) {
				objectData.flieName = object["file_name"];
			}

			//トランスフォーム
			objectData.transform = TransformLoad(object) + parentTransform;
			objectData.transform.translate = objectData.transform.translate * 6.0f;
			objectData.transform.scale = objectData.transform.scale + parentTransform.scale;
			objectData.transform.rotate = objectData.transform.rotate + parentTransform.rotate;

			objectData.isParent = false;
		}

		//子ノード
		if (object.contains("Children")) {
			SearchChildren(levelData, object, parentChildrenTransform);
		}
	}

}

EulerTransform LevelDataLoader::TransformLoad(nlohmann::json& object){
	EulerTransform result{};

	// トランスフォームのパラメータ読み込み
	nlohmann::json& transform = object["Transform"];

	// 平行移動
	result.translate.num[0] = static_cast<float>(transform["Translate"][0]);
	result.translate.num[1] = static_cast<float>(transform["Translate"][1]);
	result.translate.num[2] = static_cast<float>(transform["Translate"][2]);

	// 回転角
	result.rotate.num[0] = static_cast<float>(transform["Rotate"][0]);
	result.rotate.num[1] = static_cast<float>(transform["Rotate"][1]);
	result.rotate.num[2] = static_cast<float>(transform["Rotate"][2]);

	// スケーリング
	result.scale.num[0] = static_cast<float>(transform["Scale"][0]);
	result.scale.num[1] = static_cast<float>(transform["Scale"][1]);
	result.scale.num[2] = static_cast<float>(transform["Scale"][2]);

	return result;
}