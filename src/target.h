// target.h
// Target objects (collectibles)
#ifndef TARGET_H
#define TARGET_H

#include "vector3.h"
#include "common.h"

class Target {
private:
    float blinkRate;     // 点滅速度
    Color3f color1;      // 点滅色1（暗い色）
    Color3f color2;      // 点滅色2（明るい色）
    
public:
    Vec3f pos;           // 位置
    float size;          // サイズ（半径）
    
    // コンストラクタ
    Target();
    Target(const Vec3f& position, float targetSize = Config::TARGET_SIZE);
    
    // 描画（現在時刻で点滅効果を計算）
    void draw(float currentTime) const;
    
    // 指定位置がターゲット内部かチェック
    bool isInside(const Vec3f& testPos) const;
    
    // ゲッター・セッター
    Vec3f getPosition() const { return pos; }
    void setPosition(const Vec3f& newPos) { pos = newPos; }
    float getSize() const { return size; }
    void setSize(float newSize) { size = newSize; }
    void setBlinkRate(float rate) { blinkRate = rate; }
    void setColors(const Color3f& c1, const Color3f& c2) {
        color1 = c1;
        color2 = c2;
    }
};

#endif // TARGET_H
