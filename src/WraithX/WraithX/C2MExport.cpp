#include "stdafx.h"
#include "C2MExport.h"
#include <functional>

// We need the binarywriter class
#include "BinaryWriter.h"
void C2M::ExportC2M(const WraithModel& Model, const std::string& FileName, bool SupportsScale)
{
    WraithSubmesh Mesh;

    BinaryWriter Writer;
    // Open the file
    Writer.Create(FileName);
    bool bIsSkeletalMesh = (Model.Bones.size() > 1);
    Writer.Write<byte>(bIsSkeletalMesh);
    Writer.WriteNullTerminatedString(Model.GameName);
    Writer.WriteNullTerminatedString(Model.AssetName);
    Writer.Write<uint32_t>(Model.VertexCount());
    Writer.Write<uint32_t>(Model.SubmeshCount());
    Writer.Write<uint32_t>(Model.FaceCount());

    for (size_t i = 0; i < Model.Submeshes.size(); i++)
    {
        const auto& submesh = Model.Submeshes[i];
        Writer.WriteNullTerminatedString("surfC2Model_" + std::to_string(i));  // Surface name based on array number
        Writer.Write<uint8_t>(submesh.Verticies[0].UVLayers.size());
        Writer.Write<uint8_t>(submesh.MaterialIndicies.size());

        for (const auto& mti : submesh.MaterialIndicies)
        {
            Writer.Write<USHORT>(mti);
        }

        Writer.Write<uint32_t>(submesh.Faces.size());

        for (const auto& face : submesh.Faces)
        {
            Writer.Write<uint32_t>(face.Index3);
            Writer.Write<uint32_t>(face.Index2);
            Writer.Write<uint32_t>(face.Index1);
        }
        Writer.Write<uint32_t>(submesh.VertexCount());
        for (const auto& vertex : submesh.Verticies)
        {
            // FUTURE Writer.Write<WraithVertex>(vertex);
            Writer.Write<Vector3>(vertex.Position);
            Writer.Write<Vector3>(vertex.Normal);
            for (const auto& uvz : vertex.UVLayers)
            {
                Writer.Write<Vector2>(uvz);
            }
            for (const auto& color : vertex.Color)
            {
                Writer.Write<byte>(color);
            }
        }
    }
    Writer.Write<uint32_t>(Model.MaterialCount());
    for (const auto Material : Model.Materials)
    {
        // make it more advanced
        Writer.WriteNullTerminatedString(Material.MaterialName);
        Writer.WriteNullTerminatedString(Material.DiffuseMapName);
        Writer.WriteNullTerminatedString(Material.NormalMapName);
        Writer.WriteNullTerminatedString(Material.SpecularMapName);
    }
    // Bones / Skeletal stuff should be down here.
}



uint32_t C2M::GetVertexIndex(const std::vector<WraithVertex>& vertices, const WraithVertex& vertex)
{
    auto it = std::find_if(vertices.begin(), vertices.end(), [&](const WraithVertex& v) {
        return v.Position == vertex.Position && v.Normal == vertex.Normal && v.UVLayers == vertex.UVLayers;
        });

    if (it != vertices.end()) {
        return static_cast<uint32_t>(std::distance(vertices.begin(), it));
    }

    return 0;  // Return 0 as the default index if the vertex is not found
}


