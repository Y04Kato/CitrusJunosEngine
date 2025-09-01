/**
 * @file GlobalVariablesManager.cpp
 * @brief GlobalVariablesを使いやすく管理出来るクラス
 * @author KATO
 * @date 2025/07/10
 */

#include "GlobalVariablesManager.h"
#include "ImGuiManager.h"

#include<Windows.h>
#include<json.hpp>
#include<fstream>
#include <iostream>

const char** CreateCStringArray(const std::vector<std::string>& strings) {
	//文字列サイズ取得
	size_t size = strings.size();
	const char** cStrings = new const char* [size]; // 動的メモリ確保

	for (size_t i = 0; i < size; ++i) {
		cStrings[i] = strings[i].c_str(); // std::string を const char* に変換
	}

	return cStrings;
}

//アイテムのImGui表記
void ItemImGui(const std::string name, std::variant<bool*, int32_t*, float*, Vector2*, Vector3*, Vector4*> value) {
	value;
	//intの場合
	if (std::holds_alternative<bool*>(value)) {
		bool* ptr = *std::get_if<bool*>(&value);
		ImGui::Checkbox(name.c_str(), ptr);

	}
	else if (std::holds_alternative<int32_t*>(value)) {
		int32_t* ptr = *std::get_if<int32_t*>(&value);
		ImGui::DragInt(name.c_str(), ptr);
	}//floatの場合
	else if (std::holds_alternative<float*>(value)) {
		float* ptr = *std::get_if<float*>(&value);
		ImGui::DragFloat(name.c_str(), ptr, 0.01f);
	}//Vec2
	else if (std::holds_alternative<Vector2*>(value)) {
		Vector2* ptr = *std::get_if<Vector2*>(&value);
		ImGui::DragFloat2(name.c_str(), reinterpret_cast<float*>(ptr), 0.01f);
	}//Vector3の場合
	else if (std::holds_alternative<Vector3*>(value)) {
		Vector3* ptr = *std::get_if<Vector3*>(&value);
		ImGui::DragFloat3(name.c_str(), reinterpret_cast<float*>(ptr), 0.01f);
	}//Vec4
	else if (std::holds_alternative<Vector4*>(value)) {
		Vector4* ptr = *std::get_if<Vector4*>(&value);
		ImGui::ColorEdit4(name.c_str(), &ptr->num[0]);
	}


}

//モニター値に設定された物の操作
void MonitorItemImGui(const std::string name, MonitorItemData& data) {
	name; data;

	//値を参照
	std::variant<bool*, int32_t*, float*, Vector2*, Vector3*, std::string*>& value = data.value;

	//アイテムの値がNullの時
	if (data.items.empty()) {//各ImGuiのテキスト表示
		//intの場合
		if (std::holds_alternative<bool*>(value)) {
			bool* ptr = *std::get_if<bool*>(&value);
			ImGui::Checkbox(name.c_str(), ptr);
		}
		else if (std::holds_alternative<int32_t*>(value)) {
			int32_t* ptr = *std::get_if<int32_t*>(&value);
			std::string text = name + " : %d";
			ImGui::Text(text.c_str(), *ptr);
		}//floatの場合
		else if (std::holds_alternative<float*>(value)) {
			float* ptr = *std::get_if<float*>(&value);
			std::string text = name + " : %4.1f";
			ImGui::Text(text.c_str(), *ptr, 0.01f);
		}//Vector2の場合
		else if (std::holds_alternative<Vector2*>(value)) {
			Vector2* ptr = *std::get_if<Vector2*>(&value);
			std::string text = name + " :  %4.1f / %4.1f";
			ImGui::Text(text.c_str(), *reinterpret_cast<float*>(ptr), 0.01f);

		}//Vector3の場合
		else if (std::holds_alternative<Vector3*>(value)) {
			Vector3* ptr = *std::get_if<Vector3*>(&value);
			std::string text = name + " :  %4.1f / %4.1f / %4.1f";
			ImGui::Text(text.c_str(), *reinterpret_cast<float*>(ptr), 0.01f);
		}
		else if (std::holds_alternative<std::string*>(value)) {
			// char* の場合
			std::string* ptr = *std::get_if<std::string*>(&value);
			std::string text = name + " : %s";
			ImGui::Text(text.c_str(), ptr->c_str());
		}
	}
	else {//コンボのImGui表示
		//int型の時
		if (std::holds_alternative<int32_t*>(value)) {

			//値取得
			int& res = **std::get_if<int32_t*>(&value);
			int curr = res;

			//const char*型にstringを修正
			std::vector<const char*> item;
			for (auto& n : data.items) {
				item.push_back(n.c_str());
			}

			//ImGuiコンボ処理
			if (ImGui::Combo("Animations", &curr, item.data(), int(item.size()))) {
				res = curr;
			}
		}//string型の時
		else if (std::holds_alternative<std::string*>(value)) {
			// char* の場合
			std::string& ptr = **std::get_if<std::string*>(&value);

			//一致する場所の検索
			int count = 0;

			//const char*のvector
			std::vector<const char*> item;
			for (auto& n : data.items) {
				item.push_back(n.c_str());
			}

			//ツリー表示
			for (auto& d : data.items) {
				//値一致時
				if (d == ptr || ptr == "") {
					if (ImGui::Combo("Animations", &count, item.data(), int(item.size()))) {
						ptr = data.items[count];

					}
					return;
				}
				count++;
			}
		}
	}
}

