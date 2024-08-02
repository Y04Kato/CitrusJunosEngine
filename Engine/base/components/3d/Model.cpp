#include "Model.h"

void Model::Initialize(const std::string& directoryPath, const std::string& filename) {
	dxCommon_ = DirectXCommon::GetInstance();
	CJEngine_ = CitrusJunosEngine::GetInstance();
	textureManager_ = TextureManager::GetInstance();
	srvManager_ = SRVManager::GetInstance();
	directionalLights_ = DirectionalLights::GetInstance();
	pointLights_ = PointLights::GetInstance();

	modelData_ = LoadModelFile(directoryPath, filename);
	animation_ = LoadAnimationFile(directoryPath, filename);
	modelData_.textureIndex = textureManager_->Load(modelData_.material.textureFilePath);

	CreateVartexData();
	SetColor();
	CreateLight();
}

void Model::Initialize(const ModelData modeldata, const uint32_t texture) {
	dxCommon_ = DirectXCommon::GetInstance();
	CJEngine_ = CitrusJunosEngine::GetInstance();
	textureManager_ = TextureManager::GetInstance();
	directionalLights_ = DirectionalLights::GetInstance();
	pointLights_ = PointLights::GetInstance();

	modelData_ = modeldata;
	modelData_.textureIndex = texture;

	CreateVartexData();
	SetColor();
	CreateLight();
}

void Model::SkinningInitialize(const std::string& directoryPath, const std::string& filename) {
	dxCommon_ = DirectXCommon::GetInstance();
	CJEngine_ = CitrusJunosEngine::GetInstance();
	textureManager_ = TextureManager::GetInstance();
	srvManager_ = SRVManager::GetInstance();
	directionalLights_ = DirectionalLights::GetInstance();
	pointLights_ = PointLights::GetInstance();

	modelData_ = LoadModelFile(directoryPath, filename);
	animation_ = LoadAnimationFile(directoryPath, filename);
	skeleton_ = CreateSkeleton(modelData_.rootNode);
	modelData_.textureIndex = textureManager_->Load(modelData_.material.textureFilePath);

	CreateVartexData();
	SetColor();
	CreateLight();

	skinCluster_ = CreateSkinCluster();
}

void Model::SkinningInitialize(const ModelData modeldata, const uint32_t texture) {
	dxCommon_ = DirectXCommon::GetInstance();
	CJEngine_ = CitrusJunosEngine::GetInstance();
	textureManager_ = TextureManager::GetInstance();
	srvManager_ = SRVManager::GetInstance();
	directionalLights_ = DirectionalLights::GetInstance();
	pointLights_ = PointLights::GetInstance();

	modelData_ = modeldata;
	modelData_.textureIndex = texture;
	animation_ = LoadAnimationFile(modeldata.directoryPath,modeldata.filename);
	skeleton_ = CreateSkeleton(modelData_.rootNode);

	CreateVartexData();
	SetColor();
	CreateLight();

	skinCluster_ = CreateSkinCluster();
}

