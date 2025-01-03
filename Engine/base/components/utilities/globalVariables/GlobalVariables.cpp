/**
 * @file GlobalVariables.cpp
 * @brief 各種変数の中身をまとめて一つの.jsonファイルに保存、読み込みを行える機能
 * @author KATO
 * @date 未記録
 */

#include "GlobalVariables.h"
#include "CJEngine.h"
#include <fstream>

GlobalVariables* GlobalVariables::GetInstance() {
	static GlobalVariables instance;

	return &instance;
}

void GlobalVariables::CreateGroup(const std::string& groupName) { datas_[groupName]; }

void GlobalVariables::Update() {
	if (!ImGui::Begin("Global Variables", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}

	if (!ImGui::BeginMenuBar())
		return;

	for (std::map<std::string, Group>::iterator itGroup = datas_.begin(); itGroup != datas_.end();
		++itGroup) {
		const std::string& groupName = itGroup->first;
		Group& group = itGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str()))
			continue;

		if (ImGui::Button("Save")) {
			SaveFile(groupName);
			std::string message = std::format("{}.json saved", groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		}

		for (std::map<std::string, Item>::iterator itItem = group.items.begin();
			itItem != group.items.end(); ++itItem) {
			const std::string& itemName = itItem->first;
			Item& item = itItem->second;

			if (std::holds_alternative<int32_t>(item.value)) {
				int32_t* ptr = std::get_if<int32_t>(&item.value);
				ImGui::DragInt(itemName.c_str(), ptr);
			}
			else if (std::holds_alternative<float>(item.value)) {
				float* ptr = std::get_if<float>(&item.value);
				ImGui::DragFloat(itemName.c_str(), ptr);
			}
			else if (std::holds_alternative<Vector3>(item.value)) {
				Vector3* ptr = std::get_if<Vector3>(&item.value);
				ImGui::DragFloat3(itemName.c_str(), reinterpret_cast<float*>(ptr));
			}
			else if (std::holds_alternative<Vector4>(item.value)) {
				Vector4* ptr = std::get_if<Vector4>(&item.value);
				ImGui::ColorEdit4(itemName.c_str(), reinterpret_cast<float*>(ptr), 0);
			}
			else if (std::holds_alternative<std::string>(item.value)) {
				std::string* ptr = std::get_if<std::string>(&item.value);
				ImGui::Text("ItemName %c", ptr);
			}
		}

		ImGui::EndMenu();
	}

	ImGui::EndMenuBar();
	ImGui::End();
}

void GlobalVariables::SetValue(
	const std::string& groupName, const std::string& key, int32_t value) {
	Group& group = datas_[groupName];
	Item newItems{};

	newItems.value = value;
	group.items[key] = newItems;
}

void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, float value) {
	Group& group = datas_[groupName];
	Item newItems{};

	newItems.value = value;
	group.items[key] = newItems;
}

void GlobalVariables::SetValue(
	const std::string& groupName, const std::string& key, Vector3& value) {
	Group& group = datas_[groupName];
	Item newItems{};

	newItems.value = value;
	group.items[key] = newItems;
}

void GlobalVariables::SetValue(
	const std::string& groupName, const std::string& key, Vector4& value) {
	Group& group = datas_[groupName];
	Item newItems{};

	newItems.value = value;
	group.items[key] = newItems;
}

void GlobalVariables::SetValue(
	const std::string& groupName, const std::string& key, std::string value) {
	Group& group = datas_[groupName];
	Item newItems{};

	newItems.value = value;
	group.items[key] = newItems;
}

void GlobalVariables::SaveFile(const std::string& groupName) {
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);
	assert(itGroup != datas_.end());

	json root;
	root = json::object();

	root[groupName] = json::object();

	for (std::map<std::string, Item>::iterator itItem = itGroup->second.items.begin();
		itItem != itGroup->second.items.end(); ++itItem) {
		const std::string& itemName = itItem->first;
		Item& item = itItem->second;

		if (std::holds_alternative<int32_t>(item.value)) {
			root[groupName][itemName] = std::get<int32_t>(item.value);
		}
		else if (std::holds_alternative<float>(item.value)) {
			root[groupName][itemName] = std::get<float>(item.value);
		}
		else if (std::holds_alternative<std::string>(item.value)) {
			root[groupName][itemName] = std::get<std::string>(item.value);
		}

		if (std::holds_alternative<Vector3>(item.value)) {
			Vector3 value = std::get<Vector3>(item.value);
			root[groupName][itemName] = json::array({ value.num[0], value.num[1], value.num[2] });
		}
		else if (std::holds_alternative<Vector4>(item.value)) {
			Vector4 value = std::get<Vector4>(item.value);
			root[groupName][itemName] = json::array({ value.num[0], value.num[1], value.num[2], value.num[3] });
		}
	}

	std::filesystem::path dir(kDirectoryPath);

	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directory(kDirectoryPath);
	}

	std::string filePath = kDirectoryPath + groupName + ".json";
	std::ofstream ofs{};
	ofs.open(filePath);

	if (ofs.fail()) {
		std::string message = "Failed Open Data File Fow Write";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	ofs << std::setw(4) << root << std::endl;
	ofs.close();
}

