#include "AudioManager.h"
#include "ImGuiManager.h"
#include <cassert>

AudioManager* AudioManager::GetInstance() {
    static AudioManager instance;
    return &instance;
}

void AudioManager::Initialize() {
    Audio::GetInstance()->Initialize();
}

void AudioManager::Finalize() {
    UnloadAll();
    Audio::GetInstance()->Finalize();
}

// ===== SE =====
void AudioManager::LoadSE(const std::string& key, const char* filename) {
    if (seMap_.find(key) != seMap_.end()) return;
    seMap_[key] = Audio::GetInstance()->SoundLoad(filename);
}

void AudioManager::PlaySE(const std::string& key, float volume, bool loop) {
    auto it = seMap_.find(key);
    if (it != seMap_.end()) {
        Audio::GetInstance()->SoundPlayWave(it->second, volume, loop);
    }
    else {
        assert(false && "SE not loaded!");
    }
}

void AudioManager::StopSE(const std::string& key) {
    auto it = seMap_.find(key);
    if (it != seMap_.end()) {
        Audio::GetInstance()->SoundStopWave(&it->second);
    }
}

void AudioManager::UnloadSE(const std::string& key) {
    auto it = seMap_.find(key);
    if (it != seMap_.end()) {
        Audio::GetInstance()->SoundUnload(&it->second);
        seMap_.erase(it);
    }
}

// ===== BGM =====
void AudioManager::LoadBGM(const std::string& key, const char* filename) {
    if (bgmMap_.find(key) != bgmMap_.end()) return;
    bgmMap_[key] = Audio::GetInstance()->SoundLoad(filename);
}

void AudioManager::PlayBGM(const std::string& key, float volume, bool loop) {
    // 既存 BGM を止める
    if (!currentBGMKey_.empty()) {
        StopBGM();
    }

    auto it = bgmMap_.find(key);
    if (it != bgmMap_.end()) {
        Audio::GetInstance()->SoundPlayWave(it->second, volume, loop);
        currentBGMKey_ = key;
    }
    else {
        assert(false && "BGM not loaded!");
    }
}

void AudioManager::StopBGM() {
    if (!currentBGMKey_.empty()) {
        auto it = bgmMap_.find(currentBGMKey_);
        if (it != bgmMap_.end()) {
            Audio::GetInstance()->SoundStopWave(&it->second);
        }
        currentBGMKey_.clear();
    }
}

void AudioManager::UnloadBGM(const std::string& key) {
    if (currentBGMKey_ == key) {
        StopBGM();
    }
    auto it = bgmMap_.find(key);
    if (it != bgmMap_.end()) {
        Audio::GetInstance()->SoundUnload(&it->second);
        bgmMap_.erase(it);
    }
}

// ===== Utility =====
void AudioManager::UnloadAll() {
    for (auto& kv : seMap_) {
        Audio::GetInstance()->SoundUnload(&kv.second);
    }
    seMap_.clear();

    for (auto& kv : bgmMap_) {
        Audio::GetInstance()->SoundUnload(&kv.second);
    }
    bgmMap_.clear();

    currentBGMKey_.clear();
}

std::vector<std::string> AudioManager::GetLoadedSEKeys() const {
    std::vector<std::string> keys;
    for (auto& kv : seMap_) {
        keys.push_back(kv.first);
    }
    return keys;
}

std::vector<std::string> AudioManager::GetLoadedBGMKeys() const {
    std::vector<std::string> keys;
    for (auto& kv : bgmMap_) {
        keys.push_back(kv.first);
    }
    return keys;
}

void AudioManager::DebugImGui() {
    if (ImGui::Begin("AudioManager")) {
        // === SE ===
        ImGui::Text("=== Loaded SE ===");
        if (ImGui::SliderFloat("SE Volume", &seVolume_, 0.0f, 2.0f, "%.2f")) {
            // スライダーを動かしたら全SEの音量を更新
            for (auto& kv : seMap_) {
                Audio::GetInstance()->SoundSetVolume(&kv.second, seVolume_);
            }
        }

        for (auto& kv : seMap_) {
            if (ImGui::Button(kv.first.c_str())) {
                PlaySE(kv.first, seVolume_);
            }
        }

        ImGui::Separator();

        // === BGM ===
        ImGui::Text("=== Loaded BGM ===");
        if (ImGui::SliderFloat("BGM Volume", &bgmVolume_, 0.0f, 2.0f, "%.2f")) {
            // 再生中の BGM があれば音量を更新
            if (!currentBGMKey_.empty()) {
                auto it = bgmMap_.find(currentBGMKey_);
                if (it != bgmMap_.end()) {
                    Audio::GetInstance()->SoundSetVolume(&it->second, bgmVolume_);
                }
            }
        }

        for (auto& kv : bgmMap_) {
            if (kv.first == currentBGMKey_) {
                if (ImGui::Button((kv.first + " (Stop)").c_str())) {
                    StopBGM();
                }
            }
            else {
                if (ImGui::Button((kv.first + " (Play)").c_str())) {
                    PlayBGM(kv.first, bgmVolume_, true);
                }
            }
        }
    }
    ImGui::End();
}

