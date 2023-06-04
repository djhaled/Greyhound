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
        uint8_t MaxSkinInfluenceBuffer = 0;
        for (auto& Vertex : submesh.Verticies)
        {
            if (Vertex.WeightCount() > MaxSkinInfluenceBuffer)
                MaxSkinInfluenceBuffer = (uint8_t)Vertex.WeightCount();
        }

        uint32_t VertexCountBuffer = submesh.VertexCount();
        Writer.Write<uint32_t>(MaxSkinInfluenceBuffer);
        Writer.Write<uint32_t>(VertexCountBuffer);

        for (const auto& mti : submesh.MaterialIndicies)
        {
            Writer.Write<USHORT>(mti);
        }

        Writer.Write<uint32_t>(submesh.Faces.size());

        for (const auto& face : submesh.Faces)
        {
            //Writer.Write<uint32_t>(face.Index3);
            //Writer.Write<uint32_t>(face.Index2);
            //Writer.Write<uint32_t>(face.Index1);
            if (VertexCountBuffer <= 0xFF)
            {
                // Write as byte
                Writer.Write<uint8_t>((uint8_t)face.Index3);
                Writer.Write<uint8_t>((uint8_t)face.Index2);
                Writer.Write<uint8_t>((uint8_t)face.Index1);
            }
            else if (VertexCountBuffer <= 0xFFFF)
            {
                // Write as short
                Writer.Write<uint16_t>((uint16_t)face.Index3);
                Writer.Write<uint16_t>((uint16_t)face.Index2);
                Writer.Write<uint16_t>((uint16_t)face.Index1);
            }
            else
            {
                // Write as int
                Writer.Write<uint32_t>((uint32_t)face.Index3);
                Writer.Write<uint32_t>((uint32_t)face.Index2);
                Writer.Write<uint32_t>((uint32_t)face.Index1);
            }
        }
        for (size_t vsd = 0; vsd < submesh.Verticies.size(); vsd++)
        {
            auto& vertex = submesh.Verticies[vsd];
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
            // Write weight values
            for (uint32_t i = 0; i < MaxSkinInfluenceBuffer; i++)
            {
                // Write IDs
                Writer.Write<uint32_t>((uint32_t)vsd);
                auto WeightID = (i < vertex.WeightCount()) ? vertex.Weights[i].BoneIndex : 0;
                auto WeightValue = (i < vertex.WeightCount()) ? vertex.Weights[i].Weight : 0.0f;
                Writer.Write<uint32_t>((uint32_t)WeightID);
                // Write ID based on count
                //if (Model.BoneCount() <= 0xFF)
                //    Writer.Write<uint8_t>((uint8_t)WeightID);
                //else if (Model.BoneCount() <= 0xFFFF)
                //    Writer.Write<uint16_t>((uint16_t)WeightID);
                //else
                //    Writer.Write<uint32_t>((uint32_t)WeightID);

                // Write value
                Writer.Write<float>(WeightValue);
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

    // Write the bone info
    Writer.Write<uint32_t>(Model.BoneCount());
    for (auto& Bone : Model.Bones)
    {
        Writer.WriteNullTerminatedString(Bone.TagName);
        // Write bone flags, 0 for now
        Writer.Write<uint8_t>(0x0);

        // Write bone parent
        Writer.Write<int32_t>(Bone.BoneParent);

        // Write global matricies
        Writer.Write<Vector3>(Bone.GlobalPosition);
        Writer.Write<Quaternion>(Bone.GlobalRotation);
        // Write local matricies
        Writer.Write<Vector3>(Bone.LocalPosition);
        Writer.Write<Quaternion>(Bone.LocalRotation);

        // Write scale, if support
        if (SupportsScale)
            Writer.Write<Vector3>(Bone.BoneScale);
    }
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


