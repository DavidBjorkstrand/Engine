#include "engine/renderer/Shader.h"

#include "engine/renderer/Texture.h"

#include <string>
#include <fstream>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

Shader::Shader(string vertex_shader_path, string fragment_shader_path)
{
	string vertex_shader_source = parseShaderSource(vertex_shader_path);
	string fragment_shader_source = parseShaderSource(fragment_shader_path);

	GLuint vertex_shader = compileShader(GL_VERTEX_SHADER, vertex_shader_source);
	GLuint fragment_shader = compileShader(GL_FRAGMENT_SHADER,
		fragment_shader_source);

	_program = createShaderProgram(vertex_shader, fragment_shader);

}

void Shader::use()
{
	glUseProgram(_program);
}

void Shader::setUniformMat4(string name, glm::mat4 matrix)
{
	GLint loc = glGetUniformLocation(_program, (const GLchar *)name.c_str());

	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setUniform1f(string name, GLfloat f0)
{
	GLint loc = glGetUniformLocation(_program, (const GLchar *)name.c_str());

	glUniform1f(loc, f0);
}

void Shader::setUniform3fv(string name, glm::vec3 value)
{
	GLint loc = glGetUniformLocation(_program, (const GLchar *)name.c_str());

	glUniform3f(loc, value.x, value.y, value.z);
}

void Shader::setUniform1i(string name, GLint value)
{
	GLint loc = glGetUniformLocation(_program, (const GLchar *)name.c_str());

	glUniform1i(loc, value);
}

void Shader::bindTexture(Texture *texture, GLenum textureUnit, string name)
{
	GLuint textureID = texture->getTextureID();

	glActiveTexture(textureUnit);
	texture->bind();
	setUniform1i(name, (textureUnit - GL_TEXTURE0));
}

string Shader::parseShaderSource(string path)
{
	ifstream file(path);
	string str;
	string output;

	if (file.fail())
	{
		cout << "Couldn't open file" << endl;
		return "";
	}

	while (getline(file, str))
	{
		output += str;
		output.push_back('\n');
	}

	file.close();

	return output;
}

GLuint Shader::compileShader(GLenum target, string shader_source)
{
	const GLchar *src_ptr = shader_source.c_str();
	GLuint shader;
	GLint success;
	GLchar info_log[512];

	shader = glCreateShader(target);
	glShaderSource(shader, 1, &src_ptr, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, info_log);
		if (target == GL_VERTEX_SHADER)
		{
			cout << "ERROR VERTEX SHADER COMPILE FAILED\n" << info_log << endl;
		}
		else if (target == GL_FRAGMENT_SHADER)
		{
			cout << "ERROR FRAGMENT SHADER COMPILE FAILED\n" <<
				info_log << endl;
		}
	}

	return shader;
}

GLuint Shader::createShaderProgram(GLuint vertex_shader, GLuint fragment_shader)
{
	GLuint program = 0;
	GLint success;
	GLchar info_log[512];

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	glGetShaderiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(program, 512, NULL, info_log);
		cout << "ERROR SHADER LINKING FAILED\n" << info_log << endl;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return program;
}