#pragma once

#include <string>
#include <map>

class Material;
class Shader;

using namespace std;

class ResourceSystem
{
	public:
		ResourceSystem();
		~ResourceSystem();
		Material *findMaterial(string name);
		Shader *findShader(string name);

	private:
		void loadMaterials();
		void loadShaders();
		map<string, Material*> *_materials;
		map<string, Shader*> *_shaders;
};