//ツリーのImGui表示処理
void TreeImGui(const std::string& name, TreeData& treeData, size_t size) {
	//ツリー処理開始
	if (ImGui::TreeNode(name.c_str())) {

		//文字出力
		if (!treeData.text.empty()) {
			for (auto& text : treeData.text) {
				ImGui::Text(text.c_str());
			}
		}

		//モニター値の表示
		if (!treeData.monitorKeys.empty()) {

			//もし指定サイズ以上の場合
			if (treeData.monitorKeys.size() > size) {

				//ツリーを作成
				if (ImGui::TreeNode("--MonitorValue---")) {
					//ツリー内に含まれるvalue登場
					for (auto& key : treeData.monitorKeys) {

						//タブ値の名前
						std::string tabName = key;
						//値
						MonitorItemData& item = treeData.monitorValue[key];

						//値の条件で処理変化
						MonitorItemImGui(tabName, item);
					}
					//ツリー終了
					ImGui::TreePop();
				}
			}
			else {
				//ツリーは作らずテキストによる区分けのみ
				ImGui::Text("--MonitorValue---");

				//ツリー内に含まれるvalue登場
				for (auto& key : treeData.monitorKeys) {

					//タブ値の名前
					std::string tabName = key;
					//値
					MonitorItemData& item = treeData.monitorValue[key];

					//値の条件で処理変化
					MonitorItemImGui(tabName, item);
				}
			}
		}

		//値がある場合
		if (!treeData.valueKeys.empty()) {

			//指定サイズ以上にある場合
			if (treeData.valueKeys.size() > size) {

				//ツリーにまとめる
				if (ImGui::TreeNode("--Parameter---")) {

					//ツリー内に含まれるvalue登場
					for (auto& key : treeData.valueKeys) {

						//タブ値の名前
						std::string tabName = key;
						//値
						ItemData& item = treeData.value[key];

						//値の条件で処理変化
						ItemImGui(tabName, item.value);
					}
					//ツリー終了
					ImGui::TreePop();
				}
			}
			else {
				//テキストのみで区分け
				ImGui::Text("--Parameter---");

				//ツリー内に含まれるvalue登場
				for (auto& key : treeData.valueKeys) {

					//タブ値の名前
					std::string tabName = key;
					//値
					ItemData& item = treeData.value[key];

					//値の条件で処理変化
					ItemImGui(tabName, item.value);
				}
			}
		}

		//子ツリーの存在検知
		if (treeData.tree.size() != 0) {

			//子ツリーが指定地以上の場合
			if (treeData.tree.size() > size) {

				//つりーで分ける
				if (ImGui::TreeNode("--ChildTreeー--")) {

					//子ツリーの表示
					for (auto& key : treeData.treeKeys) {
						TreeImGui(key, treeData.tree[key], size);
					}
					//ツリー終わり
					ImGui::TreePop();
				}
			}
			else {
				//テキストで区分けする
				ImGui::Text("--ChildTreeー--");
				//子ツリーの表示
				for (auto& key : treeData.treeKeys) {
					TreeImGui(key, treeData.tree[key], size);
				}
			}
		}

		//ツリー終了
		ImGui::TreePop();
	}


}

