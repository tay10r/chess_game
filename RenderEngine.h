#pragma once

class QOpenGLTexture;

class RenderEngine
{
public:
  virtual ~RenderEngine() = default;

  /// @note Called with a current OpenGL context.
  virtual void init() = 0;

  /// @note Called with a current OpenGL context.
  virtual void cleanup() = 0;

  /// @note Called with a current OpenGL context.
  virtual void resize(int w, int h) = 0;

  /// @note Called with a current OpenGL context.
  virtual void render(QOpenGLTexture& displayTexture, int w, int h) = 0;
};
