#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace caldera {
// shader program wrapper (re: handle)
class Shader {
 public:
   Shader(const std::string &vertex_path, const std::string &fragment_path) {
      GLuint vert = compile(GL_VERTEX_SHADER, readFile(vertex_path));
      GLuint frag = compile(GL_FRAGMENT_SHADER, readFile(fragment_path));

      handle = glCreateProgram();
      glAttachShader(handle, vert);
      glAttachShader(handle, frag);
      glLinkProgram(handle);

      GLint ok = GL_FALSE;
      glGetProgramiv(handle, GL_LINK_STATUS, &ok);
      if (!ok) {
         GLchar infoLog[1024];
         glGetProgramInfoLog(handle, 1024, nullptr, infoLog);
         std::cout << "PROGRAM_LINK_ERROR: " << infoLog << "\n";
         glDeleteProgram(handle);
      }
      glDeleteShader(vert);
      glDeleteShader(frag);
   }
   ~Shader() {
      glDeleteProgram(handle);
   }

   void bind() {
      glUseProgram(handle);
   }
   void unbind() {
      glUseProgram(0);
   }

   // uniforms
   void setMat4(const std::string &name, const glm::mat4 &mat) {
      glUniformMatrix4fv(glGetUniformLocation(handle, name.c_str()), 1, GL_FALSE, &mat[0][0]);
   }
   void setMat3(const std::string &name, const glm::mat3 &mat) {
      glUniformMatrix3fv(glGetUniformLocation(handle, name.c_str()), 1, GL_FALSE, &mat[0][0]);
   }
   void setMat2(const std::string &name, const glm::mat2 &mat) {
      glUniformMatrix2fv(glGetUniformLocation(handle, name.c_str()), 1, GL_FALSE, &mat[0][0]);
   }
   void setVec4(const std::string &name, const glm::vec4 &vec) {
      glUniform4fv(glGetUniformLocation(handle, name.c_str()), 1, &vec[0]);
   }
   void setVec3(const std::string &name, const glm::vec3 &vec) {
      glUniform3fv(glGetUniformLocation(handle, name.c_str()), 1, &vec[0]);
   }
   void setInt(const std::string &name, const int value) {
      glUniform1i(glGetUniformLocation(handle, name.c_str()), value);
   }
   void setFloat(const std::string &name, const float value) {
      glUniform1f(glGetUniformLocation(handle, name.c_str()), value);
   }

 private:
   GLuint handle = 0;  // shader program handle
   GLuint compile(GLenum type, const std::string &source) {
      GLuint shader = glCreateShader(type);
      const char *c = source.c_str();
      glShaderSource(shader, 1, &c, nullptr);
      glCompileShader(shader);
      GLint ok = GL_FALSE;
      glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
      if (!ok) {
         GLchar infoLog[1024];
         glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
         std::cout << "SHADER_COMPILE_ERROR: " << infoLog << "\n";
         glDeleteShader(shader);
         return 0;
      }
      return shader;
   }
   static std::string readFile(const std::string &path) {
      std::ifstream file(path);
      if (!file) {
         std::cout << "error, could not open file: " << path << "\n";
      }
      std::stringstream ss;
      ss << file.rdbuf();
      return ss.str();
   }
};
}  // namespace caldera