GlobalVariablesManager* GlobalVariablesManager::GetInstance(){
	//インスタンス取得
	static GlobalVariablesManager ins;
	return &ins;
}

void GlobalVariablesManager::SetGroup(const std::string& group, GroupData& data){
	//グループを設定
	setDatas_[group] = data;
}

void GlobalVariablesManager::Update(){
	//デバッグウィンドウ表示
	ImGui::Begin(baseName_.c_str());
	if (ImGui::BeginTabBar("LWP")) {
		//各グループで表示
		for (auto& data : setDatas_) {

			//タブを作成
			if (ImGui::BeginTabItem(data.first.c_str())) {

				//保存処理
				if (ImGui::Button("Save Parameter")) {
					//Jsonに出力して保存
					SaveGroupItemData(data.first);
					//メッセージ出力
					std::string message = std::format("{}.json saved.", data.first);
					MessageBoxA(nullptr, message.c_str(), "CJLoader", 0);
				}

				//読み込み処理
				if (ImGui::Button("Load SavedParameter")) {
					//すべてのjson保存データを読み込み
					LoadAllSaveData();
					//セーブデータを適応
					SetLoadGroupData(data.first);
					//メッセージ出力
					std::string message = std::format("{}.json loaded.", data.first);
					MessageBoxA(nullptr, message.c_str(), "CJLoader", 0);
				}

				//モニター地がある場合
				if (!data.second.monitorKeys.empty()) {

					//指定サイズより多い場合
					if (data.second.monitorKeys.size() > nodeSize_) {

						//ツリー作成して仕分ける
						if (ImGui::TreeNode("--MonitorValue--")) {

							//ツリー内に含まれるvalue登場
							for (auto& key : data.second.monitorKeys) {

								//タブ値の名前
								std::string name = key;
								//値
								MonitorItemData& item = data.second.monitorValue[key];

								//値の条件で処理変化
								MonitorItemImGui(name, item);
							}
							//ツリー終了
							ImGui::TreePop();
						}
					}
					else {
						//テキストで区分け
						ImGui::Text("--MonitorValue--");

						//ツリー内に含まれるvalue登場
						for (auto& key : data.second.monitorKeys) {

							//タブ値の名前
							std::string name = key;
							//値
							MonitorItemData& item = data.second.monitorValue[key];

							//値の条件で処理変化
							MonitorItemImGui(name, item);
						}

					}
				}

				//バリューがある場合
				if (!data.second.valueKeys.empty()) {

					//指定サイズよりある場合
					if (data.second.valueKeys.size() > nodeSize_) {

						//ツリーで区分け
						if (ImGui::TreeNode("--Parameter--")) {

							//タブの値を表示
							for (auto& key : data.second.valueKeys) {

								//タブ値の名前
								std::string name = key;
								//値
								ItemData& item = data.second.value[key];

								//値の条件で処理変化
								ItemImGui(name, item.value);
							}
							//ツリー処理終了
							ImGui::TreePop();
						}
					}
					else {
						//テキストで区分け
						ImGui::Text("--Parameter--");

						//タブの値を表示
						for (auto& key : data.second.valueKeys) {

							//タブ値の名前
							std::string name = key;
							//値
							ItemData& item = data.second.value[key];

							//値の条件で処理変化
							ItemImGui(name, item.value);
						}
					}
				}

				//子ツリーの存在検知
				if (data.second.tree.size() != 0) {

					//指定サイズより多い場合
					if (data.second.tree.size() > nodeSize_) {

						//ツリーで区分け
						if (ImGui::TreeNode("--ChildTree--")) {
							//子ツリーの表示
							for (auto& key : data.second.treeKeys) {
								TreeImGui(key, data.second.tree[key], nodeSize_);
							}
							//ツリー終了
							ImGui::TreePop();
						}
					}
					else {
						//テキストで区分け
						ImGui::Text("--ChildTree--");
						//子ツリーの表示
						for (auto& key : data.second.treeKeys) {
							TreeImGui(key, data.second.tree[key], nodeSize_);
						}
					}
				}
				//タブ終了
				ImGui::EndTabItem();
			}
		}
		//タブバー終了
		ImGui::EndTabBar();
	}
	//ImGui終わり
	ImGui::End();
}

