#include "ModelManager.h"

ModelManager* ModelManager::GetInstance() {
    static ModelManager instance;
    return &instance;
}

std::shared_ptr<Model> ModelManager::LoadModel(const std::string& directoryPath, const std::string& filename, bool isSkinning) {
    std::string key = directoryPath + "/" + filename;

    auto it = modelCache_.find(key);
    if (it != modelCache_.end()) {
        return it->second;
    }

    std::shared_ptr<Model> model;
    if (isSkinning) {
        model.reset(Model::CreateSkinningModel(directoryPath, filename));
    }
    else {
        model.reset(Model::CreateModel(directoryPath, filename));
    }

    modelCache_[key] = model;
    return model;
}

void ModelManager::Clear() {
    modelCache_.clear();
}

void ModelManager::DebugImGui() {
    if (ImGui::Begin("Model Manager")) {
        ImGui::Text("Loaded Models: %d", static_cast<int>(modelCache_.size()));
        ImGui::Separator();
        for (auto& [key, model] : modelCache_) {
            ImGui::Text("%s", key.c_str());
        }
    }
    ImGui::End();
}

void ModelRenderer::AddInstance(const std::shared_ptr<ModelInstance>& instance) {
    renderQueue_.push_back(instance);
}

void ModelRenderer::DrawNoAnimationAll(const ViewProjection& viewProjection) {
    for (auto& inst : renderQueue_) {
        if (!inst || !inst->model) continue;

        if (!inst->isSkinning) {
            inst->model->Draw(inst->worldTransform, viewProjection, inst->materialColor);
        }
    }
}

void ModelRenderer::DrawAnimationAll(const ViewProjection& viewProjection) {
    for (auto& inst : renderQueue_) {
        if (!inst || !inst->model) continue;

        if (inst->isSkinning) {
            inst->model->SkinningDraw(inst->worldTransform, viewProjection, inst->materialColor);
        }
    }
}

void ModelRenderer::RemoveInstance(const std::shared_ptr<ModelInstance>& instance) {
    renderQueue_.remove(instance);
}

void ModelRenderer::Clear() {
    renderQueue_.clear();
}