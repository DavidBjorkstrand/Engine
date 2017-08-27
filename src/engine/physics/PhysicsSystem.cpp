#include "engine/physics/PhysicsSystem.h"

#include "engine/physics/ParticleSystem.h"
#include "engine/physics/ConstraintSolver.h"
#include "engine/physics/SpatialHashing.h"
#include "engine/physics/Collider.h"
#include "engine/physics/SoftBody.h"
#include "engine/physics/Rigidbody.h"
#include "engine/physics/SphereCollider.h"
#include "engine/physics/PlaneCollider.h"
#include "engine/scene/Scene.h"

#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <chrono>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

PhysicsSystem::PhysicsSystem()
{
	_dt = 1.0f / 500.0f;
	_dtRest = 0.0f;

	_constraintSolver = new ConstraintSolver(_dt);
	_spatialHashing = new SpatialHashing();
	_collisionConstraints = new vector<RRCollisionConstraint>();

	_g = glm::vec3(0.0f, -9.82f, 0.0f);
	_k = 0.00005f;
	_c = 0.00005f;
}

PhysicsSystem::~PhysicsSystem()
{

}

void PhysicsSystem::setScene(Scene *scene)
{
	_scene = scene;
}

void PhysicsSystem::update(float dt)
{
	_scene->traverse();
	_dtRest += dt;
	while (_dtRest >= _dt)
	{
		vector<ParticleSystem *> *particleSystems = _scene->getParticleSystems();
		vector<Collider *> *colliders = _scene->getColliders();
		vector<SoftBody *> *softBodies = _scene->getSoftBodies();
		vector<Rigidbody *> *rigidbodies = _scene->getRigidbodies();

		applySpringConstraints(softBodies);
		applyExternalForces(particleSystems);
		applyExternalForces(softBodies);
		applyExternalForces(rigidbodies);
		integrate(particleSystems);
		integrate(softBodies);
		integrate(rigidbodies);
		collisionResolution(particleSystems, softBodies, colliders);

		_dtRest -= _dt;
	}
}

void PhysicsSystem::applySpringConstraints(vector<SoftBody *> *softBodies)
{
	for (SoftBody *softBody : *softBodies)
	{
		vector<SpringConstraint> *constraints = softBody->getSpringConstraints();
		for (SpringConstraint s : *constraints)
		{
			glm::vec3 r_ij = s.i->position - s.j->position;
			glm::vec3 v_ij = s.i->velocity - s.j->velocity;
			glm::vec3 dir = glm::normalize(r_ij);
			float length = glm::length(r_ij);

			glm::vec3 f_i = -(s.ks*(length - s.length) + s.kd*(glm::dot(v_ij, r_ij) / length))*dir;
			glm::vec3 f_j = -f_i;

			s.i->force += f_i;
			s.j->force += f_j;
		}
	}
}

void PhysicsSystem::applyExternalForces(vector<ParticleSystem *> *particleSystems)
{
	for (ParticleSystem *particleSystem : *particleSystems)
	{
		for (ParticleSystem::iterator it = particleSystem->begin(); it != particleSystem->end(); it++)
		{
			applyGravity(&it);
			applyViscousFriction(&it);
			applyAirFriction(&it);
		}
	}
}

void PhysicsSystem::applyExternalForces(vector<SoftBody *> *softBodies)
{
	for (SoftBody *softBody : *softBodies)
	{
		for (ParticleSystem::iterator it = softBody->begin(); it != softBody->end(); it++)
		{
			applyGravity(&it);
			applyViscousFriction(&it);
			applyAirFriction(&it);
		}
	}
}

void PhysicsSystem::applyExternalForces(vector<Rigidbody *> *rigidbodies)
{
	for (Rigidbody *rigidbody : *rigidbodies)
	{
		applyGravity(rigidbody);
		applyViscousFriction(rigidbody);
		applyAirFriction(rigidbody);
	}
}

void PhysicsSystem::applyGravity(Particle *particle)
{
	particle->force += _g * particle->mass;
}

void PhysicsSystem::applyGravity(Rigidbody *rigidbody)
{
	rigidbody->addForce(_g * rigidbody->getMass());
}