/// <summary>
/// アイテムデータの保存
/// </summary>
/// <param name="root">jsonに保存するデータ</param>
/// <param name="data">データ</param>
void SaveItemData(
	nlohmann::json& root,
	std::map<std::string, ItemData> data) {

	//各項目について
	for (std::map<std::string, ItemData>::iterator itItem = data.begin();
		itItem != data.end(); ++itItem) {

		//項目名を取得
		const std::string& itemName = itItem->first;
		//項目の参照を取得
		ItemData& item = itItem->second;

		if (std::holds_alternative<bool*>(item.value)) {//int型の値を保持していれば
			//int32_t型の値を登録
			root[itemName] = *std::get<bool*>(item.value);

		}
		else if (std::holds_alternative<int32_t*>(item.value)) {//int型の値を保持していれば
			//int32_t型の値を登録
			root[itemName] = *std::get<int32_t*>(item.value);

		}
		else if (std::holds_alternative<float*>(item.value)) {//float型の値を保持していれば
			// int32_t型の値を登録
			root[itemName] = *std::get<float*>(item.value);
		}
		else if (std::holds_alternative<Vector2*>(item.value)) {//Vector2型の値を保持していれば
			Vector2 value = *std::get<Vector2*>(item.value);
			root[itemName] = nlohmann::json::array({ value.num[0], value.num[1] });
		}
		else if (std::holds_alternative<Vector3*>(item.value)) {//Vector3型の値を保持していれば
			Vector3 value = *std::get<Vector3*>(item.value);
			root[itemName] = nlohmann::json::array({ value.num[0], value.num[1], value.num[2] });
		}
		else if (std::holds_alternative<Vector4*>(item.value)) {//Vector4型の値を保持していれば
			Vector4 value = *std::get<Vector4*>(item.value);
			root[itemName] = nlohmann::json::array({ value.num[0], value.num[1], value.num[2],value.num[3] });
		}

	}
}

/// <summary>
/// ツリーの再帰的処理
/// </summary>
/// <param name="keyName">元グループ、ツリー名</param>
/// <param name="jsonNode">参照元のノード</param>
/// <param name="treeData">参照データ</param>
void ProcessTree(nlohmann::json& jsonNode, const std::map<std::string, TreeData>& treeData) {

	for (const auto& tree : treeData) {
		// 新しいノードを作成
		nlohmann::json newNode = nlohmann::json::object();

		//ツリー名追加
		std::string treename = tree.first;

		//valueの処理
		SaveItemData(newNode, tree.second.value);

		// 再帰的にtreeを処理
		if (!tree.second.tree.empty()) {
			//ツリーのツリー処理
			ProcessTree(newNode, tree.second.tree);
		}

		// ノードを親のjsonに追加
		jsonNode[treename] = newNode;
	}
}

