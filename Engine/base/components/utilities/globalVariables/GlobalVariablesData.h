/**
 * @file GlobalVariablesData.h
 * @brief GlobalVariablesのデータの形をここで管理
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "Vector.h"

#include<variant>
#include<string>
#include<vector>
#include<map>

//アイテムのデータ
struct ItemData {
	//値
	std::variant<bool*, int32_t*, float*, Vector2*, Vector3*, Vector4*> value;
};

//監視アイテムのデータ
struct MonitorItemData {
	//値
	std::variant<bool*, int32_t*, float*, Vector2*, Vector3*, std::string*> value;

	//コンボ時利用
	std::vector<std::string> items;
};

//ツリーデータ
struct TreeData {
	//テキスト
	std::vector<std::string>text;

	//値
	std::map<std::string, ItemData>value;

	//監視する値
	std::map<std::string, MonitorItemData>monitorValue;

	//ツリーデータ
	std::map<std::string, TreeData>tree;

	//順に表示する用のキー
	//値のキー
	std::vector<std::string>valueKeys;
	//モニター値のキー
	std::vector<std::string>monitorKeys;
	//ツリーのキー
	std::vector<std::string>treeKeys;
};

//グループのデータ
struct GroupData{
	//値
	std::map<std::string, ItemData>value;

	//監視する値
	std::map<std::string, MonitorItemData>monitorValue;

	//ツリーデータ
	std::map<std::string, TreeData>tree;

	//順に表示する用のキー
	//値のキー
	std::vector<std::string>valueKeys;
	//モニター値のキー
	std::vector<std::string>monitorKeys;
	//ツリーのキー
	std::vector<std::string>treeKeys;
};

//保存アイテムデータ
struct SavedItemData {
	//値
	std::variant<bool, int32_t, float, Vector2, Vector3, Vector4> value;
};

//保存アイテムデータツリー
struct SavedTreeData{
	//値
	std::map<std::string, SavedItemData>value;

	//ツリーデータ
	std::map<std::string, SavedTreeData>tree;
};

//保存アイテムグループデータ
struct SavedGroupData {
	//値
	std::map<std::string, SavedItemData>value;

	//ツリーデータ
	std::map<std::string, SavedTreeData>tree;
};
