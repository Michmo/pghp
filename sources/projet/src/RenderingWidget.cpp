#include <RenderingWidget.h>
#include <Eigen/Geometry>
#include <iostream>
#include <QKeyEvent>
#include <cstdlib>
#include <ctime>

using namespace Eigen;
using namespace std;



RenderingWidget::RenderingWidget()
    :
      #ifdef __APPLE__
      //QGLWidget(new Core3_2_context(QGLFormat::defaultFormat())),
      #endif
      fieldMesh(0)
    ,avatarMesh(0)
    ,mCam()
    ,move(200,20,0)

{
    mCam.setPosition(Eigen::Vector3f(200,15,0));
    mCam.setTarget(move);
    mCam.setZoom(Eigen::Vector3f(0,20,20));
    scene = Matrix4f::Identity();
    obj = new Object[2];
    mCam.lookAt(mCam.getPosition(), mCam.getTarget(), Vector3f::UnitZ());
    mCam.setPerspective(50*(M_PI)/100,1,0.2,1000000);
    transfoAvatar = Matrix4f::Identity();
}

RenderingWidget::~RenderingWidget()
{
    delete fieldMesh;
    delete avatarMesh;
    delete[] obj;
}

void RenderingWidget::paintGL()
{
    GL_TEST_ERR;

    // configure the rendering target size (viewport)
    glViewport(0, 0, mVpWidth, mVpHeight);

    mCam.lookAt(mCam.getPosition(), mCam.getTarget(), Vector3f::UnitZ());

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    avatarProgram.activate();
    GL_TEST_ERR;

    Eigen::Affine3f view(mCam.viewMatrix());
    glUniformMatrix4fv(avatarProgram.getUniformLocation("matriceTransformationAvatar"),1,false,transfoAvatar.data());
    glUniformMatrix4fv(avatarProgram.getUniformLocation("matriceVueAvatar"),1,false,view.data());
    glUniformMatrix4fv(avatarProgram.getUniformLocation("matriceProjectionAvatar"),1,false,mCam.projectionMatrix().data());
    scene = Matrix4f::Identity();
    glUniformMatrix4fv(avatarProgram.getUniformLocation("matriceSceneAvatar"),1,false,scene.data());
    Matrix3f normale = scene.block(0,0,3,3).inverse().transpose();
    glUniformMatrix3fv(avatarProgram.getUniformLocation("matriceNormaleAvatar"), 1, false, normale.data());
    glUniform3f(avatarProgram.getUniformLocation("lightPos"), 200, 800, 500);
    glUniform3f(avatarProgram.getUniformLocation("lightDir"), 1, 1, 1);
    glUniform3f(avatarProgram.getUniformLocation("lightColor"), 1, 1, 1);
    glUniform1f(avatarProgram.getUniformLocation("shininess"), 50);
    glUniform3f(avatarProgram.getUniformLocation("move"), move.x(), move.y(), move.z());
    glUniform3f(avatarProgram.getUniformLocation("posCam"), mCam.getPosition().x(), mCam.getPosition().y(), mCam.getPosition().z());
    GL_TEST_ERR;

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, cheval);
    glUniform1i(fieldProgram.getUniformLocation("chevalSampler"), 2);
    GL_TEST_ERR;

    obj[1].draw();
    GL_TEST_ERR;

    fieldProgram.activate();
    GL_TEST_ERR;

    glUniformMatrix4fv(fieldProgram.getUniformLocation("matriceVue"),1,false,view.data());
    glUniformMatrix4fv(fieldProgram.getUniformLocation("matriceProjection"),1,false,mCam.projectionMatrix().data());
    scene = Matrix4f::Identity();
    glUniformMatrix4fv(fieldProgram.getUniformLocation("matriceScene"),1,false,scene.data());
    normale = scene.block(0,0,3,3).inverse().transpose();
    glUniformMatrix3fv(fieldProgram.getUniformLocation("matriceNormale"), 1, false, normale.data());
    glUniform3f(fieldProgram.getUniformLocation("lightPos"), 200, 800, 500);
    glUniform3f(fieldProgram.getUniformLocation("lightDir"), 1, 1, 1);
    glUniform3f(fieldProgram.getUniformLocation("lightColor"), 1, 1, 1);
    glUniform1f(fieldProgram.getUniformLocation("shininess"), 10000);
    glUniform3f(fieldProgram.getUniformLocation("posCam"), mCam.getPosition().x(),mCam.getPosition().y(), mCam.getPosition().z());
    GL_TEST_ERR;

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, neige);
    glUniform1i(fieldProgram.getUniformLocation("fieldSampler"), 1);
    GL_TEST_ERR;

    obj[0].draw();
    GL_TEST_ERR;
}