void GlobalVariablesManager::SaveGroupItemData(const std::string& groupName)
{
	//グループを検索
	std::map<std::string, GroupData>::iterator itGroup = setDatas_.find(groupName);
	//未登録チェック
	assert(itGroup != setDatas_.end());

	nlohmann::json root;
	root = nlohmann::json::object();

	//jsonオブジェスト登録
	root[groupName] = nlohmann::json::object();

	//valueの処理
	SaveItemData(root[groupName], itGroup->second.value);
	//ツリーの処理
	ProcessTree(root[groupName], itGroup->second.tree);

	//ディレクトリがなければ作成する
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists("project/gamedata/levels")) {
		std::filesystem::create_directory("project/gamedata/levels");
	}

	//書き込むJSONファイルのフルパスを合成する
	std::string filepath = kDirectoryPath + groupName + ".json";
	//書き込み用ファイルストリーム
	std::ofstream ofs;
	//ファイルを書き込み用に開く
	ofs.open(filepath);

	//ファイルオープン失敗?
	if (ofs.fail()) {
		std::string message = "Failed open file for write";
		MessageBoxA(nullptr, message.c_str(), "project/gamedata/levels", 0);
		assert(0);
		return;
	}
	//ファイルにjson文字列を書き込む
	ofs << std::setw(4) << root << std::endl;
	//ファイルを閉じる
	ofs.close();
}


void SetLoadTreeData(TreeData& groupData, SavedTreeData& saveData) {

	//現在データからループで参照(value
	for (auto itItem = groupData.value.begin(); itItem != groupData.value.end(); ++itItem) {
		//アイテム名
		const std::string itemname = itItem->first;

		//	セーブデータに同名のデータを検索
		if (saveData.value.end() != saveData.value.find(itemname)) {

			//値取得
			std::variant<bool, int32_t, float, Vector2, Vector3, Vector4>& saveV = saveData.value[itemname].value;
			std::variant<bool*, int32_t*, float*, Vector2*, Vector3*, Vector4*>& dataV = groupData.value[itemname].value;

			///k型が一致する値を代入
			if (std::holds_alternative<bool>(saveV) && std::holds_alternative<bool*>(dataV)) {				//bool型
				bool savePtr = *std::get_if<bool>(&saveV);
				bool* dataPtr = *std::get_if<bool*>(&dataV);
				*dataPtr = savePtr;
			}
			else if (std::holds_alternative<int32_t>(saveV) && std::holds_alternative<int32_t*>(dataV)) {	//int32_t型
				int32_t savePtr = *std::get_if<int32_t>(&saveV);
				int32_t* dataPtr = *std::get_if<int32_t*>(&dataV);
				*dataPtr = savePtr;
			}
			else if (std::holds_alternative<float>(saveV) && std::holds_alternative<float*>(dataV)) {		//float型
				float savePtr = *std::get_if<float>(&saveV);
				float* dataPtr = *std::get_if<float*>(&dataV);
				*dataPtr = savePtr;
			}
			else if (std::holds_alternative<Vector3>(saveV) && std::holds_alternative<Vector3*>(dataV)) {	//vector3型
				Vector3 savePtr = *std::get_if<Vector3>(&saveV);
				Vector3* dataPtr = *std::get_if<Vector3*>(&dataV);
				*dataPtr = savePtr;
			}
			else if (std::holds_alternative<Vector4>(saveV) && std::holds_alternative<Vector4*>(dataV)) {	//vector4型
				Vector4 savePtr = *std::get_if<Vector4>(&saveV);
				Vector4* dataPtr = *std::get_if<Vector4*>(&dataV);
				*dataPtr = savePtr;
			}
		}
	}

	//ツリーの処理
	for (auto itItem = groupData.tree.begin(); itItem != groupData.tree.end(); ++itItem) {
		//アイテム名
		const std::string itemname = itItem->first;

		//見つかった場合
		if (saveData.tree.end() != saveData.tree.find(itemname)) {
			//子ツリーデータ読み込み
			SetLoadTreeData(groupData.tree[itemname], saveData.tree[itemname]);
		}
	}
}


