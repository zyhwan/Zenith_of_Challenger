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

    // Y�� ���� 180�� ȸ�� ��� �߰�
    XMMATRIX rootRotation = XMMatrixRotationY(XM_PI); // �Ǵ� XMConvertToRadians(180.0f) XM_PI
    XMMATRIX scaleXZ = XMMatrixScaling(5.0f, 5.0f, 5.0f); // �� XZ�� ������ Ȯ��
    XMMATRIX rootTransform = scaleXZ * rootRotation;

    ProcessNode(scene->mRootNode, scene, rootTransform);

    return true;
}


void FBXLoader::ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransform)
{
    // ����� ���� ����� XMMATRIX�� ��ȯ
    aiMatrix4x4 mat = node->mTransformation;
    aiMatrix4x4 transpose = mat; // Assimp�� row-major
    transpose.Transpose();       // DirectX�� column-major�̹Ƿ� ��ġ �ʿ�

    XMMATRIX nodeLocalTransform = XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&transpose));

    // �۷ι� ��ȯ = �θ� * ����
    XMMATRIX globalTransform = XMMatrixMultiply(nodeLocalTransform, parentTransform);

    // �޽ð� �����ϸ� ó��
    for (UINT i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        auto object = ProcessMesh(mesh, scene, globalTransform);
        m_gameObjects.push_back(object);
    }

    // �ڽ� ��� ��ȸ
    for (UINT i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, globalTransform);
    }
}

std::shared_ptr<GameObject> FBXLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& globalTransform)
{
    std::vector<TextureVertex> vertices;
    float scaleFactor = 1.0f;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        TextureVertex vertex;
        XMFLOAT3 pos = { -mesh->mVertices[i].x * scaleFactor, mesh->mVertices[i].y * scaleFactor, mesh->mVertices[i].z * scaleFactor };
        XMVECTOR transformedPos = XMVector3Transform(XMLoadFloat3(&pos), globalTransform);
        XMStoreFloat3(&vertex.position, transformedPos);

        XMFLOAT3 normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
        XMVECTOR transformedNormal = XMVector3TransformNormal(XMLoadFloat3(&normal), globalTransform);
        XMStoreFloat3(&vertex.normal, XMVector3Normalize(transformedNormal));

        vertex.uv = mesh->HasTextureCoords(0) ?
            XMFLOAT2(mesh->mTextureCoords[0][i].x, 1.0f - mesh->mTextureCoords[0][i].y) :
            XMFLOAT2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    auto device = gGameFramework->GetDevice();
    auto commandList = gGameFramework->GetCommandList();
    auto meshPtr = std::make_shared<Mesh<TextureVertex>>(device, commandList, vertices);
    m_meshes.push_back(meshPtr);

    // �ؽ�ó �ε�
    auto texture = std::make_shared<Texture>(device);
    texture->LoadTexture(device, commandList, L"Image/Base Map.dds", RootParameter::Texture);
    texture->CreateShaderVariable(device); // <- �� �ʿ���!!

    // �⺻ ������ ��� �뵵 (���̴����� �ؽ�ó ��� �� ���õ�)
    XMFLOAT4 baseColor = XMFLOAT4(1, 1, 1, 1);

    auto gameObject = std::make_shared<GameObject>(device);
    gameObject->SetMesh(meshPtr);
    gameObject->SetWorldMatrix(globalTransform);

    gameObject->SetTexture(texture);         // �ؽ�ó ����
    gameObject->SetUseTexture(true);         // ���̴����� g_texture ���
    gameObject->SetBaseColor(baseColor);     // ���� ��� �����

    return gameObject;
}
