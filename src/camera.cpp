// camera.cpp
// Camera implementation
#include "camera.h"
#include <GL/glew.h>
#include <GL/glu.h>
#include <cmath>

Camera::Camera() : Camera(Vec3f(100.0f, 100.0f, 60.0f)) {}

Camera::Camera(const Vec3f& initialPos)
    : position(initialPos)
    , lookAt(0.0f, 0.0f, 0.0f)
    , up(0.0f, 1.0f, 0.0f)  // ← Y軸を上に変更
    , lagDistance(Config::CAMERA_LAG_DISTANCE)
    , lagTime(Config::CAMERA_LAG_TIME)
{
}

void Camera::update(const Plane& plane, float deltaTime) {
    Vec3f planePos = plane.getPosition();
    float heading = plane.getHeading();
    
    // 飛行機の後方方向を計算
    float headingRad = MathUtil::degToRad(heading);
    
    // 後方ベクトル（飛行機の進行方向の逆）
    Vec3f backward(
        -std::sin(headingRad),
        0.0f,
        -std::cos(headingRad)
    );
    
    // カメラの理想位置（飛行機の後方50、上方15）
    Vec3f idealPos = planePos + backward.mul(50.0f) + Vec3f(0.0f, 15.0f, 0.0f);
    
    // スムーズに追従
    Vec3f separation = idealPos - position;
    float sepMag = separation.mag();
    
    if (sepMag > 0.1f) {
        float closeSpeed = deltaTime * sepMag / lagTime;
        Vec3f movementVec = separation.norm().mul(closeSpeed);
        position = position + movementVec;
    }
    
    // 注視点は飛行機の位置
    lookAt = planePos;
}

void Camera::applyView() const {
    gluLookAt(
        position.x, position.y, position.z,
        lookAt.x, lookAt.y, lookAt.z,
        up.x, up.y, up.z
    );
}

