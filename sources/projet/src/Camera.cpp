#include "Camera.h"
#include <iostream>

using namespace Eigen;

Camera::Camera()
{
    mViewMatrix.setIdentity();
    mProjectionMatrix.setIdentity();
}

Camera::~Camera()
{
}

void Camera::lookAt(const Eigen::Vector3f& position, const Eigen::Vector3f& target, const Eigen::Vector3f& up)
{
  Matrix3f R;
  Eigen::Vector3f vz = (position - target).normalized();
  Eigen::Vector3f vx = (up.cross(vz)).normalized();
  Eigen::Vector3f vy = (vz.cross(vx)).normalized();
  R.col(2)=vz;
  R.col(0)=vx;
  R.col(1)=vy;
  mViewMatrix.block<3,3>(0,0)=R.transpose();
  mViewMatrix.block<3,1>(0,3)=-(R.transpose()*position);
}

void Camera::setPerspective(float fovY, float aspect, float near, float far)
{

  float f = 1/tan(fovY/2);
  mProjectionMatrix (0,0)= 1/(aspect*(tan(fovY/2)));
  mProjectionMatrix (1,1)= f;
  mProjectionMatrix (2,2)= -(near+far)/(far-near);
  mProjectionMatrix (3,3)= 0;
  mProjectionMatrix (2,3)= -(2*near*far)/(far-near);
  mProjectionMatrix (3,2)= -1;
}

const Matrix4f& Camera::viewMatrix() const
{
  return mViewMatrix;
}

const Matrix4f& Camera::projectionMatrix() const
{
  return mProjectionMatrix;
}

Eigen::Vector3f
Camera::getPosition(){
    return this->mPosition;
}

void
Camera::setPosition(Eigen::Vector3f pos){
    this->mPosition = pos;
}

Eigen::Vector3f
Camera::getTarget(){
    return this->mTarget;
}

void
Camera::setTarget(Eigen::Vector3f tar){
    this->mTarget = tar;
}

Eigen::Vector3f
Camera::getZoom(){
    return this->mZoom;
}

void
Camera::setZoom(Eigen::Vector3f zoo){
    this->mZoom = zoo;
}
