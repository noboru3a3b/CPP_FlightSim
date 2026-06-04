// main.cpp
// Flight Simulator - Main program
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <memory>
#include <chrono>

#include "common.h"
#include "vector3.h"
#include "plane.h"
#include "target.h"
#include "camera.h"
#include "renderer.h"
#include "terrain.h"

// グローバル変数
GLFWwindow* window = nullptr;
std::unique_ptr<Renderer> renderer;
std::unique_ptr<Plane> plane;
std::unique_ptr<Camera> camera;
std::vector<Target> targets;
std::vector<std::unique_ptr<Model>> mesas;
std::vector<std::pair<float, Vec3f>> trailPoints;  // (time, position)

// ゲーム状態
float gameTime = 0.0f;
bool gameOver = false;
int collectedTargets = 0;

// マウス位置（正規化座標）
double mouseX = 0.5;
double mouseY = 0.5;

// ターゲット位置リスト（高度をすべて50～150の範囲に修正）
std::vector<Vec3f> targetPositions = {
    Vec3f(30.0f, 70.0f, 20.0f),
    Vec3f(100.0f, 80.0f, 80.0f),     // ★ 10.0f → 80.0f
    Vec3f(-50.0f, 90.0f, 57.0f),     // ★ -88.0f → 90.0f
    Vec3f(87.0f, 100.0f, 19.0f),     // ★ -58.0f → 100.0f
    Vec3f(-67.0f, 80.0f, 34.0f),     // ★ 31.0f → 80.0f
    Vec3f(40.0f, 70.0f, 56.0f),      // ★ -50.0f → 70.0f
    Vec3f(28.0f, 90.0f, 50.0f),      // ★ -51.0f → 90.0f
    Vec3f(77.0f, 100.0f, 70.0f),     // ★ -56.0f → 100.0f
    Vec3f(41.0f, 80.0f, 10.0f),      // ★ 44.0f → 80.0f
    Vec3f(-58.0f, 90.0f, 26.0f)      // ★ -49.0f → 90.0f
};

// 前方宣言
void initGame();
void updateGame(float deltaTime);
void renderGame();
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void errorCallback(int error, const char* description);

