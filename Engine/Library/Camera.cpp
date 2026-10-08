#include "Camera.h"

namespace gll
{
    Camera* camera = nullptr;
    Camera* activeCamera()
    {
        return camera;
    }
    void setActiveCamera(Camera* p_camera)
    {
        camera = p_camera;
    }

    void Camera::createCamera(glm::vec3 position, float yaw, float pitch)
    {
        Position = position;
        WorldUp = glm::vec3(0.0f, 0.0f, -1.0f);
        Yaw = yaw;
        Pitch = pitch;
        MovementSpeed = SPEED;
        MouseSensitivity = SENSITIVITY;
        Zoom = ZOOM;
        updateCameraVectors();
    }

    glm::mat4 Camera::GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }
    glm::mat4 Camera::GetGunMatrix()
    {
        return glm::mat4(
            glm::vec4(-Front, 0.0f),
            glm::vec4(Up, 0.0f),
            glm::vec4(Right, 0.0f),
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
        );
    }

    void Camera::ProcessMouseMovement(float xposIn, float yposIn, GLboolean constrainPitch)
    {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; 
        lastX = xpos;
        lastY = ypos;

        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }
        updateCameraVectors();
    }

    void Camera::updateCameraVectors()
    {
        glm::vec3 front;
        front.x = (cos(glm::radians(Yaw)) * cos(glm::radians(Pitch)));
        front.y = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.z = sin(glm::radians(Pitch));
        Front = glm::normalize(front);

        Ahead.x = (cos(glm::radians(Yaw)) * cos(glm::radians(0.0f)));
        Ahead.y = sin(glm::radians(Yaw)) * cos(glm::radians(0.0f));
        Ahead.z = sin(glm::radians(0.0f));
        Ahead = glm::normalize(Ahead);

        Right = glm::normalize(glm::cross(Front, WorldUp));
        RightAhead = glm::normalize(glm::cross(Ahead, WorldUp));

        Up = glm::normalize(glm::cross(Right, Front));
    }
}
