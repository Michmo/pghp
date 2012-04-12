
#ifndef RENDERINGWIDGET_H
#define RENDERINGWIDGET_H

#include "OpenGL.h"
#include <QGLWidget>
#include <QApplication>

#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "Object.h"


class RenderingWidget : public QGLWidget
{
    Q_OBJECT

    Mesh* fieldMesh;
    Mesh* avatarMesh;

    Shader fieldProgram;
    Shader avatarProgram;

    Object* obj;

    // current width and height of the OpenGL windows
    int mVpWidth, mVpHeight;

    Camera mCam;

    //Potision de l'avatar
    Eigen::Vector3f move;
    //Matrice de rotation de l'avatar
    Eigen::Matrix4f transfoAvatar;
    //Matrice de la scène
    Eigen::Matrix4f scene;


    GLuint neige;
    GLuint cheval;

protected:

    /** This method is automatically called by Qt once the GL context has been created.
      * It is called only once per execution */
    virtual void initializeGL();

    /** This method is automatically called by Qt everytime the opengl windows is resized.
      * \param width the new width of the windows (in pixels)
      * \param height the new height of the windows (in pixels)
      *
      * This function must never be called directly. To redraw the windows, emit the updateGL() signal:
      \code
      emit updateGL();
      \endcode
      */
    virtual void resizeGL(int width, int height);

    /** This method is automatically called by Qt everytime the opengl windows has to be refreshed. */
    virtual void paintGL();

    /** This method is automatically called by Qt everytime a key is pressed */
    void keyPressEvent(QKeyEvent * e);

    /** Internal function to setup the 3D scene */
    virtual void createScene();

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    
    // default constructor
    RenderingWidget();
    ~RenderingWidget();
};

#endif // RENDERINGWIDGET_H

