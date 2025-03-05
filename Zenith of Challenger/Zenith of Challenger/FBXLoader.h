#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stdafx.h"
#include "vertex.h"      // ���ؽ� ����ü ����
#include "texture.h"     // �ؽ�ó ���� ó�� ����
#include "Object.h"      // DirectX 12���� ����� ������Ʈ Ŭ���� ����
#include "Mesh.h"        // DirectX 12���� ����� Mesh Ŭ���� ����
#include "GameFramework.h" // DirectX 12���� gGameFramework ����

class FBXLoader
{
public:
    FBXLoader() = default;
    ~FBXLoader() = default;

    bool LoadFBXModel(const string& filename); // FBX ���� �ε� �Լ�

    // FBX���� �ε��� �޽��� ��ȯ�ϴ� �Լ� �߰�
    vector<shared_ptr<Mesh<TextureVertex>>> GetMeshes() { return m_meshes; }

    // FBX���� �ε��� GameObject�� ��ȯ�ϴ� �Լ� �߰�
    vector<shared_ptr<GameObject>> GetGameObjects() { return m_gameObjects; }

private:
    void ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransform); // FBX ��� ó��
    shared_ptr<GameObject> ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix); // FBX �޽� ó��

    vector<shared_ptr<Mesh<TextureVertex>>> m_meshes; // FBX���� �ε�� �޽� ����Ʈ ����
    vector<shared_ptr<GameObject>> m_gameObjects; // FBX �𵨿� GameObject ����
};
