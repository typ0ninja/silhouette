#ifndef CONTROLS_HPP
#define CONTROLS_HPP

void initCamera();
void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
glm::vec3 getCameraPosition();
bool getWFstatus();
bool getShowBaseModel();


//uniform struct
struct uni_params {
    float tess_thresh_h;
    float tess_thresh_l;
    float def_amt;
    int tess_cnt_h;
    int tess_cnt_l;
};

uni_params getParam();

#endif
