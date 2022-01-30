#include "Importer.h"

Importer::ImporterData Importer::data = Importer::ImporterData();

ModelData Importer::LoadModel(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_LimitBoneWeights);
    if (scene == nullptr)
    {
        std::cout << "Cant load scene" << std::endl;
        std::cout << importer.GetErrorString() << std::endl;
        exit(1);
    }
    data.m_Directory = path.substr(0, path.find_last_of('/'));
    data.m_Directory += "/";
    std::vector<Mesh> meshes;
    std::vector<Bone> bones;
    std::vector<Animation> animations;
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        meshes.push_back(ProcessMesh(scene->mMeshes[i], scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]));
    }
    for (int i = 0; i < scene->mNumAnimations; i++)
    {
        animations.push_back(LoadAnimation(scene->mAnimations[i],scene->mRootNode));
    }
    return { Model(std::move(meshes), std::move(bones)), animations };
}

Mesh Importer::ProcessMesh(aiMesh* mesh, aiMaterial* material)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    for (unsigned i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;
        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.texture_uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        for (int j = 0; j < 4; j++)
        {
            vertex.bone_ids[j] = -1;
            vertex.weights[j] = 0.0f;
        }
        vertices.push_back(vertex);
    }
    for (unsigned i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }
    for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
    {
        aiString str;
        material->GetTexture(aiTextureType_DIFFUSE, i, &str);
        textures.push_back(Texture((data.m_Directory + std::string(str.C_Str()))));
    }
    for (unsigned i = 0; i < mesh->mNumBones; ++i)
    {
        const aiString& bone_name = mesh->mBones[i]->mName;
        const int bone_index = data.m_Data.AddBone(std::string(bone_name.data, bone_name.length),Matrix_RowToColumn(mesh->mBones[i]->mOffsetMatrix));
        const aiBone* const bone = mesh->mBones[i];

        const aiVertexWeight* const weights = bone->mWeights;
        for (unsigned j = 0; j < bone->mNumWeights; ++j)
        {
            const unsigned vertex_id = weights[j].mVertexId;
            const float weight = weights[j].mWeight;

            AddBoneWeigt(vertices[vertex_id], bone_index, weight);
        }
    }

    return Mesh(vertices,indices, textures);
}

void Importer::AddBoneWeigt(Vertex& vertex, int bone_index, float weight)
{
    auto it = std::find_if(std::begin(vertex.bone_ids), std::end(vertex.bone_ids)
        , [&](int index) { return (index < 0) && (index != bone_index); });
    const std::size_t i = std::distance(std::begin(vertex.bone_ids), it);
    vertex.weights[i] = weight;
    vertex.bone_ids[i] = bone_index;
}

Animation Importer::LoadAnimation(const aiAnimation* animation, const aiNode* root_node)
{
    const float duration = float(animation->mDuration);
    const float ticks_per_second = float(animation->mTicksPerSecond);
    std::vector<AnimationNode> nodes;
    std::vector<const aiString*> node_names;
    struct Node
    {
        const aiNode* src = nullptr;
        int parent = -1;
    };
    std::stack<Node> dfs;
    dfs.push(Node{ root_node, -1});
    int bones_count = 0;
    while (dfs.size() > 0)
    {
        Node data = std::move(dfs.top());
        dfs.pop();
        if (data.parent != -1 && !Importer::data.m_Data.GetBone(data.src->mName.C_Str()))
        {
            continue;
        }
        AnimationNode node;
        node.parent = data.parent;
        node.node_transform = Matrix_RowToColumn(data.src->mTransformation);
        nodes.push_back(node);
        node_names.push_back(&data.src->mName);
        const int parent_index = int(nodes.size() - 1);
        for (unsigned i = 0; i < data.src->mNumChildren; ++i)
        {
            dfs.push(Node{ data.src->mChildren[i], parent_index });
        }
    }

    for (unsigned i = 0; i < animation->mNumChannels; ++i)
    {
        auto channel = animation->mChannels[i];
        const aiString& bone_name = channel->mNodeName;
        auto it = std::find_if(node_names.cbegin(), node_names.cend(), [&bone_name](const aiString* node_name){return (bone_name == *node_name);});
        const int index = int(std::distance(node_names.cbegin(), it));
        const Bone* _bone_info = data.m_Data.GetBone(bone_name.C_Str());
        AnimationNode& node = nodes[index];
        node.bone = (Bone*)_bone_info;
        if ((channel->mNumPositionKeys + channel->mNumRotationKeys + channel->mNumScalingKeys) > 0)
        {
            node.has_key_frames = true;
            node.frames = LoadBoneKeyFrames(*channel);
        }
    }
    return Animation(std::move(nodes), duration, ticks_per_second);
}

KeyFrames Importer::LoadBoneKeyFrames(const aiNodeAnim& channel)
{
    KeyFrames bone;
    bone.m_KeyPositions.reserve(channel.mNumPositionKeys);
    for (unsigned index = 0; index < channel.mNumPositionKeys; ++index)
    {
        KeyPosition data;
        data.position = Vec_ToGLM(channel.mPositionKeys[index].mValue);
        data.time_stamp = float(channel.mPositionKeys[index].mTime);
        bone.m_KeyPositions.push_back(data);
    }

    bone.m_KeyRotations.reserve(channel.mNumRotationKeys);
    for (unsigned index = 0; index < channel.mNumRotationKeys; ++index)
    {
        KeyRotation data;
        data.rotation = Quat_ToGLM(channel.mRotationKeys[index].mValue);
        data.time_stamp = float(channel.mRotationKeys[index].mTime);
        bone.m_KeyRotations.push_back(data);
    }

    bone.m_KeyScales.reserve(channel.mNumScalingKeys);
    for (unsigned index = 0; index < channel.mNumScalingKeys; ++index)
    {
        KeyScale data;
        data.scale = Vec_ToGLM(channel.mScalingKeys[index].mValue);
        data.time_stamp = float(channel.mScalingKeys[index].mTime);
        bone.m_KeyScales.push_back(data);
    }

    return bone;
}