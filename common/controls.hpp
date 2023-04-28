#ifndef CONTROLS_HPP
#define CONTROLS_HPP

void initCamera();
void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
glm::vec3 getCameraPosition();
bool getWFstatus();
bool getShowBaseModel();

#endif
