// plane.h
// Aircraft physics and rendering
#ifndef PLANE_H
#define PLANE_H

#include "vector3.h"
#include "model.h"
#include "common.h"
#include <memory>

class Plane {
private:
    std::unique_ptr<Model> visualModel;  // 視覚モデル
    
    // 物理パラメータ
    float bankRate;          // ロール速度
    float riseRate;          // 上昇速度
    float accelerationRate;  // 加速度
    float minSpeed;          // 最低速度
    float maxSpeed;          // 最高速度
    
    // プレーンモデルを生成
    void createPlaneModel();
    
public:
    // 位置と姿勢
    Vec3f pos;          // 位置
    float heading;      // 方位角（度）
    float roll;         // ロール角（度）
    float pitch;        // ピッチ角（度）
    float velocity;     // 速度
    
    // コンストラクタ
    Plane();
    Plane(const Vec3f& startPos);
    
    // 操縦入力（0.0～1.0の範囲を-1.0～1.0に変換）
    void steer(float normalizedX, float normalizedY);
    
    // 物理演算更新
    void updatePhysics(float deltaTime);
    
    // 描画
    void draw() const;
    
    // ゲッター
    const Model* getModel() const { return visualModel.get(); }
    Vec3f getPosition() const { return pos; }
    float getHeading() const { return heading; }
    float getVelocity() const { return velocity; }
    float getRoll() const { return roll; }
    float getPitch() const { return pitch; }
    
    // 垂直尾翼の上端のワールド座標を取得
    Vec3f getTailPosition() const;
    
    // セッター
    void setPosition(const Vec3f& newPos) { pos = newPos; }
    void setVelocity(float v) { 
        velocity = MathUtil::clamp(v, minSpeed, maxSpeed); 
    }  // ★ここに閉じ括弧を追加
};

#endif // PLANE_H
