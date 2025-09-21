#include "TextRenderer.h"
#include <cassert>

static const int kInitialPool = 256;

TextRenderer::TextRenderer(TextureManager* texMgr, FontAtlasGenerator* atlas)
    : texMgr_(texMgr), atlas_(atlas), textureIndex_(UINT32_MAX)
{
    spritePool_.reserve(kInitialPool);
}

TextRenderer::~TextRenderer() {}

bool TextRenderer::UploadAtlasToGPU() {
    const auto& bmp = atlas_->GetAtlasBitmap();
    int w = atlas_->GetAtlasWidth();
    int h = atlas_->GetAtlasHeight();
    if (bmp.empty() || w == 0 || h == 0) return false;

    DirectX::Image img{};
    img.format = DXGI_FORMAT_R8_UNORM;
    img.width = w;
    img.height = h;
    img.rowPitch = static_cast<size_t>(w);
    img.slicePitch = static_cast<size_t>(w) * h;
    img.pixels = const_cast<uint8_t*>(bmp.data());

    DirectX::ScratchImage scratch;
    if (FAILED(scratch.InitializeFromImage(img))) return false;

    textureIndex_ = texMgr_->LoadFromScratchImage(scratch);
    if (textureIndex_ == UINT32_MAX) return false;

    // プール作成
    for (int i = 0; i < kInitialPool; ++i) {
        auto sp = std::make_unique<PooledSprite>();
        sp->sprite = CreateSprite::CreateSpriteFromTexture({ 1.0f,1.0f }, textureIndex_);
        sp->inUse = false;
        spritePool_.push_back(std::move(sp));
    }
    return true;
}

CreateSprite* TextRenderer::AcquireSprite() {
    for (auto& sp : spritePool_) {
        if (!sp->inUse) {
            sp->inUse = true;
            return sp->sprite.get();
        }
    }
    // プール不足なら新規追加
    auto sp = std::make_unique<PooledSprite>();
    sp->sprite = CreateSprite::CreateSpriteFromTexture({ 1.0f,1.0f }, textureIndex_);
    sp->inUse = true;
    spritePool_.push_back(std::move(sp));
    return spritePool_.back()->sprite.get();
}

void TextRenderer::ResetSpriteUsage() {
    for (auto& sp : spritePool_) sp->inUse = false;
}

std::u32string TextRenderer::Utf8ToUtf32(const std::string& s) {
    std::u32string out;
    const unsigned char* ptr = (const unsigned char*)s.c_str();
    size_t len = s.size();
    size_t i = 0;
    while (i < len) {
        uint32_t codepoint = 0;
        unsigned char c = ptr[i];
        if (c < 0x80) {
            codepoint = c;
            i += 1;
        }
        else if ((c & 0xE0) == 0xC0 && i + 1 < len) {
            codepoint = ((ptr[i] & 0x1F) << 6) | (ptr[i + 1] & 0x3F);
            i += 2;
        }
        else if ((c & 0xF0) == 0xE0 && i + 2 < len) {
            codepoint = ((ptr[i] & 0x0F) << 12) | ((ptr[i + 1] & 0x3F) << 6) | (ptr[i + 2] & 0x3F);
            i += 3;
        }
        else if ((c & 0xF8) == 0xF0 && i + 3 < len) {
            codepoint = ((ptr[i] & 0x07) << 18) | ((ptr[i + 1] & 0x3F) << 12) | ((ptr[i + 2] & 0x3F) << 6) | (ptr[i + 3] & 0x3F);
            i += 4;
        }
        else {
            // invalid seq -> skip
            ++i;
            continue;
        }
        out.push_back((char32_t)codepoint);
    }
    return out;
}

void TextRenderer::DrawTextUTF8(const std::string& utf8Text, float x, float y, float scale, const Vector4& color) {
    if (textureIndex_ == UINT32_MAX) return;
    auto text32 = Utf8ToUtf32(utf8Text);

    float penX = x;
    float baselineY = y; // ここが文字列全体のベースライン

    for (char32_t ch : text32) {
        const auto& glyphs = atlas_->GetGlyphs();
        auto it = glyphs.find(ch);
        if (it == glyphs.end()) {
            penX += 8.0f * scale;
            continue;
        }

        const GlyphInfo& g = it->second;

        // スプライト取得
        CreateSprite* sprite = AcquireSprite();
        sprite->SetSize({ float(g.width) * scale, float(g.height) * scale });
        sprite->SetTextureLTSize({ float(g.bitmapX), float(g.bitmapY) }, { float(g.width), float(g.height) });
        sprite->SetAnchor({ 0.0f, 0.0f }); // 左上基準

        EulerTransform tx;
        tx.scale = { 1.0f, 1.0f };
        tx.rotate = { 0.0f, 0.0f, 0.0f };

        // ★ベースライン揃え計算★
        // penY は文字列のベースライン
        // glyph.bearingY は文字の上端からベースラインまでの距離
        float xpos = penX + float(g.bearingX) * scale;
        float ypos = baselineY - float(g.bearingY) * scale;
        tx.translate = { xpos, ypos, 0.0f };

        EulerTransform uvtx;
        uvtx.scale = { 1.0f, 1.0f };
        uvtx.rotate = { 0.0f, 0.0f, 0.0f };
        uvtx.translate = { 0.0f, 0.0f, 0.0f };

        sprite->Draw(tx, uvtx, color);

        // 次の文字位置
        penX += float(g.advance) * scale;
    }

    // 描画後にプールの使用フラグをリセット
    ResetSpriteUsage();
}