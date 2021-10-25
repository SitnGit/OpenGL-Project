#include <Camera.hpp>
#include <iostream>
Camera::Camera(glm::vec3 position, glm::vec3 up, glm::vec3 camFocusVector,float yaw, float pitch)
    : Front(glm::vec3(-5.0f, 0.0f, 0.0001f)), MovementSpeed(SPEED),
      MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
        
  Position = position;
  WorldUp = up;
  Yaw = yaw;
  Pitch = pitch;
  Up = WorldUp;
  agol=0.0f;
  agol2 = 0.0f;
  Position = glm::vec3(4.0f,3.0f,4.0f);
  axis = glm::vec3(0.0f,0.0f,1.0f);
  lightD = glm::vec3(0.0f, 20.0f, 0.0f);
  vel = glm::vec3(0.0f,0.0f,1.0f);
  flag = false;

  camFocusVector = glm::normalize(Position - Front);
  updateCameraVectors();
}
// Constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY,
               float upZ, float yaw, float pitch)
    : Front(glm::vec3(-5.0f, 0.0f, 0.0001f)),camFocusVector(glm::vec3(0.0f, 0.0f, 0.0f)), MovementSpeed(SPEED),
      MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
        
  Position = glm::vec3(posX, posY, posZ);
  WorldUp = glm::vec3(upX, upY, upZ);
  Yaw = yaw;
  Pitch = pitch;
  camFocusVector = glm::normalize(Position - Front);

  axis = glm::vec3(0.0f,0.0f,1.0f);
  lightD = glm::vec3(50.0f, 20.0f, 50.0f);

  updateCameraVectors();
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix() {

  //camFocusVector = glm::normalize(Position - Front);
  //updateCameraVectors();

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
  //forw = glm::normalize(WorldUp)+glm::normalize(forw);
  forw = glm::vec3(forw.x,0.0f,forw.z);
  forw = glm::normalize(forw);
  glm::vec3 Right = glm::cross(glm::normalize(forw),glm::normalize(WorldUp));
  //std::cout<<alphaX<<" "<<alphaZ<<std::endl;
  /*
  if (direction == FORWARD)
    Front += Front * velocity * p;
  if (direction == BACKWARD)
    Front -= Front * velocity * p;
  if (direction == LEFT)
    Front -= Right * velocity * p;
  if (direction == RIGHT)
    Front += Right * velocity * p;
  */
 
  if (direction == FORWARD){
    //Front.x = Front.x + 1*(  velocity * p);
    //Front.x = Front.x + (forw.x) * (velocity*alphaX);
    //Front.z = Front.z + (forw.z) * (velocity*alphaZ);
    Front += forw * velocity;
    vel = glm::normalize(glm::vec3(cent-Front));
    axis = glm::normalize(glm::cross(WorldUp,vel));
    dis = distance(cent,Front);
    //agol += (2.0f*3.1415) * velocity;
    agol -= 1.0f;
    flag = true;
    //lightD.x += 1*(  velocity * p);
  }
  if (direction == BACKWARD){
    //Front.x = Front.x - 1*(  velocity * p);
    Front -= forw * velocity;
    vel = glm::normalize(glm::vec3(cent-Front));
    axis = glm::normalize(glm::cross(WorldUp,vel));
    dis = distance(cent,Front);
    //agol += (2.0f*3.1415) * velocity;
    agol -= 1.0f;
    flag = true;
    //lightD.x -= 1*(  velocity * p);
  }
  if (direction == LEFT){
    //Front.z = Front.z - 1*(  velocity * p); 
    Front -= Right * velocity;
    vel = glm::normalize(glm::vec3(cent-Front));
    axis = glm::normalize(glm::cross(WorldUp,vel));
    dis = distance(cent,Front);
    //agol += (2.0f*3.1415) * velocity;
    agol2 -= 1.0f;
    flag = false;
    //lightD.z -= 1*(  velocity * p);
  }
  if (direction == RIGHT){
    //Front.z = Front.z + 1*(  velocity * p);
    Front += Right * velocity;
    vel = glm::normalize(glm::vec3(cent-Front));
    axis = glm::normalize(glm::cross(WorldUp,vel));
    dis = distance(cent,Front);
    //agol += (2.0f*3.1415) * velocity;
    agol2 -= 1.0f;
    flag = false;
    //lightD.z += 1*(  velocity * p);
  }
    updateCameraVectors();
}

// Processes input received from a mouse input system. Expects the offset
// value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset,
                                  GLboolean constrainPitch) {
  xoffset *= MouseSensitivity;
  yoffset *= MouseSensitivity;

  Yaw += xoffset;
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
  float horizDistance = calculateHorizontalDistance();
  float vertDistance = calculateVerticalDistance();
  calculateCameraPosition(horizDistance,vertDistance);
  Yaw = 180 - getRotY() + angleAroundPlayer;
  //std::cout<<Position.x<<" "<<Position.z<<" "<<std::abs(Position.x)+std::abs(Position.z)+std::abs(Position.y)<<std::endl;
  glm::vec3 direction = glm::normalize(Position-Front);

  
  Right = glm::normalize(glm::cross(
       direction, WorldUp)); // Normalize the vectors, because their length gets
                        // closer to 0 the more you look up or down which
                        // results in slower movement.
  Up = glm::normalize(glm::cross(Right, direction));
  /*
  glm::mat4 rotation_matrixX = glm::mat4(1.0f);
  rotation_matrixX = glm::rotate(rotation_matrixX, Yaw, Up);

	// rotation 2: based on the mouse vertical axis
  glm::mat4 rotation_matrixY = glm::mat4(1.0f);
	rotation_matrixY = glm::rotate(rotation_matrixY, Pitch, Right);

	// translate back to the origin, rotate and translate back to the pivot location
	glm::mat4 transformation = glm::mat4(1.0f);
  transformation = glm::translate(transformation, -Front)  * rotation_matrixX * rotation_matrixY * glm::translate(transformation, Front);
  */
  /*
  Front = glm::vec3(transformation * glm::vec4(Front, 1));
	Position = glm::vec3(transformation * glm::vec4(Position, 1));
  
  //camFocusVector = glm::normalize(Position-Front);
  */
  // Also re-calculate the Right and Up vector
  //camFocusVector += Front;
  //camFocusVector = glm::vec3(transformation * glm::vec4(camFocusVector, 1));
  //Position = camFocusVector;
  
 /*
  Right = glm::normalize(glm::cross(
      Front, WorldUp)); // Normalize the vectors, because their length gets
                        // closer to 0 the more you look up or down which
                        // results in slower movement.
  Up = glm::normalize(glm::cross(Right, Front));
  */
}