void PhysicsSystem::applyViscousFriction(Particle *particle)
{
	if (particle->predictedVelocity != glm::vec3(0.0f, 0.0f, 0.0f))
	{
		glm::vec3 uHat = glm::normalize(particle->predictedVelocity);
		float u = glm::length(particle->predictedVelocity);

		particle->force += -_k*u*uHat;
	}
}

void PhysicsSystem::applyViscousFriction(Rigidbody *rigidbody)
{
	if (rigidbody->getPredictedVelocity() != glm::vec3(0.0f, 0.0f, 0.0f))
	{
		glm::vec3 uHat = glm::normalize(rigidbody->getPredictedVelocity());
		float u = glm::length(rigidbody->getPredictedVelocity());

		rigidbody->addForce(-_k*u*uHat);
	}
}

void PhysicsSystem::applyAirFriction(Particle *particle)
{
	if (particle->predictedVelocity != glm::vec3(0.0f, 0.0f, 0.0f))
	{
		glm::vec3 uHat = glm::normalize(particle->predictedVelocity);
		float u = glm::length(particle->predictedVelocity);

		particle->force += -_c*u*u*uHat;
	}
}

void PhysicsSystem::applyAirFriction(Rigidbody *rigidbody)
{
	if (rigidbody->getPredictedVelocity() != glm::vec3(0.0f, 0.0f, 0.0f))
	{
		glm::vec3 uHat = glm::normalize(rigidbody->getPredictedVelocity());
		float u = glm::length(rigidbody->getPredictedVelocity());

		rigidbody->addForce(-_c*u*u*uHat);
	}
}

