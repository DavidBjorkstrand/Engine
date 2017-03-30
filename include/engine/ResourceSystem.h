#pragma once

#include <string>
#include <vector>

class Material;

using namespace std;

class ResourceSystem
{
	public:
		ResourceSystem();
		~ResourceSystem();
		Material *findMaterial(string name);

	private:
		void loadMaterials();
		vector<Material *> *_materials;
};