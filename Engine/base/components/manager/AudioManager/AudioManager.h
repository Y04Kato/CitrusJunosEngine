#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "Audio.h"

/// <summary>
/// SE / BGM を一括管理するマネージャ
/// </summary>
class AudioManager {
public:
    static AudioManager* GetInstance();

    // 初期化・終了処理
    void Initialize();
    void Finalize();

    // ===== SE =====
    void LoadSE(const std::string& key, const char* filename);
    void PlaySE(const std::string& key, float volume = 1.0f, bool loop = false);
    void StopSE(const std::string& key);
    void UnloadSE(const std::string& key);

    // ===== BGM =====
    void LoadBGM(const std::string& key, const char* filename);
    void PlayBGM(const std::string& key, float volume = 1.0f, bool loop = true);
    void StopBGM();
    void UnloadBGM(const std::string& key);

    // ===== Utility =====
    void UnloadAll();
    std::vector<std::string> GetLoadedSEKeys() const;
    std::vector<std::string> GetLoadedBGMKeys() const;

    // ===== AudioList =====
    void DebugImGui();

private:
    AudioManager() = default;
    ~AudioManager() = default;
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    std::unordered_map<std::string, SoundData> seMap_;
    std::unordered_map<std::string, SoundData> bgmMap_;

    // 再生中の BGM
    std::string currentBGMKey_;

    float seVolume_ = 1.0f;
    float bgmVolume_ = 1.0f;
};
