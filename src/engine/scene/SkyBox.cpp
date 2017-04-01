#include "engine/scene/SkyBox.h"

#include "engine/scene/entity/component/Mesh.h"
#include "engine/renderer/Material.h"
#include "engine/renderer/CubeMap.h"

#include <map>
#include <string>

#include <GL/glew.h>

SkyBox::SkyBox(string path)
{
	Material *material = new Material();
	material->setShader("skybox");

	CubeMap *cubeMap = new CubeMap(path);
	material->setCubeMap("skybox", cubeMap);

	_cube = Mesh::createCube();
	_cube->setMaterial(material);
	_cube->setDepthFunc(GL_LEQUAL);
}

Mesh *SkyBox::getMesh()
{
	return _cube;
}
