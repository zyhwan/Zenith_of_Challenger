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

    // Y축 기준 180도 회전 행렬 추가
    XMMATRIX rootRotation = XMMatrixRotationY(XM_PI); // 또는 XMConvertToRadians(180.0f)
    XMMATRIX identityMatrix = rootRotation;

    ProcessNode(scene->mRootNode, scene, identityMatrix);

    return true;
}


void FBXLoader::ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransform)
{
    // 노드의 로컬 행렬을 XMMATRIX로 변환
    aiMatrix4x4 mat = node->mTransformation;
    aiMatrix4x4 transpose = mat; // Assimp는 row-major
    transpose.Transpose();       // DirectX는 column-major이므로 전치 필요

    XMMATRIX nodeLocalTransform = XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&transpose));

    // 글로벌 변환 = 부모 * 로컬
    XMMATRIX globalTransform = XMMatrixMultiply(nodeLocalTransform, parentTransform);

    // 메시가 존재하면 처리
    for (UINT i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        auto object = ProcessMesh(mesh, scene, globalTransform);
        m_gameObjects.push_back(object);
    }

    // 자식 노드 순회
    for (UINT i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, globalTransform);
    }
}

std::shared_ptr<GameObject> FBXLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& globalTransform)
{
    std::vector<TextureVertex> vertices;
    float scaleFactor = 1.0f; // 지금은 크기 조정 없이

    for (UINT i = 0; i < mesh->mNumVertices; i++)
    {
        TextureVertex vertex;

        // 로컬 위치 로드
        XMFLOAT3 localPos = {
            mesh->mVertices[i].x * scaleFactor,
            mesh->mVertices[i].y * scaleFactor,
            mesh->mVertices[i].z * scaleFactor
        };

        // 글로벌 변환 적용
        XMVECTOR pos = XMLoadFloat3(&localPos);
        pos = XMVector3Transform(pos, globalTransform);
        XMStoreFloat3(&vertex.position, pos);

        // 노멀
        if (mesh->HasNormals())
        {
            XMFLOAT3 normal = {
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            };
            XMVECTOR n = XMLoadFloat3(&normal);
            n = XMVector3TransformNormal(n, globalTransform);
            n = XMVector3Normalize(n);
            XMStoreFloat3(&vertex.normal, n);
        }

        // UV
        vertex.uv = mesh->HasTextureCoords(0) ?
            XMFLOAT2(mesh->mTextureCoords[0][i].x, 1.0f - mesh->mTextureCoords[0][i].y) :
            XMFLOAT2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    auto device = gGameFramework->GetDevice();
    auto commandList = gGameFramework->GetCommandList();
    auto fbxMesh = std::make_shared<Mesh<TextureVertex>>(device, commandList, vertices);
    m_meshes.push_back(fbxMesh);

    auto gameObject = std::make_shared<GameObject>(device);
    gameObject->SetMesh(fbxMesh);
    gameObject->SetWorldMatrix(XMMatrixIdentity()); // 글로벌 위치가 버텍스에 적용됐기 때문에 단위행렬

    return gameObject;
}
