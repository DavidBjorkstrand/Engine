#pragma once

#include <iostream>
#include <map>

#include <glm/gtc/type_ptr.hpp>
#include <GL\glew.h>

class Texture;
class CubeMap;
class Shader;

using namespace std;

class Material
{
	public: 
		Material();

		~Material();

		void setName(string name);

		void setShader(string shaderName);

		void setVec3(string name, glm::vec3 vec3);

		void setFloat(string name, float f);

		void setTexture(string name, Texture *texture);

		void setCubeMap(string name, CubeMap *cubeMap);

		void setBlend(GLboolean blend);
		void setBlendSrc(GLenum blendSrc);
		void setBlendDst(GLenum blendDst);

		GLboolean getBlend();
		GLenum getBlendSrc();
		GLenum getBlendDst();

		/*
		* Gets the name of the material
		*/
		string getName();

		Shader *getShader();

		glm::vec3 getVec3(string name);

		float getFloat(string name);

		Texture *getTexture(string name);

		CubeMap *getCubeMap(string name);

	private:
		string _name;
		string _shaderName;
		Shader *_shader;

		map<string, glm::vec3> *_vec3;
		map<string, float> *_float;
		map<string, Texture *> *_texture;
		map<string, CubeMap *> *_cubeMap;

		GLboolean _blend;
		GLenum _blendSrc;
		GLenum _blendDst;

};