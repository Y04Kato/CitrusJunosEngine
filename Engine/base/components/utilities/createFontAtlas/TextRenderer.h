#pragma once
#include "FontAtlas.h"
#include "CreateSprite.h"
#include "TextureManager.h"
#include <string>
#include <vector>
#include <memory>

struct PooledSprite {
    std::unique_ptr<CreateSprite> sprite;
    bool inUse = false;
};

class TextRenderer {
public:
    TextRenderer(TextureManager* texMgr, FontAtlasGenerator* atlas);
    ~TextRenderer();

    bool UploadAtlasToGPU();
    void DrawTextUTF8(const std::string& utf8Text, float x, float y, float scale, const Vector4& color);

private:
    TextureManager* texMgr_;
    FontAtlasGenerator* atlas_;
    uint32_t textureIndex_;

    std::vector<std::unique_ptr<PooledSprite>> spritePool_;
    CreateSprite* AcquireSprite();
    void ResetSpriteUsage();
    std::u32string Utf8ToUtf32(const std::string& s);
};