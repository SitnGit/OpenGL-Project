#include <OpenGLPrj.hpp>

#include <GLFW/glfw3.h>

#include <Camera.hpp>
#include <Shader.hpp>



#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <string>

const std::string program_name = ("Camera");

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// camera
static Camera camera(glm::vec3(4.0f, 3.0f, 4.0f));
static float lastX = SCR_WIDTH / 2.0f;
static float lastY = SCR_HEIGHT / 2.0f;
static bool firstMouse = true; 
glm::vec3 cent = camera.getFront();
//glm::vec3 cent = glm::vec3(0.0f,0.0f,-1.0f);
// timing
static float deltaTime = 0.0f; // time between current frame and last frame
static float lastFrame = 0.0f;
glm::vec3 LightD = glm::vec3(3.0f,10.0f,3.0f);

int texBr = 1;
float agol=0;
std::vector<float> vertices3;

void key_callback (GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        //cent.x = cent.x+0.05f;
        agol = 90.0f;
    }
    if(key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        //cent.x = cent.x-0.05f;
        agol = 270.0f;
    }
    if(key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        //cent.z = cent.z-0.05f;
        agol = 180.0f;
    }
    if(key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        //cent.z = cent.z+0.05f;
        agol = 0.0f;
    }
}

class SceneObject{

  public:

      glm::vec3 center;
      float sizeX;
      float sizeY;
      float sizeZ;
      glm::vec3 firstPoint;
      glm::vec3 secondPoint;
      SceneObject(){};
      SceneObject(glm::vec3 firstP, glm::vec3 secondP)
      {
        firstPoint = firstP;
        secondPoint = secondP;
        calculateCenter();
        calculateSize();
      };
      void calculateCenter()
      {
        center = glm::vec3((firstPoint.x+secondPoint.x)/2.0f,(firstPoint.y+secondPoint.y)/2.0f,(firstPoint.z+secondPoint.z)/2.0f);
      };
      void calculateSize()
      {
        if((firstPoint.x>0.0f && secondPoint.x>0.0f) || (firstPoint.x<0.0f && secondPoint.x<0.0f))
        {
          sizeX = abs(firstPoint.x)-abs(secondPoint.x);
          sizeX = abs(sizeX);
        }
        else
        {
          sizeX = abs(firstPoint.x)+abs(secondPoint.x);
        }

        if((firstPoint.y>0.0f && secondPoint.y>0.0f) || (firstPoint.y<0.0f && secondPoint.y<0.0f))
        {
          sizeY = abs(firstPoint.y)-abs(secondPoint.y);
          sizeY = abs(sizeX);
        }
        else
        {
          sizeY = abs(firstPoint.y)+abs(secondPoint.y);
        }

        if((firstPoint.z>0.0f && secondPoint.z>0.0f) || (firstPoint.z<0.0f && secondPoint.z<0.0f))
        {
          sizeZ = abs(firstPoint.z)-abs(secondPoint.z);
          sizeZ = abs(sizeZ);
        }
        else
        {
          sizeZ = abs(firstPoint.z)+abs(secondPoint.z);
        }

        //std::cout<<center.x<<center.y<<center.z<<" "<<sizeX<<" "<<sizeY<<" "<<sizeZ<<std::endl;
      };

};

enum Direction {
	BACK,
	FRONT,
  STARBOARD,
  PORT
}; 

typedef std::tuple<bool, Direction, glm::vec3> Collision; 


Collision CheckCollision(SceneObject objects[]);
Direction VectorDirection(glm::vec3 target);
void GenerateCuboids(glm::vec3 prva, glm::vec3 vtora, int br, SceneObject obj[]);
unsigned int loadCubemap(std::vector<std::string> faces);
bool CoinCollision(glm::vec3 coinLoc);

