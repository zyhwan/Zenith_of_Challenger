#include "FBXLoader.h"

bool FBXLoader::LoadFBXModel(const std::string& filename)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename,
        aiProcess_Triangulate | aiProcess_ConvertToLeftHanded |
        aiProcess_GenNormals | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return false;
    }

    std::cout << "FBX 로드 성공 : " << filename << std::endl;
    std::cout << "FBX 메시 개수 : " << scene->mNumMeshes << std::endl;

    // 단위 행렬로 초기화
    XMMATRIX identityMatrix = XMMatrixIdentity();
    ProcessNode(scene->mRootNode, scene, identityMatrix);

    return true;
}

void FBXLoader::ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransform)
{
    // Assimp 행렬을 DirectX 형식으로 변환
    XMMATRIX nodeTransform = XMMatrixIdentity();
    if (node->mTransformation.a1 != 0)  // 변환 행렬이 존재하는 경우
    {
        nodeTransform = XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&node->mTransformation));
    }

    // 부모 변환과 결합하여 **글로벌 행렬** 계산
    XMMATRIX globalTransform = parentTransform * nodeTransform;

    // 현재 노드의 모든 메시 처리
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        auto gameObject = ProcessMesh(mesh, scene, globalTransform);
        m_gameObjects.push_back(gameObject);
    }

    // 자식 노드 처리 (재귀)
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, globalTransform);
    }
}


std::shared_ptr<GameObject> FBXLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& globalTransform)
{
    std::vector<TextureVertex> vertices;
    float scaleFactor = 0.1f;  // 크기 조정

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        TextureVertex vertex;

        // **FBX에서 좌표 변환**
        XMFLOAT3 originalPosition = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };

        // **좌표계를 DirectX 12에 맞게 변환 (-X 플립 및 Scale 적용)**
        XMVECTOR pos = XMVectorSet(
            -originalPosition.x * scaleFactor,
            originalPosition.y * scaleFactor,
            originalPosition.z * scaleFactor,
            1.0f
        );

        // **글로벌 Transform 적용**
        pos = XMVector3Transform(pos, globalTransform);
        XMStoreFloat3(&vertex.position, pos);

        // **노멀 변환 (정규화 필요)**
        XMFLOAT3 originalNormal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
        XMVECTOR normal = XMVector3TransformNormal(XMLoadFloat3(&originalNormal), globalTransform);
        normal = XMVector3Normalize(normal);
        XMStoreFloat3(&vertex.normal, normal);

        // **UV 좌표 변환**
        vertex.uv = mesh->HasTextureCoords(0) ?
            XMFLOAT2(mesh->mTextureCoords[0][i].x, 1.0f - mesh->mTextureCoords[0][i].y) :
            XMFLOAT2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    auto device = gGameFramework->GetDevice();
    auto commandList = gGameFramework->GetCommandList();
    std::shared_ptr<Mesh<TextureVertex>> fbxMesh = std::make_shared<Mesh<TextureVertex>>(device, commandList, vertices);
    m_meshes.push_back(fbxMesh);

    // **GameObject 생성 및 변환 행렬 적용**
    auto gameObject = std::make_shared<GameObject>(device);
    gameObject->SetMesh(fbxMesh);
    gameObject->SetWorldMatrix(globalTransform);  // 올바른 위치 적용

    return gameObject;
}
