// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;
glm::vec3 position;

//uniform parameters
bool cntl_wireframe = false;
bool cntl_baseMeshWf = false;
int modelnum = 1;
uni_params sh_params = { -.35, -.6, .1, 8, 4 };

glm::mat4 getViewMatrix(){
    return ViewMatrix;
}

glm::mat4 getProjectionMatrix(){
    return ProjectionMatrix;
}

glm::vec3 getCameraPosition() {
    return position;
}

void initCamera() {
    position = glm::vec3( 0, 0, 5 );
}

bool getWFstatus(){
    return cntl_wireframe;
}

bool getShowBaseModel(){
    return cntl_baseMeshWf;
}

int getModelNum(){
    return modelnum;
}

uni_params getParam(){
    return sh_params;
}

// Initial position : on +Z
//glm::vec3 position = glm::vec3( 0, 0, 5 );
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.003f;


void computeMatricesFromInputs(){

    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Get mouse position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Reset mouse position for next frame
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Compute new orientation
    horizontalAngle += mouseSpeed * float(1024/2 - xpos );
    verticalAngle   += mouseSpeed * float( 768/2 - ypos );

    // Direction : Spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );
    
    // Right vector
    glm::vec3 right = glm::vec3(
        sin(horizontalAngle - 3.14f/2.0f),
        0,
        cos(horizontalAngle - 3.14f/2.0f)
    );
    
    // Up vector
    glm::vec3 up = glm::cross( right, direction );

    // Move forward
    if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
        position += direction * deltaTime * speed;
    }
    // Move backward
    if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
        position -= direction * deltaTime * speed;
    }
    // Strafe right
    if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
        position += right * deltaTime * speed;
    }
    // Strafe left
    if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
        position -= right * deltaTime * speed;
    }
    // Toggle Wireframe on model
    if (glfwGetKey( window, GLFW_KEY_1 ) == GLFW_PRESS){
        //negative edge for toggle
        if (glfwGetKey( window, GLFW_KEY_1 ) == GLFW_RELEASE){
            if(cntl_wireframe){
                cntl_wireframe = false;
            } else {
                cntl_wireframe = true;
            }
        }
    }
    
    // Toggle Wireframe on model
    if (glfwGetKey( window, GLFW_KEY_2 ) == GLFW_PRESS){
        //negative edge for toggle
        if (glfwGetKey( window, GLFW_KEY_2 ) == GLFW_RELEASE){
            if(cntl_baseMeshWf){
                cntl_baseMeshWf = false;
            } else {
                cntl_baseMeshWf = true;
            }
        }
    }
    
    // increase outer tess angle
    if (glfwGetKey( window, GLFW_KEY_K ) == GLFW_PRESS){
        //negative edge
        if (glfwGetKey( window, GLFW_KEY_K ) == GLFW_RELEASE){
            
            sh_params.tess_thresh_h += .04;
        }
    }
    
    // decrease outer tess angle
    if (glfwGetKey( window, GLFW_KEY_L ) == GLFW_PRESS){
        //negative edge
        if (glfwGetKey( window, GLFW_KEY_L ) == GLFW_RELEASE){
            
            sh_params.tess_thresh_h -= .04;
        }
    }
    
    
    // increase inner tess angle
    if (glfwGetKey( window, GLFW_KEY_P ) == GLFW_PRESS){
        //negative edge
        if (glfwGetKey( window, GLFW_KEY_P ) == GLFW_RELEASE){
            
            sh_params.tess_thresh_l -= .04;
        }
    }
    
    // decrease inner tess angle
    if (glfwGetKey( window, GLFW_KEY_O ) == GLFW_PRESS){
        //negative edge
        if (glfwGetKey( window, GLFW_KEY_O ) == GLFW_RELEASE){
            
            sh_params.tess_thresh_l += .04;
        }
    }
    
    // increase deformation amount
    if (glfwGetKey( window, GLFW_KEY_EQUAL ) == GLFW_PRESS){
        //negative edge
        if (glfwGetKey( window, GLFW_KEY_EQUAL ) == GLFW_RELEASE){
            
            sh_params.def_amt += .01;
        }
    }
    
    // decrease deformation amount
    if (glfwGetKey( window, GLFW_KEY_MINUS ) == GLFW_PRESS){
        //negative edge
        if (glfwGetKey( window, GLFW_KEY_MINUS ) == GLFW_RELEASE){
            
            sh_params.def_amt -= .01;
        }
    }
    
    // increase outer tess amt
    if (glfwGetKey( window, GLFW_KEY_N ) == GLFW_PRESS){
        //negative edge
        if (glfwGetKey( window, GLFW_KEY_N ) == GLFW_RELEASE){
            
            sh_params.tess_cnt_h -= 1;
        }
    }
    
    // decrease outer tess amt
    if (glfwGetKey( window, GLFW_KEY_M ) == GLFW_PRESS){
        //negative edge
        if (glfwGetKey( window, GLFW_KEY_M ) == GLFW_RELEASE){
            
            sh_params.tess_cnt_h += 1;
        }
    }
    
    // increase inner tess amt
    if (glfwGetKey( window, GLFW_KEY_PERIOD ) == GLFW_PRESS){
        //negative edge
        if (glfwGetKey( window, GLFW_KEY_PERIOD ) == GLFW_RELEASE){
            
            sh_params.tess_cnt_l += 1;
        }
    }
    
    // decrease inner tess amt
    if (glfwGetKey( window, GLFW_KEY_COMMA ) == GLFW_PRESS){
        //negative edge
        if (glfwGetKey( window, GLFW_KEY_COMMA ) == GLFW_RELEASE){
            
            sh_params.tess_cnt_l -= 1;
        }
    }
    
    if (glfwGetKey( window, GLFW_KEY_3 ) == GLFW_PRESS){
        //negative edge
        if (glfwGetKey( window, GLFW_KEY_3 ) == GLFW_RELEASE){
            
            modelnum = 1;
        }
    }
    
    if (glfwGetKey( window, GLFW_KEY_4 ) == GLFW_PRESS){
        //negative edge
        if (glfwGetKey( window, GLFW_KEY_4 ) == GLFW_RELEASE){
            
            modelnum = 2;
        }
    }

    float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

    // Projection matrix : 45¡ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    ViewMatrix       = glm::lookAt(
                                position,           // Camera is here
                                position+direction, // and looks here : at the same position, plus "direction"
                                up                  // Head is up (set to 0,-1,0 to look upside-down)
                           );

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}
