#version 330 core

layout (location = 0) in vec3  aPos;
layout (location = 1) in vec2  aUv;
layout (location = 2) in ivec4 in_BoneIds;
layout (location = 3) in vec4  in_Weights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 bone_transforms[100];

out vec2 TexCoord;

void main()
{
    mat4 S = mat4(0.f);
    for (int i = 0; i < 4; ++i)
    {
        if (in_BoneIds[i] >= 0)
        {
            S += (bone_transforms[in_BoneIds[i]] * in_Weights[i]);
        }
    }
    mat4 MVP = projection * view * model;
    gl_Position = MVP  * S * vec4(aPos, 1.f);
    TexCoord = aUv;
}