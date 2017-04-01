#pragma once

#include <string>

class ResourceSystem;
class Material;
class Shader;

using namespace std;

class Resources
{
	public:
		static void init(ResourceSystem *resourceSystem);
		static Material *findMaterial(string name);
		static Shader *findShader(string name);

	private:
		static ResourceSystem *_resourceSystem;

};