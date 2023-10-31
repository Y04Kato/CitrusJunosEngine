#include "Model.h"

void Model::Initialize(const std::string& directoryPath, const std::string& filename){
    dxCommon_ = DirectXCommon::GetInstance();
    CJEngine_ = CitrusJunosEngine::GetInstance();
    textureManager_ = TextureManager::GetInstance();
    directionalLights_ = DirectionalLights::GetInstance();

    modelData_ = LoadObjFile(directoryPath, filename);
    texture_ = textureManager_->Load(modelData_.material.textureFilePath);
  
    CreateVartexData();
    SetColor();
    CreateDictionalLight();
}

void Model::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, const Vector4& material){
    Transform uvTransform = { { 1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };

    Matrix4x4 uvtransformMtrix = MakeScaleMatrix(uvTransform.scale);
    uvtransformMtrix = Multiply(uvtransformMtrix, MakeRotateZMatrix(uvTransform.rotate.num[2]));
    uvtransformMtrix = Multiply(uvtransformMtrix, MakeTranslateMatrix(uvTransform.translate));

    *material_ = { material,isDirectionalLight_ };
    material_->uvTransform = uvtransformMtrix;
    *directionalLight_ = directionalLights_->GetDirectionalLight();

    dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
    //形状を設定。PS0にせっていしているものとはまた別
    dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldTransform.constBuff_->GetGPUVirtualAddress());
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());

    dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(texture_));
    dxCommon_->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);

}

void Model::Finalize(){

}

Model* Model::CreateModelFromObj(const std::string& directoryPath, const std::string& filename){
    Model* model = new Model();
    model->Initialize(directoryPath, filename);
    return model;
}

ModelData Model::LoadObjFile(const std::string& directoryPath, const std::string& filename){
    ModelData modelData;//構築するModelData
    std::vector<Vector4> positions;//位置
    std::vector<Vector3> normals;//法線
    std::vector<Vector2> texcoords;//テクスチャ座標
    std::string line;//ファイルから読んだ1行を格納するもの

    std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
    assert(file.is_open());

    while (std::getline(file, line)){
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;//先頭の識別子を読む

        //identifierに応じた処理
        if (identifier == "v") {
            Vector4 position;
            s >> position.num[0] >> position.num[1] >> position.num[2];
            position.num[2] *= -1.0f;
            position.num[3] = 1.0f;
            positions.push_back(position);
        }
        else if (identifier == "vt") {
            Vector2 texcoord;
            s >> texcoord.num[0] >> texcoord.num[1];
            texcoord.num[1] = 1.0f - texcoord.num[1];
            texcoords.push_back(texcoord);
        }
        else if (identifier == "vn") {
            Vector3 normal;
            s >> normal.num[0] >> normal.num[1] >> normal.num[2];
            normal.num[2] *= -1.0f;
            normals.push_back(normal);
        }
        else if (identifier == "f") {
            VertexData triangle[3];
            //面は三角形限定 その他は未対応
            for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
                std::string vertexDefinition;
                s >> vertexDefinition;
                //頂点の要素へのIndexは【位置/UV/法線】で格納されているので、分解してIndexを取得する
                std::istringstream v(vertexDefinition);
                uint32_t elementIndeices[3];
                for (int32_t element = 0; element < 3; ++element) {
                    std::string index;
                    std::getline(v, index, '/');// /区切りでIndexを積んでいく
                    elementIndeices[element] = std::stoi(index);
                }
                //要素へのIndexから、実際の要素の値を取得して、頂点を構築する
                Vector4 position = positions[elementIndeices[0] - 1];
                Vector2 texcoord = texcoords[elementIndeices[1] - 1];
                Vector3 normal = normals[elementIndeices[2] - 1];
                VertexData vertex = { position,texcoord,normal };
                modelData.vertices.push_back(vertex);
                triangle[faceVertex] = { position,texcoord,normal };

            }
            modelData.vertices.push_back(triangle[2]);
            modelData.vertices.push_back(triangle[1]);
            modelData.vertices.push_back(triangle[0]);
        }
        else if (identifier == "mtllib") {
            //materialTemplateLibraryファイルの名前を取得
            std::string materialFilname;
            s >> materialFilname;
            //基本的にobjファイルと同一階層にmtlは存在させるから、ディレクトリ名とファイル名を渡す
            modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilname);
        }

    }
    return modelData;
}

MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename){
    MaterialData materialData;//構築するマテリアルデータ
    std::string line;
    std::ifstream file(directoryPath + "/" + filename);
    assert(file.is_open());
    while (std::getline(file, line))
    {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;
        //identifierに応じた処理
        if (identifier == "map_Kd") {
            std::string textureFilname;
            s >> textureFilname;
            //連結してファイルパスにする
            materialData.textureFilePath = directoryPath + "/" + textureFilname;
        }
    }
    return materialData;
}

void Model::CreateVartexData(){
    vertexResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * modelData_.vertices.size());


    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();

    vertexBufferView_.SizeInBytes = sizeof(VertexData) * (UINT)modelData_.vertices.size();

    vertexBufferView_.StrideInBytes = sizeof(VertexData);

    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

    std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
}

void Model::SetColor(){
    materialResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material));

    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&material_));
    material_->uvTransform = MakeIdentity4x4();

}

void Model::CreateDictionalLight(){
    directionalLightResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(DirectionalLight));
    directionalLightResource_->Map(0, NULL, reinterpret_cast<void**>(&directionalLight_));
}

void Model::SetDirectionalLightFlag(bool isDirectionalLight) {
    isDirectionalLight_ = isDirectionalLight;
}