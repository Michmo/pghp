
#ifndef CAMERA_H
#define CAMERA_H

#include <Eigen/Geometry>

class Camera
{
private:
    int posY;
    int posZ;

public:
    Camera();
    virtual ~Camera();

    /** Setup the camera position and orientation based on its \a position, \a a target point, \a the up vector */
    void lookAt(const Eigen::Vector3f& position, const Eigen::Vector3f& target, const Eigen::Vector3f& up);

    /** Setup the perspective projection matrix */
    void setPerspective(float fovY, float aspect, float near, float far);

    /** Returns the affine transformation matrix from the global space to the camera space */
    const Eigen::Matrix4f& viewMatrix(void) const;
    /** Returns the perspective projection matrix */
    const Eigen::Matrix4f& projectionMatrix(void) const;

    int getPosY();
    int getPosZ();
    void setPosY(int y);
    void setPosZ(int z);

protected:
    void updateViewMatrix() const;
    void updateProjectionMatrix() const;

protected:
    Eigen::Matrix4f mViewMatrix;
    Eigen::Matrix4f mProjectionMatrix;
};

#endif // EIGEN_CAMERA_H