void GlobalVariablesManager::SetLoadGroupData(const std::string& groupName)
{
	//現在のパラメータデータ
	GroupData& groupdata = setDatas_[groupName];

	//保存データ
	SavedGroupData& saveData = saveDatas_[groupName];

	//同グループ名所持が一致しない場合
	if (setDatas_.end() == setDatas_.find(groupName) || saveDatas_.end() == saveDatas_.find(groupName)) {
		return;
	}

	//現在データからループで参照(value
	for (auto itItem = groupdata.value.begin(); itItem != groupdata.value.end(); ++itItem) {
		//アイテム名
		const std::string itemname = itItem->first;

		//セーブデータに同名のデータを検索
		if (saveData.value.end() != saveData.value.find(itemname)) {

			//各値取得
			std::variant<bool, int32_t, float, Vector2, Vector3, Vector4>& saveV = saveData.value[itemname].value;
			std::variant<bool*, int32_t*, float*, Vector2*, Vector3*, Vector4*>& dataV = groupdata.value[itemname].value;

			//合致する値を保存
			if (std::holds_alternative<bool>(saveV) && std::holds_alternative<bool*>(dataV)) {				//bool型
				bool savePtr = *std::get_if<bool>(&saveV);
				bool* dataPtr = *std::get_if<bool*>(&dataV);
				*dataPtr = savePtr;
			}
			else if (std::holds_alternative<int32_t>(saveV) && std::holds_alternative<int32_t*>(dataV)) {	//in32_t型
				int32_t savePtr = *std::get_if<int32_t>(&saveV);
				int32_t* dataPtr = *std::get_if<int32_t*>(&dataV);
				*dataPtr = savePtr;
			}
			else if (std::holds_alternative<float>(saveV) && std::holds_alternative<float*>(dataV)) {		//flaot型
				float savePtr = *std::get_if<float>(&saveV);
				float* dataPtr = *std::get_if<float*>(&dataV);
				*dataPtr = savePtr;
			}
			else if (std::holds_alternative<Vector2>(saveV) && std::holds_alternative<Vector2*>(dataV)) {	//vector3型
				Vector2 savePtr = *std::get_if<Vector2>(&saveV);
				Vector2* dataPtr = *std::get_if<Vector2*>(&dataV);
				*dataPtr = savePtr;
			}
			else if (std::holds_alternative<Vector3>(saveV) && std::holds_alternative<Vector3*>(dataV)) {	//vector3型
				Vector3 savePtr = *std::get_if<Vector3>(&saveV);
				Vector3* dataPtr = *std::get_if<Vector3*>(&dataV);
				*dataPtr = savePtr;
			}
			else if (std::holds_alternative<Vector4>(saveV) && std::holds_alternative<Vector4*>(dataV)) {	//vector4型
				Vector4 savePtr = *std::get_if<Vector4>(&saveV);
				Vector4* dataPtr = *std::get_if<Vector4*>(&dataV);
				*dataPtr = savePtr;
			}
		}

	}
	//同名ツリー処理
		//ツリーの処理
	for (auto itItem = groupdata.tree.begin(); itItem != groupdata.tree.end(); ++itItem) {
		//アイテム名
		const std::string itemname = itItem->first;

		//見つかった場合
		if (saveData.tree.end() != saveData.tree.find(itemname)) {
			//ツリーデータの処理
			SetLoadTreeData(itItem->second, saveData.tree[itemname]);
		}
	}
}

void GlobalVariablesManager::ClearSetData()
{
	//データをクリア
	setDatas_.clear();
}

