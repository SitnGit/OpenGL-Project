#include <Camera.hpp>
#include <iostream>
Camera::Camera(glm::vec3 position, glm::vec3 up, float pitch)
    : Front(glm::vec3(-5.0f, 0.0f, 0.0001f)), MovementSpeed(SPEED),
      MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
        
  Position = position;
  WorldUp = up;
  Pitch = pitch;
  Up = WorldUp;
  agol=0.0f;
  agol2 = 0.0f;
  Position = glm::vec3(4.0f,3.0f,4.0f);
  axis = glm::vec3(0.0f,0.0f,1.0f);
  vel = glm::vec3(0.0f,0.0f,1.0f);
  flag = false;

  updateCameraVectors();
}
// Constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY,
               float upZ, float pitch)
    : Front(glm::vec3(-5.0f, 0.0f, 0.0001f)), MovementSpeed(SPEED),
      MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
        
  Position = glm::vec3(posX, posY, posZ);
  WorldUp = glm::vec3(upX, upY, upZ);
  Pitch = pitch;

  axis = glm::vec3(0.0f,0.0f,1.0f);

  updateCameraVectors();
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix() {

  return glm::lookAt(Position, Front, Up);
}

void Camera::LimitOnMovement()
{
    if (Front.x >= 25)
    {
        Front.x = 25;
    }

    if (Front.x <= -25)
    {
        Front.x = -25;
    }

    if (Front.z >= 25)
    {
        Front.z = 25;
    }

    if (Front.z <= -25)
    {
        Front.z = -25;
    }

}
// Processes input received from any keyboard-like input system. Accepts input
// parameter in the form of camera defined ENUM (to abstract it from windowing
// systems)
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
  float velocity = MovementSpeed * deltaTime;
  float p = 1;
  float dis = 0.0f;
  glm::vec3 cent = Front;
  glm::vec3 forw = (Front - Position);
  forw = glm::vec3(forw.x,0.0f,forw.z);
  forw = glm::normalize(forw);
  glm::vec3 Right = glm::cross(glm::normalize(forw),glm::normalize(WorldUp));
  
 
  if (direction == FORWARD){
    Front += forw * velocity;
    vel = glm::normalize(glm::vec3(cent-Front));
    axis = glm::normalize(glm::cross(WorldUp,vel));
    dis = distance(cent,Front);
    agol -= 1.0f;
    flag = true;
  }
  if (direction == BACKWARD){
    Front -= forw * velocity;
    vel = glm::normalize(glm::vec3(cent-Front));
    axis = glm::normalize(glm::cross(WorldUp,vel));
    dis = distance(cent,Front);
    agol -= 1.0f;
    flag = true;
  }
  if (direction == LEFT){
    Front -= Right * velocity;
    vel = glm::normalize(glm::vec3(cent-Front));
    axis = glm::normalize(glm::cross(WorldUp,vel));
    dis = distance(cent,Front);
    agol2 -= 1.0f;
    flag = false;
  }
  if (direction == RIGHT){
    Front += Right * velocity;
    vel = glm::normalize(glm::vec3(cent-Front));
    axis = glm::normalize(glm::cross(WorldUp,vel));
    dis = distance(cent,Front);
    agol2 -= 1.0f;
    flag = false;
  }
    updateCameraVectors();
}

// Processes input received from a mouse input system. Expects the offset
// value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset,
                                  GLboolean constrainPitch) {
  xoffset *= MouseSensitivity;
  yoffset *= MouseSensitivity;

  Pitch += yoffset;
  angleAroundPlayer -= (xoffset * 0.3f);
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
float Camera::calculateHorizontalDistance()
{
  return distanceFromPlayer * cos(glm::radians(Pitch));
}
float Camera::calculateVerticalDistance()
{
  return distanceFromPlayer * sin(glm::radians(Pitch));
}
void Camera::calculateCameraPosition(float horizDistance, float vertDistance){
  Position.y = Front.y + vertDistance;
  float theta = getRotY() + angleAroundPlayer;
  float offsetX = horizDistance * sin(glm::radians(theta));
  float offsetZ = horizDistance * cos(glm::radians(theta));
  Position.x = Front.x + offsetX;
  Position.z = Front.z + offsetZ;
}
float Camera::getRotY()
{
  glm::vec3 Z(0.0f,0.0f,1.0f);
  return acos(glm::dot(glm::normalize(vel),glm::normalize(Z)));
}
// Processes input received from a mouse scroll-wheel event. Only requires
// input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset) {
  if (Zoom >= 1.0f && Zoom <= 45.0f){
  //Zoom = Zoom;
    Zoom -= yoffset;
    distanceFromPlayer -= Zoom;
  }
  if (Zoom <= 1.0f)
  //Zoom = Zoom;
    Zoom = 1.0f;
  if (Zoom >= 45.0f)
    Zoom = 45.0f;
    //Zoom = Zoom;
}

// Calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors() {
  // Calculate the new Front vector
  float horizDistance = calculateHorizontalDistance();
  float vertDistance = calculateVerticalDistance();
  calculateCameraPosition(horizDistance,vertDistance);

  glm::vec3 direction = glm::normalize(Position-Front);

  
  Right = glm::normalize(glm::cross(
       direction, WorldUp)); // Normalize the vectors, because their length gets
                        // closer to 0 the more you look up or down which
                        // results in slower movement.
  Up = glm::normalize(glm::cross(Right, direction));
  
}
