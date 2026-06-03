// target.cpp
// Target implementation
#include "target.h"
#include <GL/gl.h>
#include <cmath>

// デフォルトコンストラクタ
Target::Target()
    : blinkRate(Config::TARGET_BLINK_RATE),
      color1(Color3f::ORANGE),
      color2(Color3f::YELLOW),
      pos(0.0f, 0.0f, 0.0f),
      size(Config::TARGET_SIZE)
{
}

// 位置指定コンストラクタ
Target::Target(const Vec3f& position, float targetSize)
    : blinkRate(Config::TARGET_BLINK_RATE),
      color1(Color3f::ORANGE),
      color2(Color3f::YELLOW),
      pos(position),
      size(targetSize)
{
}

void Target::draw(float currentTime) const {
    // 点滅効果の計算（sin波で0.0～1.0の範囲）
    float blinkFactor = (std::sin(currentTime * blinkRate * 2.0f * MathUtil::PI) + 1.0f) * 0.5f;
    
    // 色を補間
    Color3f currentColor(
        MathUtil::lerp(color1.r, color2.r, blinkFactor),
        MathUtil::lerp(color1.g, color2.g, blinkFactor),
        MathUtil::lerp(color1.b, color2.b, blinkFactor)
    );
    
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    
    // キューブを描画
    glColor3f(currentColor.r, currentColor.g, currentColor.b);
    
    float s = size;  // 半径をキューブの半辺長として使用
    
    // 前面
    glBegin(GL_QUADS);
    glVertex3f(-s, -s,  s);
    glVertex3f( s, -s,  s);
    glVertex3f( s,  s,  s);
    glVertex3f(-s,  s,  s);
    glEnd();
    
    // 背面
    glBegin(GL_QUADS);
    glVertex3f(-s, -s, -s);
    glVertex3f(-s,  s, -s);
    glVertex3f( s,  s, -s);
    glVertex3f( s, -s, -s);
    glEnd();
    
    // 上面
    glBegin(GL_QUADS);
    glVertex3f(-s,  s, -s);
    glVertex3f(-s,  s,  s);
    glVertex3f( s,  s,  s);
    glVertex3f( s,  s, -s);
    glEnd();
    
    // 下面
    glBegin(GL_QUADS);
    glVertex3f(-s, -s, -s);
    glVertex3f( s, -s, -s);
    glVertex3f( s, -s,  s);
    glVertex3f(-s, -s,  s);
    glEnd();
    
    // 右面
    glBegin(GL_QUADS);
    glVertex3f( s, -s, -s);
    glVertex3f( s,  s, -s);
    glVertex3f( s,  s,  s);
    glVertex3f( s, -s,  s);
    glEnd();
    
    // 左面
    glBegin(GL_QUADS);
    glVertex3f(-s, -s, -s);
    glVertex3f(-s, -s,  s);
    glVertex3f(-s,  s,  s);
    glVertex3f(-s,  s, -s);
    glEnd();
    
    // エッジを黒で描画（輪郭を強調）
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    
    // 前面の枠
    glBegin(GL_LINE_LOOP);
    glVertex3f(-s, -s,  s);
    glVertex3f( s, -s,  s);
    glVertex3f( s,  s,  s);
    glVertex3f(-s,  s,  s);
    glEnd();
    
    // 背面の枠
    glBegin(GL_LINE_LOOP);
    glVertex3f(-s, -s, -s);
    glVertex3f( s, -s, -s);
    glVertex3f( s,  s, -s);
    glVertex3f(-s,  s, -s);
    glEnd();
    
    // 縦のエッジ
    glBegin(GL_LINES);
    glVertex3f(-s, -s, -s);
    glVertex3f(-s, -s,  s);
    
    glVertex3f( s, -s, -s);
    glVertex3f( s, -s,  s);
    
    glVertex3f( s,  s, -s);
    glVertex3f( s,  s,  s);
    
    glVertex3f(-s,  s, -s);
    glVertex3f(-s,  s,  s);
    glEnd();
    
    glPopMatrix();
}

bool Target::isInside(const Vec3f& testPos) const {
    // キューブの境界チェック（各軸で範囲内かどうか）
    float dx = std::abs(testPos.x - pos.x);
    float dy = std::abs(testPos.y - pos.y);
    float dz = std::abs(testPos.z - pos.z);
    
    // すべての軸でsize以内ならキューブ内部
    return (dx <= size && dy <= size && dz <= size);
}

