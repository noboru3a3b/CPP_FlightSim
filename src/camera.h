// camera.h
// Camera control and positioning
#ifndef CAMERA_H
#define CAMERA_H

#include "vector3.h"
#include "plane.h"
#include "common.h"

class Camera {
private:
    Vec3f position;      // カメラ位置
    Vec3f lookAt;        // 注視点
    Vec3f up;            // 上方向ベクトル
    
    float lagDistance;   // 飛行機からの遅延距離
    float lagTime;       // 遅延時間（秒）
    
public:
    // コンストラクタ
    Camera();
    Camera(const Vec3f& initialPos);
    
    // カメラ位置を更新（飛行機を追従）
    void update(const Plane& plane, float deltaTime);
    
    // ビュー行列を適用
    void applyView() const;
    
    // ゲッター
    Vec3f getPosition() const { return position; }
    Vec3f getLookAt() const { return lookAt; }
    Vec3f getUp() const { return up; }
    
    // セッター
    void setPosition(const Vec3f& pos) { position = pos; }
    void setLookAt(const Vec3f& target) { lookAt = target; }
    void setUp(const Vec3f& upVec) { up = upVec; }
    void setLagDistance(float distance) { lagDistance = distance; }
    void setLagTime(float time) { lagTime = time; }
};

#endif // CAMERA_H