int main() {
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(
      GLFW_OPENGL_FORWARD_COMPAT,
      GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

  // glfw window creation
  // --------------------
  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
                                        program_name.c_str(), nullptr, nullptr);
  glfwSetKeyCallback(window,key_callback);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  
  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  // build and compile our shader program
  // ------------------------------------
  std::string shader_location("../res/shaders/");

  std::string used_shaders("shader");
  std::string ambient_shaders("ambienz");
  std::string scene_shaders("scenery");
  std::string sky_shaders("skybox");
  std::string coin_shaders("coin");
  std::string screen_shaders("screen");

  Shader ourShader(shader_location + used_shaders + std::string(".vert"),
                   shader_location + used_shaders + std::string(".frag"));
  Shader myShader(shader_location + ambient_shaders + std::string(".vert"),
                   shader_location + ambient_shaders + std::string(".frag"));
  Shader urShader(shader_location + scene_shaders + std::string(".vert"),
                   shader_location + scene_shaders + std::string(".frag"));
  Shader skyShader(shader_location + sky_shaders + std::string(".vert"),
                   shader_location + sky_shaders + std::string(".frag"));
  Shader coinShader(shader_location + coin_shaders + std::string(".vert"),
                   shader_location + coin_shaders + std::string(".frag"));   
  Shader screenShader(shader_location + screen_shaders + std::string(".vert"),
                   shader_location + screen_shaders + std::string(".frag"));               

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float vertices[] = {
        -25.0f, -1.0f, -25.0f, 0.0f, 1.0f,
        -25.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        0.0f, -1.0f, -25.0f,  1.0f, 1.0f,

        -25.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        0.0f, -1.0f, -25.0f,  1.0f, 1.0f,
        0.0f, -1.0f, 0.0f, 1.0f, 0.0f,


         25.0f, -1.0f, -25.0f, 1.0f, 1.0f,
         0.0f, -1.0f, -25.0f,  0.0f, 1.0f,
        0.0f, -1.0f, 0.0f, 0.0f, 0.0f,

        0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        25.0f, -1.0f, -25.0f, 1.0f, 1.0f,
        25.0f, -1.0f, 0.0f, 1.0f, 0.0f,


         25.0f, -1.0f,  25.0f, 1.0f, 0.0f,
        25.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, -1.0f, 25.0f, 0.0f, 0.0f,

        25.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, -1.0f, 25.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f, 1.0f,


        -25.0f, -1.0f,  25.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 25.0f, 1.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 1.0f, 1.0f,

        0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        -25.0f, -1.0f,  25.0f, 0.0f, 0.0f,
        -25.0f, -1.0f,  0.0f, 0.0f, 1.0f,

  };

  std::vector<float> vertices2;
    std::vector<int> indices;
    
    float brStack = 100.0f;
    float brSector = 200.0f;

    float radius = 1.0f;
    float stepSector= 2*glm::pi<float>()/brSector;
    float stepStack = glm::pi<float>()/brStack;
    float angle1, angle2; //angle1 -> stack,, angle2->step

    float perimetar = 2.0f * glm::pi<float>();
    int ind1, ind2;
    double x, y, z, pom;    //pom = radius *cos
    
    //float red=1.0f, green=1.0f, blue = 1.0f;

    for(int i=0; i<=brStack;++i){

        angle1 = glm::pi<float>()/2 - i*stepStack;
        pom = radius*glm::cos(angle1);
        z=radius*glm::sin(angle1);
        //vertices
        for(int j=0; j<= brSector; ++j){

            cent = camera.getFront();
            angle2 = j*stepSector;

            x = pom * cosf (angle2);
            y = pom * sinf (angle2);

            //vertices2.push_back(x+cent.x);
            //vertices2.push_back(y+cent.y);
            //vertices2.push_back(z+cent.z);
            vertices2.push_back(x);
            vertices2.push_back(y);
            vertices2.push_back(z);

            //vertices2.push_back(red);
            //vertices2.push_back(green);
            //vertices2.push_back(blue);
        }
        //blue-=(float)1/brStack;
        //green-=(float)1/brStack;
        //red-=(float)1/brStack;
    }
    //indices
    for(int i=0;i<brStack;++i){
        
        ind1 = i*(brSector + 1);
        ind2= ind1+brSector + 1;

        for(int j=0;j<brSector;++j, ++ind1, ++ind2){
            
            if(i != 0){
                indices.push_back(ind1);
                indices.push_back(ind2);
                indices.push_back(ind1 + 1);
            }
            if(i != (brStack -1)){
                indices.push_back(ind1 + 1);
                indices.push_back(ind2);
                indices.push_back(ind2 + 1);
            }
        }
    }

float skyboxVertices[] = {
    // positions          
    -50.0f,  50.0f, -50.0f,
    -50.0f, -50.0f, -50.0f,
     50.0f, -50.0f, -50.0f,
     50.0f, -50.0f, -50.0f,
     50.0f,  50.0f, -50.0f,
    -50.0f,  50.0f, -50.0f,

    -50.0f, -50.0f,  50.0f,
    -50.0f, -50.0f, -50.0f,
    -50.0f,  50.0f, -50.0f,
    -50.0f,  50.0f, -50.0f,
    -50.0f,  50.0f,  50.0f,
    -50.0f, -50.0f,  50.0f,

     50.0f, -50.0f, -50.0f,
     50.0f, -50.0f,  50.0f,
     50.0f,  50.0f,  50.0f,
     50.0f,  50.0f,  50.0f,
     50.0f,  50.0f, -50.0f,
     50.0f, -50.0f, -50.0f,

    -50.0f, -50.0f,  50.0f,
    -50.0f,  50.0f,  50.0f,
     50.0f,  50.0f,  50.0f,
     50.0f,  50.0f,  50.0f,
     50.0f, -50.0f,  50.0f,
    -50.0f, -50.0f,  50.0f,

    -50.0f,  50.0f, -50.0f,
     50.0f,  50.0f, -50.0f,
     50.0f,  50.0f,  50.0f,
     50.0f,  50.0f,  50.0f,
    -50.0f,  50.0f,  50.0f,
    -50.0f,  50.0f, -50.0f,

    -50.0f, -50.0f, -50.0f,
    -50.0f, -50.0f,  50.0f,
     50.0f, -50.0f, -50.0f,
     50.0f, -50.0f, -50.0f,
    -50.0f, -50.0f,  50.0f,
     50.0f, -50.0f,  50.0f
};

float screenVertices[] = {
  -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
  1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
  1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
  1.0f, 1.0f, 0.0f, 1.0f, 1.0f
};

  std::vector<float> coinVertices;
  float const PI_OVER_4 = glm::quarter_pi<float>();
  float const PI_OVER_3 = PI_OVER_4*4.0f/3.0f;
  float angle = 0.0f;
  float otstap = 0.3f;

  for (auto i=0; i<75; ++i) {
            coinVertices.push_back(glm::cos(angle)/7.0f);
            coinVertices.push_back(glm::sin(angle)/7.0f);
            coinVertices.push_back(0.0f);

            coinVertices.push_back(glm::cos(angle)/3.7f);
            coinVertices.push_back(glm::sin(angle)/3.7f);
            coinVertices.push_back(0.0f);

            angle = angle+PI_OVER_3/12.0f;
            coinVertices.push_back(glm::cos(angle)/3.7f);
            coinVertices.push_back(glm::sin(angle)/3.7f);
            coinVertices.push_back(0.0f);

            coinVertices.push_back(glm::cos(angle)/3.7f);
            coinVertices.push_back(glm::sin(angle)/3.7f);
            coinVertices.push_back(0.1f);

            coinVertices.push_back(glm::cos(angle)/3.7f);
            coinVertices.push_back(glm::sin(angle)/3.7f);
            coinVertices.push_back(0.0f);

            coinVertices.push_back(glm::cos(angle-PI_OVER_3/12.0f)/3.7f);
            coinVertices.push_back(glm::sin(angle-PI_OVER_3/12.0f)/3.7f);
            coinVertices.push_back(0.1f);

            coinVertices.push_back(glm::cos(angle-PI_OVER_3/12.0f)/3.7f);
            coinVertices.push_back(glm::sin(angle-PI_OVER_3/12.0f)/3.7f);
            coinVertices.push_back(0.1f);

            coinVertices.push_back(glm::cos(angle-PI_OVER_3/12.0f)/3.7f);
            coinVertices.push_back(glm::sin(angle-PI_OVER_3/12.0f)/3.7f);
            coinVertices.push_back(0.0f);

            coinVertices.push_back(glm::cos(angle)/3.7f);
            coinVertices.push_back(glm::sin(angle)/3.7f);
            coinVertices.push_back(0.0f);

            coinVertices.push_back(glm::cos(angle)/3.7f);
            coinVertices.push_back(glm::sin(angle)/3.7f);
            coinVertices.push_back(0.1f);

            coinVertices.push_back(glm::cos(angle-PI_OVER_3/12.0f)/3.7f);
            coinVertices.push_back(glm::sin(angle-PI_OVER_3/12.0f)/3.7f);
            coinVertices.push_back(0.1f);

            coinVertices.push_back(glm::cos(angle-PI_OVER_3/12.0f)/7.0f);
            coinVertices.push_back(glm::sin(angle-PI_OVER_3/12.0f)/7.0f);
            coinVertices.push_back(0.1f);

            //vtora piramida

            coinVertices.push_back(glm::cos(angle-PI_OVER_3/12.0f)/7.0f);
            coinVertices.push_back(glm::sin(angle-PI_OVER_3/12.0f)/7.0f);
            coinVertices.push_back(0.0f);

            coinVertices.push_back(glm::cos(angle)/7.0f);
            coinVertices.push_back(glm::sin(angle)/7.0f);
            coinVertices.push_back(0.0f);

            coinVertices.push_back(glm::cos(angle)/3.7f);
            coinVertices.push_back(glm::sin(angle)/3.7f);
            coinVertices.push_back(0.0f);

            coinVertices.push_back(glm::cos(angle-PI_OVER_3/12.0f)/7.0f);
            coinVertices.push_back(glm::sin(angle-PI_OVER_3/12.0f)/7.0f);
            coinVertices.push_back(0.0f);

            coinVertices.push_back(glm::cos(angle)/7.0f);
            coinVertices.push_back(glm::sin(angle)/7.0f);
            coinVertices.push_back(0.0f);

            coinVertices.push_back(glm::cos(angle)/7.0f);
            coinVertices.push_back(glm::sin(angle)/7.0f);
            coinVertices.push_back(0.1f);

            coinVertices.push_back(glm::cos(angle-PI_OVER_3/12.0f)/7.0f);
            coinVertices.push_back(glm::sin(angle-PI_OVER_3/12.0f)/7.0f);
            coinVertices.push_back(0.0f);

            coinVertices.push_back(glm::cos(angle)/7.0f);
            coinVertices.push_back(glm::sin(angle)/7.0f);
            coinVertices.push_back(0.1f);

            coinVertices.push_back(glm::cos(angle-PI_OVER_3/12.0f)/7.0f);
            coinVertices.push_back(glm::sin(angle-PI_OVER_3/12.0f)/7.0f);
            coinVertices.push_back(0.1f);

            coinVertices.push_back(glm::cos(angle)/3.7f);
            coinVertices.push_back(glm::sin(angle)/3.7f);
            coinVertices.push_back(0.1f);

            coinVertices.push_back(glm::cos(angle)/7.0f);
            coinVertices.push_back(glm::sin(angle)/7.0f);
            coinVertices.push_back(0.1f);

            coinVertices.push_back(glm::cos(angle-PI_OVER_3/12.0f)/7.0f);
            coinVertices.push_back(glm::sin(angle-PI_OVER_3/12.0f)/7.0f);
            coinVertices.push_back(0.1f);
    }

  SceneObject *objects = new SceneObject[17];
  //bool poX = true;
  //GenerateCuboids(glm::vec3(-6.0f, -1.0f, -6.0f),glm::vec3(-2.0f, 2.0f, -4.0f),0,objects);
  //GenerateCuboids(glm::vec3(10.0f, -1.0f, 5.0f),glm::vec3(15.0f, 2.0f, 15.0f),1,objects);
  
  GenerateCuboids(glm::vec3(-2.0f, -1.0f, -2.0f),glm::vec3(2.0f, 2.0f, 2.0f),0,objects);
  GenerateCuboids(glm::vec3(-10.0f, -1.0f, -11.0f),glm::vec3(-6.0f, 2.0f, -5.0f),1,objects);
  GenerateCuboids(glm::vec3(-12.0f, -1.0f, 5.0f),glm::vec3(-6.0f, 2.0f, 9.0f),2,objects);
  GenerateCuboids(glm::vec3(6.0f, -1.0f, 4.0f),glm::vec3(10.0f, 2.0f, 10.0f),3,objects);
  GenerateCuboids(glm::vec3(6.0f, -1.0f, -9.0f),glm::vec3(12.0f, 2.0f, -5.0f),4,objects);
  
  GenerateCuboids(glm::vec3(0.0f, -1.0f, -23.0f),glm::vec3(4.0f, 2.0f, -16.0f),5,objects);
  GenerateCuboids(glm::vec3(7.0f, -1.0f, -21.0f),glm::vec3(17.0f, 4.0f, -17.0f),6,objects);
  GenerateCuboids(glm::vec3(15.0f, -1.0f, -14.0f),glm::vec3(19.0f, 3.0f, -4.0f),7,objects);
  //GenerateCuboids(glm::vec3(12.0f, -1.0f, -1.0f),glm::vec3(20.0f, 2.0f, 3.0f),8,objects);
  GenerateCuboids(glm::vec3(19.0f, -1.0f, 14.0f),glm::vec3(23.0f, 3.0f, 25.0f),9,objects);
  GenerateCuboids(glm::vec3(1.0f, -1.0f, 18.0f),glm::vec3(6.0f, 2.0f, 23.0f),10,objects);
  GenerateCuboids(glm::vec3(-2.0f, -1.0f, 11.0f),glm::vec3(2.0f, 1.0f, 14.0f),11,objects);
  GenerateCuboids(glm::vec3(-13.0f, -1.0f, 14.0f),glm::vec3(-6.0f, 2.0f, 21.0f),12,objects);
  GenerateCuboids(glm::vec3(-21.0f, -1.0f, 7.0f),glm::vec3(-18.0f, 7.0f, 17.0f),13,objects);
  GenerateCuboids(glm::vec3(-21.0f, -1.0f, -2.0f),glm::vec3(-17.0f, 3.0f, 2.0f),14,objects);
  GenerateCuboids(glm::vec3(-21.0f, -1.0f, -11.0f),glm::vec3(-17.0f, 2.0f, -7.0f),15,objects);
  GenerateCuboids(glm::vec3(-20.0f, -1.0f, -23.0f),glm::vec3(-7.0f, 5.0f, -15.0f),16,objects);
  
  //std::cout<<sizeof(objects)<<std::endl;

  unsigned int VBO, VBO2, VBO3, VBO4, coinVBO, screenVBO, VAO, VAO2, VAO3, VAO4, coinVAO, screenVAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO2);
  glGenBuffers(1, &VBO2);
  glGenVertexArrays(1, &VAO3);
  glGenBuffers(1, &VBO3);
  glGenVertexArrays(1, &VAO4);
  glGenBuffers(1, &VBO4);
  glGenVertexArrays(1, &coinVAO);
  glGenBuffers(1, &coinVBO);
  glGenVertexArrays(1, &screenVAO);
  glGenBuffers(1, &screenVBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);
  // texture coord attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        reinterpret_cast<void *>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(VAO2);

  glBindBuffer(GL_ARRAY_BUFFER, VBO2);
  glBufferData(GL_ARRAY_BUFFER, vertices2.size()*sizeof(float), &vertices2[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(int), &indices[0], GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(VAO3);

  glBindBuffer(GL_ARRAY_BUFFER, VBO3);
  //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices3), vertices3, GL_STATIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, vertices3.size()*sizeof(float), &vertices3[0], GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        reinterpret_cast<void *>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(VAO4);

  glBindBuffer(GL_ARRAY_BUFFER, VBO4);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(coinVAO);

  glBindBuffer(GL_ARRAY_BUFFER, coinVBO);
  glBufferData(GL_ARRAY_BUFFER, coinVertices.size()*sizeof(float), &coinVertices[0], GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);
  // texture coord attribute

  glBindVertexArray(screenVAO);

  glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
  //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices3), vertices3, GL_STATIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        reinterpret_cast<void *>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                       // reinterpret_cast<void *>(3 * sizeof(float)));
  //glEnableVertexAttribArray(1);
  //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),(void *)(3 * sizeof(float)));
  //glEnableVertexAttribArray(3);
  // load and create a texture
  // -------------------------
  unsigned int texture1, texture2, textureStart, textureWin, textureLose;
  
  // texture 1
  // ---------
  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);
  // set the texture wrapping parameters
  glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
      GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load image, create texture and generate mipmaps
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(
      true); // tell stb_image.h to flip loaded texture's on the y-axis.
  // The FileSystem::getPath(...) is part of the GitHub repository so we can
  // find files on any IDE/platform; replace it with your own image path.
  unsigned char *data = stbi_load("../res/textures/platform2.jpg", &width,
                                  &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);

  // texture 2
  // ---------
  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);
  // set the texture wrapping parameters
  glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
      GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  stbi_set_flip_vertically_on_load(
      true);
  // load image, create texture and generate mipmaps
  data = stbi_load("../res/textures/crate.jpg", &width, &height,
                   &nrChannels, 0);
  if (data) {
    // note that the awesomeface.png has transparency and thus an alpha channel,
    // so make sure to tell OpenGL the data type is of GL_RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);

  
  glGenTextures(1, &textureStart);
  glBindTexture(GL_TEXTURE_2D, textureStart);
  // set the texture wrapping parameters
  glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
      GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  stbi_set_flip_vertically_on_load(
      true);
  // load image, create texture and generate mipmaps
  data = stbi_load("../res/textures/Start.jpg", &width, &height,
                   &nrChannels, 0);
  if (data) {
    // note that the awesomeface.png has transparency and thus an alpha channel,
    // so make sure to tell OpenGL the data type is of GL_RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);
  
  glGenTextures(1, &textureWin);
  glBindTexture(GL_TEXTURE_2D, textureWin);
  // set the texture wrapping parameters
  glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
      GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  stbi_set_flip_vertically_on_load(
      true);
  // load image, create texture and generate mipmaps
  data = stbi_load("../res/textures/Winner.jpg", &width, &height,
                   &nrChannels, 0);
  if (data) {
    // note that the awesomeface.png has transparency and thus an alpha channel,
    // so make sure to tell OpenGL the data type is of GL_RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);
  
  glGenTextures(1, &textureLose);
  glBindTexture(GL_TEXTURE_2D, textureLose);
  // set the texture wrapping parameters
  glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
      GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  stbi_set_flip_vertically_on_load(
      true);
  // load image, create texture and generate mipmaps
  data = stbi_load("../res/textures/Loser.jpg", &width, &height,
                   &nrChannels, 0);
  if (data) {
    // note that the awesomeface.png has transparency and thus an alpha channel,
    // so make sure to tell OpenGL the data type is of GL_RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);
  
  // tell opengl for each sampler to which texture unit it belongs to (only has
  // to be done once)
  // -------------------------------------------------------------------------------------------
  std::vector<std::string> faces = 
{
    "../res/textures/right.jpg",
    "../res/textures/left.jpg",
    "../res/textures/top.jpg",
    "../res/textures/bottom.jpg",
    "../res/textures/front.jpg",
    "../res/textures/back.jpg"
};
unsigned int cubemapTexture = loadCubemap(faces);  
  ourShader.use();
  ourShader.setInt("texture1", 0);
  urShader.use();
  urShader.setInt("texture2", 1);
  skyShader.use();
  skyShader.setInt("skybox", 0);
  screenShader.use();
  screenShader.setInt("textureWin", 2);
  screenShader.setInt("textureLose", 3);
  screenShader.setInt("textureStart", 4);
  //ourShader.setInt("texture1", 0);
  //ourShader.setInt("texture2", 1);
  glEnable(GL_DEPTH_TEST);
  // render loop
  // -----------

  glm::vec3 coinLocs [] = {
    glm::vec3(17.0f, 0.0f, -22.0f),
    glm::vec3(3.0, 0.0f, 0.0f),
    glm::vec3(-15.0, 0.0f, -24.0f),
    glm::vec3(25.0, 0.0f, 25.0f),
    glm::vec3(3.0, 0.0f, 24.0f)
  };
  
  glm::vec3 coinLoc = coinLocs[rand()%5];
  //glm::vec3 coinLoc = coinLocs[1];
  //bool start = true;
  while (!glfwWindowShouldClose(window)) {
    // per-frame time logic
    // --------------------
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // input
    // -----
    processInput(window);

    // render
    // ------
    //std::cout<<coinLoc.x<<" "<<coinLoc.y<<" "<<coinLoc.z<<std::endl;
  
    // bind textures on corresponding texture units
    if(glfwGetTime()<5)
    {
      screenShader.use();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, textureStart);
        glBindVertexArray(screenVAO);
        glm::mat4 projection = glm::mat4(1.0f);
        screenShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = glm::mat4(1.0f);
        screenShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        //float angle = 20.0f * 0;
        //model = glm::rotate (model, glm::radians (angle), glm::vec3 (1.0f, 0.3f, 0.5f));
        screenShader.setMat4("model", model);
        screenShader.setInt("which",2);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    else{
    bool endGameWin = CoinCollision(glm::vec3(coinLoc));
    // activate shader
    if (glfwGetTime() > 20  && endGameWin==false)
    {
      screenShader.use();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, textureLose);
        glBindVertexArray(screenVAO);
        glm::mat4 projection = glm::mat4(1.0f);
        screenShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = glm::mat4(1.0f);
        screenShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        //float angle = 20.0f * 0;
        //model = glm::rotate (model, glm::radians (angle), glm::vec3 (1.0f, 0.3f, 0.5f));
        screenShader.setMat4("model", model);
        screenShader.setInt("which",1);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    else
    {
      if (!endGameWin)
      {

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        //ourShader.setInt("texture1", 0);
        cent = camera.getFront();

        glm::vec3 Axis = camera.getAxis();
        LightD = camera.lightD;

        //agol = dist/perimetar * deltaTime;
        agol = camera.getAgol();
        // pass projection matrix to shader (note that in this case it could change
        // every frame)
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.Zoom), SCR_WIDTH * 1.0f / SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        //float angle = 20.0f * 0;
        //model = glm::rotate (model, glm::radians (angle), glm::vec3 (1.0f, 0.3f, 0.5f));
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, 32);

        glBindVertexArray(VAO2);

        //std::cout<<camera.lightPos.x;
        myShader.use();
        myShader.setVec3("objectColor", 0.0f, 1.0f, 0.0f);
        myShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        //myShader.setVec3("lightPos", LightD);
        myShader.setVec3("viewPos", camera.Position);

        // view/projection transformations
        projection = glm::perspective(
            glm::radians(camera.Zoom), SCR_WIDTH * 1.0f / SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        //view = glm::translate(view, glm::vec3(cent.x, cent.y, cent.z));
        myShader.setMat4("projection", projection);
        myShader.setMat4("view", view);

        // world transformation

        model = glm::mat4(1.0f);
        angle = 20.0f * 0;
        //model = glm::rotate (model, glm::radians (angle), glm::vec3 (1.0f, 0.3f, 0.5f));
        Collision col = CheckCollision(objects);
        if (std::get<0>(col)) // if collision is true
        {
          // collision resolution
          Direction dir = std::get<1>(col);
          glm::vec3 diff_vector = std::get<2>(col);
          if (dir == PORT || dir == STARBOARD) // left-right collision
          {
            // relocate
            float penetration = 1.0f - std::abs(diff_vector.z);
            if (dir == PORT)
            {
              //cent.x += penetration;
              camera.Front.z += penetration;
            } // move ball to right
            else
            {
              //cent.x -= penetration;
              camera.Front.z -= penetration;
            } // move ball to left;
          }
          else // front-back collision
          {
            // relocate
            float penetration = 1.0f - std::abs(diff_vector.x);
            if (dir == FRONT)
            {
              //cent.x -= penetration;
              camera.Front.x -= penetration;
            } // move ball back up
            else
            {
              //cent.x += penetration;
              camera.Front.x += penetration;
            } // move ball back down
          }
        }
        model = glm::translate(model, glm::vec3(cent.x, cent.y, cent.z));
        //std::cout<<col;
        model = glm::rotate(model, glm::radians(agol), Axis);
        myShader.setMat4("model", model);
        myShader.setVec3("lightPos", cent.x, 20.0f, cent.z);

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void *)0);

        glBindVertexArray(VAO3);

        urShader.use();
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        //urShader.setInt("texture2", 0);
        projection = glm::perspective(
            glm::radians(camera.Zoom), SCR_WIDTH * 1.0f / SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        //view = glm::translate(view, glm::vec3(cent.x, cent.y, cent.z));
        urShader.setMat4("projection", projection);
        urShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        urShader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 4048);

        camera.LimitOnMovement();

        //std::cout << glfwGetTime() << std::endl;

        coinShader.use();

        glBindVertexArray(coinVAO);

        projection = glm::perspective(
            glm::radians(camera.Zoom), SCR_WIDTH * 1.0f / SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        //view = glm::translate(view, glm::vec3(cent.x, cent.y, cent.z));
        coinShader.setMat4("projection", projection);
        coinShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(coinLoc));
        model =
            glm::rotate(model, (float)glfwGetTime() * glm::radians(100.0f),
                        glm::vec3(0.0f, 1.0f, 0.0f));
        coinShader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 2048);

        glDepthMask(GL_FALSE);
        skyShader.use();
        // ... set view and projection matrix
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyShader.setMat4("view", view);
        skyShader.setMat4("projection", projection);
        glBindVertexArray(VAO4);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 64);
        glDepthMask(GL_TRUE);
      }
      else
      {
        screenShader.use();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, textureWin);
        glBindVertexArray(screenVAO);
        glm::mat4 projection = glm::mat4(1.0f);
        screenShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = glm::mat4(1.0f);
        screenShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        //float angle = 20.0f * 0;
        //model = glm::rotate (model, glm::radians (angle), glm::vec3 (1.0f, 0.3f, 0.5f));
        screenShader.setMat4("model", model);
        screenShader.setInt("which",0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
      }
    }
    }
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
    // etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO2);
  glDeleteBuffers(1, &VBO2);
  glDeleteBuffers(1, &EBO);

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  delete [] objects;
  glfwTerminate();
  return 0;
}

