#include "Cube.h"

#include "TextureManager.h"

#include "stb_image.h"

#include <spdlog/spdlog.h>

Cube::Cube()
{
}

Cube::~Cube()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void Cube::create(const Shader& shader, const glm::vec3& pos)
{
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.5f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right     0r
         0.5f,  0.5f, 0.5f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f, // top right     1r
         0.5f,  0.5f, 0.5f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f, // top right     2r

         0.5f, -0.5f, 0.5f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right  3g  
         0.5f, -0.5f, 0.5f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right  4g
         0.5f, -0.5f, 0.5f,    0.0f, 1.0f, 0.0f,   0.0f, 1.0f, // bottom right  5g

        -0.5f, -0.5f, 0.5f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left   6b
        -0.5f, -0.5f, 0.5f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left   7b
        -0.5f, -0.5f, 0.5f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left   8b

        -0.5f,  0.5f, 0.5f,    1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // top left     9y
        -0.5f,  0.5f, 0.5f,    1.0f, 1.0f, 0.0f,   1.0f, 1.0f,  // top left     10y
        -0.5f,  0.5f, 0.5f,    1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // top left     11y

         0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 1.0f, // top right     12c
         0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f, // top right     13c
         0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f, // top right     14c

         0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f, // bottom right  15m
         0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 1.0f,   1.0f, 1.0f, // bottom right  16m
         0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 1.0f,   1.0f, 1.0f, // bottom right  17m

        -0.5f, -0.5f, -0.5f,   0.5f, 0.5f, 0.5f,   0.0f, 0.0f, // bottom left   18gr
        -0.5f, -0.5f, -0.5f,   0.5f, 0.5f, 0.5f,   1.0f, 0.0f, // bottom left   19gr
        -0.5f, -0.5f, -0.5f,   0.5f, 0.5f, 0.5f,   1.0f, 0.0f, // bottom left   20gr

        -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,  // top left     21w
        -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // top left      22w
        -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f  // top left      23w
    };
    unsigned int indices[] = {
        0, 3, 9, // first triangle
        3, 6, 9,  // second triangle

        1, 4, 16,
        1, 13, 16,

        15, 12, 21,
        15, 18, 21,

        22, 11, 19,
        11, 19, 7,

        2, 14, 23,
        2, 10, 23,

        8, 5, 20,
        5, 20, 17
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // pos attrib
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attrib
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture attrib
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    shader.setBool("hasTexture", false);
    shader.setFloat4("newColor", 1.0f, 1.0f, 1.0f, 1.0f);
    shader.setInt("Texture", 0);

    m_Pos = pos;
    translate(shader, m_Pos);
    m_Rotation = glm::vec3(0.0f);
    m_Scale = glm::vec3(1.0f);
    m_Color = glm::vec4(1.0f);
}

void Cube::setTexture(const std::string& textureID)
{
    if (textureID == "")
    {
        m_Texture = 0;
    }
    else
    {
        m_Texture = TextureManager::get(textureID);
    }
}

void Cube::translate(const Shader& shader, const glm::vec3& deltaPos)
{
    m_Pos += deltaPos;
    m_ModelMatrix = glm::translate(m_ModelMatrix, deltaPos);
}

void Cube::rotate(const Shader& shader, float angle, const glm::vec3& axis)
{
    m_ModelMatrix = glm::rotate(m_ModelMatrix, angle, axis);
    m_Rotation = extractEulerAngles();
}

void Cube::scale(const Shader& shader, const glm::vec3& scaleFactor)
{
    m_Scale *= scaleFactor;
    m_ModelMatrix = glm::scale(m_ModelMatrix, scaleFactor);
}

void Cube::setColor(const Shader& shader, const glm::vec4& newColor)
{
    m_Color = newColor;
    shader.setFloat4("newColor", newColor.x, newColor.y, newColor.z, newColor.t);
}

void Cube::applyTransformation(const glm::mat4& modelMatrix)
{
    m_ModelMatrix = m_ModelMatrix * modelMatrix;
}

void Cube::render(const Shader& shader)
{
    shader.use();
    shader.setMatrix4("model", m_ModelMatrix);
    glBindVertexArray(m_VAO);
    if (m_Texture != 0)
    {
        shader.setBool("hasTexture", true);
        glBindTexture(GL_TEXTURE_2D, m_Texture);
    }
    else
    {
        shader.setBool("hasTexture", false);
    }
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

glm::vec3 Cube::extractEulerAngles()
{
    glm::vec3 eulerAngles;

    eulerAngles.x = atan2(m_ModelMatrix[2][1], m_ModelMatrix[2][2]); // Rotación en X
    eulerAngles.y = atan2(-m_ModelMatrix[2][0], sqrt(m_ModelMatrix[2][1] * m_ModelMatrix[2][1] + m_ModelMatrix[2][2] * m_ModelMatrix[2][2])); // Rotación en Y
    eulerAngles.z = atan2(m_ModelMatrix[1][0], m_ModelMatrix[0][0]); // Rotación en Z

    return eulerAngles;
}

