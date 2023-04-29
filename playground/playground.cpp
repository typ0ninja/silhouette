// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/tangentspace.hpp>

struct shader_buff{
    //Globals for model shader
    GLuint ModelVAOID;
    GLuint programID;
    GLuint MatrixID;
    GLuint ViewMatrixID;
    GLuint ModelMatrixID;
    GLuint ModelView3x3MatrixID;
    GLuint cameraPosID;
    GLuint DiffuseTexture;
    GLuint NormalTexture;
    GLuint SpecularTexture;
    GLuint DiffuseTextureID;
    GLuint NormalTextureID;
    GLuint SpecularTextureID;
    GLuint LightID;
    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint normalbuffer;
    GLuint tangentbuffer;
    GLuint bitangentbuffer;
    GLuint elementbuffer;
    std::vector<unsigned short> indices;

    //uniform buffer object for params
    GLuint paramStructID;
    GLuint paramStructbuffer;
    uni_params curParam;
};

shader_buff tess_items;
shader_buff reference_items;


//Initialize the shaders for the model
void initModel() {
    glGenVertexArrays(1, &tess_items.ModelVAOID);
    glBindVertexArray(tess_items.ModelVAOID);

    // Create and compile our GLSL program from the shaders
    tess_items.programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader", "TCS.shader", "TES.shader" );

    // Get a handle for our "MVP" uniform
    tess_items.MatrixID = glGetUniformLocation(tess_items.programID, "MVP");
    tess_items.ViewMatrixID = glGetUniformLocation(tess_items.programID, "V");
    tess_items.ModelMatrixID = glGetUniformLocation(tess_items.programID, "M");
    tess_items.ModelView3x3MatrixID = glGetUniformLocation(tess_items.programID, "MV3x3");
    tess_items.cameraPosID = glGetUniformLocation(tess_items.programID, "cameraPos");
    
    //uniform struct stuff
    // Bind the uniform block to a binding point
    GLuint bindingPoint = 0;
    tess_items.paramStructID = glGetUniformBlockIndex(tess_items.programID, "uni_params");
    glUniformBlockBinding(tess_items.programID, tess_items.paramStructID, bindingPoint);

    // Create a buffer object and allocate memory for the uniform block data
    //GLuint ubo;
    glGenBuffers(1, &tess_items.paramStructbuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, tess_items.paramStructbuffer);
    tess_items.curParam = getParam();
    glBufferData(GL_UNIFORM_BUFFER, sizeof(uni_params), &tess_items.curParam, GL_STATIC_DRAW);

    // Bind the buffer object to the binding point
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, tess_items.paramStructbuffer);
    
    // Load the texture
    tess_items.DiffuseTexture = loadDDS("uvmap.DDS");
    tess_items.NormalTexture = loadBMP_custom("wavenormal.bmp");
    tess_items.SpecularTexture = loadDDS("specular.DDS");
    
    
    /*
    // Load the texture
    DiffuseTexture = loadDDS("Barrel_AlbedoTransparency.DDS");
    NormalTexture = loadBMP_custom("Barrel_Normal.bmp");
    SpecularTexture = loadDDS("Barrel_MetallicSmoothness.DDS");
     */
    
    // Get a handle for our "myTextureSampler" uniform
    tess_items.DiffuseTextureID  = glGetUniformLocation(tess_items.programID, "DiffuseTextureSampler");
    tess_items.NormalTextureID  = glGetUniformLocation(tess_items.programID, "NormalTextureSampler");
    tess_items.SpecularTextureID  = glGetUniformLocation(tess_items.programID, "SpecularTextureSampler");

    // Read our .obj file
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    //bool res = loadAssImp("subd_sphere.obj", indices, vertices, uvs, normals);
    bool res = loadOBJ("subd_sphere.obj", vertices, uvs, normals);
    //bool res = loadOBJ("barrel_obj.obj", vertices, uvs, normals);
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    computeTangentBasis(
        vertices, uvs, normals, // input
        tangents, bitangents    // output
    );

    //std::vector<unsigned short> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> indexed_uvs;
    std::vector<glm::vec3> indexed_normals;
    std::vector<glm::vec3> indexed_tangents;
    std::vector<glm::vec3> indexed_bitangents;
    indexVBO_TBN(
        vertices, uvs, normals, tangents, bitangents,
                 tess_items.indices, indexed_vertices, indexed_uvs, indexed_normals, indexed_tangents, indexed_bitangents
    );

    // Load it into a VBO

    glGenBuffers(1, &tess_items.vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, tess_items.vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &tess_items.uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, tess_items.uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &tess_items.normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, tess_items.normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &tess_items.tangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, tess_items.tangentbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_tangents.size() * sizeof(glm::vec3), &indexed_tangents[0], GL_STATIC_DRAW);

    glGenBuffers(1, &tess_items.bitangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, tess_items.bitangentbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_bitangents.size() * sizeof(glm::vec3), &indexed_bitangents[0], GL_STATIC_DRAW);

    // Generate a buffer for the indices as well
    glGenBuffers(1, &tess_items.elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tess_items.elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, tess_items.indices.size() * sizeof(unsigned short), &tess_items.indices[0] , GL_STATIC_DRAW);

    // Get a handle for our "LightPosition" uniform
    tess_items.LightID = glGetUniformLocation(tess_items.programID, "LightPosition_worldspace");
    
    glBindVertexArray(0);
}


