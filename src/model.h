// model.h
// 3D Model data structure
#ifndef MODEL_H
#define MODEL_H

#include "vector3.h"
#include "common.h"
#include <vector>

// 三角形構造体
struct Triangle {
    std::vector<int> indices;  // 頂点インデックス（3つ）
    Color3f color;             // 三角形の色
    
    Triangle() : indices(3) {}
    Triangle(int i0, int i1, int i2, const Color3f& col) 
        : indices{i0, i1, i2}, color(col) {}
};

// 3Dモデルクラス
class Model {
public:
    std::vector<Vec3f> verts;      // 頂点配列
    std::vector<std::pair<int, int>> lines;  // ライン（エッジ）インデックスペア
    std::vector<Triangle> tris;    // 三角形配列
    
    Model() {}
    
    // 頂点を追加
    int addVertex(const Vec3f& v) {
        verts.push_back(v);
        return static_cast<int>(verts.size() - 1);
    }
    
    // ラインを追加
    void addLine(int i0, int i1) {
        lines.push_back(std::make_pair(i0, i1));
    }
    
    // 三角形を追加
    void addTriangle(int i0, int i1, int i2, const Color3f& color) {
        tris.push_back(Triangle(i0, i1, i2, color));
    }
    
    // モデルの中心座標を取得
    Vec3f getCenter() const {
        if (verts.empty()) return Vec3f(0, 0, 0);
        
        Vec3f sum(0, 0, 0);
        for (const auto& v : verts) {
            sum = sum + v;
        }
        return sum.mul(1.0f / verts.size());
    }
    
    // 境界ボックスの最小・最大を取得
    void getBounds(Vec3f& min, Vec3f& max) const {
        if (verts.empty()) {
            min = max = Vec3f(0, 0, 0);
            return;
        }
        
        min = max = verts[0];
        for (const auto& v : verts) {
            if (v.x < min.x) min.x = v.x;
            if (v.y < min.y) min.y = v.y;
            if (v.z < min.z) min.z = v.z;
            if (v.x > max.x) max.x = v.x;
            if (v.y > max.y) max.y = v.y;
            if (v.z > max.z) max.z = v.z;
        }
    }
    
    // モデルを平行移動
    void translate(const Vec3f& offset) {
        for (auto& v : verts) {
            v = v + offset;
        }
    }
    
    // モデルをスケーリング
    void scale(float s) {
        for (auto& v : verts) {
            v = v.mul(s);
        }
    }
    
    // デバッグ情報を出力
    void printInfo() const {
        std::cout << "Model Info:" << std::endl;
        std::cout << "  Vertices: " << verts.size() << std::endl;
        std::cout << "  Lines: " << lines.size() << std::endl;
        std::cout << "  Triangles: " << tris.size() << std::endl;
    }
};

#endif // MODEL_H