void LoadTreeData(SavedTreeData& treeData, const nlohmann::json& jsonNode) {
	// 各アイテムについて
	for (auto itItem = jsonNode.begin(); itItem != jsonNode.end(); ++itItem) {

		//名前取得
		const std::string& itemName = itItem.key();

		//セーブデータ取得
		SavedItemData& data = treeData.value[itemName];

		// bool 型を保持している場合
		if (itItem->is_boolean()) {
			//値取得
			bool value = itItem->get<bool>();
			data.value = value;
		}//int ぁタを保持している場合
		else if (itItem->is_number_integer()) {
			//値取得
			int32_t value = itItem->get<int32_t>();
			data.value = value;
		}
		// float 型を保持している場合
		else if (itItem->is_number_float()) {
			float value = itItem->get<float>();
			data.value = value;

		}// Vector3 (3 要素の配列) を保持している場合
		else if (itItem->is_array() && itItem->size() == 3) {
			Vector3 value = { itItem->at(0).get<float>(), itItem->at(1).get<float>(), itItem->at(2).get<float>() };
			data.value = value;
		}
		else if (itItem->is_array() && itItem->size() == 4) {
			Vector4 value = { itItem->at(0).get<float>(), itItem->at(1).get<float>(), itItem->at(2).get<float>(),itItem->at(3).get<float>() };
			data.value = value;
		}//子ツリーがある場合
		else if (itItem->is_object()) {
			// 子ツリーを再帰的に処理
			SavedTreeData& newTree = treeData.tree[itemName];
			//ツリーデータ読み込み
			LoadTreeData(newTree, *itItem);
		}
	}
}

void GlobalVariablesManager::LoadGroupData(const std::string& groupName)
{
	//よみこむJSONファイルのフルパスを合成する
	std::string filepath = kDirectoryPath + groupName + ".json";
	//読み込み用ファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に聞く
	ifs.open(filepath);

	// ファイルオープン失敗?
	if (ifs.fail()) {
		std::string message = "Failed open file for write";
		MessageBoxA(nullptr, message.c_str(), "project/gamedata/levels", 0);
		assert(0);
		return;
	}

	nlohmann::json root;
	//json文字列からjsonのデータの構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();

	//グループを検索
	nlohmann::json::iterator itGroup = root.find(groupName);

	//未登録チェック
	assert(itGroup != root.end());

	//各データクリア
	saveDatas_[groupName].value.clear();
	saveDatas_[groupName].tree.clear();

	//各アイテムについて
	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		//アイテム名を取得
		const std::string& itemName = itItem.key();

		SavedItemData& data = saveDatas_[groupName].value[itemName];

		//int型を保持していた場合
		if (itItem->is_boolean()) {
			bool value = itItem->get<bool>();
			data.value = value;
		}
		else if (itItem->is_number_integer()) {
			int32_t value = itItem->get<int32_t>();
			data.value = value;
		}
		else if (itItem->is_number_float()) {// float型を保持していた場合
			float value = itItem->get<float>();

			data.value = value;
		}
		else if (itItem->is_array() && itItem->size() == 2) { // 要素数２の配列であれば
			Vector2 value = { itItem->at(0), itItem->at(1) };

			data.value = value;
		}
		else if (itItem->is_array() && itItem->size() == 3) { // 要素数３の配列であれば
			Vector3 value = { itItem->at(0), itItem->at(1), itItem->at(2) };

			data.value = value;
		}
		else if (itItem->is_array() && itItem->size() == 4) { // 要素数３の配列であれば
			Vector4 value = { itItem->at(0), itItem->at(1), itItem->at(2),itItem->at(3) };

			data.value = value;
		}       // 子ツリーの読み込み処理
		else if (itItem->is_object()) {
			LoadTreeData(saveDatas_[groupName].tree[itemName], *itItem);
		}
	}

}

void GlobalVariablesManager::LoadAllSaveData()
{
	// ディレクトリがなければ返す
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists("project/gamedata/levels/")) {
		return;
	}

	std::filesystem::directory_iterator dir_it("project/gamedata/levels");
	for (const std::filesystem::directory_entry& entry : dir_it) {
		//ファイルパスを取得
		const std::filesystem::path& filepath = entry.path();

		//ファイル拡張子を取得
		std::string extention = filepath.extension().string();
		//.jsonファイル以外はスキップ
		if (extention.compare(".json") != 0) {
			continue;
		}
		LoadGroupData(filepath.stem().string());

	}
}

void GlobalVariablesManager::SetLoadAllData()
{

	for (auto& data : setDatas_) {
		std::string name = data.first;

		SetLoadGroupData(name);
	}


}