//Initialize the shaders for the model
void initRef() {
    glGenVertexArrays(1, &reference_items.ModelVAOID);
    glBindVertexArray(reference_items.ModelVAOID);

    // Create and compile our GLSL program from the shaders
    reference_items.programID = LoadShaders( "wireframe.vertexshader", "wireframe.fragmentshader");

    // Get a handle for our "MVP" uniform
    reference_items.MatrixID = glGetUniformLocation(reference_items.programID, "MVP");
    reference_items.ViewMatrixID = glGetUniformLocation(reference_items.programID, "V");
    reference_items.ModelMatrixID = glGetUniformLocation(reference_items.programID, "M");
    reference_items.ModelView3x3MatrixID = glGetUniformLocation(reference_items.programID, "MV3x3");
    reference_items.cameraPosID = glGetUniformLocation(reference_items.programID, "cameraPos");
    
    //uniform struct stuff
    // Bind the uniform block to a binding point
//    GLuint bindingPoint = 0;
//    reference_items.paramStructID = glGetUniformBlockIndex(reference_items.programID, "uni_params");
//    glUniformBlockBinding(reference_items.programID, reference_items.paramStructID, bindingPoint);

    // Create a buffer object and allocate memory for the uniform block data
    //GLuint ubo;
//    glGenBuffers(1, &reference_items.paramStructbuffer);
//    glBindBuffer(GL_UNIFORM_BUFFER, reference_items.paramStructbuffer);
//    reference_items.curParam = getParam();
//    glBufferData(GL_UNIFORM_BUFFER, sizeof(uni_params), &reference_items.curParam, GL_STATIC_DRAW);
//
//    // Bind the buffer object to the binding point
//    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, reference_items.paramStructbuffer);
    
    // Load the texture
    reference_items.DiffuseTexture = loadDDS("uvmap.DDS");
    reference_items.NormalTexture = loadBMP_custom("wavenormal.bmp");
    reference_items.SpecularTexture = loadDDS("specular.DDS");
    
    
    
    // Load the texture
    //DiffuseTexture = loadDDS("Barrel_AlbedoTransparency.DDS");
    //NormalTexture = loadBMP_custom("Barrel_Normal.bmp");
    //SpecularTexture = loadDDS("Barrel_MetallicSmoothness.DDS");
     
    
    // Get a handle for our "myTextureSampler" uniform
    reference_items.DiffuseTextureID  = glGetUniformLocation(reference_items.programID, "DiffuseTextureSampler");
    reference_items.NormalTextureID  = glGetUniformLocation(reference_items.programID, "NormalTextureSampler");
    reference_items.SpecularTextureID  = glGetUniformLocation(reference_items.programID, "SpecularTextureSampler");

    // Read our .obj file
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    //bool res = loadAssImp("subd_sphere.obj", indices, vertices, uvs, normals);
    bool res = loadOBJ("subd_sphere.obj", vertices, uvs, normals);
    //bool res = loadOBJ("barrel_obj.obj", vertices, uvs, normals);
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    computeTangentBasis(
        vertices, uvs, normals, // input
        tangents, bitangents    // output
    );

    //std::vector<unsigned short> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> indexed_uvs;
    std::vector<glm::vec3> indexed_normals;
    std::vector<glm::vec3> indexed_tangents;
    std::vector<glm::vec3> indexed_bitangents;
    indexVBO_TBN(
        vertices, uvs, normals, tangents, bitangents,
                 reference_items.indices, indexed_vertices, indexed_uvs, indexed_normals, indexed_tangents, indexed_bitangents
    );

    // Load it into a VBO

    glGenBuffers(1, &reference_items.vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, reference_items.vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &reference_items.uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, reference_items.uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &reference_items.normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, reference_items.normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &reference_items.tangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, reference_items.tangentbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_tangents.size() * sizeof(glm::vec3), &indexed_tangents[0], GL_STATIC_DRAW);

    glGenBuffers(1, &reference_items.bitangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, reference_items.bitangentbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_bitangents.size() * sizeof(glm::vec3), &indexed_bitangents[0], GL_STATIC_DRAW);

    // Generate a buffer for the indices as well
    glGenBuffers(1, &reference_items.elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, reference_items.elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, reference_items.indices.size() * sizeof(unsigned short), &reference_items.indices[0] , GL_STATIC_DRAW);

    // Get a handle for our "LightPosition" uniform
    reference_items.LightID = glGetUniformLocation(reference_items.programID, "LightPosition_worldspace");
    
    glBindVertexArray(0);
}


