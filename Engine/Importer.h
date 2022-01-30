#pragma once
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Bone.h"
#include "Model.h"
#include "Animation.h"
#include <stack>

struct CustomNode
{
	int parent;
	Bone* bone;
	glm::mat4 transform;
	//changable
	glm::mat4 final_transform;
};

static glm::mat4 Matrix_RowToColumn(const aiMatrix4x4& m)
{
	const glm::vec4 c1(m.a1, m.b1, m.c1, m.d1);
	const glm::vec4 c2(m.a2, m.b2, m.c2, m.d2);
	const glm::vec4 c3(m.a3, m.b3, m.c3, m.d3);
	const glm::vec4 c4(m.a4, m.b4, m.c4, m.d4);
	return glm::mat4(c1, c2, c3, c4);
}

static glm::vec3 Vec_ToGLM(const aiVector3D& vec)
{
	return glm::vec3(vec.x, vec.y, vec.z);
}

static glm::quat Quat_ToGLM(const aiQuaternion& pOrientation)
{
	return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
}

struct BoneData
{
	std::map<std::string, Bone> bones_with_names;
	int next_bone_id = 0;

	int AddBone(std::string&& name, glm::mat4 bone_to_model)
	{
		if (GetBone(name.c_str()))
		{
			return GetBone(name.c_str())->m_ID;
		}
		auto data = bones_with_names.insert(std::make_pair(std::move(name), Bone(-1, bone_to_model)));
		Bone& info = data.first->second;
		info.m_ID = next_bone_id++;
		return info.m_ID;
	}

	const Bone* GetBone(const char* name)  const
	{
		auto it = bones_with_names.find(name);
		return ((it != bones_with_names.end()) ? &(it->second) : nullptr);
	}
};

struct Node {
	int parent = -1;
	aiNode* node;
};

struct ModelData {
	Model model;
	std::vector<Animation> animations;
};

class Importer
{
public:
	static ModelData LoadModel(const std::string& path);

private:
	static Mesh ProcessMesh(aiMesh* mesh, aiMaterial* material);
	static Animation LoadAnimation(const aiAnimation* animation,const aiNode* root_node);
	static KeyFrames LoadBoneKeyFrames(const aiNodeAnim& channel);
public:
	struct ImporterData
	{
		std::vector<Node> NodeWithParents;
		std::vector<std::string> NodeNames;
		std::string m_Directory = "";
		BoneData    m_Data;
		std::vector<CustomNode> initialnodes;
	};
	static ImporterData data;
	static void AddBoneWeigt(Vertex& vertex, int bone_index, float weight);
};