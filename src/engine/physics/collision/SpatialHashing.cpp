#include "engine/physics/collision/SpatialHashing.h"

#include "engine/physics/ParticleSystem.h"
#include "engine/physics/collision/SphereCollider.h"

#include <map>
#include <vector>
#include <math.h>
#include <algorithm>

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

SpatialHashing::SpatialHashing()
{
	_particleMap = new map<long, vector<Particle *> *>();
	_sphereMap = new map<long, vector<SphereCollider *> *>();
	_subdivisions = new vector<GLuint>();
	_possibleParticleCollisions = new vector<Particle *>();
	_possibleSphereCollisions = new vector<SphereCollider *>();
}

void SpatialHashing::insert(SphereCollider *sphereCollider)
{
	GLuint subdivision = log2(sphereCollider->getRadius() * 2);
	GLuint k = pow(2, subdivision);
	glm::vec3 cellPosition = glm::vec3(floor(sphereCollider->getPosition().x / k), floor(sphereCollider->getPosition().y / k), floor(sphereCollider->getPosition().z / k));
	glm::vec3 min = sphereCollider->getPosition() - sphereCollider->getRadius();
	min.x = floor(min.x / k);
	min.y = floor(min.y / k);
	min.z = floor(min.z / k);
	glm::vec3 max = sphereCollider->getPosition() + sphereCollider->getRadius();
	max.x = floor(max.x / k);
	max.y = floor(max.y / k);
	max.z = floor(max.z / k);

	int count = 0;
	for (float x = min.x; x <= max.x; x++)
	{
		for (float y = min.y; y <= max.y; y++)
		{
			for (float z = min.z; z <= max.z; z++)
			{
				long key = hash(x, y, z, subdivision);
				insertIntoMap(key, sphereCollider);
				count++;
			}
		}
	}

	if (std::find(_subdivisions->begin(), _subdivisions->end(), subdivision) == _subdivisions->end())
	{
		_subdivisions->push_back(subdivision);
	}
}

vector<SphereCollider *> *SpatialHashing::checkSphere(SphereCollider *sphereCollider)
{
	_possibleSphereCollisions->clear();

	for (vector<GLuint>::iterator l = _subdivisions->begin(); l != _subdivisions->end(); l++)
	{
		GLuint k = pow(2, *l);
		glm::vec3 cellPosition = glm::vec3(floor(sphereCollider->getPosition().x / k), floor(sphereCollider->getPosition().y / k), floor(sphereCollider->getPosition().z / k));
		glm::vec3 min = sphereCollider->getPosition() - sphereCollider->getRadius();
		min.x = floor(min.x / k);
		min.y = floor(min.y / k);
		min.z = floor(min.z / k);
		glm::vec3 max = sphereCollider->getPosition() + sphereCollider->getRadius();
		max.x = floor(max.x / k);
		max.y = floor(max.y / k);
		max.z = floor(max.z / k);

		for (float x = min.x; x <= max.x; x++)
		{
			for (float y = min.y; y <= max.y; y++)
			{
				for (float z = min.z; z <= max.z; z++)
				{
					long key = hash(x, y, z, *l);
					vector<SphereCollider *> *cellContent = _sphereMap->find(key)->second;
					cellContent->erase(std::remove(cellContent->begin(), cellContent->end(), sphereCollider), cellContent->end());

					vector<SphereCollider *>::iterator s;
					for (s = cellContent->begin(); s != cellContent->end(); s++)
					{
						if (*s == sphereCollider)
						{
							s = cellContent->erase(s);
						}
						else if(std::find(_possibleSphereCollisions->begin(), _possibleSphereCollisions->end(), *s) == _possibleSphereCollisions->end())
						{
							_possibleSphereCollisions->push_back(*s);
						}
					}
				}
			}
		}
	}

	return _possibleSphereCollisions;
}

vector<SphereCollider *> *SpatialHashing::checkSphere(Particle *particle)
{
	_possibleSphereCollisions->clear();

	for (vector<GLuint>::iterator l = _subdivisions->begin(); l != _subdivisions->end(); l++)
	{
		GLuint k = pow(2, *l);
		glm::vec3 cellPosition = glm::vec3(floor(particle->position.x / k), floor(particle->position.y / k), floor(particle->position.z / k));
		glm::vec3 min = particle->position - particle->radius;
		min.x = floor(min.x / k);
		min.y = floor(min.y / k);
		min.z = floor(min.z / k);
		glm::vec3 max = particle->position + particle->radius;
		max.x = floor(max.x / k);
		max.y = floor(max.y / k);
		max.z = floor(max.z / k);

		for (float x = min.x; x <= max.x; x++)
		{
			for (float y = min.y; y <= max.y; y++)
			{
				for (float z = min.z; z <= max.z; z++)
				{
					long key = hash(x, y, z, *l);

					if (_sphereMap->find(key) != _sphereMap->end())
					{
						vector<SphereCollider *> *cellContent = _sphereMap->find(key)->second;

						for (SphereCollider *sphereCollider : *cellContent)
						{
							if (std::find(_possibleSphereCollisions->begin(), _possibleSphereCollisions->end(), sphereCollider) == _possibleSphereCollisions->end())
							{
								_possibleSphereCollisions->push_back(sphereCollider);
							}
						}
					}
				}
			}
		}
	}

	return _possibleSphereCollisions;
}

void SpatialHashing::clear()
{
	_subdivisions->clear();
}

long SpatialHashing::hash(float x, float y, float z, GLuint subdivision)
{
	long hash = 5381;

	hash = 73856093 * x;
	hash = hash ^ ((long)(19349663 * y));
	hash = hash ^ ((long)(83492791* z));
	hash = hash ^ ((long)(67867979 * subdivision));

	return hash % NUM_BUCKETS;
}

void SpatialHashing::insertIntoMap(long hash, Particle *particle)
{
	if (_particleMap->find(hash) == _particleMap->end())
	{
		_particleMap->insert(pair<long, vector<Particle*> *>(hash, new vector<Particle *>()));
	}

	_particleMap->find(hash)->second->push_back(particle);
}

void SpatialHashing::insertIntoMap(long hash, SphereCollider *sphereCollider)
{
	if (_sphereMap->find(hash) == _sphereMap->end())
	{
		_sphereMap->insert(pair<long, vector<SphereCollider *> *>(hash, new vector<SphereCollider *>()));
	}

	_sphereMap->find(hash)->second->push_back(sphereCollider);
}