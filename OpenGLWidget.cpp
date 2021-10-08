#include "OpenGLWidget.h"

#include "RenderEngine.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>

#include <cassert>

OpenGLWidget::OpenGLWidget(RenderEngine& renderEngine, QWidget* parent)
  : QOpenGLWidget(parent)
  , m_renderEngine(renderEngine)
{}

OpenGLWidget::~OpenGLWidget()
{
  makeCurrent();

  m_renderEngine.cleanup();

  m_textureBlitter.destroy();

  m_displayTexture.destroy();

  doneCurrent();
}

void
OpenGLWidget::initializeGL()
{
  [[maybe_unused]] bool success = m_displayTexture.create();

  assert(success);

  m_displayTexture.bind();

  m_displayTexture.setWrapMode(QOpenGLTexture::ClampToEdge);

  m_displayTexture.setMinMagFilters(QOpenGLTexture::Nearest,
                                    QOpenGLTexture::Linear);

  m_displayTexture.release();

  m_textureBlitter.create();
}

void
OpenGLWidget::paintGL()
{
  m_renderEngine.render(m_displayTexture, width(), height());

  m_textureBlitter.bind();

  const QRect targetRect(QPoint(0, 0), size());

  const QMatrix4x4 transform = QOpenGLTextureBlitter::targetTransform(
    targetRect, QRect(QPoint(0, 0), size()));

  m_textureBlitter.blit(m_displayTexture.textureId(),
                        transform,
                        QOpenGLTextureBlitter::OriginBottomLeft);

  m_textureBlitter.release();
}

void
OpenGLWidget::resizeGL(int w, int h)
{
  QOpenGLFunctions* functions = context()->functions();

  functions->glViewport(0, 0, w, h);

  resizeDisplayTexture(w, h);

  m_renderEngine.resize(w, h);
}

void
OpenGLWidget::resizeDisplayTexture(int w, int h)
{
  QOpenGLFunctions* functions = context()->functions();

  m_displayTexture.bind();

  functions->glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, nullptr);

  m_displayTexture.release();
}
