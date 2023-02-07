#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Macro for detecting an openGL error.
// Only works if the program is compiled in debug mode.
// 
// Usage:
// GL_CHECK(myfunction); // In case of OpenGL call
// auto v = GL_CHECK(myfunction_withreturn); // In case of function returning values
// GL_CHECK("My checkpoint"); // In case of no function call
// 
// Note calling GL_CHECK(); will result to an error (arg stmt is missing)
inline void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        // Flush a error message in the error exit
        std::cerr << "[ERROR] OpenGL error " << (int)err << " at " << fname << ":" << line << " for " << stmt << "\n";
        std::cerr.flush();
        // Raise exception and terminate the program. 
        // Can be commented out if needed
        abort(); 
    }
}
#ifdef _DEBUG
#define GL_CHECK(stmt) stmt;  \
        CheckOpenGLError(#stmt, __FILE__, __LINE__);
#else
#define GL_CHECK(stmt) stmt
#endif

// Helper object that simplify the shader loading and interactions
// Can be extended if necessary
class ShaderProgram
{
public:
   // ------------------------------------------------------------------------
   // constructor
   ShaderProgram();
   
   // ------------------------------------------------------------------------
   // attach shader from sources 
   // return true if sucessfull
   bool addShaderFromSource(GLenum type, const std::string& path);
   
   // ------------------------------------------------------------------------
   // link the different shaders to make a full program 
   // return true if sucessfull
   bool link();

   // ------------------------------------------------------------------------
   // get program ID to interact directly with the shader program
   inline GLuint programId() const { return m_ID; }

   // ------------------------------------------------------------------------
   // use shader program
   inline void bind() const { 
       if(!m_linked) {
            // Warn user
            std::cerr << "[ERROR] Shader is not properly linked!\n";
            if(m_terminate) {
                abort();
            }
       }
       glUseProgram(m_ID); 
   }
   

    // get id value corresponding to uniform
    // ------------------------------------------------------------------------
    inline int uniformLocation(const std::string name) const { 
        GLint v =  glGetUniformLocation(m_ID, name.c_str()); 
        if(v == -1) {
            std::cerr << "[ERROR] Uniform '" << name << "' is not found.\n";
            if(m_terminate) {
                abort();
            }
        }
        return v;
    }
    // get id value corresponding to attribute
    // ------------------------------------------------------------------------
     inline int attributeLocation(const std::string name) const { 
        GLint v =  glGetAttribLocation(m_ID, name.c_str()); 
        if(v == -1) {
            std::cerr << "[ERROR] Attribute '" << name << "' is not found.\n";
            if(m_terminate) {
                abort();
            }
        }
        return v;
    }

    // utility uniform functions
    // ------------------------------------------------------------------------
    inline void setBool(const std::string& name, bool value) const { 
        int loc = uniformLocation(name);
        if(loc != -1) {
            glUniform1i(loc, (int)value);
        } 
    }
    // ------------------------------------------------------------------------
    inline void setInt(const std::string& name, int value) const { 
        int loc = uniformLocation(name);
        if(loc != -1) {
             glUniform1i(loc, value); 
        }
    }
    // ------------------------------------------------------------------------
    inline void setFloat(const std::string& name, float value) const { 
        int loc = uniformLocation(name);
        if(loc != -1) {
            glUniform1f(loc, value); 
        }  
    }
    // ------------------------------------------------------------------------
    inline void setMat4(const std::string& name, const glm::mat4& mat) const { 
        int loc = uniformLocation(name);
        if(loc != -1) {
            glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
        }  
    }

    // ------------------------------------------------------------------------
    inline void setMat3(const std::string& name, const glm::mat3& mat) const { 
        int loc = uniformLocation(name);
        if(loc != -1) {
            glUniformMatrix3fv(loc, 1, GL_FALSE, &mat[0][0]); 
        }      
    }

    // ------------------------------------------------------------------------
    inline void setVec4(const std::string& name, const glm::vec4& value) const { 
        int loc = uniformLocation(name);
        if(loc != -1) {
            glUniform4fv(loc, 1, &value[0]);
        } 

    }

    // ------------------------------------------------------------------------
    inline void setVec3(const std::string& name, const glm::vec3& value) const { 
        int loc = uniformLocation(name);
        if(loc != -1) {
            glUniform3fv(loc, 1, &value[0]);   
        } 
    }

    // change if we terminate or not the program on error
    // ------------------------------------------------------------------------
    inline void setTerminate(bool v) { m_terminate = v; }

private:
    // Shader program id
    GLuint m_ID;
    // Does the shader is link?
    bool m_linked = false;
    // Do we terminate the program if invalid value is detected?
    bool m_terminate = true;
    // List of the different shaders (can be reused if necessary)
    std::map<std::string, GLuint> m_shaders_ids;
};
#endif