//Use the shaders to draw the model
void drawModel() {
    glBindVertexArray(tess_items.ModelVAOID);

    // Use our shader
    glUseProgram(tess_items.programID);

    // Compute the MVP matrix from keyboard and mouse input
    computeMatricesFromInputs();
    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    glm::mat4 ViewMatrix = getViewMatrix();
    glm::mat4 ModelMatrix = glm::mat4(1.0);
    glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
    glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
    glm::vec3 cameraPos = getCameraPosition();

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(tess_items.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(tess_items.ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(tess_items.ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
    glUniformMatrix4fv(tess_items.ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
    glUniformMatrix3fv(tess_items.ModelView3x3MatrixID, 1, GL_FALSE, &ModelView3x3Matrix[0][0]);
    glUniform3f(tess_items.cameraPosID, cameraPos.x, cameraPos.y, cameraPos.z);

    
    //glUniform3fv(uniformLocation, 1, glm::value_ptr(myVec3));
    

    glm::vec3 lightPos = glm::vec3(4,4,4);
    glUniform3f(tess_items.LightID, lightPos.x, lightPos.y, lightPos.z);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tess_items.DiffuseTexture);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(tess_items.DiffuseTextureID, 0);
    
    // Bind our normal texture in Texture Unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tess_items.NormalTexture);
    // Set our "NormalTextureSampler" sampler to use Texture Unit 1
    glUniform1i(tess_items.NormalTextureID, 1);
    
    // Bind our specular texture in Texture Unit 2
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tess_items.SpecularTexture);
    // Set our "SpecularTextureSampler" sampler to use Texture Unit 2
    glUniform1i(tess_items.SpecularTextureID, 2);
    
    
    //update input vals
    tess_items.curParam = getParam();
    
    // Bind the buffer object to the binding point
    glBindBuffer(GL_UNIFORM_BUFFER, tess_items.paramStructbuffer);
    
    // Map the buffer memory to a pointer that can be written to
    GLvoid* p = glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uni_params), GL_MAP_WRITE_BIT);
    if(p == NULL){
        printf("P is null");
    }
    uni_params* ubo_data = static_cast<uni_params*>(p);
    if(ubo_data == NULL){
        printf("null ubo data");
    }
    ubo_data->tess_thresh_h = tess_items.curParam.tess_thresh_h;
    
    ubo_data->tess_thresh_l = tess_items.curParam.tess_thresh_l;
    ubo_data->def_amt = tess_items.curParam.def_amt;
    ubo_data->tess_cnt_h = tess_items.curParam.tess_cnt_h;
    ubo_data->tess_cnt_l = tess_items.curParam.tess_cnt_l;
    
    glUnmapBuffer(GL_UNIFORM_BUFFER);
     


    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, tess_items.vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, tess_items.uvbuffer);
    glVertexAttribPointer(
        1,                                // attribute
        2,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, tess_items.normalbuffer);
    glVertexAttribPointer(
        2,                                // attribute
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    
    // 4th attribute buffer : tangents
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, tess_items.tangentbuffer);
    glVertexAttribPointer(
        3,                                // attribute
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    // 5th attribute buffer : bitangents
    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, tess_items.bitangentbuffer);
    glVertexAttribPointer(
        4,                                // attribute
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tess_items.elementbuffer);
    
    if(getWFstatus()){
        //turn on wireframe
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Draw the triangles !
    glDrawElements(
        GL_PATCHES,      // mode
        tess_items.indices.size(),    // count
        GL_UNSIGNED_SHORT,   // type
        (void*)0           // element array buffer offset
    );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    
    glBindVertexArray(0);

}


