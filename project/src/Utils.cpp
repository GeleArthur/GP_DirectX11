#include "Utils.h"
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

Utils::ParsedObj Utils::ParseOBJ(const std::string& filename, bool flipAxisAndWinding)
{
    
    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(filename))
    {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        throw;
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    ParsedObj out;

    for (const auto& shape : shapes)
    {
        out.indices.reserve(shape.mesh.indices.size());
        out.positions.reserve(shape.mesh.indices.size());
        out.uv.reserve(shape.mesh.indices.size());
        out.normal.reserve(shape.mesh.indices.size());
        for (size_t j = 0; j < shape.mesh.indices.size(); ++j)
        {
            out.indices.push_back(j);
            const auto index = shape.mesh.indices[j];

            out.positions.emplace_back(
            attrib.vertices[3 * index.vertex_index + 0],
            attrib.vertices[3 * index.vertex_index + 1],
            attrib.vertices[3 * index.vertex_index + 2]
            );

            out.uv.emplace_back(
            attrib.texcoords[2*index.texcoord_index+0],
            1.0f - attrib.texcoords[2*index.texcoord_index+1]
            );

            out.normal.emplace_back(
                attrib.normals[3*index.normal_index+0],
                attrib.normals[3*index.normal_index+1],
                attrib.normals[3*index.normal_index+2]
                );
        }
    }
    
    return out;
}