void Model::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, const Vector4& material) {
	EulerTransform uvTransform = { { 1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };

	Matrix4x4 uvtransformMtrix = MakeScaleMatrix(uvTransform.scale);
	uvtransformMtrix = Multiply(uvtransformMtrix, MakeRotateZMatrix(uvTransform.rotate.num[2]));
	uvtransformMtrix = Multiply(uvtransformMtrix, MakeTranslateMatrix(uvTransform.translate));

	cameraData_->worldPosition = viewProjection.translation_;

	if (isDirectionalLight_ == false) {
		*material_ = { material,0 };
	}
	else {
		*material_ = { material,lightNum_ };
	}

	material_->uvTransform = uvtransformMtrix;
	material_->shininess = 100.0f;
	*directionalLight_ = directionalLights_->GetDirectionalLight();
	*pointLight_ = pointLights_->GetPointLight();

	if (isKeyframeAnim_) {//KeyframeAnimationの場合
		if (isManualAnimTime_) {

		}
		else {
			animationTime_ += 1.0f / 60.0f;//時間を進める
			animationTime_ = std::fmod(animationTime_, animation_.duration);//最後までいったらリピート再生
		}

		NodeAnimation& rootNodeAnimation = animation_.nodeAnimations[modelData_.rootNode.name];//rootNodeのAnimationを取得
		Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
		Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);
		Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
		Matrix4x4 localM = MakeQuatAffineMatrix(scale, MakeRotateMatrix(rotate), translate);

		world_ = worldTransform;
		world_.constMap->matWorld = Multiply(localM, Multiply(modelData_.rootNode.localMatrix, worldTransform.matWorld_));
		world_.constMap->inverseTranspose = Inverse(Transpose(world_.constMap->matWorld));
	}
	else {
		world_ = worldTransform;
		world_.constMap->matWorld = Multiply(modelData_.rootNode.localMatrix, worldTransform.matWorld_);
		world_.constMap->inverseTranspose = Inverse(Transpose(world_.constMap->matWorld));
	}

	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1,&vertexBufferView_);
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	//形状を設定。PS0にせっていしているものとはまた別
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, pointLightResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, world_.constBuff_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, cameraResource_->GetGPUVirtualAddress());



	if (isVAT_ == true) {//VATモデルである場合
		ImGui::Text("VATAnimFrame %f", vatData_->VATTime);
		vatData_->VATTime = vatData_->VATTime / vatData_->MaxVATTime;
		ImGui::Text("VATAnimFrame /MaxVATAnimTime  %f", vatData_->VATTime);
		ImGui::Text("%f %f %f %f", vatData_->VatPositionTexSize.num[0], vatData_->VatPositionTexSize.num[1], vatData_->VatPositionTexSize.num[2], vatData_->VatPositionTexSize.num[3]);

		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(7, textureManager_->GetGPUHandle(vatPosTex_));
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(8, textureManager_->GetGPUHandle(vatRotTex_));
		dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(9, vatResource_->GetGPUVirtualAddress());
		if (isSetEnviromentTexture_ == true) {
			dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(10, textureManager_->GetGPUHandle(environmentTexture_));
		}
		else {
			dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(10, textureManager_->GetGPUHandle(textureManager_->ddsSample));
		}

	}
	else {
		if (isSetEnviromentTexture_ == true) {
			dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(7, textureManager_->GetGPUHandle(environmentTexture_));
		}
		else {
			dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(7, textureManager_->GetGPUHandle(textureManager_->ddsSample));
		}
	}

	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(modelData_.textureIndex));
	dxCommon_->GetCommandList()->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1, 0, 0, 0);

}

void Model::SkinningDraw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, const Vector4& material) {
	EulerTransform uvTransform = { { 1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };

	Matrix4x4 uvtransformMtrix = MakeScaleMatrix(uvTransform.scale);
	uvtransformMtrix = Multiply(uvtransformMtrix, MakeRotateZMatrix(uvTransform.rotate.num[2]));
	uvtransformMtrix = Multiply(uvtransformMtrix, MakeTranslateMatrix(uvTransform.translate));

	cameraData_->worldPosition = viewProjection.translation_;

	if (isDirectionalLight_ == false) {
		*material_ = { material,0 };
	}
	else {
		*material_ = { material,lightNum_ };
	}

	material_->uvTransform = uvtransformMtrix;
	material_->shininess = 100.0f;
	*directionalLight_ = directionalLights_->GetDirectionalLight();
	*pointLight_ = pointLights_->GetPointLight();

	animationTime_ += 1.0f / 60.0f;//時間を進める
	animationTime_ = std::fmod(animationTime_, animation_.duration);//最後までいったらリピート再生
	world_ = worldTransform;
	ApplyAnimation(skeleton_, animation_, animationTime_);
	Update(skeleton_, skinCluster_);

	D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
	vertexBufferView_,
	skinCluster_.influenceBufferView
	};

	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 2, vbvs);
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	//形状を設定。PS0にせっていしているものとはまた別
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, pointLightResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, world_.constBuff_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, cameraResource_->GetGPUVirtualAddress());

	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(modelData_.textureIndex));
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(7, skinCluster_.paletteSrvHandle.GPU);
	if (isSetEnviromentTexture_ == true) {
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(8, textureManager_->GetGPUHandle(environmentTexture_));
	}
	else {
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(8, textureManager_->GetGPUHandle(textureManager_->ddsSample));
	}
	dxCommon_->GetCommandList()->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1, 0, 0, 0);

}

void Model::Finalize() {

}

Model* Model::CreateModel(const std::string& directoryPath, const std::string& filename) {
	Model* model = new Model();
	model->Initialize(directoryPath, filename);
	return model;
}

Model* Model::CreateModel(const ModelData modeldata, const uint32_t texture) {
	Model* model = new Model();
	model->Initialize(modeldata, texture);
	return model;
}

