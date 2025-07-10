/**
 * @file GlobalVariablesTree.h
 * @brief GlobalVariablesDataを元にデータをツリーでまとめるクラス
 * @author KATO
 * @date 2025/07/10
 */

#pragma once
#include "GlobalVariablesData.h"

 //グローバル値ツリークラス
class GlobalVariablesTree {
public:
	//ツリーデータ作成
	GlobalVariablesTree(const std::string& treeName = "ツリー");
	~GlobalVariablesTree() = default;

	//ツリーのすべてのパラの前に表示するテキストの追加
	void SetText(const std::string& text);

	/// <summary>
	/// 保存可能な値を追加
	/// </summary>
	/// <param name="name">ImGui時の補足</param>
	/// <param name="value"></param>
	void SetValue(const std::string& name, std::variant<bool*, int32_t*, float*, Vector2*, Vector3*, Vector4*> value);

	//監視値追加
	void SetMonitorValue(const std::string& name, std::variant<bool*, int32_t*, float*, Vector2*, Vector3*, std::string*> value);

	//ImGuiのコンボでデバッグ
	void SetMonitorCombo(const std::string& name, std::string* item, const std::vector<std::string>& items);

	//ImGuiのコンボでデバッグ
	void SetMonitorCombo(const std::string& name, int32_t* item, const std::vector<std::string>& items);


	//ツリーデータ格納
	void SetTreeData(GlobalVariablesTree& tree);

public:
	//ツリー名
	std::string name_;

	//データ
	TreeData datas_;
};

/// <summary>
/// GlobalVariablesTreeの別名
/// </summary>
using GvariTree = GlobalVariablesTree;