int main() {
    // GLFWエラーコールバック設定
    glfwSetErrorCallback(errorCallback);
    
    // GLFW初期化
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // OpenGLバージョン設定（3.3 Core Profile）
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    
    // ウィンドウ作成
    window = glfwCreateWindow(
        Config::SCREEN_WIDTH,
        Config::SCREEN_HEIGHT,
        Config::WINDOW_TITLE,
        nullptr,
        nullptr
    );
    
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // OpenGLコンテキストを設定
    glfwMakeContextCurrent(window);
    
    // VSync有効化（画面ティアリング防止）
    glfwSwapInterval(1);
    
    // コールバック関数を設定
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    
    // レンダラー初期化
    renderer = std::make_unique<Renderer>(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT);
    if (!renderer->initialize()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // ゲーム初期化
    initGame();
    
    std::cout << "Flight Simulator started!" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  Mouse: Control aircraft" << std::endl;
    std::cout << "  ESC: Quit" << std::endl;
    
    // メインループ
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    while (!glfwWindowShouldClose(window) && !gameOver) {
        // 時間計測
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        float deltaTime = elapsed.count();
        lastTime = currentTime;
        
        // ゲーム時間更新
        gameTime += deltaTime;
        
        // 更新
        updateGame(deltaTime);
        
        // 描画
        renderGame();
        
        // バッファスワップとイベント処理
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // 終了処理
    int minutes = static_cast<int>(gameTime) / 60;
    int seconds = static_cast<int>(gameTime) % 60;
    
    std::cout << "\n=== Game Over ===" << std::endl;
    std::cout << "Elapsed Time: " << minutes << ":" 
              << (seconds < 10 ? "0" : "") << seconds << std::endl;
    std::cout << "Targets Collected: " << collectedTargets 
              << " / " << (collectedTargets + targetPositions.size()) << std::endl;
    
    glfwTerminate();
    return 0;
}

void initGame() {
    // 飛行機初期化（初期姿勢は0度で画面奥向き）
    plane = std::make_unique<Plane>(Vec3f(0.0f, 100.0f, 0.0f));
    
    // カメラ初期化（飛行機の真後ろに配置）
    camera = std::make_unique<Camera>(Vec3f(0.0f, 115.0f, -50.0f));
    
    // 初期ターゲット配置（高度を100付近に修正）
    targets.clear();
    targets.push_back(Target(Vec3f(50.0f, 100.0f, 100.0f)));
    targets.push_back(Target(Vec3f(-50.0f, 100.0f, 150.0f)));
    
    // 地形生成（メサ）- ★すべてY=0に統一
    std::vector<Vec3f> mesaCenters = {
        Vec3f(0.0f, 0.0f, 200.0f),      // 前方
        Vec3f(-300.0f, 0.0f, 0.0f),     // 左
        Vec3f(300.0f, 0.0f, 0.0f)       // 右
    };
    
    mesas = TerrainGenerator::createMultipleMesas(mesaCenters);
    
    std::cout << "Game initialized with " << mesas.size() << " mesas" << std::endl;
}

void updateGame(float deltaTime) {
    // 飛行機の操縦
    plane->steer(static_cast<float>(mouseX), static_cast<float>(mouseY));
    
    // 物理演算更新
    plane->updatePhysics(deltaTime);
    
    // カメラ更新
    camera->update(*plane, deltaTime);
    
    // 軌跡更新（しっぽの先から放出）
    static float lastTrailTime = 0.0f;
    if (gameTime - lastTrailTime > Config::TRAIL_INTERVAL) {
        // ★シンプル！
        Vec3f tailPos = plane->getTailPosition();
        
        trailPoints.push_back(std::make_pair(gameTime, tailPos));
        lastTrailTime = gameTime;
        
        // 古い軌跡を削除
        if (trailPoints.size() > Config::MAX_TRAIL_POINTS) {
            trailPoints.erase(trailPoints.begin());
        }
    }
	// ターゲットとの衝突判定
    for (auto it = targets.begin(); it != targets.end(); ) {
        if (it->isInside(plane->getPosition())) {
            collectedTargets++;
            std::cout << "Target collected! (" << collectedTargets << ")" << std::endl;
            
            // 次のターゲット位置を設定
            if (!targetPositions.empty()) {
                it->setPosition(targetPositions[0]);
                targetPositions.erase(targetPositions.begin());
                ++it;
            } else {
                // すべてのターゲットを収集完了
                it = targets.erase(it);
                if (targets.empty()) {
                    std::cout << "All targets collected!" << std::endl;
                    gameOver = true;
                }
            }
        } else {
            ++it;
        }
    }
}

void renderGame() {
    // 3D描画の準備
    renderer->setup3DProjection();  // プロジェクション行列を設定
    camera->applyView();             // ビュー行列を設定（gluLookAt）
    
    // シーン描画（内部でsetup3DProjectionを呼ばない）
    renderer->renderScene(*plane, targets, mesas, trailPoints, gameTime);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            case GLFW_KEY_R:
                // ゲームリセット
                std::cout << "Restarting game..." << std::endl;
                gameTime = 0.0f;
                collectedTargets = 0;
                trailPoints.clear();
                targetPositions = {
                    Vec3f(30.0f, 70.0f, 20.0f),
                    Vec3f(100.0f, 10.0f, 80.0f),
                    Vec3f(-1.7f, -88.0f, 57.0f),
                    Vec3f(87.0f, -58.0f, 19.0f),
                    Vec3f(-67.0f, 31.0f, 34.0f),
                    Vec3f(40.0f, -50.0f, 56.0f),
                    Vec3f(28.0f, -51.0f, 50.0f),
                    Vec3f(77.0f, -56.0f, 70.0f),
                    Vec3f(41.0f, 44.0f, 10.0f),
                    Vec3f(-58.0f, -49.0f, 26.0f)
                };
                initGame();
                break;
        }
    }
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    // マウス座標を正規化（0.0～1.0）
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    mouseX = xpos / width;
    mouseY = ypos / height;
    
    // 範囲チェック
    if (mouseX < 0.0) mouseX = 0.0;
    if (mouseX > 1.0) mouseX = 1.0;
    if (mouseY < 0.0) mouseY = 0.0;
    if (mouseY > 1.0) mouseY = 1.0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    renderer->resize(width, height);
}

void errorCallback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}