void RenderingWidget::initializeGL()
{
  std::cout << "Using OpenGL version: \"" << glGetString(GL_VERSION) << "\"" << std::endl;

  glClearColor(1,1,1,0);

  glEnable(GL_DEPTH_TEST);

  createScene();
}

void RenderingWidget::resizeGL(int width, int height)
{
  mVpWidth = width;
  mVpHeight = height;
}


void RenderingWidget::createScene()
{
  fieldProgram.loadFromFiles(PGHP_DIR"/shaders/field.vert", PGHP_DIR"/shaders/field.frag");
  avatarProgram.loadFromFiles(PGHP_DIR"/shaders/avatar.vert", PGHP_DIR"/shaders/avatar.frag");

  QImage neigeTex(PGHP_DIR"/data/neige.png");
  glGenTextures(1,&neige);
  glBindTexture(GL_TEXTURE_2D, neige);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, neigeTex.width(), neigeTex.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, neigeTex.bits());
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  GL_TEST_ERR;

  QImage boisTex(PGHP_DIR"/data/wood1.jpg");
  glGenTextures(1,&cheval);
  glBindTexture(GL_TEXTURE_2D, cheval);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, boisTex.width(), boisTex.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, boisTex.bits());
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  GL_TEST_ERR;

  fieldMesh = new Mesh(PGHP_DIR"/data/test.off");
  fieldMesh->Mesh::computeNormals();
  fieldMesh->Mesh::Initialize();
  obj[0].attachMesh(fieldMesh);
  obj[0].attachShader(&fieldProgram);

  avatarMesh = new Mesh(PGHP_DIR"/data/rockling-horse.obj");
  avatarMesh->Mesh::makeUnitary();
  avatarMesh->Mesh::computeNormalsPosition();
  avatarMesh->Mesh::Initialize();
  obj[1].attachMesh(avatarMesh);
  obj[1].attachShader(&avatarProgram);

  move.z() = fieldMesh->findZ(move.x(), move.y());
  mCam.getPosition().z() = fieldMesh->findZ(mCam.getPosition().x(), mCam.getPosition().y());
  mCam.setTarget(move);
}


void RenderingWidget::keyPressEvent(QKeyEvent * e)
{
    float zMove;

    switch(e->key())
    {
    case Qt::Key_1:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        updateGL();
        break;
    case Qt::Key_2:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        updateGL();
        break;
    case Qt::Key_Plus:
        mCam.setZoom(Eigen::Vector3f(mCam.getZoom().x(), mCam.getZoom().y()-1, mCam.getZoom().z()-1));
        updateGL();
        break;
    case Qt::Key_Minus:
        mCam.setZoom(Eigen::Vector3f(mCam.getZoom().x(), mCam.getZoom().y()+1, mCam.getZoom().z()+1));
        updateGL();
        break;
    case Qt::Key_Up:
      zMove =  fieldMesh->findZ(move.x(), move.y() + 0.5);
      //if(zMove <= 0) {
        move.z() = zMove;
        move.y() += 0.5;
      //}
        //transfoAvatar = avatarMesh->orientMesh(move,Eigen::Vector3f(move.x(),move.y(),move.z()),fieldMesh->findNormal(move.x(),move.y()));
        //cout << transfoAvatar <<endl;
        updateGL();
        break;
    case Qt::Key_Down:
      zMove =  fieldMesh->findZ(move.x(), move.y() - 0.5);
      //if(zMove <= 0) {
        move.z() = zMove;
        move.y() -= 0.5;
      //}
        updateGL();
        break;
    case Qt::Key_Left:
      zMove =  fieldMesh->findZ(move.x() - 0.5, move.y());
      //if(zMove <= 0){
        move.z() = zMove;
        move.x() -= 0.5;
      //}
        updateGL();
        break;
    case Qt::Key_Right:
      zMove =  fieldMesh->findZ(move.x() + 0.5, move.y());
      //if(zMove <= 0){
        move.z() = zMove;
        move.x() += 0.5;
      //}
        updateGL();
        break;
    case Qt::Key_Escape:
        exit(1);
        break;
    default:
        break;
    }
    mCam.setTarget(move);
    mCam.setPosition(Eigen::Vector3f(move.x(),
                                     move.y()-mCam.getZoom().y(),
                                     fieldMesh->findZ(mCam.getPosition().x(), mCam.getPosition().z())+mCam.getZoom().z()));
//    position.x() = move.x();
//    position.y() = move.y() - mCam.getPosY();
//    position.z() = fieldMesh->findZ(position.x(), position.y()) + mCam.getPosZ();
}

#include <RenderingWidget.moc>
