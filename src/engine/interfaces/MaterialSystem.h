#pragma once

#include <string>

class MaterialManager;
class Material;

using namespace std;

class MaterialSystem
{
	public:
		static void init(MaterialManager *materialManager);
		static Material *find(string name);

	private:
		static MaterialManager *_materialManager;

};