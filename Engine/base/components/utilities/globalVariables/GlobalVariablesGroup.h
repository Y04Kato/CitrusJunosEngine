/**
 * @file GlobalVariablesGroup.h
 * @brief GlobalVariablesのデータをグループにまとめるクラス
 * @author KATO
 * @date 2025/07/10
 */

#pragma once
#include "GlobalVariablesTree.h"

 //グローバル値をまとめたグループ
class GlobalVariablesGroup {
public:
	/// <summary>
	/// 生成（日本語だと出力名がバグるので注意)
	/// </summary>
	/// <param name="groupName"></param>
	GlobalVariablesGroup(const std::string& groupName);
	//削除時データを送る
	~GlobalVariablesGroup();

	/// <summary>
	/// デバッグ値追加
	/// </summary>
	/// <param name="name">名前</param>
	/// <param name="value">値</param>
	void SetValue(const std::string& name, std::variant<bool*, int32_t*, float*, Vector2*, Vector3*, Vector4*> value);

	/// <summary>
	/// 保存されないデバッグ値の追加
	/// </summary>
	/// <param name="name">値の名前</param>
	/// <param name="value">値</param>
	void SetMonitorValue(const std::string& name, std::variant<bool*, int32_t*, float*, Vector2*, Vector3*, std::string*> value);

	/// <summary>
	/// ImGuiのコンボでデバッグ
	/// </summary>
	/// <param name="name">コンボ名</param>
	/// <param name="item">変更する値</param>
	/// <param name="items">文字列群</param>
	void SetMonitorCombo(const std::string& name, std::string* item, const std::vector<std::string>& items);

	/// <summary>
	/// ツリーデータ格納
	/// </summary>
	/// <param name="tree">ツリーデータ</param>
	void SetTreeData(GlobalVariablesTree& tree);

private: //**プライベート変数**//

	//グループ名(日本語だとエラー)
	std::string groupName_;

	//データ
	GroupData datas_;
};


/// <summary>
/// GlobalVariablesGroupの別名
/// </summary>
using GVariGroup = GlobalVariablesGroup;