Model* Model::CreateSkinningModel(const std::string& directoryPath, const std::string& filename) {
	Model* model = new Model();
	model->SkinningInitialize(directoryPath, filename);
	return model;
}
Model* Model::CreateSkinningModel(const ModelData modeldata, const uint32_t texture) {
	Model* model = new Model();
	model->SkinningInitialize(modeldata, texture);
	return model;
}

ModelData Model::LoadModelFile(const std::string& directoryPath, const std::string& filename) {
	ModelData modelData;//構築するModelData

	modelData.directoryPath = directoryPath;
	modelData.filename = filename;

	Assimp::Importer importer;

	std::string file(directoryPath + "/" + filename);//ファイルを開く

	const aiScene* scene = importer.ReadFile(file.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);

	//meshが存在しない物は対応しない
	assert(scene->HasMeshes());

	modelData.rootNode = ReadNode(scene->mRootNode);

	//meshの解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());//法線がないmeshは非対応
		if (mesh->HasTextureCoords(0) == true) {//TexCoordの確認
			isLoadTexCoord_ = true;
		}
		else {//TexCoordがない場合
			isLoadTexCoord_ = false;
		}
		modelData.vertices.resize(mesh->mNumVertices);//最初に頂点数分のメモリ確保

		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
			modelData.vertices[vertexIndex].position = { -position.x,position.y,position.z,1.0f };
			modelData.vertices[vertexIndex].normal = { -normal.x,normal.y,normal.z };
			if (isLoadTexCoord_ == true) {//TexCoordの設定
				modelData.vertices[vertexIndex].texcoord = { texcoord.x,texcoord.y };
			}
			else {//無い場合、手動で設定する
				modelData.vertices[vertexIndex].texcoord = { 32.0f,32.0f };
			}
		}

		//meshの中身(face)の解析を行う
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);//三角形のみ対応

			//faceの中身(vertex)の解析を行う
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				//faceから取得したvertexIndexでmeshからデータを取り出す
				uint32_t vertexIndex = face.mIndices[element];

				modelData.indices.push_back(vertexIndex);
			}
		}

		//SkinCluster用のデータ解析
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			//Jointごとの格納領域を作る
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modelData.skinClusterData[jointName];

			//InverseBindPoseMatrixの抽出
			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D scale, translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
			Matrix4x4 bindPoseMatrix = MakeQuatAffineMatrix(
				{ scale.x,scale.y,scale.z },
				MakeRotateMatrix({ rotate.x,-rotate.y,-rotate.z,rotate.w }),
				{ -translate.x,translate.y,translate.z }
			);
			jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);

			//Weight情報を解析
			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight,bone->mWeights[weightIndex].mVertexId });
			}
		}
	}

	//materialの解析(現在はマルチマテリアル非対応)
	if (isLoadTexCoord_ == true) {//モデルにテクスチャがテクスチャが設定されている場合
		modelData.material.textureFilePath = "project/gamedata/resources/null.png";
		for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
			aiMaterial* material = scene->mMaterials[materialIndex];

			//Materialに設定されているTextureを用途に応じて取得する
			//模様
			if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
				aiString textureFilePath;
				material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
				modelData.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
			}
			else {
				
			}
		}
	}
	else {//モデルにテクスチャがテクスチャが設定されていない場合
		modelData.material.textureFilePath = "project/gamedata/resources/null.png";
	}

	return modelData;
}

Animation Model::LoadAnimationFile(const std::string& directoryPath, const std::string& filename) {
	Animation animation;//今回作るアニメーション
	Assimp::Importer importer;

	std::string file(directoryPath + "/" + filename);//ファイルを開く

	const aiScene* scene = importer.ReadFile(file.c_str(), 0);

	//アニメーションが存在しない場合はスルー
	if (scene->mNumAnimations != 0) {
		isKeyframeAnim_ = true;
		aiAnimation* animationAssimp = scene->mAnimations[0];//最初のアニメーションのみ現在は対応
		animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);//時間の単位を秒に変換

		//assimpでは個々のNodeAnimationをchannelと呼び、channelを回してNodeAnimation情報を取る
		for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
			aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
			NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
			//Translate
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
				aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
				KeyframeVector3 keyframe;
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);//秒に変換
				keyframe.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };//右手➡左手
				nodeAnimation.translate.keyframes.push_back(keyframe);
			}
			//Rotate
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
				aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
				KeyframeQuaternion keyframe;
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);//秒に変換
				keyframe.value = { keyAssimp.mValue.x,-keyAssimp.mValue.y,-keyAssimp.mValue.z ,keyAssimp.mValue.w };//右手➡左手
				nodeAnimation.rotate.keyframes.push_back(keyframe);
			}
			//Scale
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
				aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
				KeyframeVector3 keyframe;
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);//秒に変換
				keyframe.value = { keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };//右手➡左手
				nodeAnimation.scale.keyframes.push_back(keyframe);
			}
		}
	}

	return animation;
}

