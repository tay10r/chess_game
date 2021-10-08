#include <QApplication>

#include <QMainWindow>

#include "CxxRenderEngine.h"
#include "OpenGLWidget.h"

int
main(int argc, char** argv)
{
  QApplication app(argc, argv);

  QMainWindow mainWindow;

  mainWindow.resize(1280, 720);

  mainWindow.show();

  std::unique_ptr<CxxRenderEngine> cxxRenderEngine = CxxRenderEngine::create();

  OpenGLWidget openGLWidget(*cxxRenderEngine, &mainWindow);

  mainWindow.setCentralWidget(&openGLWidget);

  return app.exec();
}