void PhysicsSystem::collisionResolution(vector<ParticleSystem *> *particleSystems, vector<SoftBody *> *softbodies, vector<Collider *> *colliders)
{
	float k = 10000.0f;
	float d = 3.0f;
	float e = (4.0f / k) / ((_dt*_dt)*(1.0f + 4.0f*d));
	float a = (4.0f / _dt) / (1.0f + 4.0f*d);
	float b = (4.0f*d) / (1.0f + 4.0f*d);

	vector<SphereCollider *> sphereColliders;
	vector<PlaneCollider *> planeColliders;

	_collisionConstraints->clear();

	for (Collider *collider : *colliders)
	{
		SphereCollider *sphereCollider = nullptr;
		PlaneCollider *planeCollider = nullptr;

		sphereCollider = dynamic_cast<SphereCollider *> (collider);
		planeCollider = dynamic_cast<PlaneCollider *> (collider);

		if (sphereCollider != nullptr)
		{
			sphereColliders.push_back(sphereCollider);
		}
		else if (planeCollider != nullptr)
		{
			planeColliders.push_back(planeCollider);
		}
	}

	_spatialHashing->clear();

	for (SphereCollider *sphereCollider : sphereColliders)
	{
		_spatialHashing->insert(sphereCollider);
	}

	for (ParticleSystem *particleSystem : *particleSystems)
	{
		for (ParticleSystem::iterator it = particleSystem->begin(); it != particleSystem->end(); it++)
		{
			Particle *particle = &it;
			vector<SphereCollider *> *possibleIntersections = _spatialHashing->checkSphere(particle);

			for (SphereCollider *collider : *possibleIntersections)
			{
				Intersection intersection = collider->checkCollision(particle);

				if (intersection.intersecting)
				{
					glm::vec3 relativeVelocity = collider->getRigidbody()->getVelocity() - particle->velocity;
					glm::vec3 normal = intersection.normal;
					glm::vec3 normalVelocity = glm::dot(relativeVelocity, normal)*normal;
					glm::vec3 tangent = glm::normalize(relativeVelocity - normalVelocity);

					float NdotV = glm::dot(normal, relativeVelocity);
					glm::vec3 r = -normal*collider->getRadius();

					if (NdotV < 0.0f && glm::length(relativeVelocity) > 0.5f)
					{
						float inertiaPart = glm::dot(glm::cross(collider->getRigidbody()->getInvI()*glm::cross(r, normal), r), normal);
						float j = -((1.0f + 0.1f) * glm::dot(relativeVelocity, normal)) / (collider->getRigidbody()->getInverseMass() + particle->inverseMass + inertiaPart);
						glm::vec3 impulse = (j*normal - 0.9f*j*tangent);

						glm::vec3 newVelocitySphere = collider->getRigidbody()->getVelocity() + collider->getRigidbody()->getInverseMass()*impulse;
						glm::vec3 newVelocityParticle = particle->velocity - particle->inverseMass*impulse;
						glm::vec3 newAngularVelocity = collider->getRigidbody()->getAngularVelocity() + collider->getRigidbody()->getInvI()*glm::cross(r, impulse);

						collider->getRigidbody()->setVelocity(newVelocitySphere);
						collider->getRigidbody()->setAngularVelocity(newAngularVelocity);
						particle->velocity = newVelocityParticle;
					}
					else if (NdotV < 0.00001f)
					{
						float Dkk = glm::dot(normal, normal)*collider->getRigidbody()->getInverseMass() + glm::dot(-normal, -normal)*particle->inverseMass + e;
						float q = -_dt*(glm::dot(normal, collider->getRigidbody()->getForce()*collider->getRigidbody()->getInverseMass()) + glm::dot(-normal, particle->force*particle->inverseMass));
						q -= b*(glm::dot(normal, collider->getRigidbody()->getVelocity()) + glm::dot(-normal, particle->velocity));
						q -= a*intersection.distance;
						float lambda = -(1.0f / Dkk)*(-q);

						if (lambda < 0.0f)
						{
							lambda = 0.0f;
						}

						collider->getRigidbody()->setVelocity(collider->getRigidbody()->getVelocity() + collider->getRigidbody()->getInverseMass()*normal*lambda);
						collider->getRigidbody()->setAngularVelocity(collider->getRigidbody()->getAngularVelocity() + collider->getRigidbody()->getInvI()*glm::cross(r, normal));
						particle->velocity += particle->inverseMass*(-normal)*lambda;
					}
				}
			}

			for (PlaneCollider *planeCollider : planeColliders)
			{
				Intersection intersection = planeCollider->checkCollision(particle);

				if (intersection.intersecting)
				{
					glm::vec3 relativeVelocity = particle->velocity;
					glm::vec3 normal = intersection.normal;
					glm::vec3 normalVelocity = glm::dot(relativeVelocity, normal)*normal;
					glm::vec3 tangent = glm::normalize(relativeVelocity - normalVelocity);

					float NdotV = glm::dot(-normal, relativeVelocity);

					if (NdotV < 0.0f && glm::length(normalVelocity) > 0.5f)
					{
						float j = -((1.0f + 0.1f) * glm::dot(relativeVelocity, normal))*particle->mass;
						glm::vec3 impulse = (j*normal - 0.5f*j*tangent);
						glm::vec3 newVelocityParticle = particle->velocity + particle->inverseMass*impulse;
						particle->velocity = newVelocityParticle;

					}
					else if (NdotV < 0.00001f)
					{
						float Dkk = glm::dot(-normal, -normal)*particle->inverseMass + e;
						float q = -_dt*glm::dot(-normal, particle->force*particle->inverseMass);
						q -= b*glm::dot(-normal, particle->velocity);
						q -= a*intersection.distance;
						float lambda = -(1.0f / Dkk)*(-q);

						if (lambda < 0.0f)
						{
							lambda = 0.0f;
						}


						particle->velocity += particle->inverseMass*(-normal)*lambda;
					}
				}
			}
		}
	}

	for (SoftBody *softbody : *softbodies)
	{
		for (ParticleSystem::iterator it = softbody->begin(); it != softbody->end(); it++)
		{
			Particle *particle = &it;
			vector<SphereCollider *> *possibleIntersections = _spatialHashing->checkSphere(particle);

			for (SphereCollider *collider : *possibleIntersections)
			{
				Intersection intersection = collider->checkCollision(particle);

				if (intersection.intersecting)
				{
					glm::vec3 relativeVelocity = collider->getRigidbody()->getVelocity() - particle->velocity;
					glm::vec3 normal = intersection.normal;
					glm::vec3 normalVelocity = glm::dot(relativeVelocity, normal)*normal;
					glm::vec3 tangent = glm::normalize(relativeVelocity - normalVelocity);

					float NdotV = glm::dot(normal, relativeVelocity);
					glm::vec3 r = -normal*collider->getRadius();

					if (NdotV < 0.0f && glm::length(relativeVelocity) > 0.5f)
					{
						float inertiaPart = glm::dot(glm::cross(collider->getRigidbody()->getInvI()*glm::cross(r, normal), r), normal);
						float j = -((1.0f + 0.1f) * glm::dot(relativeVelocity, normal)) / (collider->getRigidbody()->getInverseMass() + particle->inverseMass + inertiaPart);
						glm::vec3 impulse = (j*normal - 0.9f*j*tangent);

						glm::vec3 newVelocitySphere = collider->getRigidbody()->getVelocity() + collider->getRigidbody()->getInverseMass()*impulse;
						glm::vec3 newVelocityParticle = particle->velocity - particle->inverseMass*impulse;
						glm::vec3 newAngularVelocity = collider->getRigidbody()->getAngularVelocity() + collider->getRigidbody()->getInvI()*glm::cross(r, impulse);

						collider->getRigidbody()->setVelocity(newVelocitySphere);
						collider->getRigidbody()->setAngularVelocity(newAngularVelocity);
						particle->velocity = newVelocityParticle;
					}
					else if (NdotV < 0.00001f)
					{
						float Dkk = glm::dot(normal, normal)*collider->getRigidbody()->getInverseMass() + glm::dot(-normal, -normal)*particle->inverseMass + e;
						float q = -_dt*(glm::dot(normal, collider->getRigidbody()->getForce()*collider->getRigidbody()->getInverseMass()) + glm::dot(-normal, particle->force*particle->inverseMass));
						q -= b*(glm::dot(normal, collider->getRigidbody()->getVelocity()) + glm::dot(-normal, particle->velocity));
						q -= a*intersection.distance;
						float lambda = -(1.0f / Dkk)*(-q);

						if (lambda < 0.0f)
						{
							lambda = 0.0f;
						}

						collider->getRigidbody()->setVelocity(collider->getRigidbody()->getVelocity() + collider->getRigidbody()->getInverseMass()*normal*lambda);
						collider->getRigidbody()->setAngularVelocity(collider->getRigidbody()->getAngularVelocity() + collider->getRigidbody()->getInvI()*glm::cross(r, normal));
						particle->velocity += particle->inverseMass*(-normal)*lambda;
					}
				}
			}

			for (PlaneCollider *planeCollider : planeColliders)
			{
				Intersection intersection = planeCollider->checkCollision(particle);

				if (intersection.intersecting)
				{
					glm::vec3 relativeVelocity = particle->velocity;
					glm::vec3 normal = intersection.normal;
					glm::vec3 normalVelocity = glm::dot(relativeVelocity, normal)*normal;
					glm::vec3 tangent = glm::normalize(relativeVelocity - normalVelocity);

					float NdotV = glm::dot(-normal, relativeVelocity);

					if (NdotV < 0.0f && glm::length(normalVelocity) > 0.5f)
					{
						float j = -((1.0f + 0.1f) * glm::dot(relativeVelocity, normal))*particle->mass;
						glm::vec3 impulse = (j*normal - 0.9f*j*tangent);
						glm::vec3 newVelocityParticle = particle->velocity + particle->inverseMass*impulse;
						particle->velocity = newVelocityParticle;

					}
					else if (NdotV < 0.00001f)
					{
						float Dkk = glm::dot(-normal, -normal)*particle->inverseMass + e;
						float q = -_dt*glm::dot(-normal, particle->force*particle->inverseMass);
						q -= b*glm::dot(-normal, particle->velocity);
						q -= a*intersection.distance;
						float lambda = -(1.0f / Dkk)*(-q);

						if (lambda < 0.0f)
						{
							lambda = 0.0f;
						}


						particle->velocity += particle->inverseMass*(-normal)*lambda;
					}
				}
			}
		}
	}

	for (SphereCollider *sphereCollider : sphereColliders)
	{
		vector<SphereCollider *> *possibleIntersections = _spatialHashing->checkSphere(sphereCollider);

		for (SphereCollider *sphereCollider2 : *possibleIntersections)
		{
			Intersection intersection = sphereCollider2->checkCollision(sphereCollider);

			if (intersection.intersecting)
			{
				glm::vec3 relativeVelocity = sphereCollider->getRigidbody()->getVelocity() - sphereCollider2->getRigidbody()->getVelocity();
				glm::vec3 normal = -intersection.normal;
				glm::vec3 normalVelocity = glm::dot(relativeVelocity, normal)*normal;
				glm::vec3 tangent = glm::normalize(relativeVelocity - normalVelocity);

				float NdotV = glm::dot(normal, relativeVelocity);
				glm::vec3 r = -normal*sphereCollider->getRadius();

				if (NdotV < 0.0f && glm::length(relativeVelocity) > 0.5f)
				{
					float inertiaPart = glm::dot(glm::cross(sphereCollider->getRigidbody()->getInvI()*glm::cross(r, normal), r), normal) + glm::dot(glm::cross(sphereCollider2->getRigidbody()->getInvI()*glm::cross(-r, -normal), -r), -normal);
					float j = -((1.0f + 0.1f) * glm::dot(relativeVelocity, normal)) / (sphereCollider->getRigidbody()->getInverseMass() + sphereCollider2->getRigidbody()->getInverseMass() + inertiaPart);
					glm::vec3 impulse = (j*normal);

					glm::vec3 newVelocitySphere = sphereCollider->getRigidbody()->getVelocity() + sphereCollider->getRigidbody()->getInverseMass()*impulse;
					glm::vec3 newVelocitySphere2 = sphereCollider2->getRigidbody()->getVelocity() - sphereCollider2->getRigidbody()->getInverseMass()*impulse;
					glm::vec3 newAngularVelocity = sphereCollider->getRigidbody()->getAngularVelocity() + sphereCollider->getRigidbody()->getInvI()*glm::cross(r, impulse);
					glm::vec3 newAngularVelocity2 = sphereCollider2->getRigidbody()->getAngularVelocity() - sphereCollider2->getRigidbody()->getInvI()*glm::cross(-r, impulse);

					sphereCollider->getRigidbody()->setVelocity(newVelocitySphere);
					sphereCollider->getRigidbody()->setAngularVelocity(newAngularVelocity);
					sphereCollider2->getRigidbody()->setVelocity(newVelocitySphere2);
					sphereCollider2->getRigidbody()->setAngularVelocity(newAngularVelocity2);

				}
				else if (NdotV < 0.00001f)
				{
					RRCollisionConstraint collisionConstraint;

					collisionConstraint.i = sphereCollider->getRigidbody();
					collisionConstraint.j = sphereCollider2->getRigidbody();
					collisionConstraint.overlap = intersection.distance;
					collisionConstraint.normal = intersection.normal;
					collisionConstraint.twoBodies = true;

					_collisionConstraints->push_back(collisionConstraint);
				}
			}
		}

		for (PlaneCollider *planeCollider : planeColliders)
		{
			Intersection intersection = planeCollider->checkCollision(sphereCollider);

			if (intersection.intersecting)
			{
				glm::vec3 relativeVelocity = sphereCollider->getRigidbody()->getVelocity();
				glm::vec3 normal = intersection.normal;
				glm::vec3 normalVelocity = glm::dot(relativeVelocity, normal)*normal;
				glm::vec3 tangent = glm::normalize(relativeVelocity - normalVelocity);

				float NdotV = glm::dot(-normal, relativeVelocity);
				glm::vec3 r = normal*sphereCollider->getRadius();

				if (NdotV < 0.0f && glm::length(normalVelocity) > 0.5f)
				{
					float inertiaPart = glm::dot(glm::cross(sphereCollider->getRigidbody()->getInvI()*glm::cross(r, normal), r), normal);
					float j = -((1.0f + 0.5f) * glm::dot(relativeVelocity, normal)) / (sphereCollider->getRigidbody()->getInverseMass() + inertiaPart);
					glm::vec3 impulse = (j*normal);

					glm::vec3 newVelocitySphere = sphereCollider->getRigidbody()->getVelocity() + sphereCollider->getRigidbody()->getInverseMass()*impulse;
					glm::vec3 newAngularVelocity = sphereCollider->getRigidbody()->getAngularVelocity() + sphereCollider->getRigidbody()->getInvI()*glm::cross(r, impulse);

					sphereCollider->getRigidbody()->setVelocity(newVelocitySphere);
					sphereCollider->getRigidbody()->setAngularVelocity(newAngularVelocity);
				}
				else if (NdotV < 0.00001f)
				{
					RRCollisionConstraint collisionConstraint;

					collisionConstraint.i = sphereCollider->getRigidbody();
					collisionConstraint.overlap = intersection.distance;
					collisionConstraint.normal = intersection.normal;
					collisionConstraint.twoBodies = false;

					_collisionConstraints->push_back(collisionConstraint);
				}
			}
		}
	}

	if (!_collisionConstraints->empty())
	{
		_constraintSolver->solveConstraints(_collisionConstraints);
	}
	
}

