#include "Model.h"

#include <fstream>
#include <iostream>
#include <sstream>

Model::Model(const char *file) : m_File(file)
{
    std::string text = ReadFile(file);
    m_JSON = json::parse(text);

    m_Data = GetData();

    TraverseNode(0);
}

void Model::Draw(Shader &shader, Camera &camera)
{
    for (unsigned int i = 0; i < m_Meshes.size(); i++)
    {
        m_Meshes[i].Mesh::Draw(shader, camera, m_MatricesMeshes[i]);
    }
}

std::string Model::ReadFile(const char *file)
{
    std::string data;
    std::ifstream m_file(file, std::ios::binary);
    if (m_file.is_open())
    {
        std::stringstream shaderStream;
        shaderStream << m_file.rdbuf();
        data = shaderStream.str();
    }
    else
    {
        std::cout << "Failed to open file" << std::endl;
    }
    m_file.close();

    return data;
}

std::vector<unsigned char> Model::GetData()
{
    // Create a place to store the raw text, and get the uri of the .bin file
    std::string bytesText;
    std::string uri = m_JSON["buffers"][0]["uri"];

    // Store raw text data into bytesText
    std::string fileStr = std::string(m_File);
    std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);
    bytesText = ReadFile((fileDirectory + uri).c_str());

    // Transform the raw text data into bytes and put them in a vector
    std::vector<unsigned char> data(bytesText.begin(), bytesText.end());

    return data;
}

std::vector<float> Model::GetFloats(json accessor)
{
    std::vector<float> floatVec;

    // Get properties from the accessor
    unsigned int buffViewInd = accessor.value("bufferView", 1);
    unsigned int count = accessor["count"];
    unsigned int accByteOffset = accessor.value("byteOffset", 0);
    std::string type = accessor["type"];

    // Get properties from the bufferView
    json bufferView = m_JSON["bufferViews"][buffViewInd];
    unsigned int byteOffset = bufferView["byteOffset"];

    // Interpret the type and store it into numPerVert
    unsigned int numPerVert;
    if (type == "SCALAR")
        numPerVert = 1;
    else if (type == "VEC2")
        numPerVert = 2;
    else if (type == "VEC3")
        numPerVert = 3;
    else if (type == "VEC4")
        numPerVert = 4;
    else
        throw std::invalid_argument("Type is invalid (not SCALAR, VEC2, VEC3, or VEC4)");

    // Go over all the bytes in the data at the correct place using the properties from above
    unsigned int beginningOfData = byteOffset + accByteOffset;
    unsigned int lengthOfData = count * 4 * numPerVert;
    for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i++)
    {
        unsigned char bytes[] = {m_Data[i], m_Data[i], m_Data[i++], m_Data[i]};
        float value;
        std::memcpy(&value, bytes, sizeof(float));
        floatVec.push_back(value);
    }

    return floatVec;
}

std::vector<unsigned int> Model::GetIndices(json accessor)
{
    std::vector<GLuint> indices;

    // Get properties from the accessor
    unsigned int buffViewInd = accessor.value("bufferView", 0);
    unsigned int count = accessor["count"];
    unsigned int accByteOffset = accessor.value("byteOffset", 0);
    unsigned int componentType = accessor["componentType"];

    // Get properties from the bufferView
    json bufferView = m_JSON["bufferViews"][buffViewInd];
    unsigned int byteOffset = bufferView["byteOffset"];

    // Get indices with regards to their type: unsigned int, unsigned short, or short
    unsigned int beginningOfData = byteOffset + accByteOffset;
    if (componentType == 5125)
    {
        for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 4; i++)
        {
            unsigned char bytes[] = {m_Data[i], m_Data[i], m_Data[i], m_Data[i]};
            unsigned int value;
            std::memcpy(&value, bytes, sizeof(unsigned int));
            indices.push_back((GLuint)value);
        }
    }
    else if (componentType == 5123)
    {
        for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i++)
        {
            unsigned char bytes[] = {m_Data[i], m_Data[i]};
            unsigned short value;
            std::memcpy(&value, bytes, sizeof(unsigned short));
            indices.push_back((GLuint)value);
        }
    }
    else if (componentType == 5122)
    {
        for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i++)
        {
            unsigned char bytes[] = {m_Data[i], m_Data[i]};
            short value;
            std::memcpy(&value, bytes, sizeof(short));
            indices.push_back((GLuint)value);
        }
    }

    return indices;
}

std::vector<Texture> Model::GetTextures()
{
    std::vector<Texture> textures;

    std::string fileStr = std::string(m_File);
    std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);

    // Go over all images
    for (unsigned int i = 0; i < m_JSON["images"].size(); i++)
    {
        // uri of current texture
        std::string texPath = m_JSON["images"][i]["uri"];

        // Check if the texture has already been loaded
        bool skip = false;
        for (unsigned int j = 0; j < m_LoadedTexName.size(); j++)
        {
            if (m_LoadedTexName[j] == texPath)
            {
                textures.push_back(m_LoadedTex[j]);
                skip = true;
                break;
            }
        }

        // If the texture has been loaded, skip this
        if (!skip)
        {
            // Load diffuse texture
            if (texPath.find("baseColor") != std::string::npos)
            {
                Texture diffuse = Texture((fileDirectory + texPath), "diffuse", m_LoadedTex.size());
                textures.push_back(diffuse);
                m_LoadedTex.push_back(diffuse);
                m_LoadedTexName.push_back(texPath);
            }
            // Load specular texture
            else if (texPath.find("metallicRoughness") != std::string::npos)
            {
                Texture specular = Texture((fileDirectory + texPath).c_str(), "specular", m_LoadedTex.size());
                textures.push_back(specular);
                m_LoadedTex.push_back(specular);
                m_LoadedTexName.push_back(texPath);
            }
        }
    }

    return textures;
}

