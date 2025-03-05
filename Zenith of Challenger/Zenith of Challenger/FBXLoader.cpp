#include "FBXLoader.h"

bool FBXLoader::LoadFBXModel(const std::string& filename)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return false;
    }

    std::cout << "FBX �ε� ���� : " << filename << std::endl;
    std::cout << "FBX �޽� ���� : " << scene->mNumMeshes << std::endl;

    // �⺻ ���� ��ķ� �ʱ�ȭ
    XMMATRIX identityMatrix = XMMatrixIdentity();
    ProcessNode(scene->mRootNode, scene, identityMatrix);

    return true;
}

void FBXLoader::ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransform)
{
    // ����� ��ȯ ��� ��������
    XMMATRIX transformMatrix = XMMatrixIdentity();

    if (node->mTransformation.a1 != 0)  // ��ȯ ����� �����ϴ� ���
    {
        transformMatrix = XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&node->mTransformation));
    }

    // �θ� ��ȯ�� ����
    XMMATRIX globalTransform = transformMatrix * parentTransform;

    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        auto gameObject = ProcessMesh(mesh, scene, globalTransform);
        m_gameObjects.push_back(gameObject);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, globalTransform);
    }
}


std::shared_ptr<GameObject> FBXLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix)
{
    std::vector<TextureVertex> vertices;

    // ũ�� ���� (Scale Factor ����)
    float scaleFactor = 0.1f; // ũ�⸦ 10�� ���̵��� ����

    // ���ؽ� ������ �ε�
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        TextureVertex vertex;

        // ��ġ ��ȯ ���� (DirectX ��ǥ�� ��ȯ + Scale ����)
        XMFLOAT3 originalPosition = { mesh->mVertices[i].x * scaleFactor,
                                      mesh->mVertices[i].y * scaleFactor,
                                      mesh->mVertices[i].z * -scaleFactor };
        XMVECTOR pos = XMLoadFloat3(&originalPosition);
        pos = XMVector3Transform(pos, transformMatrix);
        XMStoreFloat3(&vertex.position, pos);

        // ��� ��ȯ (ũ�� ������ ���� ���� ���� ��ȯ)
        XMFLOAT3 originalNormal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
        XMVECTOR normal = XMLoadFloat3(&originalNormal);
        normal = XMVector3TransformNormal(normal, transformMatrix);
        XMStoreFloat3(&vertex.normal, normal);

        // UV ��ǥ ��ȯ (DirectX ��ǥ�� ����)
        vertex.uv = mesh->HasTextureCoords(0) ?
            XMFLOAT2(mesh->mTextureCoords[0][i].x, 1.0f - mesh->mTextureCoords[0][i].y) :
            XMFLOAT2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    auto device = gGameFramework->GetDevice();
    auto commandList = gGameFramework->GetCommandList();
    std::shared_ptr<Mesh<TextureVertex>> fbxMesh = std::make_shared<Mesh<TextureVertex>>(device, commandList, vertices);
    m_meshes.push_back(fbxMesh);

    // �ؽ�ó �ε� (DDS ���� ���)
    std::shared_ptr<Texture> texture = nullptr;
    std::string loadedTexturePath = "None"; // �ε�� �ؽ�ó ��� ���� ����

    if (scene->mMaterials && mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiString texturePath;

        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
        {
            std::string originalPath = texturePath.C_Str();
            size_t lastSlash = originalPath.find_last_of("/\\");
            std::string textureFileName = (lastSlash != std::string::npos) ? originalPath.substr(lastSlash + 1) : originalPath;

            // DDS ���Ϸ� ���� (png �� dds)
            std::string ddsTextureFile = "Textures/" + textureFileName;
            ddsTextureFile.replace(ddsTextureFile.find(".png"), 4, ".dds"); // Ȯ���� ����

            std::wstring fullPathW(ddsTextureFile.begin(), ddsTextureFile.end());
            std::string fullPath(ddsTextureFile); // UTF-8 �������� ��ȯ�Ͽ� �ܼ� ���

            // ���� ���� ���� Ȯ��
            if (!std::filesystem::exists(fullPathW))
            {
                std::cerr << "DDS �ؽ�ó ������ �������� �ʽ��ϴ�: " << fullPath << std::endl;
            }
            else
            {
                std::cout << "DDS �ؽ�ó �ε� �õ�: " << fullPath << std::endl;
                texture = std::make_shared<Texture>(gGameFramework->GetDevice());
                texture->LoadTexture(gGameFramework->GetDevice(), gGameFramework->GetCommandList(), fullPathW, RootParameter::Texture);

                loadedTexturePath = fullPath; // �ε�� �ؽ�ó ��� ����
                std::cout << "DDS �ؽ�ó �ε� �Ϸ�: " << fullPath << std::endl;
            }
        }
    }

    // GameObject ���� �� �ؽ�ó ����
    auto gameObject = std::make_shared<GameObject>(device);
    gameObject->SetMesh(fbxMesh);
    gameObject->SetWorldMatrix(transformMatrix);

    if (texture)
    {
        gameObject->SetTexture(texture);
        std::cout << "�ؽ�ó ���� �Ϸ�: " << loadedTexturePath << std::endl;
    }
    else
    {
        std::cout << "�ؽ�ó ���� (�ܻ� ��� ����)" << std::endl;
    }

    return gameObject;
}
