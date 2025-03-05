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

    // 기본 단위 행렬로 초기화
    XMMATRIX identityMatrix = XMMatrixIdentity();
    ProcessNode(scene->mRootNode, scene, identityMatrix);

    return true;
}

void FBXLoader::ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransform)
{
    // 노드의 변환 행렬 가져오기
    XMMATRIX transformMatrix = XMMatrixIdentity();

    if (node->mTransformation.a1 != 0)  // 변환 행렬이 존재하는 경우
    {
        transformMatrix = XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&node->mTransformation));
    }

    // 부모 변환과 결합
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

    // 크기 조정 (Scale Factor 적용)
    float scaleFactor = 0.1f; // 크기를 10배 줄이도록 설정

    // 버텍스 데이터 로드
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        TextureVertex vertex;

        // 위치 변환 적용 (DirectX 좌표계 변환 + Scale 적용)
        XMFLOAT3 originalPosition = { mesh->mVertices[i].x * scaleFactor,
                                      mesh->mVertices[i].y * scaleFactor,
                                      mesh->mVertices[i].z * -scaleFactor };
        XMVECTOR pos = XMLoadFloat3(&originalPosition);
        pos = XMVector3Transform(pos, transformMatrix);
        XMStoreFloat3(&vertex.position, pos);

        // 노멀 변환 (크기 조정이 없는 방향 벡터 변환)
        XMFLOAT3 originalNormal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
        XMVECTOR normal = XMLoadFloat3(&originalNormal);
        normal = XMVector3TransformNormal(normal, transformMatrix);
        XMStoreFloat3(&vertex.normal, normal);

        // UV 좌표 변환 (DirectX 좌표계 보정)
        vertex.uv = mesh->HasTextureCoords(0) ?
            XMFLOAT2(mesh->mTextureCoords[0][i].x, 1.0f - mesh->mTextureCoords[0][i].y) :
            XMFLOAT2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    auto device = gGameFramework->GetDevice();
    auto commandList = gGameFramework->GetCommandList();
    std::shared_ptr<Mesh<TextureVertex>> fbxMesh = std::make_shared<Mesh<TextureVertex>>(device, commandList, vertices);
    m_meshes.push_back(fbxMesh);

    // 텍스처 로드 (DDS 파일 사용)
    std::shared_ptr<Texture> texture = nullptr;
    std::string loadedTexturePath = "None"; // 로드된 텍스처 경로 저장 변수

    if (scene->mMaterials && mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiString texturePath;

        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
        {
            std::string originalPath = texturePath.C_Str();
            size_t lastSlash = originalPath.find_last_of("/\\");
            std::string textureFileName = (lastSlash != std::string::npos) ? originalPath.substr(lastSlash + 1) : originalPath;

            // DDS 파일로 변경 (png → dds)
            std::string ddsTextureFile = "Textures/" + textureFileName;
            ddsTextureFile.replace(ddsTextureFile.find(".png"), 4, ".dds"); // 확장자 변경

            std::wstring fullPathW(ddsTextureFile.begin(), ddsTextureFile.end());
            std::string fullPath(ddsTextureFile); // UTF-8 형식으로 변환하여 콘솔 출력

            // 파일 존재 여부 확인
            if (!std::filesystem::exists(fullPathW))
            {
                std::cerr << "DDS 텍스처 파일이 존재하지 않습니다: " << fullPath << std::endl;
            }
            else
            {
                std::cout << "DDS 텍스처 로드 시도: " << fullPath << std::endl;
                texture = std::make_shared<Texture>(gGameFramework->GetDevice());
                texture->LoadTexture(gGameFramework->GetDevice(), gGameFramework->GetCommandList(), fullPathW, RootParameter::Texture);

                loadedTexturePath = fullPath; // 로드된 텍스처 경로 저장
                std::cout << "DDS 텍스처 로드 완료: " << fullPath << std::endl;
            }
        }
    }

    // GameObject 생성 및 텍스처 적용
    auto gameObject = std::make_shared<GameObject>(device);
    gameObject->SetMesh(fbxMesh);
    gameObject->SetWorldMatrix(transformMatrix);

    if (texture)
    {
        gameObject->SetTexture(texture);
        std::cout << "텍스처 적용 완료: " << loadedTexturePath << std::endl;
    }
    else
    {
        std::cout << "텍스처 없음 (단색 출력 예정)" << std::endl;
    }

    return gameObject;
}
