#pragma once

#include <map>

class Mesh;
class CubeMap;

using namespace std;

class SkyBox
{
	public:
		SkyBox(map<string, string> *pathMap);

		Mesh *getMesh();

	private:
		map<string, string> *_pathMap;
		Mesh *_cube;
};