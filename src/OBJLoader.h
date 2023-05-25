#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>

#include "Buffer.h"
#include "Shader.h"

static std::vector<Vertex> loadOBJ(const char *fileName)
{
    // Vertex portions
    std::vector<glm::fvec3> vertexPosition;
}