Node Model::ReadNode(aiNode* node) {
	Node result;
	//NodeからTransform情報を得る
	aiVector3D scale, translate;
	aiQuaternion rotate;
	//assimpの行列からSRTを抽出する関数
	node->mTransformation.Decompose(scale, rotate, translate);
	//Scaleはそのまま
	result.transform.scale = { scale.x,scale.y,scale.z };
	//x軸反転、更に回転方向の為に軸反転
	result.transform.rotate = { rotate.x,-rotate.y,-rotate.z,rotate.w };
	//x軸を反転
	result.transform.translate = { -translate.x,translate.y,translate.z };
	result.localMatrix = MakeQuatAffineMatrix(result.transform.scale, MakeRotateMatrix(result.transform.rotate), result.transform.translate);

	//列ベクトル形式を行ベクトル形式に転置
	Matrix4x4 localM = Transpose(result.localMatrix);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.localMatrix.m[i][j] = localM.m[i][j];
		}
	}
	//Node名を格納
	result.name = node->mName.C_Str();
	//子供の数だけ確保
	result.children.resize(node->mNumChildren);
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		//再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}

Skeleton Model::CreateSkeleton(const Node& rootNode) {
	Skeleton skeleton;
	skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

	//名前とIndexのマッピングを行い、アクセスしやすくする
	for (const Joint& joint : skeleton.joints) {
		skeleton.jointMap.emplace(joint.name, joint.index);
	}

	return skeleton;
}

int32_t Model::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {
	Joint joint;
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = MakeIdentity4x4();
	joint.transform = node.transform;
	joint.index = int32_t(joints.size());
	joint.parent = parent;
	joints.push_back(joint);//SkeletonのJoint列に追加
	for (const Node& child : node.children) {
		//子Jointを作成し、そのIndexを登録
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}

	//自身のIndexを返す
	return joint.index;
}

void Model::Update(Skeleton& skeleton, SkinCluster& skinCluster) {
	//全てのジョイントを更新
	for (Joint& joint : skeleton.joints) {
		joint.localMatrix = MakeQuatAffineMatrix(joint.transform.scale, MakeRotateMatrix(joint.transform.rotate), joint.transform.translate);
		if (joint.parent) {//親が居れば親の行列を掛ける
			joint.skeletonSpaceMatrix = Multiply(joint.localMatrix, skeleton.joints[*joint.parent].skeletonSpaceMatrix);
		}
		else {//親が居ないのでLocalMatrixとSkeletonSpaceMatrixは一致する
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}

	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {
		assert(jointIndex < skinCluster.inverseBindPoseMatrices.size());
		skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix = Multiply(skinCluster.inverseBindPoseMatrices[jointIndex], skeleton.joints[jointIndex].skeletonSpaceMatrix);
		skinCluster.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix = Transpose((Inverse(skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix)));
	}
}

void Model::ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime) {
	for (Joint& joint : skeleton.joints) {
		//対象のJointのAnimationがあれば値の適用を行う
		//このif文は初期化付きのif文
		if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform.translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
			joint.transform.rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
			joint.transform.scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);
		}
	}
}

void Model::SetAnimationTime(float animationTime) {
	animationTime_ = animationTime / 60.0f;
	isManualAnimTime_ = true;
}

