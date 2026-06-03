// terrain.h
// Terrain generation (Seussian mesas)
#ifndef TERRAIN_H
#define TERRAIN_H

#include "vector3.h"
#include "model.h"
#include "common.h"
#include <memory>
#include <vector>

// メサ（台地）生成クラス
class TerrainGenerator {
private:
    // パステルカラーのパレット
    static std::vector<Color3f> colorPalette;
    
public:
    // Seuss風のメサを生成
    static std::unique_ptr<Model> createSeussMesa(const Vec3f& center);
    
    // 複数のメサを生成
    static std::vector<std::unique_ptr<Model>> createMultipleMesas(
        const std::vector<Vec3f>& centers
    );
    
private:
    // リング状の頂点を生成
    static std::vector<Vec3f> createRing(
        const Vec3f& center,
        float radius,
        float altitude,
        int numVerts
    );
    
    // 2つのリング間に三角形を生成
    static void connectRings(
        Model& model,
        const std::vector<int>& ring1Indices,
        const std::vector<int>& ring2Indices,
        const Color3f& color
    );
};

#endif // TERRAIN_H
