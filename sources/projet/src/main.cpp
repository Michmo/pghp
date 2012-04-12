#include <clocale>
#include <RenderingWidget.h>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  setlocale(LC_ALL, "C");

  QGLFormat gl_profile(QGL::DoubleBuffer | QGL::Rgba | QGL::AlphaChannel);
  QGLFormat::setDefaultFormat(gl_profile);

  RenderingWidget simple_gl;
  simple_gl.resize(600,500);
  simple_gl.show();
  return app.exec();
}
