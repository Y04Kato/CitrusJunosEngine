/**
 * @file Vector.h
 * @brief 各種Vector構造体をまとめた部分
 * @author KATO
 * @date 未記録
 */

#pragma once
#include <array>
#include <cstddef>
#include <cassert>
#include <initializer_list>

struct VectorInt2 {
    int x;
    int y;

    // 等価比較演算子
    bool operator==(const VectorInt2& other) const noexcept {
        return x == other.x && y == other.y;
    }

    bool operator!=(const VectorInt2& other) const noexcept {
        return !(*this == other);
    }
};

struct VectorInt3 {
    int x;
    int y;
    int z;

    // 等価比較演算子
    bool operator==(const VectorInt3& other) const noexcept {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const VectorInt3& other) const noexcept {
        return !(*this == other);
    }
};

struct VectorInt4 {
    int x;
    int y;
    int z;
    int w;

    // 等価比較演算子
    bool operator==(const VectorInt4& other) const noexcept {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }

    bool operator!=(const VectorInt4& other) const noexcept {
        return !(*this == other);
    }
};

template<typename T, size_t N>
struct Vector {
    std::array<T, N> num{}; // 生配列としてのアクセス用

    // C++17以降ならif constexprでx/y/z/wを制限
    T& x() { static_assert(N >= 1, "Vector has no x"); return num[0]; }
    const T& x() const { static_assert(N >= 1, "Vector has no x"); return num[0]; }

    T& y() { static_assert(N >= 2, "Vector has no y"); return num[1]; }
    const T& y() const { static_assert(N >= 2, "Vector has no y"); return num[1]; }

    T& z() { static_assert(N >= 3, "Vector has no z"); return num[2]; }
    const T& z() const { static_assert(N >= 3, "Vector has no z"); return num[2]; }

    T& w() { static_assert(N >= 4, "Vector has no w"); return num[3]; }
    const T& w() const { static_assert(N >= 4, "Vector has no w"); return num[3]; }

    // インデックスアクセス
    T& operator[](size_t index) { return num[index]; }
    const T& operator[](size_t index) const { return num[index]; }

    // ImGui 等に渡す生ポインタ
    T* ptr() { return num.data(); }
    const T* ptr() const { return num.data(); }

    // コンストラクタ
    Vector() { num.fill(T{}); }
    explicit Vector(T value) { num.fill(value); }

    template<typename... Args>
    Vector(Args... args) : num{ static_cast<T>(args)... } {}

    Vector(const VectorInt2& v) {
        num.fill(T{});
        num[0] = static_cast<T>(v.x);
        num[1] = static_cast<T>(v.y);
    }

    Vector(const VectorInt3& v) {
        num.fill(T{});
        num[0] = static_cast<T>(v.x);
        num[1] = static_cast<T>(v.y);
        num[2] = static_cast<T>(v.z);
    }

    Vector(const VectorInt4& v) {
        num.fill(T{});
        num[0] = static_cast<T>(v.x);
        num[1] = static_cast<T>(v.y);
        num[2] = static_cast<T>(v.z);
        num[3] = static_cast<T>(v.w);
    }
};

// 型エイリアス
using Vector2 = Vector<float, 2>;
using Vector3 = Vector<float, 3>;
using Vector4 = Vector<float, 4>;