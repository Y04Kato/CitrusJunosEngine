/**
 * @file GlobalVariables.h
 * @brief 各種変数の中身をまとめて一つの.jsonファイルに保存、読み込みを行える機能
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "Vector.h"
#include <json.hpp>
#include <map>
#include <string>
#include <variant>

class GlobalVariables {
public:
	static GlobalVariables* GetInstance();

	void CreateGroup(const std::string& groupName);
	void Update();

	//値を設定する
	void SetValue(const std::string& groupName, const std::string& key, int32_t value);
	void SetValue(const std::string& groupName, const std::string& key, float value);
	void SetValue(const std::string& groupName, const std::string& key, Vector3& value);
	void SetValue(const std::string& groupName, const std::string& key, Vector4& value);
	void SetValue(const std::string& groupName, const std::string& key, std::string value);

	//セーブ&ロード
	void SaveFile(const std::string& groupName);
	void LoadFile(const std::string& groupName);
	void LoadFiles();

	//値を追加する
	void AddItem(const std::string& groupName, const std::string& key, int32_t value);
	void AddItem(const std::string& groupName, const std::string& key, float value);
	void AddItem(const std::string& groupName, const std::string& key, Vector3 value);
	void AddItem(const std::string& groupName, const std::string& key, Vector4 value);
	void AddItem(const std::string& groupName, const std::string& key, std::string value);

	//指定したデータを削除
	void RemoveItem(const std::string& groupName, const std::string& key);

	//値を取得する
	int32_t GetIntValue(const std::string& groupName, const std::string& key);
	float GetFloatValue(const std::string& groupName, const std::string& key);
	Vector3 GetVector3Value(const std::string& groupName, const std::string& key);
	Vector4 GetVector4Value(const std::string& groupName, const std::string& key);
	std::string GetStringValue(const std::string& groupName, const std::string& key);

	//データグループを探す
	bool GroupNameSearch(const std::string& groupName);

private:
	GlobalVariables() = default;
	~GlobalVariables() = default;
	GlobalVariables(const GlobalVariables& obj) = default;
	GlobalVariables& operator=(const GlobalVariables& obj) = default;

public:
	struct Item {
		std::variant<int32_t, float, Vector3, Vector4, std::string> value;
	};

	struct Group {
		std::map<std::string, Item> items;
	};

	std::map<std::string, Group> datas_;

	using json = nlohmann::json;
	const std::string kDirectoryPath = "project/gamedata/levels/";
};