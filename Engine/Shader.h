#pragma once
#define GLEW_STATIC
#include <glew.h>
#include <string>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include "glm/glm.hpp"

class Shader
{
public:
	Shader(const std::string& vertex_path, const std::string& fragment_path);
	~Shader();
	void Bind() const;
	void UnBind() const;
    void SetFloat(unsigned int id, float value);
    void SetFloatArray(unsigned int id, int lenght, float* data);
    void SetInteger(unsigned int id, int value);
    void SetIntegerArray(unsigned int id, int lenght, int* data);
    void SetVector2f(unsigned int id, const glm::vec2& value);
    void SetVector3f(unsigned int id, const glm::vec3& value);
    void SetVector4f(unsigned int id, const glm::vec4& value);
    void SetMatrix3(unsigned int id, const glm::mat3& matrix);
    void SetMatrix4(unsigned int id, const glm::mat4& matrix);
    void SetMatrix4Array(unsigned int id, int lenght, glm::mat4* data);
	unsigned int GetUniformID(const std::string& name) const;
private:
    static std::string ParseFile(const std::string& path)
    {
        std::string line, source;
        std::ifstream file(path);
        if (file.is_open())
        {
            while (getline(file, line))
            {
                source += line + "\n";
            }
            file.close();
            return source;
        }
        else {
            std::cout << "file not loaded" << std::endl;
            std::string error = std::string("");
            return error;
        }
    }
private:
	unsigned int m_ID;
};