void GlobalVariables::LoadFile(const std::string& groupName) {
	std::string filePath = kDirectoryPath + groupName + ".json";

	std::ifstream ifs;
	ifs.open(filePath);

	if (ifs.fail()) {
		std::string message = "Failed Open Data File For Write";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	json root;
	ifs >> root;
	ifs.close();

	json::iterator itGroup = root.find(groupName);

	assert(itGroup != root.end());

	for (json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		const std::string& itemName = itItem.key();

		if (itItem->is_number_integer()) {
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName, itemName, value);
		}
		else if (itItem->is_number_float()) {
			double value = itItem->get<double>();
			SetValue(groupName, itemName, static_cast<float>(value));
		}
		else if (itItem->is_string()) {
			std::string value = itItem->get<std::string>();
			SetValue(groupName, itemName, static_cast<std::string>(value));
		}
		else if (itItem->is_array() && itItem->size() == 3) {
			Vector3 value = { itItem->at(0), itItem->at(1), itItem->at(2) };
			SetValue(groupName, itemName, value);
		}
		else if (itItem->is_array() && itItem->size() == 4) {
			Vector4 value = { itItem->at(0), itItem->at(1), itItem->at(2) ,itItem->at(3) };
			SetValue(groupName, itemName, value);
		}
	}
}

void GlobalVariables::LoadFiles() {
	std::filesystem::path dir(kDirectoryPath);

	if (!std::filesystem::exists(kDirectoryPath)) {
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath);

	for (const std::filesystem::directory_entry& entry : dir_it) {
		const std::filesystem::path& filePath = entry.path();
		std::string extension = filePath.extension().string();

		if (extension.compare(".json") != 0) {
			continue;
		}

		LoadFile(filePath.stem().string());
	}
}

void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, int32_t value) {
	Group& group = datas_[groupName];

	if (group.items.find(key) == group.items.end()) {
		SetValue(groupName, key, value);
	}
}

void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, float value) {
	Group& group = datas_[groupName];

	if (group.items.find(key) == group.items.end()) {
		SetValue(groupName, key, value);
	}
}

void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, Vector3 value) {
	Group& group = datas_[groupName];

	if (group.items.find(key) == group.items.end()) {
		SetValue(groupName, key, value);
	}
}

void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, Vector4 value) {
	Group& group = datas_[groupName];

	if (group.items.find(key) == group.items.end()) {
		SetValue(groupName, key, value);
	}
}

void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, std::string value) {
	Group& group = datas_[groupName];

	if (group.items.find(key) == group.items.end()) {
		SetValue(groupName, key, value);
	}
}

int32_t GlobalVariables::GetIntValue(const std::string& groupName, const std::string& key) {
	assert(datas_.find(groupName) != datas_.end());

	Group& group = datas_[groupName];
	assert(group.items.find(key) != group.items.end());

	return std::get<int32_t>(group.items[key].value);
}

float GlobalVariables::GetFloatValue(const std::string& groupName, const std::string& key) {
	assert(datas_.find(groupName) != datas_.end());

	Group& group = datas_[groupName];
	assert(group.items.find(key) != group.items.end());

	return std::get<float>(group.items[key].value);
}

Vector3 GlobalVariables::GetVector3Value(const std::string& groupName, const std::string& key) {
	assert(datas_.find(groupName) != datas_.end());

	Group& group = datas_[groupName];
	assert(group.items.find(key) != group.items.end());

	return std::get<Vector3>(group.items[key].value);
}

Vector4 GlobalVariables::GetVector4Value(const std::string& groupName, const std::string& key) {
	assert(datas_.find(groupName) != datas_.end());

	Group& group = datas_[groupName];
	assert(group.items.find(key) != group.items.end());

	return std::get<Vector4>(group.items[key].value);
}

std::string GlobalVariables::GetStringValue(const std::string& groupName, const std::string& key) {
	assert(datas_.find(groupName) != datas_.end());

	Group& group = datas_[groupName];
	assert(group.items.find(key) != group.items.end());

	return std::get<std::string>(group.items[key].value);
}

void GlobalVariables::RemoveItem(const std::string& groupName, const std::string& key) {
	// グループが存在するかを確認
	auto groupIt = datas_.find(groupName);
	if (groupIt == datas_.end()) {
		return; // グループが存在しない場合は何もしない
	}

	// グループ内にアイテムが存在するかを確認
	auto itemIt = groupIt->second.items.find(key);
	if (itemIt == groupIt->second.items.end()) {
		return; // アイテムが存在しない場合は何もしない
	}

	// アイテムを削除
	groupIt->second.items.erase(itemIt);
}

bool GlobalVariables::GroupNameSearch(const std::string& groupName) {
	// グループが存在するかを確認
	auto groupIt = datas_.find(groupName);
	if (groupIt == datas_.end()) {//グループが存在しない場合はfalseを返す
		return false;
	}
	else {//ある場合trueを返す
		return true;
	}
}