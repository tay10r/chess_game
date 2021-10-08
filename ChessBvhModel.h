#pragma once

#include <bvh/bvh.hpp>
#include <bvh/primitive_intersectors.hpp>
#include <bvh/ray.hpp>
#include <bvh/single_ray_traverser.hpp>
#include <bvh/triangle.hpp>

#include <vector>

class QMatrix4x4;

struct Mesh final
{
  using Vec3f = bvh::Vector3<float>;

  using Triangle = bvh::Triangle<float>;

  using Bvh = bvh::Bvh<float>;

  using Ray = bvh::Ray<float>;

  using Intersector = bvh::ClosestPrimitiveIntersector<Bvh, Triangle, true>;

  using Traverser = bvh::SingleRayTraverser<Bvh>;

  using Hit = typename Intersector::Result;

  std::vector<Triangle> triangles;

  Bvh bvh;

  void addTriangle(const Vec3f& a, const Vec3f& b, const Vec3f& c);

  void commitBvh();

  std::optional<Hit> findClosestHit(const Ray& ray) const noexcept
  {
    Intersector intersector(bvh, triangles.data());

    Traverser traverser(bvh);

    return traverser.traverse(ray, intersector);
  }
};

class ChessBvhModel final
{
public:
  using Vec3f = bvh::Vector3<float>;

  using Bvh = bvh::Bvh<float>;

  using Triangle = bvh::Triangle<float>;

  ChessBvhModel();

  const Mesh& board() const noexcept { return m_board; }

  const Mesh& pawn() const noexcept { return m_pawn; }

private:
  static Mesh loadMesh(const char* path, const QMatrix4x4& modelTransform);

  void createBoard();

private:
  Mesh m_board;
  Mesh m_pawn;
  Mesh m_rook;
  Mesh m_bishop;
  Mesh m_kight;
  Mesh m_queen;
  Mesh m_king;
};
