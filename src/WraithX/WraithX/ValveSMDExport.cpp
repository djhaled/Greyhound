#include "stdafx.h"

// The class we are implementing
#include "ValveSMDExport.h"

// We need the textwriter, hashing, and filesystems classes
#include "TextWriter.h"
#include "FileSystems.h"

void ExportVertexInfo(TextWriter& Writer, const WraithVertex& Vertex)
{
    // Output the base info
    Vector3 NormNormal = Vertex.Normal.GetNormalized();

    // Export the SMD settings
    Writer.WriteFmt("0 %f %f %f %f %f %f %f %f %d ", Vertex.Position.X, Vertex.Position.Y, Vertex.Position.Z, NormNormal.X, NormNormal.Y, NormNormal.Z, Vertex.UVLayers[0].U, (1 - Vertex.UVLayers[0].V), (uint32_t)Vertex.Weights.size());
    // Iterate and output
    for (auto& Weight : Vertex.Weights)
    {
        // Output
        Writer.WriteFmt("%d %f ", Weight.BoneIndex, Weight.Weight);
    }
    // End the line
    Writer.NewLine();
}

void ValveSMD::ExportSMD(const WraithModel& Model, const std::string& FileName)
{
    // Create a new writer
    auto Writer = TextWriter();
    // Open the model file
    Writer.Create(FileName);
    // Set buffer
    Writer.SetWriteBuffer(0x100000);
    // Write header and begin node
    Writer.WriteLine(
        "version 1\n"
        "// Generated by Wraith - Game extraction tools\n"
        "// Please credit DTZxPorter for using it!\n"
        "nodes"
        );
    // Loop through bones
    uint32_t BoneIndex = 0;
    // Iterate
    for (auto& Bone : Model.Bones)
    {
        // Output bone data
        Writer.WriteLineFmt("%d \"%s\" %d", BoneIndex, Bone.TagName.c_str(), Bone.BoneParent);
        // Advance
        BoneIndex++;
    }
    // End nodes, start skeleton
    Writer.WriteLine(
        "end\n"
        "skeleton\ntime 0"
        );
    // Reset
    BoneIndex = 0;
    // Iterate
    for (auto& Bone : Model.Bones)
    {
        // Local rotation
        Vector3 LocalRotation = Bone.LocalRotation.ToEulerAngles();
        // Output bone positions
        Writer.WriteLineFmt("%d %f %f %f %f %f %f", BoneIndex, Bone.LocalPosition.X, Bone.LocalPosition.Y, Bone.LocalPosition.Z, VectorMath::DegreesToRadians(LocalRotation.X), VectorMath::DegreesToRadians(LocalRotation.Y), VectorMath::DegreesToRadians(LocalRotation.Z));
        // Advance
        BoneIndex++;
    }
    // End skeleton
    Writer.WriteLine("end");
    // Loop through submeshes and output
    for (auto& Submesh : Model.Submeshes)
    {
        // Start block
        Writer.WriteLine("triangles");
        // Grab material reference
        const WraithMaterial& Material = (Submesh.MaterialIndicies[0] > -1) ? Model.Materials[Submesh.MaterialIndicies[0]] : WraithMaterial::DefaultMaterial;
        // Loop through faces
        for (auto& Face : Submesh.Faces)
        {
            // Output material name
            Writer.WriteLine(Material.MaterialName);
            // Output face vertex info for the faces (bone root, vertex, normal, uv) (Reverse clockwise order)
            ExportVertexInfo(Writer, Submesh.Verticies[Face.Index3]);
            ExportVertexInfo(Writer, Submesh.Verticies[Face.Index2]);
            ExportVertexInfo(Writer, Submesh.Verticies[Face.Index1]);
        }
        // Close block
        Writer.WriteLine("end");
    }
}