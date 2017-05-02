#pragma once

#include <queue>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

using namespace std;

struct Particle
{
	GLuint index;
	float radius;
	float mass;
	float inverseMass;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 predictedVelocity;
	glm::vec3 acceleration;
	glm::vec3 force;
};

class ParticleSystem
{
	private:
		Particle *_particles;
		GLuint _numParticles;
		GLuint _numActiveParticles;
		GLboolean *_valid;
		GLuint _validRange;
		queue<GLuint> _freeIndexQueue;
		float _particleMass;
		float _inverseParticleMass;
		float _particleRadius;

	public:
		class iterator
		{
			private:
				GLuint _currentIndex;
				Particle *_particles;
				GLboolean *_valid;
				GLuint _validRange;

			public:
				iterator(GLuint currentIndex, Particle *particles, GLboolean *valid, GLuint validRange) 
				{ 
					_currentIndex = currentIndex;
					_particles = particles;
					_valid = valid;
					_validRange = validRange;
				}

				iterator()
				{
					_currentIndex = 0;
					_particles = nullptr;
					_valid = nullptr;
					_validRange = 0;
				}

				void decrementRange()
				{
					_validRange--;
				}

				iterator operator++(int) 
				{ 
					_currentIndex++;

					if (_currentIndex > _validRange)
					{
						_currentIndex = _validRange;
					}

					while (_currentIndex < _validRange && !_valid[_currentIndex]) 
					{ 
						_currentIndex++;
					} 
					return *this; 
				}

				Particle operator*() 
				{ 
					return _particles[_currentIndex];
				}

				Particle *operator->() 
				{ 
					return &_particles[_currentIndex];
				}

				Particle *operator&() 
				{ 
					return &_particles[_currentIndex];
				}

				GLboolean operator==(iterator rhs) 
				{ 
					return _currentIndex == rhs._currentIndex;
				}
				
				GLboolean operator!=(iterator rhs) 
				{ 
					return _currentIndex != rhs._currentIndex;
				}
		};

		ParticleSystem(GLuint numParticles, float particleMass, float particleRadius);
		~ParticleSystem();
		GLuint createParticle(glm::vec3 position, glm::vec3 velocity);
		iterator destroyParticle(iterator iterator);
		GLuint getNumActiveParticles();
		GLboolean isFull();
		iterator begin();
		iterator end();

};