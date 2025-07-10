/**
 * @file GlobalVariablesManager.h
 * @brief GlobalVariablesを使いやすく管理出来るクラス
 * @author KATO
 * @date 2025/07/10
 */

#pragma once
#include "GlobalVariablesData.h"

 //すべてのGlobalVariableの管理
class GlobalVariablesManager {
public:
	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns>インスタンス返却</returns>
	static GlobalVariablesManager* GetInstance();

private:
	GlobalVariablesManager() = default;
	~GlobalVariablesManager() = default;
	GlobalVariablesManager(const GlobalVariablesManager& o) = delete;
	const GlobalVariablesManager& operator=(const GlobalVariablesManager& o) = delete;

public:
	/// <summary>
	/// グループの追加
	/// </summary>
	/// <param name="group">グループデータ名</param>
	/// <param name="data">グループデータ</param>
	void SetGroup(const std::string& group, GroupData& data);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// Jsonに保存されているデータを読み込み
	/// </summary>
	void LoadAllSaveData();

	/// <summary>
	/// 読み込まれているセーブデータを今のパラメータに読み込む
	/// </summary>
	void SetLoadAllData();

	/// <summary>
	/// セットしたデータの削除
	/// </summary>
	void ClearSetData();

private:
	/// <summary>
	/// グループのセーブデータのセット
	/// </summary>
	/// <param name="groupName">グループ名</param>
	void SetLoadGroupData(const std::string& groupName);

	/// <summary>
	/// グループデータの保存
	/// </summary>
	/// <param name="groupName"></param>
	void SaveGroupItemData(const std::string& groupName);

	/// <summary>
	/// グループデータの読み込み
	/// </summary>
	/// <param name="groupName"></param>
	void LoadGroupData(const std::string& groupName);

private:
	//基本ウィンドウの名前
	std::string baseName_ = "Game";

	//グループのデータ群
	std::map<std::string, GroupData> setDatas_;

	//保存データ群
	std::map<std::string, SavedGroupData> saveDatas_;

	//グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "project/gamedata/levels/";

	//ノード表記にまとめ始める量
	size_t nodeSize_ = 4;
};

/// <summary>
/// GlobalVariablesManagerの別名
/// </summary>
using GvariM = GlobalVariablesManager;
