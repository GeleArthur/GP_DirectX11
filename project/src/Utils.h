#pragma once
#include <fstream>
#include <vector>

#include "Math.h"

struct Vertex_PosTexture;

namespace Utils
{
	bool ParseOBJ(const std::string& filename, std::vector<Vertex_PosTexture>& vertices, std::vector<uint32_t>& indices, bool flipAxisAndWinding = true);
}
