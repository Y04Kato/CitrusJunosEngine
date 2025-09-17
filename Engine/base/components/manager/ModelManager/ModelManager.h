#pragma once
#include "Model.h"
#include <unordered_map>
#include <memory>
#include <string>

struct ModelInstance {
    std::shared_ptr<Model> model;         // 共有されるモデルデータ
    WorldTransform worldTransform;        // 個別のワールド変換
    Vector4 materialColor = { 1.0f,1.0f,1.0f,1.0f };    // 個別の色
    bool isSkinning = false;              // スキニングモデルかどうか

    ModelInstance() {
        worldTransform.Initialize();
    }
};

class ModelManager {
public:
    static ModelManager* GetInstance();

    /// <summary>
    /// モデルのロード（キャッシュ付き）
    /// </summary>
    std::shared_ptr<Model> LoadModel(const std::string& directoryPath, const std::string& filename, bool isSkinning = false);

    /// <summary>
    /// キャッシュを全削除
    /// </summary>
    void Clear();

    /// <summary>
    /// ImGuiでキャッシュされているモデル一覧を表示
    /// </summary>
    void DebugImGui();

private:
    ModelManager() = default;
    std::unordered_map<std::string, std::shared_ptr<Model>> modelCache_;
};

class ModelRenderer {
public:
    void AddInstance(const std::shared_ptr<ModelInstance>& instance);
    void DrawNoAnimationAll(const ViewProjection& viewProjection);
    void DrawAnimationAll(const ViewProjection& viewProjection);
    void RemoveInstance(const std::shared_ptr<ModelInstance>& instance);
    void Clear();

private:
    std::list<std::shared_ptr<ModelInstance>> renderQueue_;
};