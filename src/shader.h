#pragma once

#include <fstream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

// i have personally reused this class in many of my opengl projects
// i dont see an issue if it works perfectly fine

namespace caldera {
// this is a wrapper class for a shader program
class shader {
 public:
   shader(const std::string &vertex_path, const std::string &fragment_path) {
      // build shader program instantly
      GLuint vert = compile(GL_VERTEX_SHADER, read_file(vertex_path));
      GLuint frag = compile(GL_FRAGMENT_SHADER, read_file(fragment_path));

      program = glCreateProgram();
      glAttachShader(program, vert);
      glAttachShader(program, frag);
      glLinkProgram(program);

      GLint ok = GL_FALSE;
      glGetProgramiv(program, GL_LINK_STATUS, &ok);
      if (!ok) {
         GLchar info_log[1024];
         glGetProgramInfoLog(program, 1024, nullptr, info_log);
         SPDLOG_ERROR("SHADER_LINK_ERROR: {}", info_log);
         glDeleteProgram(program);
      }
      SPDLOG_INFO("linked shader program: {}", program);
      glDeleteShader(vert);
      glDeleteShader(frag);
   }
   ~shader() {
      unbind();  // jic
      glDeleteProgram(program);
   }

   void bind() {
      glUseProgram(program);
   }
   void unbind() {
      glUseProgram(0);
   }

   // uniforms
   void setMat4(const std::string &name, const glm::mat4 &mat) {
      glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
   }
   void setMat3(const std::string &name, const glm::mat3 &mat) {
      glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
   }
   void setMat2(const std::string &name, const glm::mat2 &mat) {
      glUniformMatrix2fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
   }
   void setVec4(const std::string &name, const glm::vec4 &vec) {
      glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &vec[0]);
   }
   void setVec3(const std::string &name, const glm::vec3 &vec) {
      glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &vec[0]);
   }
   void setInt(const std::string &name, const int value) {
      glUniform1i(glGetUniformLocation(program, name.c_str()), value);
   }
   void setFloat(const std::string &name, const float value) {
      glUniform1f(glGetUniformLocation(program, name.c_str()), value);
   }

 private:
   GLuint program = 0;  // shader program
   GLuint compile(GLenum type, const std::string &source) {
      GLuint shader = glCreateShader(type);

      const char *c = source.c_str();
      glShaderSource(shader, 1, &c, nullptr);
      glCompileShader(shader);

      GLint ok = GL_FALSE;
      glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
      if (!ok) {
         GLchar info_log[1024];
         glGetShaderInfoLog(shader, 1024, nullptr, info_log);
         SPDLOG_ERROR("SHADER_COMPILE_ERROR: {}", info_log);
         glDeleteShader(shader);
         return -1;  // need to be very careful with this, I believe 0 is a valid handle
      }
      SPDLOG_INFO("compiled shader source: {}", shader);
      return shader;
   }
   static std::string read_file(const std::string &path) {
      std::ifstream file(path);
      if (!file)
         SPDLOG_ERROR("error, could not open file: {}", path);
      std::stringstream ss;
      ss << file.rdbuf();
      return ss.str();
   }
};
}  // namespace caldera
