// vector3.h
// 3D Vector mathematics
#ifndef VECTOR3_H
#define VECTOR3_H

#include <cmath>
#include <iostream>

class Vec3f {
public:
    float x, y, z;
    
    // コンストラクタ
    Vec3f() : x(0.0f), y(0.0f), z(0.0f) {}
    Vec3f(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    
    // ベクトル加算
    Vec3f add(const Vec3f& v) const {
        return Vec3f(x + v.x, y + v.y, z + v.z);
    }
    
    Vec3f operator+(const Vec3f& v) const {
        return add(v);
    }
    
    // ベクトル減算
    Vec3f sub(const Vec3f& v) const {
        return Vec3f(x - v.x, y - v.y, z - v.z);
    }
    
    Vec3f operator-(const Vec3f& v) const {
        return sub(v);
    }
    
    // スカラー乗算
    Vec3f mul(float s) const {
        return Vec3f(x * s, y * s, z * s);
    }
    
    Vec3f operator*(float s) const {
        return mul(s);
    }
    
    // 内積
    float dot(const Vec3f& v) const {
        return x * v.x + y * v.y + z * v.z;
    }
    
    // 外積
    Vec3f cross(const Vec3f& v) const {
        return Vec3f(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }
    
    // ベクトルの長さの二乗
    float magSqr() const {
        return x * x + y * y + z * z;
    }
    
    // ベクトルの長さ
    float mag() const {
        return std::sqrt(magSqr());
    }
    
    // 正規化（単位ベクトル化）
    Vec3f norm() const {
        float m = mag();
        if (m < 1e-6f) {
            return Vec3f(0.0f, 0.0f, 0.0f);
        }
        return Vec3f(x / m, y / m, z / m);
    }
    
    // 2点間の距離
    float distance(const Vec3f& v) const {
        return sub(v).mag();
    }
    
    // 同一点判定
    bool samePoint(const Vec3f& v, float epsilon) const {
        return distance(v) <= epsilon;
    }
    
    // 出力ストリーム
    friend std::ostream& operator<<(std::ostream& os, const Vec3f& v) {
        os << "[" << v.x << ", " << v.y << ", " << v.z << "]";
        return os;
    }
};

// スカラーとベクトルの乗算（左側）
inline Vec3f operator*(float s, const Vec3f& v) {
    return v.mul(s);
}

#endif // VECTOR3_H
