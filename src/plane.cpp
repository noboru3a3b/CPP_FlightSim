// plane.cpp
// Aircraft implementation
#include "plane.h"
#include <GL/gl.h>
#include <cmath>

// デフォルトコンストラクタ
Plane::Plane()
    : visualModel(nullptr),
      bankRate(0.0f),
      riseRate(0.0f),
      accelerationRate(0.0f),
      minSpeed(20.0f),
      maxSpeed(100.0f),
      pos(0.0f, 100.0f, 0.0f),
      heading(0.0f),     // ★ 初期値0度（画面奥向き）
      roll(0.0f),        // ★ 初期値0度（水平）
      pitch(0.0f),       // ★ 初期値0度（水平）
      velocity(50.0f)
{
    createPlaneModel();
}

// 位置指定コンストラクタ
Plane::Plane(const Vec3f& startPos)
    : visualModel(nullptr),
      bankRate(0.0f),
      riseRate(0.0f),
      accelerationRate(0.0f),
      minSpeed(20.0f),
      maxSpeed(100.0f),
      pos(startPos),
      heading(0.0f),     // ★ 初期値0度（画面奥向き）
      roll(0.0f),        // ★ 初期値0度（水平）
      pitch(0.0f),       // ★ 初期値0度（水平）
      velocity(50.0f)
{
    createPlaneModel();
}

void Plane::createPlaneModel() {
    visualModel = std::make_unique<Model>();
    
    float scale = 5.0f;
    
    // 頂点定義
    int v0 = visualModel->addVertex(Vec3f(0.0f, 0.0f, 3.0f * scale));      // 機首
    int v1 = visualModel->addVertex(Vec3f(-2.0f * scale, 0.0f, -2.0f * scale));  // 左翼後端
    int v2 = visualModel->addVertex(Vec3f(2.0f * scale, 0.0f, -2.0f * scale));   // 右翼後端
    int v3 = visualModel->addVertex(Vec3f(0.0f, 0.5f * scale, -1.5f * scale));   // 垂直尾翼
    
    // 三角形1: 左主翼（上面）
    visualModel->addTriangle(v0, v1, v3, Color3f(1.0f, 0.0f, 0.0f));
    
    // 三角形2: 右主翼（上面）
    visualModel->addTriangle(v0, v3, v2, Color3f(1.0f, 0.0f, 0.0f));
    
    // 三角形3: 底面（左右をつなぐ）
    visualModel->addTriangle(v0, v2, v1, Color3f(1.0f, 0.3f, 0.3f));
    
    // 裏面も追加（両面表示）
    visualModel->addTriangle(v0, v3, v1, Color3f(0.8f, 0.0f, 0.0f));
    visualModel->addTriangle(v0, v2, v3, Color3f(0.8f, 0.0f, 0.0f));
    visualModel->addTriangle(v0, v1, v2, Color3f(0.9f, 0.2f, 0.2f));
    
    std::cout << "Plane model created with " << visualModel->verts.size() 
              << " vertices and " << visualModel->tris.size() << " triangles" << std::endl;
}

void Plane::steer(float normalizedX, float normalizedY) {
    // normalizedX, normalizedY は 0.0～1.0 の範囲
    // -1.0～1.0 に変換
    float steerX = (normalizedX - 0.5f) * 2.0f;
    float steerY = (normalizedY - 0.5f) * 2.0f;
    
    // ロール制御（左右）
    bankRate = steerX * 90.0f;
    
    // ピッチ制御（上下）
    riseRate = -steerY * 80.0f;
    
    // スロットル制御
    accelerationRate = -steerY * 20.0f;
}

Vec3f Plane::getTailPosition() const {
    if (!visualModel || visualModel->verts.empty()) {
        return pos;
    }

    // 垂直尾翼の頂点（v3）のローカル座標
    Vec3f tailLocal = visualModel->verts[3];  // v3 = 垂直尾翼の上端

    // 回転を適用（OpenGLと逆順：ロール → ピッチ → ヨー）
    float headingRad = MathUtil::degToRad(heading);
    float pitchRad = MathUtil::degToRad(pitch);
    float rollRad = MathUtil::degToRad(roll);

    // 1. ロール回転（Z軸周り）
    float x1 = tailLocal.x * std::cos(rollRad) - tailLocal.y * std::sin(rollRad);
    float y1 = tailLocal.x * std::sin(rollRad) + tailLocal.y * std::cos(rollRad);
    float z1 = tailLocal.z;

    // 2. ピッチ回転（X軸周り）
    float x2 = x1;
    float y2 = y1 * std::cos(pitchRad) - z1 * std::sin(pitchRad);
    float z2 = y1 * std::sin(pitchRad) + z1 * std::cos(pitchRad);

    // 3. ヨー回転（Y軸周り）: glRotatef(heading, 0,1,0) と同じ向き
    float x3 = x2 * std::cos(headingRad) + z2 * std::sin(headingRad);
    float y3 = y2;
    float z3 = -x2 * std::sin(headingRad) + z2 * std::cos(headingRad);

    // ワールド座標に変換
    return Vec3f(pos.x + x3, pos.y + y3, pos.z + z3);
}

