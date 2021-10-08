#include "CxxRenderEngine.h"

#include "ChessBvhModel.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>

#include <random>
#include <vector>

namespace {

class CxxRenderEngineImpl final : public CxxRenderEngine
{
public:
  using Vec3f = bvh::Vector3<float>;

  using Ray = bvh::Ray<float>;

  using Hit = Mesh::Hit;

  void init() override {}

  void cleanup() override {}

  void render(QOpenGLTexture& displayTexture, int w, int h) override
  {
    std::vector<float> rgb(w * h * 3);

    const Vec3f rayOrigin(0, 1, 2);

    const float aspect = float(w) / h;

#pragma omp parallel for

    for (int i = 0; i < (w * h); i++) {

      const int x = i % w;
      const int y = i / w;

      const float u = (x + 0.5f) / w;
      const float v = (y + 0.5f) / h;

      const float dx = ((u * 2) - 1) * aspect;
      const float dy = ((v * 2) - 1);
      const float dz = -1;

      Ray ray(rayOrigin, bvh::normalize(Vec3f(dx, dy, dz)));

      Vec3f color = trace(ray, m_rngs[i], 0);

      rgb[(i * 3) + 0] = color[0];
      rgb[(i * 3) + 1] = color[1];
      rgb[(i * 3) + 2] = color[2];
    }

    displayTexture.bind();

    QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

    const int level = 0;
    const int xOffset = 0;
    const int yOffset = 0;

    functions->glTexSubImage2D(
      GL_TEXTURE_2D, level, xOffset, yOffset, w, h, GL_RGB, GL_FLOAT, &rgb[0]);

    displayTexture.release();
  }

  template<typename Rng>
  Vec3f trace(const Ray& ray, Rng& rng, int depth)
  {
    if (depth >= m_maxDepth)
      return Vec3f(0, 0, 0);

    const Mesh& pawn = m_bvhModel.pawn();

    const Mesh& board = m_bvhModel.board();

    const std::optional<Hit> pawnHit = pawn.findClosestHit(ray);
    if (pawnHit)
      return shadeHit(pawn, ray, *pawnHit, rng, depth);

    const std::optional<Hit> boardHit = board.findClosestHit(ray);

    if (boardHit)
      return shadeBoardHit(board, ray, *boardHit, rng, depth);

    return onMiss(ray);
  }

  template<typename Rng>
  Vec3f shadeHit(const Mesh& mesh,
                 const Ray& ray,
                 const Hit& hit,
                 Rng& rng,
                 int depth)
  {
    const float distance = hit.distance() - m_shadowBias;

    const Vec3f hitPos = ray.origin + (ray.direction * distance);

    const Vec3f normal = -mesh.triangles[hit.primitive_index].n;

    Vec3f secondRayDir = sampleHemisphere(normal, rng);

    Ray secondRay(hitPos, secondRayDir);

    return Vec3f(0.8, 0.8, 0.8) * trace(secondRay, rng, depth + 1);
  }

  template<typename Rng>
  Vec3f shadeBoardHit(const Mesh& mesh,
                      const Ray& ray,
                      const Hit& hit,
                      Rng& rng,
                      int depth)
  {
    const float distance = hit.distance() - m_shadowBias;

    const Vec3f hitPos = ray.origin + (ray.direction * distance);

    const float u = int((hitPos[0] + 1.0) * 4.0) % 2;
    const float v = int((hitPos[2] + 1.0) * 4.0) % 2;

    Vec3f secondRayDir = sampleHemisphere(Vec3f(0, 1, 0), rng);

    Ray secondRay(hitPos, secondRayDir);

    return Vec3f(u, v, 0.8) * trace(secondRay, rng, depth + 1);
  }

  Vec3f onMiss(const Ray& ray) const
  {
    const Vec3f lo(1, 1, 1);

    const Vec3f hi(0.5, 0.7, 1.0);

    const Vec3f up(0, 1, 0);

    const float alpha = (bvh::dot(ray.direction, up) + 1.0f) * 0.5f;

    return lerp(lo, hi, alpha);
  }

  void resize(int w, int h) override
  {
    std::mt19937 seedRng(w * h);

    m_rngs.clear();

    for (int i = 0; i < (w * h); i++)
      m_rngs.emplace_back(seedRng());
  }

  static Vec3f lerp(const Vec3f& a, const Vec3f& b, float alpha) noexcept
  {
    return a + ((b - a) * alpha);
  }

  template<typename Rng>
  static Vec3f sampleHemisphere(const Vec3f& n, Rng& rng)
  {
    std::uniform_real_distribution<float> dist(0, 1);

    for (int i = 0; i < 64; i++) {

      Vec3f v(dist(rng), dist(rng), dist(rng));

      if ((bvh::dot(v, v) <= 1) && (bvh::dot(v, n) >= 0))
        return bvh::normalize(v);
    }

    return n;
  }

private:
  ChessBvhModel m_bvhModel;

  float m_shadowBias = 0.00001;

  int m_maxDepth = 3;

  std::vector<std::minstd_rand> m_rngs;
};

} // namespace

std::unique_ptr<CxxRenderEngine>
CxxRenderEngine::create()
{
  return std::unique_ptr<CxxRenderEngine>(new CxxRenderEngineImpl());
}
