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
        for (uint32_t j = 0u; j < shape.mesh.indices.size(); ++j)
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

    for (uint32_t i = 0; i < out.indices.size(); i += 3)
    {
        uint32_t index0 = out.indices[i];
        uint32_t index1 = out.indices[static_cast<size_t>(i) + 1];
        uint32_t index2 = out.indices[static_cast<size_t>(i) + 2];

        const Vector3& p0 = out.positions[index0];
        const Vector3& p1 = out.positions[index1];
        const Vector3& p2 = out.positions[index2];
        const Vector2& uv0 = out.uv[index0];
        const Vector2& uv1 = out.uv[index1];
        const Vector2& uv2 = out.uv[index2];

        const Vector3 edge0 = p1 - p0;
        const Vector3 edge1 = p2 - p0;
        const Vector2 diffX = Vector2(uv1.x - uv0.x, uv2.x - uv0.x);
        const Vector2 diffY = Vector2(uv1.y - uv0.y, uv2.y - uv0.y);
        float r = 1.f / Vector2::Cross(diffX, diffY);

        Vector3 tangent = (edge0 * diffY.y - edge1 * diffY.x) * r;
        out.tangent.emplace_back(tangent);
        out.tangent.emplace_back(tangent);
        out.tangent.emplace_back(tangent);
    }
    
    
    return out;
}

