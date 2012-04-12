
#ifndef PGHP_OBJECT_H
#define PGHP_OBJECT_H

#include <Mesh.h>
#include <Shader.h>

class Object
{
public:
    Object();
    void attachMesh(const Mesh* mesh);
    void attachShader(const Shader* shader);
    void setTransformation(const Eigen::Matrix4f& mat);
    Eigen::Matrix4f getTransformation();
    void setRotation(float alphaX, float alphaY, float alphaZ);
    void draw();
protected:
    const Shader* mShader;
    const Mesh* mGeometry;
    Eigen::Matrix4f mTransformation;
};

#endif
