#pragma once

#include <string>
#include <map>

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

class Texture;
class CubeMap;

using namespace std;

//Class that acts as a interface for a OpenGL shader. Also has functions for 
//parsing shader code, compiling and linking.
class Shader
{
	public:
		//************************************
		// Method:    Constructor for the Shader class. Reads in shader code 
		//            from file, compiles and links it.
		// FullName:  Shader::Shader
		// Access:    public 
		// Parameter: string vertex_shader_path - Path to vertex shader source.
		// Parameter: string fragment_shader_path - Path to fragment shader 
		//            source. 
		//************************************
		Shader(string vertex_shader_path, string fragment_shader_path, map<string, string> *uniforms);

		//************************************
		// Method:    Activates the shader program with glUseProgram().
		// FullName:  Shader::Use
		// Access:    public 
		// Returns:   void
		//************************************
		void use();

		map<string, string> *getUniforms();

		bool hasUniform(string name);

		//************************************
		// Method:    Sets 4x4 matrix uniforms in the shader program.
		// FullName:  Shader::SetUniformMat4
		// Access:    public 
		// Returns:   void
		// Parameter: string name - Name of uniform.
		// Parameter: const GLfloat * value - Pointer to the matrix 
		//            (column major).
		//************************************
		void setUniformMat4(string name, glm::mat4 matrix);

		//************************************
		// Method:    Set float uniforms in the shader program.
		// FullName:  Shader::SetUniform1f
		// Access:    public 
		// Returns:   void
		// Parameter: string name - Name of uniform.
		// Parameter: GLfloat f0 - The float value. 
		//************************************
		void setUniform1f(string name, GLfloat f0);

		//************************************
		// Method:    Sets vec3 shader uniforms in the shader program.
		// FullName:  Shader::SetUniform3f
		// Access:    public 
		// Returns:   void
		// Parameter: string name - Name of uniform.
		// Parameter: GLfloat f0 - First value.
		// Parameter: GLfloat f1 - Second value.
		// Parameter: GLfloat f2 - Third value.
		//************************************
		void setUniform3fv(string name, glm::vec3 values);

		//************************************
		// Method:    Sets int uniforms in the shader program.
		// FullName:  Shader::SetUniform1i
		// Access:    public 
		// Returns:   void
		// Parameter: string name - Name of uniform.
		// Parameter: GLint value - Value of the int.
		//************************************
		void setUniform1i(string name, GLint value);

		void bindTexture(Texture *texture, GLenum textureUnit, string name);

		void bindCubeMap(CubeMap *cubeMap, GLenum textureUnit, string name);

	private:
		//************************************
		// Method:    Parses the shader source from a text file and puts it in 
		//            a string.
		// FullName:  Shader::ParseShaderSource
		// Access:    private 
		// Returns:   std::string - Shader source in string
		// Parameter: string path - Path to shader source. 
		//************************************
		string parseShaderSource(string path);

		//************************************
		// Method:    Compiles a shader from source.
		// FullName:  Shader::CompileShader
		// Access:    private 
		// Returns:   GLuint - Compiled shader
		// Parameter: GLenum target - Target for compiler (Vertex/Fragment)
		// Parameter: string shader_source - String of shader source. 
		//************************************
		GLuint compileShader(GLenum target, string shader_source);

		//************************************
		// Method:    Links the shader program from compiled shaders. 
		// FullName:  Shader::CreateShaderProgram
		// Access:    private 
		// Returns:   GLuint - Shader program
		// Parameter: GLuint vertex_shader - Vertex shader.
		// Parameter: GLuint fragment_shader - Fragment shader.
		//************************************
		GLuint createShaderProgram(GLuint vertex_shader,
			GLuint fragment_shader);

		GLuint _program;
		map<string, string> *_uniforms;
};