void Plane::updatePhysics(float deltaTime) {
    // ロール更新
    roll += bankRate * deltaTime;
    roll *= 0.97f; // 減衰
    roll = MathUtil::clamp(roll, -60.0f, 60.0f);

    // ピッチ更新
    pitch += riseRate * deltaTime * 0.5f;
    pitch *= 0.99f; // 減衰
    pitch = MathUtil::clamp(pitch, -60.0f, 60.0f);

    // スロットルによる速度変化
    velocity += accelerationRate * deltaTime;

    // エネルギー保存に基づく速度補正（高度差→運動エネルギーへ）
    // 移動計算と同じ符号（-pitch）を用いて、今フレームでの高度変化を見積もる
    const float g = 9.8f;
    const float energyFactor = 1.5f; // 効果を強める係数（物理的には1.0が基準）
    float pitchRadMove = MathUtil::degToRad(-pitch);

    // 今フレームの見込み上昇/下降量（pos更新と同じ 0.5 スケールを反映）
    float vy_est = velocity * std::sin(pitchRadMove);
    float deltaH = vy_est * deltaTime * 0.5f;

    // v_new^2 = v^2 - 2 g Δh
    float v2 = velocity * velocity - 2.0f * g * deltaH * energyFactor;
    if (v2 < 0.0f) v2 = 0.0f; // 数値安定化
    velocity = std::sqrt(v2);

    // 速度クランプ
    velocity = MathUtil::clamp(velocity, minSpeed, maxSpeed);

    // 旋回：ロールに応じて方位角を変更
    float turnRate = -roll * 0.5f;
    heading += turnRate * deltaTime;

    // headingを-180～180度に正規化
    while (heading > 180.0f) heading -= 360.0f;
    while (heading < -180.0f) heading += 360.0f;

    // 機体の姿勢（heading, pitch）に応じた移動
    float headingRad = MathUtil::degToRad(heading);
    float pitchRad = MathUtil::degToRad(-pitch); // 移動用は既存実装に合わせて -pitch

    float horizontalVelocity = velocity * std::cos(pitchRad);

    // 速度成分
    float vx = horizontalVelocity * std::sin(headingRad);
    float vy = velocity * std::sin(pitchRad);
    float vz = horizontalVelocity * std::cos(headingRad);

    // 位置更新（既存実装の0.5スケールを維持）
    pos.x += vx * deltaTime * 0.5f;
    pos.y += vy * deltaTime * 0.5f;
    pos.z += vz * deltaTime * 0.5f;

    // 衝突判定等はなし（すり抜け）
}

void Plane::draw() const {
    if (!visualModel || visualModel->verts.empty()) {
        return;
    }
    
    glPushMatrix();
    
    // 位置移動
    glTranslatef(pos.x, pos.y, pos.z);
    
    // 回転を適用（ヨー → ピッチ → ロール）
    glRotatef(heading, 0.0f, 1.0f, 0.0f);  // ヨー（方位角）
    glRotatef(pitch, 1.0f, 0.0f, 0.0f);    // ピッチ（上下）
    glRotatef(roll, 0.0f, 0.0f, 1.0f);     // ロール（傾き）
    
    // 三角形を描画
    glBegin(GL_TRIANGLES);
    for (const auto& tri : visualModel->tris) {
        glColor3f(tri.color.r, tri.color.g, tri.color.b);
        
        const Vec3f& v0 = visualModel->verts[tri.indices[0]];
        const Vec3f& v1 = visualModel->verts[tri.indices[1]];
        const Vec3f& v2 = visualModel->verts[tri.indices[2]];
        
        glVertex3f(v0.x, v0.y, v0.z);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
    }
    glEnd();
    
    glPopMatrix();
}

