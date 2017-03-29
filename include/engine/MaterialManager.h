#pragma once

#include <string>
#include <vector>

class Material;

using namespace std;

class MaterialManager
{
	public:
		MaterialManager();
		~MaterialManager();
		Material *find(string name);

	private:
		void loadMaterials();
		vector<Material *> *_materials;
};