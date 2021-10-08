#pragma once

#include <QOpenGLTexture>
#include <QOpenGLTextureBlitter>
#include <QOpenGLWidget>

class RenderEngine;

class OpenGLWidget final : public QOpenGLWidget
{
public:
  OpenGLWidget(RenderEngine&, QWidget* parent);

  ~OpenGLWidget();

protected:
  void initializeGL() override;

  void paintGL() override;

  void resizeGL(int w, int h) override;

  void resizeDisplayTexture(int w, int h);

private:
  RenderEngine& m_renderEngine;

  QOpenGLTexture m_displayTexture{ QOpenGLTexture::Target2D };

  QOpenGLTextureBlitter m_textureBlitter;
};
