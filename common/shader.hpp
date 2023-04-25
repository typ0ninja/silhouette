#ifndef SHADER_HPP
#define SHADER_HPP

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path, const char * TCS_path, const char * TES_path);

std::string readShader(const char * shader_path);

void compileShader(const char * shader_path, std::string ShaderCode, GLuint shaderID, GLint Result, int * InfoLogLength);

#endif
