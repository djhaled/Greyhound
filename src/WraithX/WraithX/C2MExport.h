#pragma once

#include <string>
#include <cstdint>

// We need to include external libraries for models
#include "WraithModel.h"

// A class that handles writing SEModel model files
class C2M
{
public:
    // Export a WraithModel to a SEModel file
    static void ExportC2M(const WraithModel& Model, const std::string& FileName, bool SupportsScale = false);
    static uint32_t GetVertexIndex(const std::vector<WraithVertex>& vertices, const WraithVertex& vertex);

};