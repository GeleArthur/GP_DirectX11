#include "Utils.h"
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

bool Utils::ParseOBJ(const std::string& filename, std::vector<Vertex_PosTexture>& vertices, std::vector<uint32_t>& indices, bool flipAxisAndWinding)
{
    
    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(filename))
    {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        return false;
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();
    

    for (const auto& shape : shapes)
    {
        for (size_t j = 0; j < shape.mesh.indices.size(); ++j)
        {
            indices.push_back(j);
            auto index = shape.mesh.indices[j];
            
            vertices.push_back({
                {
                    attrib.vertices[3*index.vertex_index+0],
                    attrib.vertices[3*index.vertex_index+1],
                    attrib.vertices[3*index.vertex_index+2]
                },
                {},
                {
                    attrib.texcoords[2*index.texcoord_index+0],
                    1.0f - attrib.texcoords[2*index.texcoord_index+1],
                },
                {},
                {},
            });
        }
    }
    
    return true;
}

