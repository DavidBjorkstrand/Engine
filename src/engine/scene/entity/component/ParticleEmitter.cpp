#include "engine/scene/entity/component/ParticleEmitter.h"

#include "engine/physics/PhysicsSystem.h"
#include "engine/renderer/RenderSystem.h"
#include "engine/renderer/Material.h"
#include "engine/scene/Scene.h"
#include "engine/scene/entity/Entity.h"
#include "engine/scene/entity/Transform.h"
#include "engine/scene/entity/component/Mesh.h"

#include <windows.h>
#include <iostream>
#include <stdlib.h> 
#include <cstdlib>
#include <stdexcept>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

ParticleEmitter::ParticleEmitter()
{
	_inActiveParticles = new vector<Particle>();
	_activeParticles = new vector<Particle>();
	_particleMeshes = new vector<Mesh *>();

	for (int i = 0; i < 5000; ++i)
	{
		Particle particle;

		particle.rigidbody = PhysicsSystem::createRigidbody();

		_inActiveParticles->push_back(particle);

		Mesh *mesh = Mesh::createPlane();
		mesh->setMaterial("particle");
		mesh->getMaterial()->setShader("particle");
		mesh->getMaterial()->setBlend(true);
		mesh->getMaterial()->setBlendSrc(GL_SRC_ALPHA);
		mesh->getMaterial()->setBlendDst(GL_ONE);
		_particleMeshes->push_back(mesh);
	}
}

void ParticleEmitter::accept(Scene *scene)
{
	scene->visit(this);
}

void ParticleEmitter::update()
{
	
	SYSTEMTIME time;
	GetSystemTime(&time);
	long currentTime = (time.wSecond * 1000) + time.wMilliseconds;
	vector<GLuint> toRemove;

	for (GLuint i = 0; i < _activeParticles->size(); ++i)
	{
		Particle particle = _activeParticles->at(i);
		if (currentTime - particle.startTime > 10000)
		{
			toRemove.push_back(i);
		}
		else if (particle.rigidbody->position.y < 0)
		{
			toRemove.push_back(i);
		}
	}

	for (int i = toRemove.size()-1; i >= 0; i--)
	{
		GLuint removeIndex = toRemove.at(i);
		Particle particle = _activeParticles->at(removeIndex);

		_inActiveParticles->push_back(particle);
		_activeParticles->erase(_activeParticles->begin() + removeIndex);
	}

	
	for (int i = 0; i < 20; i++)
	{
		if (!_inActiveParticles->empty())
		{
			glm::vec3 position = getEntity()->getTransform()->getWorldPosition();
			float randomR = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*0.5f;
			float randomPiX = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.0f*3.14f -3.14f;
			float randomPiZ = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.0f*3.14f - 3.14f;

			float randomX = randomR*glm::sin(randomPiX);
			float randomZ = randomR*glm::cos(randomPiZ);

			position += glm::vec3(randomX, 1.0f, randomZ);

			randomX = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 20.0f - 10.0f;
			float randomY = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 35.0f + 45.0f;
			randomZ = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 20.0f - 10.f;
			glm::vec3 velocity = glm::vec3(0.0f, 50.0f, -10.0f);

			Particle particle = _inActiveParticles->at(0);

			particle.rigidbody->position = position;
			particle.rigidbody->velocity = velocity;
			particle.rigidbody->acceleration = glm::vec3(0.0);
			particle.startTime = currentTime;

			_inActiveParticles->erase(_inActiveParticles->begin());
			_activeParticles->push_back(particle);

		}
		

	//cout << _activeParticles->size() << endl;
	} 

	
	
}

vector<RenderCommand> ParticleEmitter::getRenderCommands()
{
	vector<RenderCommand> renderCommands;

	/*if (!_activeParticles->empty())
	{
		cout << glm::to_string(_activeParticles->at(0).rigidbody->position) << endl;
	}*/
	

	for (GLuint i = 0; i < _activeParticles->size(); i++)
	{
		RenderCommand renderCommand;

		renderCommand.mesh = _particleMeshes->at(0);
		renderCommand.modelMatrix = glm::translate(glm::mat4(), _activeParticles->at(i).rigidbody->position) * glm::scale(glm::mat4(), glm::vec3(0.2f, 0.2f, 0.2f));
		renderCommand.fluid = true;
		renderCommand.index = _activeParticles->at(i).rigidbody->index;

		renderCommands.push_back(renderCommand);
	}

	return renderCommands;
}

vector<Rigidbody *> ParticleEmitter::getRigidBodies()
{
	vector<Rigidbody *> rigidbodies;

	for (GLuint i = 0; i < _activeParticles->size(); i++)
	{
		Rigidbody *rigidbody = _activeParticles->at(i).rigidbody;

		rigidbodies.push_back(rigidbody);
	}

	return rigidbodies;
}