// renderer.cpp
// Renderer implementation
#include "renderer.h"
#include <GL/glew.h>
#include <GL/glu.h>
#include <cmath>

Renderer::Renderer() : Renderer(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT) {}

Renderer::Renderer(int width, int height)
    : screenWidth(width)
    , screenHeight(height)
{
}

bool Renderer::initialize() {
    // GLEWの初期化
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW initialization failed: " 
                  << glewGetErrorString(err) << std::endl;
        return false;
    }
    
    // OpenGLの設定
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);  // 空色に変更
    glClearDepth(1.0);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);  // LEQUAL → LESS に変更
    
    // カリング有効化（裏面を描画しない）
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);  // 反時計回りが表
    
    // アルファブレンディング有効化
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // その他の設定
    glShadeModel(GL_SMOOTH);  // FLAT → SMOOTH に変更
    glLineWidth(1.5f);
    glPointSize(2.0f);
    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    
    return true;
}

void Renderer::resize(int width, int height) {
    screenWidth = width;
    screenHeight = height;
    
    if (height == 0) height = 1;
    
    glViewport(0, 0, width, height);
}

void Renderer::beginScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endScene() {
    // 特に何もしない（SwapBuffersは呼び出し側で行う）
}

void Renderer::setup3DProjection() const {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    float aspect = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
    gluPerspective(Config::FOV, aspect, Config::NEAR_PLANE, Config::FAR_PLANE);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Renderer::setup2DProjection() const {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, screenWidth, 0, screenHeight);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Renderer::renderScene(
    const Plane& plane,
    const std::vector<Target>& targets,
    const std::vector<std::unique_ptr<Model>>& terrain,
    const std::vector<std::pair<float, Vec3f>>& trailPoints,
    float currentTime
) {
    beginScene();
    
    // ★ setup3DProjection()は呼ばない（main.cppで既に設定済み）
    // ★ カメラのビュー行列も既に設定済みと仮定
    
    // 地形を描画
    for (const auto& mesa : terrain) {
        if (mesa) {
            drawModel(mesa.get());
        }
    }
    
    // ターゲットを描画
    for (const auto& target : targets) {
        target.draw(currentTime);
    }
    
    // 飛行機を描画
    plane.draw();
    
    // 軌跡を描画
    drawTrail(trailPoints);
    
    // 2D UI描画
    setup2DProjection();
    glDisable(GL_DEPTH_TEST);
    drawUI(plane, targets);
    glEnable(GL_DEPTH_TEST);
    
    endScene();
}


void Renderer::drawModel(const Model* model) const {
    if (!model) return;
    
    // 三角形を描画
    glBegin(GL_TRIANGLES);
    for (const auto& tri : model->tris) {
        glColor3f(tri.color.r, tri.color.g, tri.color.b);
        
        for (int idx : tri.indices) {
            const Vec3f& v = model->verts[idx];
            glVertex3f(v.x, v.y, v.z);
        }
    }
    glEnd();
    
    // エッジを描画
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    for (const auto& line : model->lines) {
        const Vec3f& v1 = model->verts[line.first];
        const Vec3f& v2 = model->verts[line.second];
        
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
    }
    glEnd();
}

void Renderer::drawTrail(const std::vector<std::pair<float, Vec3f>>& trailPoints) const {
    if (trailPoints.empty()) return;
    
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
    for (const auto& point : trailPoints) {
        const Vec3f& pos = point.second;
        glVertex3f(pos.x, pos.y, pos.z);
    }
    glEnd();
}

void Renderer::drawRadar(const Plane& plane, const std::vector<Target>& targets) const {
    const float radarSize = Config::RADAR_SIZE;
    const float radarX = Config::RADAR_X;
    const float radarY = Config::RADAR_Y;
    const int numSteps = Config::RADAR_SEGMENTS;
    
    // レーダー円を描画
    glColor3f(0.5f, 1.0f, 0.5f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < numSteps; ++i) {
        float f = static_cast<float>(i) / numSteps;
        float radians = f * 2.0f * MathUtil::PI;
        
        float x = std::cos(radians);
        float y = std::sin(radians);
        
        glVertex2f(x * radarSize + radarX, y * radarSize + radarY);
    }
    glEnd();
    
    // ターゲットの相対位置を描画
    for (const auto& target : targets) {
        float dx = target.pos.x - plane.pos.x;
        float dy = target.pos.y - plane.pos.y;
        float dz = target.pos.z - plane.pos.z;
        
        // 高度で色分け
        if (dz > target.size) {
            glColor3f(1.0f, 0.0f, 0.0f);  // 上方：赤
        } else if (dz < -target.size) {
            glColor3f(0.0f, 0.0f, 1.0f);  // 下方：青
        } else {
            glColor3f(0.0f, 1.0f, 0.0f);  // 同じ高度：緑
        }
        
        // 方位角を計算
        float heading = std::atan2(dy, dx);
        float planeHeadingRadians = MathUtil::degToRad(plane.heading);
        float relHeading = heading - planeHeadingRadians + MathUtil::degToRad(90.0f);
        
        float x = std::cos(relHeading);
        float y = std::sin(relHeading);
        
        // レーダー上に線を描画
        glBegin(GL_LINES);
        glVertex2f(radarX, radarY);
        glVertex2f(x * radarSize + radarX, y * radarSize + radarY);
        glEnd();
    }
}

void Renderer::drawUI(const Plane& plane, const std::vector<Target>& targets) const {
    // レーダーを描画
    drawRadar(plane, targets);
    
    // 速度表示などの追加UI要素をここに追加可能
    // 例：速度、高度、方位角など
}
