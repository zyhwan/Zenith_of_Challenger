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

    std::cout << "FBX �ε� ���� : " << filename << std::endl;
    std::cout << "FBX �޽� ���� : " << scene->mNumMeshes << std::endl;

    // ���� ��ķ� �ʱ�ȭ
    XMMATRIX identityMatrix = XMMatrixIdentity();
    ProcessNode(scene->mRootNode, scene, identityMatrix);

    return true;
}

void FBXLoader::ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransform)
{
    // Assimp ����� DirectX �������� ��ȯ
    XMMATRIX nodeTransform = XMMatrixIdentity();
    if (node->mTransformation.a1 != 0)  // ��ȯ ����� �����ϴ� ���
    {
        nodeTransform = XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&node->mTransformation));
    }

    // �θ� ��ȯ�� �����Ͽ� **�۷ι� ���** ���
    XMMATRIX globalTransform = parentTransform * nodeTransform;

    // ���� ����� ��� �޽� ó��
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        auto gameObject = ProcessMesh(mesh, scene, globalTransform);
        m_gameObjects.push_back(gameObject);
    }

    // �ڽ� ��� ó�� (���)
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, globalTransform);
    }
}


std::shared_ptr<GameObject> FBXLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& globalTransform)
{
    std::vector<TextureVertex> vertices;
    float scaleFactor = 0.1f;  // ũ�� ����

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        TextureVertex vertex;

        // **FBX���� ��ǥ ��ȯ**
        XMFLOAT3 originalPosition = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };

        // **��ǥ�踦 DirectX 12�� �°� ��ȯ (-X �ø� �� Scale ����)**
        XMVECTOR pos = XMVectorSet(
            -originalPosition.x * scaleFactor,
            originalPosition.y * scaleFactor,
            originalPosition.z * scaleFactor,
            1.0f
        );

        // **�۷ι� Transform ����**
        pos = XMVector3Transform(pos, globalTransform);
        XMStoreFloat3(&vertex.position, pos);

        // **��� ��ȯ (����ȭ �ʿ�)**
        XMFLOAT3 originalNormal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
        XMVECTOR normal = XMVector3TransformNormal(XMLoadFloat3(&originalNormal), globalTransform);
        normal = XMVector3Normalize(normal);
        XMStoreFloat3(&vertex.normal, normal);

        // **UV ��ǥ ��ȯ**
        vertex.uv = mesh->HasTextureCoords(0) ?
            XMFLOAT2(mesh->mTextureCoords[0][i].x, 1.0f - mesh->mTextureCoords[0][i].y) :
            XMFLOAT2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    auto device = gGameFramework->GetDevice();
    auto commandList = gGameFramework->GetCommandList();
    std::shared_ptr<Mesh<TextureVertex>> fbxMesh = std::make_shared<Mesh<TextureVertex>>(device, commandList, vertices);
    m_meshes.push_back(fbxMesh);

    // **GameObject ���� �� ��ȯ ��� ����**
    auto gameObject = std::make_shared<GameObject>(device);
    gameObject->SetMesh(fbxMesh);
    gameObject->SetWorldMatrix(globalTransform);  // �ùٸ� ��ġ ����

    return gameObject;
}
