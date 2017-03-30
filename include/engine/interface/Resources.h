#pragma once

#include <string>

class ResourceSystem;
class Material;

using namespace std;

class Resources
{
	public:
		static void init(ResourceSystem *resourceSystem);
		static Material *findMaterial(string name);

	private:
		static ResourceSystem *_resourceSystem;

};