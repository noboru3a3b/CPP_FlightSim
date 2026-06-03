// renderer.h
// OpenGL rendering management
#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#include "vector3.h"
#include "model.h"
#include "plane.h"
#include "target.h"
#include <vector>
#include <memory>

class Renderer {
private:
    int screenWidth;
    int screenHeight;
    
    // レーダー描画
    void drawRadar(const Plane& plane, const std::vector<Target>& targets) const;
    
    // UI描画
    void drawUI(const Plane& plane, const std::vector<Target>& targets) const;
    
    // モデル描画
    void drawModel(const Model* model) const;
    
    // 軌跡描画
    void drawTrail(const std::vector<std::pair<float, Vec3f>>& trailPoints) const;
    
public:
    // コンストラクタ
    Renderer();
    Renderer(int width, int height);
    
    // OpenGL初期化
    bool initialize();
    
    // ビューポート設定
    void resize(int width, int height);
    
    // 3Dシーン描画
    void beginScene();
    void endScene();
    
    // 3D描画モード設定
    void setup3DProjection() const;
    
    // 2D描画モード設定（UI用）
    void setup2DProjection() const;
    
    // シーン全体を描画
    void renderScene(
        const Plane& plane,
        const std::vector<Target>& targets,
        const std::vector<std::unique_ptr<Model>>& terrain,
        const std::vector<std::pair<float, Vec3f>>& trailPoints,
        float currentTime
    );
    
    // ゲッター
    int getWidth() const { return screenWidth; }
    int getHeight() const { return screenHeight; }
};

#endif // RENDERER_H
