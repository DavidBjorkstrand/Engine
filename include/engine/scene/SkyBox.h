#pragma once

#include <map>
#include <string>

class Mesh;
class CubeMap;

using namespace std;

class SkyBox
{
	public:
		SkyBox(string path);

		Mesh *getMesh();

	private:
		map<string, string> *_pathMap;
		Mesh *_cube;
};