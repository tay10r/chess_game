#include "ChessBvhModel.h"

#include <bvh/sweep_sah_builder.hpp>

#include <QFile>
#include <QMatrix4x4>

#include "deps/tiny_obj_loader.h"

void
Mesh::addTriangle(const Vec3f& a, const Vec3f& b, const Vec3f& c)
{
  triangles.emplace_back(a, b, c);
}

void
Mesh::commitBvh()
{
  auto [bboxes, centers] =
    bvh::compute_bounding_boxes_and_centers(triangles.data(), triangles.size());

  auto global_bbox =
    bvh::compute_bounding_boxes_union(bboxes.get(), triangles.size());

  bvh::SweepSahBuilder<Bvh> builder(bvh);

  builder.build(global_bbox, bboxes.get(), centers.get(), triangles.size());

  std::vector<Triangle> permutedTriangles(triangles.size());

  for (size_t i = 0; i < triangles.size(); i++)
    permutedTriangles[i] = triangles[bvh.primitive_indices[i]];

  triangles = std::move(permutedTriangles);
}

ChessBvhModel::ChessBvhModel()
{
  createBoard();

  QMatrix4x4 modelTransform;

  modelTransform.scale(0.25);

  m_pawn = loadMesh(":/pawn.obj", modelTransform);
}

void
ChessBvhModel::createBoard()
{
  const Vec3f p00(-1, 0, -1);
  const Vec3f p01(-1, 0, 1);
  const Vec3f p10(1, 0, -1);
  const Vec3f p11(1, 0, 1);

  m_board.addTriangle(p00, p10, p11);
  m_board.addTriangle(p11, p01, p00);
  m_board.commitBvh();
}

Mesh
ChessBvhModel::loadMesh(const char* objPath, const QMatrix4x4& modelTransform)
{
  Mesh mesh;

  QFile file(objPath);

  if (!file.open(QIODevice::ReadOnly))
    return mesh;

  const std::string data = file.readAll().toStdString();

  tinyobj::ObjReader reader;

  if (!reader.ParseFromString(data, ""))
    return mesh;

  const auto& attrib = reader.GetAttrib();

  auto applyTransform = [modelTransform](const Vec3f& in) -> Vec3f {
    QVector4D tmp = modelTransform * QVector4D(in[0], in[1], in[2], 1.0);

    return Vec3f(tmp[0], tmp[1], tmp[2]);
  };

  for (const auto& shape : reader.GetShapes()) {

    const auto& shapeMesh = shape.mesh;

    for (size_t i = 0; i < shapeMesh.indices.size(); i += 3) {

      const int a = shapeMesh.indices[i + 0].vertex_index * 3;
      const int b = shapeMesh.indices[i + 1].vertex_index * 3;
      const int c = shapeMesh.indices[i + 2].vertex_index * 3;

      const float* aVert = &attrib.vertices[a];
      const float* bVert = &attrib.vertices[b];
      const float* cVert = &attrib.vertices[c];

      const Vec3f aPos = applyTransform(Vec3f(aVert[0], aVert[1], aVert[2]));
      const Vec3f bPos = applyTransform(Vec3f(bVert[0], bVert[1], bVert[2]));
      const Vec3f cPos = applyTransform(Vec3f(cVert[0], cVert[1], cVert[2]));

      mesh.addTriangle(aPos, bPos, cPos);
    }
  }

  mesh.commitBvh();

  return mesh;
}