bool CoinCollision(glm::vec3 coinLoc)
{
  float coinSize = 0.5f;
  glm::vec3 coinCenter(coinLoc);
  glm::vec3 aabb_half_extents(coinSize / 2.0f, coinSize / 2.0f, coinSize/2.0f);
  glm::vec3 difference = cent - coinCenter;
  glm::vec3 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
  glm::vec3 closest = coinCenter + clamped;
  difference = closest - cent;
  if (glm::length(difference) <= 1.0f)
    return true;
  return false;
}

Collision CheckCollision(SceneObject objects[]) // AABB - Circle collision
{
    //std::cout<<sizeof(objects)<<std::endl;
    for(int i=0;i<17;++i)
    {
      glm::vec3 aabb_half_extents(objects[i].sizeX / 2.0f, objects[i].sizeY / 2.0f, objects[i].sizeZ/2.0f);
      glm::vec3 difference = cent - objects[i].center;
      glm::vec3 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
      glm::vec3 closest = objects[i].center + clamped;
      difference = closest - cent;
      //bool result = (glm::length(difference) < 1.0f);
      if (glm::length(difference) <= 1.0f){
        //std::cout<<1<<std::endl;
        return std::make_tuple(true, VectorDirection(difference), difference);
      }
    }
    //std::cout<<0<<std::endl;
    return std::make_tuple(false, FRONT, glm::vec3(0.0f, 0.0f, 0.0f));
} 

