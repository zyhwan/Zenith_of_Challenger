#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stdafx.h"
#include "vertex.h"      // 버텍스 구조체 포함
#include "texture.h"     // 텍스처 관련 처리 포함
#include "Object.h"      // DirectX 12에서 사용할 오브젝트 클래스 포함
#include "Mesh.h"        // DirectX 12에서 사용할 Mesh 클래스 포함
#include "GameFramework.h" // DirectX 12에서 gGameFramework 참조

class FBXLoader
{
public:
    FBXLoader() = default;
    ~FBXLoader() = default;

    bool LoadFBXModel(const string& filename); // FBX 파일 로드 함수

    // FBX에서 로드한 메쉬를 반환하는 함수 추가
    vector<shared_ptr<Mesh<TextureVertex>>> GetMeshes() { return m_meshes; }

    // FBX에서 로드한 GameObject를 반환하는 함수 추가
    vector<shared_ptr<GameObject>> GetGameObjects() { return m_gameObjects; }

private:
    void ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransform); // FBX 노드 처리
    shared_ptr<GameObject> ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix); // FBX 메시 처리

    vector<shared_ptr<Mesh<TextureVertex>>> m_meshes; // FBX에서 로드된 메쉬 리스트 저장
    vector<shared_ptr<GameObject>> m_gameObjects; // FBX 모델용 GameObject 저장
};
