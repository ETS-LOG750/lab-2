// ---------------------------------------------------
/* Code based on Learn openGL camera class
 *
 * https://github.com/JoeyDeVries/LearnOpenGL/blob/master/src/1.getting_started/7.4.camera_class/camera_class.cpp
 * Copyright (C) Joey de Vries -
 * licensed under the terms of the CC BY-NC 4.0 license as published by Creative Commons
 *
 * https://creativecommons.org/licenses/by-nc/4.0/legalcode
 *
 */

#include "Camera.h"
#include <glm/gtx/rotate_vector.hpp>

Camera::Camera(int width, int height,
    const glm::vec3& position,
    const glm::vec3& at) :
    m_position(position),
    m_direction(at - position),
    m_image_ratio(float(width) / height)
{
    m_direction = glm::normalize(m_direction);
    computeAngles();
    updateProjectionMatrix();
}

void Camera::keybordEvents(GLFWwindow* w, const float delta_time) {
    // amout of moving the camera
    const float delta = 3.f * delta_time;
    // compute the perpendicular vector to view direction and up
    const glm::vec3 right = glm::cross(m_direction, m_up);

    // Keyboard displacements
    bool update_position = false;
    if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS) {
        m_position += delta * m_direction;
        update_position = true;
    }
    if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS) {
        m_position -= delta * m_direction;
        update_position = true;
    }
    if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS) {
        m_position -= delta * right;
        update_position = true;
    }
    if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS) {
        m_position += delta * right;
        update_position = true;
    }

    if (update_position) {
        updateProjectionMatrix();
    }
}

void Camera::mouseEvents(const glm::vec2& mousePos, bool clicked) {
    // Compute offset displacement of mouse position
    glm::vec2 offset = mousePos - m_last_mouse_pos;
    m_last_mouse_pos = mousePos;

    // Apply this offset only if we already capture 
    // on click event (from the previous frame)
    // and we continue to press the mouse button
    if (clicked && m_mouse_was_clicked) {
        offset *= 0.2;
        yaw += offset.x;
        pitch -= offset.y;

        const glm::vec3 Vaxis(0.0f, 1.0f, 0.0f);

        // Rotate the view vector by yaw around the vertical axis
        glm::vec3 View(1.0f, 0.0f, 0.0f);
        View = glm::rotate(View, glm::radians(-yaw), Vaxis);
        View = glm::normalize(View);

        // Rotate the view vector by the pitch around the horizontal axis
        glm::vec3 Haxis = glm::cross(Vaxis, View);
        Haxis = glm::normalize(Haxis);
        View = glm::rotate(View, glm::radians(-pitch), Haxis);
        View = glm::normalize(View);

        m_direction = View;
        m_direction = glm::normalize(m_direction);

        m_up = glm::cross(m_direction, Haxis);
        m_up = glm::normalize(m_up);

        updateProjectionMatrix();
    }
    m_mouse_was_clicked = clicked;
}

void Camera::viewportEvents(int width, int height) {
    // Update the matrix
    m_image_ratio = float(width) / height;
    updateProjectionMatrix();
}

void Camera::computeAngles() {
    // Horizontal direction
    glm::vec3 h_dir = glm::vec3(m_direction.x, 0.0, -m_direction.z);
    h_dir = glm::normalize(h_dir);
    // Compute yaw
    yaw = glm::degrees(asin(std::abs(h_dir.z)));
    if (h_dir.z >= 0.0) {
        if (h_dir.x >= 0.0) {
            yaw = 360 - yaw;
        }
        else {
            yaw = 180 + yaw;
        }
    }
    else {
        if (h_dir.x >= 0.0) {
            // Nothing
        }
        else {
            yaw = 180 - yaw;
        }
    }
    // Compute pitch
    pitch = glm::degrees(asin(m_direction.y));
}

void Camera::updateProjectionMatrix() {
    if (m_nearFarFixed) {
        m_proj_matrix = glm::perspective(m_fov, m_image_ratio, 0.1f, 100.0f);
    }
    else {
        m_proj_matrix = glm::perspective(m_fov, m_image_ratio, zNear(), zFar());
    }
}

void Camera::showEntireScene() {
    float yview = m_scene_radius / sin(m_fov / 2);
    float xview = m_scene_radius / (atan(tan(m_fov / 2.0) * m_image_ratio));
    float distance = std::max(xview, yview);

    m_position = m_scene_center - distance * m_direction;
    updateProjectionMatrix();
}