//Use the shaders to draw the model
void drawReference() {
    glBindVertexArray(reference_items.ModelVAOID);

    // Use our shader
    glUseProgram(reference_items.programID);

    // Compute the MVP matrix from keyboard and mouse input
    computeMatricesFromInputs();
    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    glm::mat4 ViewMatrix = getViewMatrix();
    glm::mat4 ModelMatrix = glm::mat4(1.0);
    glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
    glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
    glm::vec3 cameraPos = getCameraPosition();

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(reference_items.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(reference_items.ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(reference_items.ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
    glUniformMatrix4fv(reference_items.ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
    glUniformMatrix3fv(reference_items.ModelView3x3MatrixID, 1, GL_FALSE, &ModelView3x3Matrix[0][0]);
    glUniform3f(reference_items.cameraPosID, cameraPos.x, cameraPos.y, cameraPos.z);

    
    //glUniform3fv(uniformLocation, 1, glm::value_ptr(myVec3));
    

    glm::vec3 lightPos = glm::vec3(4,4,4);
    glUniform3f(reference_items.LightID, lightPos.x, lightPos.y, lightPos.z);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, reference_items.DiffuseTexture);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(reference_items.DiffuseTextureID, 0);
    
    // Bind our normal texture in Texture Unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, reference_items.NormalTexture);
    // Set our "NormalTextureSampler" sampler to use Texture Unit 1
    glUniform1i(reference_items.NormalTextureID, 1);
    
    // Bind our specular texture in Texture Unit 2
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, reference_items.SpecularTexture);
    // Set our "SpecularTextureSampler" sampler to use Texture Unit 2
    glUniform1i(reference_items.SpecularTextureID, 2);
    
    
    //update input vals
    //reference_items.curParam = getParam();
    
    // Bind the buffer object to the binding point
    //glBindBuffer(GL_UNIFORM_BUFFER, reference_items.paramStructbuffer);
    
    // Map the buffer memory to a pointer that can be written to
//    GLvoid* p = glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uni_params), GL_MAP_WRITE_BIT);
//    if(p == NULL){
//        printf("P is null");
//    }
//    uni_params* ubo_data = static_cast<uni_params*>(p);
//    if(ubo_data == NULL){
//        printf("null ubo data");
//    }
//    ubo_data->tess_thresh_h = reference_items.curParam.tess_thresh_h;
//
//    ubo_data->tess_thresh_l = reference_items.curParam.tess_thresh_l;
//    ubo_data->def_amt = reference_items.curParam.def_amt;
//    ubo_data->tess_cnt_h = reference_items.curParam.tess_cnt_h;
//    ubo_data->tess_cnt_l = reference_items.curParam.tess_cnt_l;
//
//    glUnmapBuffer(GL_UNIFORM_BUFFER);
     


    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, reference_items.vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, reference_items.uvbuffer);
    glVertexAttribPointer(
        1,                                // attribute
        2,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, reference_items.normalbuffer);
    glVertexAttribPointer(
        2,                                // attribute
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    
    // 4th attribute buffer : tangents
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, reference_items.tangentbuffer);
    glVertexAttribPointer(
        3,                                // attribute
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    // 5th attribute buffer : bitangents
    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, reference_items.bitangentbuffer);
    glVertexAttribPointer(
        4,                                // attribute
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, reference_items.elementbuffer);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
//    if(getWFstatus()){
//        //turn on wireframe
//        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    } else {
//        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//    }

    // Draw the triangles !
    glDrawElements(
        GL_TRIANGLES,      // mode
        reference_items.indices.size(),    // count
        GL_UNSIGNED_SHORT,   // type
        (void*)0           // element array buffer offset
    );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    
    glBindVertexArray(0);

}

int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "Silhouette Tool", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    initCamera();
    initModel();
    initRef();

    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    do{

        // Measure speed
        double currentTime = glfwGetTime();
        nbFrames++;
        if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
            // printf and reset
            printf("%f ms/frame\n", 1000.0/double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //draw the stuff
        drawModel();
        drawReference();
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO and shader
    glDeleteBuffers(1, &reference_items.vertexbuffer);
    glDeleteBuffers(1, &reference_items.uvbuffer);
    glDeleteBuffers(1, &reference_items.normalbuffer);
    glDeleteBuffers(1, &reference_items.tangentbuffer);
    glDeleteBuffers(1, &reference_items.bitangentbuffer);
    glDeleteBuffers(1, &reference_items.elementbuffer);
    glDeleteProgram(reference_items.programID);
    glDeleteTextures(1, &reference_items.DiffuseTexture);
    glDeleteTextures(1, &reference_items.NormalTexture);
    glDeleteTextures(1, &reference_items.SpecularTexture);
    glDeleteVertexArrays(1, &reference_items.ModelVAOID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