void PhysicsSystem::integrate(vector<ParticleSystem *> *particleSystems)
{
	for (ParticleSystem *particleSystem : *particleSystems)
	{
		for (ParticleSystem::iterator it = particleSystem->begin(); it != particleSystem->end(); it++)
		{
			Particle *particle = &it;

			//a_(n)
			glm::vec3 newAcceleration = particle->force * particle->inverseMass;

			//v_(n)
			particle->velocity = particle->velocity + 0.5f*(particle->acceleration + newAcceleration)*_dt;

			particle->acceleration = newAcceleration;
			particle->force = glm::vec3(0.0f);

			//r_(n+1)
			particle->position = particle->position + particle->velocity*_dt + 0.5f*particle->acceleration*_dt*_dt;
			particle->predictedVelocity = particle->velocity + particle->acceleration*_dt;
		}
	}
}

void PhysicsSystem::integrate(vector<SoftBody *> *softBodies)
{
	for (SoftBody *softBody : *softBodies)
	{
		if (!softBody->getDistanceConstraints()->empty())
		{
			_constraintSolver->solveConstraints(softBody->getDistanceConstraints());
		}

		for (ParticleSystem::iterator it = softBody->begin(); it != softBody->end(); it++)
		{
			Particle *particle = &it;

			//a_(n)
			glm::vec3 newAcceleration = particle->force * particle->inverseMass;

			//v_(n)
			particle->velocity = particle->velocity + 0.5f*(particle->acceleration + newAcceleration)*_dt;

			particle->acceleration = newAcceleration;
			particle->force = glm::vec3(0.0f);

			//r_(n+1)
			particle->position = particle->position + particle->velocity*_dt + 0.5f*particle->acceleration*_dt*_dt;
			particle->predictedVelocity = particle->velocity + particle->acceleration*_dt;

		}
	}
}