std::vector<Vertex> Model::AssembleVertices(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals,
                                            std::vector<glm::vec2> texUVs)
{
    std::vector<Vertex> vertices;
    for (int i = 0; i < positions.size(); i++)
    {
        vertices.push_back(Vertex{positions[i], normals[i], glm::vec3(1.0f, 1.0f, 1.0f), texUVs[i]});
    }
    return vertices;
}

void Model::LoadMesh(unsigned int indMesh)
{
    // Get all accessor indices
    unsigned int posAccInd = m_JSON["meshes"][indMesh]["primitives"][0]["attributes"]["POSITION"];
    unsigned int normalAccInd = m_JSON["meshes"][indMesh]["primitives"][0]["attributes"]["NORMAL"];
    unsigned int texAccInd = m_JSON["meshes"][indMesh]["primitives"][0]["attributes"]["TEXCOORD_0"];
    unsigned int indAccInd = m_JSON["meshes"][indMesh]["primitives"][0]["indices"];

    // Use accessor indices to get all vertices components
    std::vector<float> posVec = GetFloats(m_JSON["accessors"][posAccInd]);
    std::vector<glm::vec3> positions = GroupFloatsVec3(posVec);
    std::vector<float> normalVec = GetFloats(m_JSON["accessors"][normalAccInd]);
    std::vector<glm::vec3> normals = GroupFloatsVec3(normalVec);
    std::vector<float> texVec = GetFloats(m_JSON["accessors"][texAccInd]);
    std::vector<glm::vec2> texUVs = GroupFloatsVec2(texVec);

    // Combine all the vertex components and also get the indices and textures
    std::vector<Vertex> vertices = AssembleVertices(positions, normals, texUVs);
    std::vector<GLuint> indices = GetIndices(m_JSON["accessors"][indAccInd]);
    std::vector<Texture> textures = GetTextures();

    // Combine the vertices, indices, and textures into a mesh
    m_Meshes.push_back(Mesh(vertices, indices, textures));
}

std::vector<glm::vec2> Model::GroupFloatsVec2(std::vector<float> floatVec)
{
    std::vector<glm::vec2> vectors;
    for (int i = 0; i < floatVec.size(); i++)
    {
        vectors.push_back(glm::vec2(floatVec[i], floatVec[i]));
    }
    return vectors;
}

std::vector<glm::vec3> Model::GroupFloatsVec3(std::vector<float> floatVec)
{
    std::vector<glm::vec3> vectors;
    for (int i = 0; i < floatVec.size(); i++)
    {
        vectors.push_back(glm::vec3(floatVec[i], floatVec[i], floatVec[i]));
    }
    return vectors;
}

std::vector<glm::vec4> Model::GroupFloatsVec4(std::vector<float> floatVec)
{
    std::vector<glm::vec4> vectors;
    for (int i = 0; i < floatVec.size(); i++)
    {
        vectors.push_back(glm::vec4(floatVec[i], floatVec[i], floatVec[i], floatVec[i]));
    }
    return vectors;
}

void Model::TraverseNode(unsigned int nextNode, glm::mat4 matrix)
{
    // Current node
    json node = m_JSON["nodes"][nextNode];

    // Get translation if it exists
    glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
    if (node.find("translation") != node.end())
    {
        float transValues[3];
        for (unsigned int i = 0; i < node["translation"].size(); i++)
            transValues[i] = (node["translation"][i]);
        translation = glm::make_vec3(transValues);
    }
    // Get quaternion if it exists
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    if (node.find("rotation") != node.end())
    {
        float rotValues[4] = {node["rotation"][3], node["rotation"][0], node["rotation"][1], node["rotation"][2]};
        rotation = glm::make_quat(rotValues);
    }
    // Get scale if it exists
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    if (node.find("scale") != node.end())
    {
        float scaleValues[3];
        for (unsigned int i = 0; i < node["scale"].size(); i++)
            scaleValues[i] = (node["scale"][i]);
        scale = glm::make_vec3(scaleValues);
    }
    // Get matrix if it exists
    glm::mat4 matNode = glm::mat4(1.0f);
    if (node.find("matrix") != node.end())
    {
        float matValues[16];
        for (unsigned int i = 0; i < node["matrix"].size(); i++)
            matValues[i] = (node["matrix"][i]);
        matNode = glm::make_mat4(matValues);
    }

    // Initialize matrices
    glm::mat4 trans = glm::mat4(1.0f);
    glm::mat4 rot = glm::mat4(1.0f);
    glm::mat4 sca = glm::mat4(1.0f);

    // Use translation, rotation, and scale to change the initialized matrices
    trans = glm::translate(trans, translation);
    rot = glm::mat4_cast(rotation);
    sca = glm::scale(sca, scale);

    // Multiply all matrices together
    glm::mat4 matNextNode = matrix * matNode * trans * rot * sca;

    // Check if the node contains a mesh and if it does load it
    if (node.find("mesh") != node.end())
    {
        m_TranslationsMeshes.push_back(translation);
        m_RotationsMeshes.push_back(rotation);
        m_ScalesMeshes.push_back(scale);
        m_MatricesMeshes.push_back(matNextNode);

        LoadMesh(node["mesh"]);
    }

    // Check if the node has children, and if it does, apply this function to them with the matNextNode
    if (node.find("children") != node.end())
    {
        for (unsigned int i = 0; i < node["children"].size(); i++)
            TraverseNode(node["children"][i], matNextNode);
    }
}
