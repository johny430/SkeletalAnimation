#include "Model.h"

Model::Model(std::vector<Mesh>&& meshes, std::vector<Bone>&& bones)
    : m_Shader("assets/vampire/animation.vert","assets/vampire/animation.frag"), m_Meshes(std::move(meshes)), m_Bones(std::move(bones))
{
    m_ProjectionPtr = m_Shader.GetUniformID("projection");
    m_ModelPtr = m_Shader.GetUniformID("model");
    m_ViewPtr = m_Shader.GetUniformID("view");
    m_TransformsPtr = m_Shader.GetUniformID("bone_transforms");
}

Model::~Model()
{
}

void Model::Draw(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model,std::span<const glm::mat4> transforms)
{
    m_Shader.Bind();
    m_Shader.SetMatrix4(m_ProjectionPtr, projection);
    m_Shader.SetMatrix4(m_ViewPtr, view);
    m_Shader.SetMatrix4(m_ModelPtr, model);
    glUniformMatrix4fv(m_TransformsPtr, GLsizei(transforms.size()), GL_FALSE, glm::value_ptr(transforms[0]));
    for (Mesh& mesh : m_Meshes)
    {
        mesh.Draw();
    }
}
