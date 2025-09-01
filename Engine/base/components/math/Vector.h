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
};

// 型エイリアス
using Vector2 = Vector<float, 2>;
using Vector3 = Vector<float, 3>;
using Vector4 = Vector<float, 4>;