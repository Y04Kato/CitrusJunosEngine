/**
 * @file LevelData.h
 * @brief .jsonから読み取るオブジェクトデータの形を構造体としてクラス化している部分
 * @author KATO
 * @date 未記録
 */

#pragma once
#include <string>
#include <variant>
#include <vector>
#include "Model.h"

class LevelData{
public:
	//メッシュデータ
	struct MeshData{
		std::string name;
		std::string flieName;
		EulerTransform transform;
		bool isParent;
	};

	using ObjectData = MeshData;

public:

	//オブジェクトの情報
	std::vector<ObjectData> objectsData_;

};