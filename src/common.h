// common.h
// Flight Simulator - Common definitions and utilities
#ifndef COMMON_H
#define COMMON_H

#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include <iostream>

// 定数定義
namespace Config {
    // ウィンドウ設定
    constexpr int SCREEN_WIDTH = 800;
    constexpr int SCREEN_HEIGHT = 600;
    constexpr const char* WINDOW_TITLE = "Fanciful Flight Simulator";
    
    // 物理パラメータ
    constexpr float MIN_SPEED = 5.0f;
    constexpr float MAX_SPEED = 50.0f;
    constexpr float BANK_RATE = 0.5f;
    constexpr float RISE_RATE = 0.8f;
    constexpr float ACCELERATION_RATE = 1.0f;
    
    // カメラ設定
    constexpr float FOV = 60.0f;
    constexpr float NEAR_PLANE = 1.0f;
    constexpr float FAR_PLANE = 1000.0f;
    constexpr float CAMERA_LAG_DISTANCE = 50.0f;
    constexpr float CAMERA_LAG_TIME = 0.1f;
    
    // レーダー設定
    constexpr float RADAR_SIZE = 80.0f;
    constexpr float RADAR_X = 700.0f;
    constexpr float RADAR_Y = 500.0f;
    constexpr int RADAR_SEGMENTS = 32;
    
    // ターゲット設定
    constexpr float TARGET_SIZE = 10.0f;
    constexpr float TARGET_BLINK_RATE = 1.5f;
    
    // 軌跡設定
    constexpr int MAX_TRAIL_POINTS = 50;
    constexpr float TRAIL_INTERVAL = 0.1f; // 秒
}

// 色定義
struct Color3f {
    float r, g, b;
    
    Color3f() : r(0.0f), g(0.0f), b(0.0f) {}
    Color3f(float r_, float g_, float b_) : r(r_), g(g_), b(b_) {}
    
    // パステルカラーパレット（元のPythonコードから）
    static const Color3f PINK;
    static const Color3f GREEN;
    static const Color3f TURQUOISE;
    static const Color3f ORANGE;
    static const Color3f LAVENDER;
    static const Color3f YELLOW;
    static const Color3f WHITE;
    static const Color3f BLACK;
    static const Color3f LIGHT_GREEN;
    static const Color3f DARK_GREEN;
    static const Color3f RED;
    static const Color3f BLUE;
};

// 色定義の実装
inline const Color3f Color3f::PINK(0.972f, 0.631f, 0.808f);
inline const Color3f Color3f::GREEN(0.718f, 1.0f, 0.584f);
inline const Color3f Color3f::TURQUOISE(0.286f, 0.988f, 0.902f);
inline const Color3f Color3f::ORANGE(1.0f, 0.749f, 0.333f);
inline const Color3f Color3f::LAVENDER(0.741f, 0.737f, 0.965f);
inline const Color3f Color3f::YELLOW(0.988f, 1.0f, 0.451f);
inline const Color3f Color3f::WHITE(1.0f, 1.0f, 1.0f);
inline const Color3f Color3f::BLACK(0.0f, 0.0f, 0.0f);
inline const Color3f Color3f::LIGHT_GREEN(0.5f, 1.0f, 0.5f);
inline const Color3f Color3f::DARK_GREEN(0.0f, 0.25f, 0.0f);
inline const Color3f Color3f::RED(1.0f, 0.0f, 0.0f);
inline const Color3f Color3f::BLUE(0.0f, 0.0f, 1.0f);

// 数学ユーティリティ
namespace MathUtil {
    constexpr float PI = 3.14159265358979323846f;
    
    inline float degToRad(float degrees) {
        return degrees * PI / 180.0f;
    }
    
    inline float radToDeg(float radians) {
        return radians * 180.0f / PI;
    }
    
    inline float clamp(float value, float min, float max) {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }
    
    inline float lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }
}

#endif // COMMON_H
