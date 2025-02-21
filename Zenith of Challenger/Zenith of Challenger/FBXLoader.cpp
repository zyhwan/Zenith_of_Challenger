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

    std::cout << "FBX 로드 성공 : " << filename << std::endl;
    std::cout << "FBX 메시 개수 : " << scene->mNumMeshes << std::endl;
    ProcessNode(scene->mRootNode, scene);
    return true;
}

void FBXLoader::ProcessNode(aiNode* node, const aiScene* scene)
{
    std::cout << "FBX 노드 처리 중: " << node->mName.C_Str() << std::endl;

    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        std::cout << "FBX 메시 로드: " << mesh->mName.C_Str()
            << " | 버텍스 개수: " << mesh->mNumVertices << std::endl;
        ProcessMesh(mesh, scene);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

void FBXLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
    vector<TextureVertex> vertices;
    vector<UINT> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        TextureVertex vertex;
        vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
        vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
        vertex.uv = mesh->HasTextureCoords(0) ?
            XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) :
            XMFLOAT2(0.0f, 0.0f);
        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    auto device = gGameFramework->GetDevice();
    auto commandList = gGameFramework->GetCommandList();
    shared_ptr<Mesh<TextureVertex>> fbxMesh =
        make_shared<Mesh<TextureVertex>>(device, commandList, vertices);
    m_meshes.push_back(fbxMesh);

    // GameObject를 생성하여 FBX 모델을 저장
    auto gameObject = make_shared<GameObject>(device);
    gameObject->SetMesh(fbxMesh);
    gameObject->SetScale(XMFLOAT3{ 10.0f, 10.0f, 10.0f });
    gameObject->SetPosition(XMFLOAT3{ 0.0f, 0.0f, 0.0f });
    m_gameObjects.push_back(gameObject);
}