void PhysicsSystem::integrate(vector<Rigidbody *> *rigidbodies)
{
	for (Rigidbody *rigidbody : *rigidbodies)
	{
		glm::vec3 newAcceleration = rigidbody->getForce() * rigidbody->getInverseMass();
		glm::vec3 torqueGyro = glm::cross(rigidbody->getAngularVelocity(), rigidbody->getI()*rigidbody->getAngularVelocity());

		rigidbody->setVelocity(rigidbody->getVelocity() + 0.5f*(rigidbody->getAcceleration() + newAcceleration)*_dt);
		rigidbody->setAngularVelocity(rigidbody->getAngularVelocity() + _dt*rigidbody->getInvI()*torqueGyro);

		rigidbody->setAcceleration(newAcceleration);
		rigidbody->resetForce();

		rigidbody->setPosition(rigidbody->getPosition() + rigidbody->getVelocity()*_dt + 0.5f*rigidbody->getAcceleration()*_dt*_dt);
		rigidbody->setPredictedVelocity(rigidbody->getVelocity() + rigidbody->getAcceleration()*_dt);

		glm::vec3 omega = rigidbody->getAngularVelocity();
		glm::quat q = glm::quat(0.0f, omega)* (_dt / 2.0f);
		q = glm::exp(q);
		q = glm::normalize(q);

		rigidbody->setRotation(glm::normalize(q*rigidbody->getRotation()));

	}
}

