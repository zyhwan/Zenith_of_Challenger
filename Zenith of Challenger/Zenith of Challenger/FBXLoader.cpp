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
    XMMATRIX rootRotation = XMMatrixRotationY(XM_PI); // �Ǵ� XMConvertToRadians(180.0f)
    XMMATRIX identityMatrix = rootRotation;

    ProcessNode(scene->mRootNode, scene, identityMatrix);

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
    float scaleFactor = 1.0f; // ������ ũ�� ���� ����

    for (UINT i = 0; i < mesh->mNumVertices; i++)
    {
        TextureVertex vertex;

        // ���� ��ġ �ε�
        XMFLOAT3 localPos = {
            mesh->mVertices[i].x * scaleFactor,
            mesh->mVertices[i].y * scaleFactor,
            mesh->mVertices[i].z * scaleFactor
        };

        // �۷ι� ��ȯ ����
        XMVECTOR pos = XMLoadFloat3(&localPos);
        pos = XMVector3Transform(pos, globalTransform);
        XMStoreFloat3(&vertex.position, pos);

        // ���
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
    gameObject->SetWorldMatrix(XMMatrixIdentity()); // �۷ι� ��ġ�� ���ؽ��� ����Ʊ� ������ �������

    return gameObject;
}
