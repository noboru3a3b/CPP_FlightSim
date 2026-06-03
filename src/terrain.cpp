// terrain.cpp
// Terrain generation implementation
#include "terrain.h"
#include <cmath>
#include <map>

// 静的メンバの初期化
std::vector<Color3f> TerrainGenerator::colorPalette = {
    Color3f::PINK,
    Color3f::GREEN,
    Color3f::TURQUOISE,
    Color3f::ORANGE,
    Color3f::LAVENDER,
    Color3f::YELLOW
};

std::unique_ptr<Model> TerrainGenerator::createSeussMesa(const Vec3f& center) {
    auto model = std::make_unique<Model>();
    
    // メサのパラメータ
    const float height = 50.0f;
    const int numNeckRings = 8;
    const float neckHeight = 5.0f;
    const float neckBottomRadius = 17.0f;
    const float neckTopRadius = 5.0f;
    const int numShoulderRings = 10;
    const float shoulderBottomRadius = 65.0f;
    const int numVerts = 36;  // 各リングの頂点数
    
    // リングごとの頂点インデックスを保存
    std::map<int, std::vector<int>> ringVertexIndices;
    
    // 肩部分のリングを生成（下から上へ）
    for (int shoulderIndex = 0; shoulderIndex <= numShoulderRings; ++shoulderIndex) {
        float frac = static_cast<float>(shoulderIndex) / numShoulderRings;
        float thisRadius = (1.0f - frac) * (shoulderBottomRadius - neckBottomRadius) 
                          + neckBottomRadius;
        float thisAlt = frac * neckHeight;
        
        std::vector<Vec3f> ringVerts = createRing(center, thisRadius, thisAlt, numVerts);
        
        std::vector<int> indices;
        for (const auto& v : ringVerts) {
            indices.push_back(model->addVertex(v));
        }
        ringVertexIndices[shoulderIndex] = indices;
        
        // リングのエッジを追加
        for (int i = 0; i < numVerts; ++i) {
            int next = (i + 1) % numVerts;
            model->addLine(indices[i], indices[next]);
        }
    }
    
    // 首部分のリングを生成
    for (int neckIndex = 1; neckIndex <= numNeckRings; ++neckIndex) {
        float frac = static_cast<float>(neckIndex) / numNeckRings;
        float thisRadius = (1.0f - frac) * (neckBottomRadius - neckTopRadius) 
                          + neckTopRadius;
        float thisAlt = frac * (height - neckHeight) + neckHeight;
        
        std::vector<Vec3f> ringVerts = createRing(center, thisRadius, thisAlt, numVerts);
        
        std::vector<int> indices;
        for (const auto& v : ringVerts) {
            indices.push_back(model->addVertex(v));
        }
        
        int ringNum = neckIndex + numShoulderRings;
        ringVertexIndices[ringNum] = indices;
        
        // リングのエッジを追加
        for (int i = 0; i < numVerts; ++i) {
            int next = (i + 1) % numVerts;
            model->addLine(indices[i], indices[next]);
        }
    }
    
    // リング間に三角形を生成（上昇方向）
    int totalRings = numShoulderRings + numNeckRings + 1;
    for (int ringNum = 0; ringNum < totalRings - 1; ++ringNum) {
        const auto& currentRing = ringVertexIndices[ringNum];
        const auto& nextRing = ringVertexIndices[ringNum + 1];
        
        Color3f color = colorPalette[ringNum % colorPalette.size()];
        
        for (size_t vertIndex = 0; vertIndex < currentRing.size(); ++vertIndex) {
            size_t nextVertIndex = (vertIndex + 1) % currentRing.size();
            
            // 現在のリングの頂点
            Vec3f thisVert = model->verts[currentRing[vertIndex]];
            
            // 次のリングで最も近い頂点を見つける
            int bestIndex = 0;
            float bestDistSqr = 999999.0f;
            
            for (size_t nextRingVertIndex = 0; nextRingVertIndex < nextRing.size(); 
                 ++nextRingVertIndex) {
                Vec3f nextRingVert = model->verts[nextRing[nextRingVertIndex]];
                float distSqr = thisVert.sub(nextRingVert).magSqr();
                
                if (distSqr < bestDistSqr) {
                    bestDistSqr = distSqr;
                    bestIndex = static_cast<int>(nextRingVertIndex);
                }
            }
            
            // 三角形を追加（上向き）
            model->addTriangle(
                currentRing[vertIndex],
                currentRing[nextVertIndex],
                nextRing[bestIndex],
                color
            );
        }
        
        // 三角形を追加（下向き）
        for (size_t vertIndex = 0; vertIndex < currentRing.size(); ++vertIndex) {
            size_t prevVertIndex = (vertIndex + currentRing.size() - 1) % currentRing.size();
            
            Vec3f thisVert = model->verts[currentRing[vertIndex]];
            
            // 次のリングで最も近い頂点を見つける
            int bestIndex = 0;
            float bestDistSqr = 999999.0f;
            
            for (size_t nextRingVertIndex = 0; nextRingVertIndex < nextRing.size(); 
                 ++nextRingVertIndex) {
                Vec3f nextRingVert = model->verts[nextRing[nextRingVertIndex]];
                float distSqr = thisVert.sub(nextRingVert).magSqr();
                
                if (distSqr < bestDistSqr) {
                    bestDistSqr = distSqr;
                    bestIndex = static_cast<int>(nextRingVertIndex);
                }
            }
            
            // 三角形を追加（異なる色）
            Color3f prevColor = colorPalette[(ringNum > 0 ? ringNum - 1 : 0) 
                                            % colorPalette.size()];
            model->addTriangle(
                currentRing[vertIndex],
                currentRing[prevVertIndex],
                nextRing[bestIndex],
                prevColor
            );
        }
    }
    
    return model;
}

std::vector<std::unique_ptr<Model>> TerrainGenerator::createMultipleMesas(
    const std::vector<Vec3f>& centers
) {
    std::vector<std::unique_ptr<Model>> mesas;
    
    for (const auto& center : centers) {
        mesas.push_back(createSeussMesa(center));
    }
    
    return mesas;
}

std::vector<Vec3f> TerrainGenerator::createRing(
    const Vec3f& center,
    float radius,
    float altitude,
    int numVerts
) {
    std::vector<Vec3f> verts;
    
    for (int i = 0; i < numVerts; ++i) {
        float frac = static_cast<float>(i) / numVerts;
        float radians = frac * 2.0f * MathUtil::PI;
        
        float x = std::cos(radians) * radius;
        float z = std::sin(radians) * radius;  // ← Y→Z に変更
        
        Vec3f v(x, altitude, z);  // ← (x, y, z) → (x, altitude, z)
        verts.push_back(v + center);
    }
    
    return verts;
}

void TerrainGenerator::connectRings(
    Model& model,
    const std::vector<int>& ring1Indices,
    const std::vector<int>& ring2Indices,
    const Color3f& color
) {
    size_t numVerts = ring1Indices.size();
    
    for (size_t i = 0; i < numVerts; ++i) {
        size_t next = (i + 1) % numVerts;
        
        // 2つの三角形で四角形を形成
        model.addTriangle(
            ring1Indices[i],
            ring1Indices[next],
            ring2Indices[i],
            color
        );
        
        model.addTriangle(
            ring1Indices[next],
            ring2Indices[next],
            ring2Indices[i],
            color
        );
    }
}
