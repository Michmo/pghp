
#include <Object.h>

Object::Object()
    : mShader(0), mGeometry(0), mTransformation(Eigen::Matrix4f::Identity())
{}

void Object::attachMesh(const Mesh* mesh)
{
    mGeometry = mesh;
}

void Object::attachShader(const Shader* shader)
{
    mShader = shader;
}

void Object::setTransformation(const Eigen::Matrix4f& mat)
{
    mTransformation = mat;
}

Eigen::Matrix4f Object::getTransformation(){
    return this->mTransformation;
}

void Object::draw()
{
    mGeometry->drawGeometry(mShader->id());
}

void Object::setRotation(float alphaX, float alphaY, float alphaZ){
    this->mTransformation.setIdentity();
    Eigen::Matrix4f rotX = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f rotY = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f rotZ = Eigen::Matrix4f::Identity();
    //Initialisation de rotX
    rotX(1,1) = cos(alphaX); rotX(1,2) = -sin(alphaX);
    rotX(2,1) = sin(alphaX); rotX(2,2) = cos(alphaX);
    //Initialisation de rotY
    rotY(0,0) = cos(alphaY); rotY(0,2) = sin(alphaY);
    rotY(2,0) = -sin(alphaY); rotY(2,2) = cos(alphaY);
    //Initialisation de rotZ
    rotZ(0,0) = cos(alphaZ); rotZ(0,1) = -sin(alphaZ);
    rotZ(1,0) = sin(alphaZ); rotZ(1,1) = cos(alphaZ);
    //Multiplication des matrices
    this->mTransformation = rotX * rotY * rotZ;
}
