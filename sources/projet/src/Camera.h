
#ifndef CAMERA_H
#define CAMERA_H

#include <Eigen/Geometry>

class Camera
{

protected:
    Eigen::Matrix4f mViewMatrix;
    Eigen::Matrix4f mProjectionMatrix;

private:
    Eigen::Vector3f mPosition;
    Eigen::Vector3f mTarget;
    Eigen::Vector3f mZoom;

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

    /** Set the position of the camera */
    void setPosition(Eigen::Vector3f pos);

    /** Get the position of the camera */
    Eigen::Vector3f getPosition();

    /** Set the target point of the camera */
    void setTarget(Eigen::Vector3f tar);

    /** Get the target point of the camera */
    Eigen::Vector3f getTarget();

    void setZoom(Eigen::Vector3f zoo);

    Eigen::Vector3f getZoom();
protected:
    void updateViewMatrix() const;
    void updateProjectionMatrix() const;

};

#endif // EIGEN_CAMERA_H
