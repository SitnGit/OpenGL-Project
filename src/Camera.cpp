#include <Camera.hpp>
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 up, glm::vec3 camFocusVector,float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -5.0f)), MovementSpeed(SPEED),
      MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
        
  Position = position;
  WorldUp = up;
  Yaw = yaw;
  Pitch = pitch;
  Up = WorldUp;
  
  camFocusVector = glm::normalize(Position - Front);
  updateCameraVectors();
}
// Constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY,
               float upZ, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -5.0f)),camFocusVector(glm::vec3(0.0f, 0.0f, 0.0f)), MovementSpeed(SPEED),
      MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
        
  Position = glm::vec3(posX, posY, posZ);
  WorldUp = glm::vec3(upX, upY, upZ);
  Yaw = yaw;
  Pitch = pitch;
  camFocusVector = glm::normalize(Position - Front);
  updateCameraVectors();
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix() {

  //camFocusVector = glm::normalize(Position - Front);
  //updateCameraVectors();

  return glm::lookAt(Position, Front, Up);
}

// Processes input received from any keyboard-like input system. Accepts input
// parameter in the form of camera defined ENUM (to abstract it from windowing
// systems)
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
  float velocity = MovementSpeed * deltaTime;
  float p = 1;
  if (direction == FORWARD)
    Position += Front * velocity * p;
  if (direction == BACKWARD)
    Position -= Front * velocity * p;
  if (direction == LEFT)
    Position -= Right * velocity * p;
  if (direction == RIGHT)
    Position += Right * velocity * p;
}

// Processes input received from a mouse input system. Expects the offset
// value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset,
                                  GLboolean constrainPitch) {
  xoffset *= MouseSensitivity;
  yoffset *= MouseSensitivity;

  Yaw += xoffset;
  Pitch += yoffset;

  // Make sure that when pitch is out of bounds, screen doesn't get flipped
  
  if (constrainPitch) {
    if (Pitch > 89.0f)
      Pitch = 89.0f;
    if (Pitch < -89.0f)
      Pitch = -89.0f;
  }

  // Update Front, Right and Up Vectors using the updated Euler angles
  updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires
// input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset) {
  if (Zoom >= 1.0f && Zoom <= 45.0f)
  Zoom = Zoom;
    //Zoom -= yoffset;
  if (Zoom <= 1.0f)
  Zoom = Zoom;
    //Zoom = 1.0f;
  if (Zoom >= 45.0f)
    //Zoom = 45.0f;
    Zoom = Zoom;
}

// Calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors() {
  // Calculate the new Front vector
  /*
  glm::vec3 front;
  //glm::vec3 camFocusVector;
  front.x = static_cast<float>(cos(glm::radians(static_cast<double>(Yaw))) *
                               cos(glm::radians(static_cast<double>(Pitch))));
  front.y = static_cast<float>(sin(glm::radians(static_cast<double>(-Pitch))));
  front.z = static_cast<float>(sin(glm::radians(static_cast<double>(Yaw))) *
                               cos(glm::radians(static_cast<double>(Pitch))));
  Front = glm::normalize(front);
  */

  glm::vec3 direction = glm::normalize(Position-Front);

  Right = glm::normalize(glm::cross(
       direction, Up)); // Normalize the vectors, because their length gets
                        // closer to 0 the more you look up or down which
                        // results in slower movement.
  Up = glm::normalize(glm::cross(Right, direction));
  glm::mat4 rotation_matrixX = glm::mat4(1.0f);
  rotation_matrixX = glm::rotate(rotation_matrixX, Yaw, Up);

	// rotation 2: based on the mouse vertical axis
  glm::mat4 rotation_matrixY = glm::mat4(1.0f);
	rotation_matrixY = glm::rotate(rotation_matrixY, Pitch, Right);

	// translate back to the origin, rotate and translate back to the pivot location
	glm::mat4 transformation = glm::mat4(1.0f);
  transformation = glm::translate(transformation, -Front)  * rotation_matrixX * rotation_matrixY * glm::translate(transformation, Front);
  /*
  Front = glm::vec3(transformation * glm::vec4(Front, 1));
	Position = glm::vec3(transformation * glm::vec4(Position, 1));
  
  //camFocusVector = glm::normalize(Position-Front);
  */
  // Also re-calculate the Right and Up vector
  //camFocusVector += Front;
  camFocusVector = glm::vec3(transformation * glm::vec4(camFocusVector, 1));
  Position = camFocusVector;
  
 /*
  Right = glm::normalize(glm::cross(
      Front, WorldUp)); // Normalize the vectors, because their length gets
                        // closer to 0 the more you look up or down which
                        // results in slower movement.
  Up = glm::normalize(glm::cross(Right, Front));
  */
}