Direction VectorDirection(glm::vec3 target)
{
    glm::vec3 compass[] = {
        glm::vec3(-1.0f, 0.0f, 0.0f),	// back
        glm::vec3(1.0f, 0.0f, 0.0f),	// forward
        glm::vec3(0.0f, 0.0f, 1.0f),	// right
        glm::vec3(0.0f, 0.0f, -1.0f)	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
} 

void GenerateCuboids(glm::vec3 prva, glm::vec3 vtora, int br, SceneObject obj[])
{
  obj[br] = SceneObject(prva,vtora);
  glm::vec3 centar = obj[br].center;
  float golX = obj[br].sizeX/2.0f;
  float golY = obj[br].sizeY/2.0f;
  float golZ = obj[br].sizeZ/2.0f;
  
  {
  //strana kon z
  vertices3.push_back(centar.x+golX);
  vertices3.push_back(centar.y-golY);
  vertices3.push_back(centar.z+golZ);
  vertices3.push_back(1.0f);
  vertices3.push_back(0.0f);
  vertices3.push_back(centar.x-golX);
  vertices3.push_back(centar.y-golY);
  vertices3.push_back(centar.z+golZ);
  vertices3.push_back(0.0f);
  vertices3.push_back(0.0f);
  vertices3.push_back(vtora.x);
  vertices3.push_back(vtora.y);
  vertices3.push_back(vtora.z);
  vertices3.push_back(1.0f);
  vertices3.push_back(1.0f);

  vertices3.push_back(centar.x-golX);
  vertices3.push_back(centar.y-golY);
  vertices3.push_back(centar.z+golZ);
  vertices3.push_back(0.0f);
  vertices3.push_back(0.0f);
  vertices3.push_back(vtora.x);
  vertices3.push_back(vtora.y);
  vertices3.push_back(vtora.z);
  vertices3.push_back(1.0f);
  vertices3.push_back(1.0f);
  vertices3.push_back(centar.x-golX);
  vertices3.push_back(centar.y+golY);
  vertices3.push_back(centar.z+golZ);
  vertices3.push_back(0.0f);
  vertices3.push_back(1.0f);

  //strana kon -x
  vertices3.push_back(centar.x-golX);
  vertices3.push_back(centar.y-golY);
  vertices3.push_back(centar.z+golZ);
  vertices3.push_back(1.0f);
  vertices3.push_back(0.0f);
  vertices3.push_back(centar.x-golX);
  vertices3.push_back(centar.y+golY);
  vertices3.push_back(centar.z+golZ);
  vertices3.push_back(1.0f);
  vertices3.push_back(1.0f);
  vertices3.push_back(prva.x);
  vertices3.push_back(prva.y);
  vertices3.push_back(prva.z);
  vertices3.push_back(0.0f);
  vertices3.push_back(0.0f);

  vertices3.push_back(centar.x-golX);
  vertices3.push_back(centar.y+golY);
  vertices3.push_back(centar.z+golZ);
  vertices3.push_back(1.0f);
  vertices3.push_back(1.0f);
  vertices3.push_back(prva.x);
  vertices3.push_back(prva.y);
  vertices3.push_back(prva.z);
  vertices3.push_back(0.0f);
  vertices3.push_back(0.0f);
  vertices3.push_back(centar.x-golX);
  vertices3.push_back(centar.y+golY);
  vertices3.push_back(centar.z-golZ);
  vertices3.push_back(0.0f);
  vertices3.push_back(1.0f);

  //strana kon -z
  vertices3.push_back(prva.x);
  vertices3.push_back(prva.y);
  vertices3.push_back(prva.z);
  vertices3.push_back(1.0f);
  vertices3.push_back(0.0f);
  vertices3.push_back(centar.x-golX);
  vertices3.push_back(centar.y+golY);
  vertices3.push_back(centar.z-golZ);
  vertices3.push_back(1.0f);
  vertices3.push_back(1.0f);
  vertices3.push_back(centar.x+golX);
  vertices3.push_back(centar.y+golY);
  vertices3.push_back(centar.z-golZ);
  vertices3.push_back(0.0f);
  vertices3.push_back(1.0f);

  vertices3.push_back(prva.x);
  vertices3.push_back(prva.y);
  vertices3.push_back(prva.z);
  vertices3.push_back(1.0f);
  vertices3.push_back(0.0f);
  vertices3.push_back(centar.x+golX);
  vertices3.push_back(centar.y+golY);
  vertices3.push_back(centar.z-golZ);
  vertices3.push_back(0.0f);
  vertices3.push_back(1.0f);
  vertices3.push_back(centar.x+golX);
  vertices3.push_back(centar.y-golY);
  vertices3.push_back(centar.z-golZ);
  vertices3.push_back(0.0f);
  vertices3.push_back(0.0f);

  //strana kon x
  vertices3.push_back(vtora.x);
  vertices3.push_back(vtora.y);
  vertices3.push_back(vtora.z);
  vertices3.push_back(0.0f);
  vertices3.push_back(1.0f);
  vertices3.push_back(centar.x+golX);
  vertices3.push_back(centar.y-golY);
  vertices3.push_back(centar.z+golZ);
  vertices3.push_back(0.0f);
  vertices3.push_back(0.0f);
  vertices3.push_back(centar.x+golX);
  vertices3.push_back(centar.y-golY);
  vertices3.push_back(centar.z-golZ);
  vertices3.push_back(1.0f);
  vertices3.push_back(0.0f);

  vertices3.push_back(vtora.x);
  vertices3.push_back(vtora.y);
  vertices3.push_back(vtora.z);
  vertices3.push_back(0.0f);
  vertices3.push_back(1.0f);
  vertices3.push_back(centar.x+golX);
  vertices3.push_back(centar.y-golY);
  vertices3.push_back(centar.z-golZ);
  vertices3.push_back(1.0f);
  vertices3.push_back(0.0f);
  vertices3.push_back(centar.x+golX);
  vertices3.push_back(centar.y+golY);
  vertices3.push_back(centar.z-golZ);
  vertices3.push_back(1.0f);
  vertices3.push_back(1.0f);

  //gorna strana
  vertices3.push_back(vtora.x);
  vertices3.push_back(vtora.y);
  vertices3.push_back(vtora.z);
  vertices3.push_back(1.0f);
  vertices3.push_back(0.0f);
  vertices3.push_back(centar.x-golX);
  vertices3.push_back(centar.y+golY);
  vertices3.push_back(centar.z+golZ);
  vertices3.push_back(0.0f);
  vertices3.push_back(0.0f);
  vertices3.push_back(centar.x-golX);
  vertices3.push_back(centar.y+golY);
  vertices3.push_back(centar.z-golZ);
  vertices3.push_back(0.0f);
  vertices3.push_back(1.0f);

  vertices3.push_back(vtora.x);
  vertices3.push_back(vtora.y);
  vertices3.push_back(vtora.z);
  vertices3.push_back(1.0f);
  vertices3.push_back(0.0f);
  vertices3.push_back(centar.x-golX);
  vertices3.push_back(centar.y+golY);
  vertices3.push_back(centar.z-golZ);
  vertices3.push_back(0.0f);
  vertices3.push_back(1.0f);
  vertices3.push_back(centar.x+golX);
  vertices3.push_back(centar.y+golY);
  vertices3.push_back(centar.z-golZ);
  vertices3.push_back(1.0f);
  vertices3.push_back(1.0f);
  }
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
      stbi_set_flip_vertically_on_load(false);
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}  
// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
  


}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposd, double yposd) {
  float xpos = static_cast<float>(xposd);
  float ypos = static_cast<float>(yposd);
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset =
      lastY - ypos; // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