SkinCluster Model::CreateSkinCluster() {
	SkinCluster skinCluster;
	//Palettr用のリソース確保
	skinCluster.paletteResource = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(WellForGPU) * skeleton_.joints.size());
	WellForGPU* mappedPalette = nullptr;
	skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));

	skinCluster.mappedPalette = { mappedPalette,skeleton_.joints.size() };//spanを使ってアクセス可にする
	skinCluster.paletteSrvHandle = srvManager_->GetDescriptorHandle();
	skinCluster.paletteSrvHandle.CPU.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	skinCluster.paletteSrvHandle.GPU.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//Paletter用のsrv作成、structuredBufferでアクセス可にする
	D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc{};
	paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	paletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	paletteSrvDesc.Buffer.FirstElement = 0;
	paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	paletteSrvDesc.Buffer.NumElements = UINT(skeleton_.joints.size());
	paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);
	dxCommon_->GetDevice()->CreateShaderResourceView(skinCluster.paletteResource.Get(), &paletteSrvDesc, skinCluster.paletteSrvHandle.CPU);

	//influence用のリソースを確保、頂点ごとにinfluence情報を追加できるようにする
	skinCluster.influenceResource = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexInfluence) * modelData_.vertices.size());
	VertexInfluence* mappedInfluence = nullptr;
	skinCluster.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData_.vertices.size());//weightを0にしておく
	skinCluster.mappedInfluence = { mappedInfluence,modelData_.vertices.size() };

	//influence用のVBVを作成
	skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
	skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData_.vertices.size());
	skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

	//InverseBindPoseMatrixを書く方擦る場所を用意して単位行列で埋める
	skinCluster.inverseBindPoseMatrices.resize(skeleton_.joints.size());
	std::generate(skinCluster.inverseBindPoseMatrices.begin(), skinCluster.inverseBindPoseMatrices.end(), MakeIdentity4x4);

	//modelDataを解析してinfluenceを埋める
	for (const auto& jointWeight : modelData_.skinClusterData) {
		//JointWeight、firstはjoint名なので、skeletonに対象が含まれているか確認
		auto it = skeleton_.jointMap.find(jointWeight.first);
		if (it == skeleton_.jointMap.end()) {//最後までない場合次に回す
			continue;
		}
		//(*it).secondにjointのindexが入っているので、外套のindexのinverseBindPoseMatrixを代入
		skinCluster.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		//空いているところに入れる
		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
			//該当のvertexIndexのinfluence情報を参照
			auto& currentInfluence = skinCluster.mappedInfluence[vertexWeight.vertexIndex];
			for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {//空いているところに入れる
				if (currentInfluence.weight[index] == 0.0f) {//weight=0が空いている所
					//weightとjointIndexを代入
					currentInfluence.weight[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}
			}
		}
	}

	return skinCluster;
}

void Model::CreateVartexData() {
	vertexResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * modelData_.vertices.size());

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();

	vertexBufferView_.SizeInBytes = sizeof(VertexData) * (UINT)modelData_.vertices.size();

	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

	//IndexBuffer用のResourceとIndexBufferViewを作成
	indexResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(uint32_t) * modelData_.indices.size());
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * modelData_.indices.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	//ResourceにIndexの内容をコピー
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex_));
	std::memcpy(mappedIndex_, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());
}

void Model::SetColor() {
	materialResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material));

	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&material_));
	material_->uvTransform = MakeIdentity4x4();
}

void Model::CreateLight() {
	//DirectionalLight
	directionalLightResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(DirectionalLight));
	directionalLightResource_->Map(0, NULL, reinterpret_cast<void**>(&directionalLight_));

	//PointLight
	pointLightResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(PointLight));
	pointLightResource_->Map(0, NULL, reinterpret_cast<void**>(&pointLight_));

	//ライティング用のカメラリソース
	cameraResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(CameraForGPU));
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
	cameraData_->worldPosition = { 0,0,0 };
}

void Model::SetDirectionalLightFlag(bool isDirectionalLight, int lightNum) {
	isDirectionalLight_ = isDirectionalLight;
	lightNum_ = lightNum;
}

void Model::LoadVATData(const std::string& directoryPath, const VATData* vatdata) {
	isVAT_ = true;

	std::string vatPos = directoryPath + "/VAT_pos.png";
	std::string vatRot = directoryPath + "/VAT_rot.png";

	vatPosTex_ = textureManager_->Load(vatPos);
	vatRotTex_ = textureManager_->Load(vatRot);

	vatResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(VATData));
	vatResource_->Map(0, NULL, reinterpret_cast<void**>(&vatData_));

	vatData_->VATTime = vatdata->VATTime;
	vatData_->MaxVATTime = vatdata->MaxVATTime;
	vatData_->VatPositionTexSize = vatdata->VatPositionTexSize;
	vatData_->VatNormalTexSize = vatdata->VatNormalTexSize;
}