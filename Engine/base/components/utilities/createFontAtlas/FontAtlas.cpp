#include "FontAtlas.h"
#include <iostream>
#include <algorithm>
#include <cassert>

FontAtlasGenerator::FontAtlasGenerator() : ft_(nullptr), face_(nullptr), pixelSize_(0), atlasWidth_(0), atlasHeight_(0) {
    if (FT_Init_FreeType(&ft_)) {
        std::cerr << "FT_Init_FreeType failed\n";
        ft_ = nullptr;
    }
}

FontAtlasGenerator::~FontAtlasGenerator() {
    if (face_) FT_Done_Face(face_);
    if (ft_) FT_Done_FreeType(ft_);
}

bool FontAtlasGenerator::LoadFont(const std::string& fontPath, int pixelSize) {
    if (!ft_) return false;
    if (FT_New_Face(ft_, fontPath.c_str(), 0, &face_)) {
        std::cerr << "Failed to load font: " << fontPath << std::endl;
        return false;
    }
    FT_Set_Pixel_Sizes(face_, 0, pixelSize);
    pixelSize_ = pixelSize;
    return true;
}

bool FontAtlasGenerator::BuildAtlas(const std::u32string& charset, int atlasWidth, int atlasHeight) {
    if (!face_) return false;
    atlasWidth_ = atlasWidth;
    atlasHeight_ = atlasHeight;
    atlas_.assign(atlasWidth_ * atlasHeight_, 0);
    glyphs_.clear();

    int penX = 0;
    int penY = 0;
    int rowHeight = 0;

    for (char32_t ch : charset) {
        if (FT_Load_Char(face_, (FT_ULong)ch, FT_LOAD_RENDER)) {
            std::cerr << "Warning: could not load glyph " << (uint32_t)ch << std::endl;
            continue;
        }

        FT_GlyphSlot g = face_->glyph;
        int bw = g->bitmap.width;
        int bh = g->bitmap.rows;

        // If glyph is empty (space), still store advance and empty box
        if (bw == 0 || bh == 0) {
            int adv = (g->advance.x >> 6);
            GlyphInfo info{};
            info.width = 0; info.height = 0;
            info.bearingX = g->bitmap_left; info.bearingY = g->bitmap_top;
            info.advance = adv;
            info.u0 = info.v0 = info.u1 = info.v1 = 0.0f;
            info.bitmapX = info.bitmapY = 0;
            glyphs_[ch] = info;
            continue;
        }

        // move to next row if needed
        if (penX + bw >= atlasWidth_) {
            penX = 0;
            penY += rowHeight + 1;
            rowHeight = 0;
        }

        if (penY + bh >= atlasHeight_) {
            std::cerr << "Atlas too small: need larger atlas for charset\n";
            return false;
        }

        // blit glyph bitmap into atlas (g->bitmap.buffer contains grayscale row-major)
        for (int y = 0; y < bh; ++y) {
            const uint8_t* srcRow = &g->bitmap.buffer[y * g->bitmap.pitch];
            uint8_t* dstRow = &atlas_[(penY + y) * atlasWidth_ + penX];
            memcpy(dstRow, srcRow, bw);
        }

        // save glyph info
        GlyphInfo info{};
        info.bitmapX = penX;
        info.bitmapY = penY;
        info.width = bw;
        info.height = bh;
        info.bearingX = g->bitmap_left;
        info.bearingY = g->bitmap_top;
        info.advance = (g->advance.x >> 6);
        info.u0 = float(penX) / float(atlasWidth_);
        info.v0 = float(penY) / float(atlasHeight_);
        info.u1 = float(penX + bw) / float(atlasWidth_);
        info.v1 = float(penY + bh) / float(atlasHeight_);

        glyphs_[ch] = info;

        penX += bw + 1;
        rowHeight = std::max(rowHeight, bh);
    }

    return true;
}

std::u32string FontAtlasGenerator::BuildCharset(const std::u32string& extraKanji) {
    std::u32string charset;

    // --- 基本 ASCII (スペース, 英数字, 記号) ---
    for (char32_t c = 0x20; c <= 0x7E; c++) {
        charset.push_back(c);
    }

    // --- 全角数字 ---
    for (char32_t c = U'０'; c <= U'９'; c++) {
        charset.push_back(c);
    }

    // --- ひらがな (U+3041 ～ U+3096 あたりまで) ---
    for (char32_t c = 0x3041; c <= 0x3096; c++) {
        charset.push_back(c);
    }

    // --- カタカナ (U+30A1 ～ U+30FA) ---
    for (char32_t c = 0x30A1; c <= 0x30FA; c++) {
        charset.push_back(c);
    }

    // カタカナ濁点・半濁点など追加
    charset.push_back(0x30FC); // ー
    charset.push_back(0x309B); // ゛
    charset.push_back(0x309C); // ゜

    // --- 必要な漢字を手動で追加 ---
    for (auto c : extraKanji) {
        charset.push_back(c);
    }

    return charset;
}