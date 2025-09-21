#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H

struct GlyphInfo {
    float u0, v0;
    float u1, v1;
    int width, height;
    int bearingX, bearingY;
    int advance;
    int bitmapX, bitmapY; // atlas 内のピクセル左上位置（CreateSprite に渡すため）
};

class FontAtlasGenerator {
public:
    FontAtlasGenerator();
    ~FontAtlasGenerator();

    // フォントのロード (フォントファイルパス、ピクセル高さ)
    bool LoadFont(const std::string& fontPath, int pixelSize);

    // 与えた UTF-32 文字セットから atlas を作る
    // atlasWidth/atlasHeight は十分大きく（例: 2048）
    bool BuildAtlas(const std::u32string& charset, int atlasWidth, int atlasHeight);

    std::u32string BuildCharset(const std::u32string& extraKanji = U"");

    // 生成結果アクセス
    const std::vector<uint8_t>& GetAtlasBitmap() const { return atlas_; } // R8 (width*height)
    const std::unordered_map<char32_t, GlyphInfo>& GetGlyphs() const { return glyphs_; }
    int GetAtlasWidth() const { return atlasWidth_; }
    int GetAtlasHeight() const { return atlasHeight_; }

private:
    FT_Library ft_;
    FT_Face face_;
    int pixelSize_;
    int atlasWidth_, atlasHeight_;
    std::vector<uint8_t> atlas_; // row-major R8
    std::unordered_map<char32_t, GlyphInfo> glyphs_;
};