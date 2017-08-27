#include "engine/physics/PhysicsSystem.h"

#include "engine/physics/ParticleSystem.h"
#include "engine/physics/ConstraintSolver.h"
#include "engine/physics/SpatialHashing.h"
#include "engine/physics/Collider.h"
#include "engine/physics/SoftBody.h"
#include "engine/physics/Rigidbody.h"
#include "engine/physics/SphereCollider.h"
#include "engine/physics/PlaneCollider.h"
#include "engine/physics/RCollisionConstraint.h"
#include "engine/physics/RRCollisionConstraint.h"
#include "engine/scene/Scene.h"

#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <chrono>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_cross_product.hpp>

PhysicsSystem::PhysicsSystem()
{
	_dt = 1.0f / 500.0f;
	_dtRest = 0.0f;

	_constraintSolver = new ConstraintSolver(_dt);
	_spatialHashing = new SpatialHashing();

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
	vector<SphereCollider *> sphereColliders;
	vector<PlaneCollider *> planeColliders;

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

	for (SphereCollider *sphereCollider : sphereColliders)
	{
		vector<SphereCollider *> *possibleIntersections = _spatialHashing->checkSphere(sphereCollider);

		for (SphereCollider *sphereCollider2 : *possibleIntersections)
		{
			Intersection intersection = sphereCollider2->checkCollision(sphereCollider);

			if (intersection.intersecting)
			{
				Rigidbody *r1 = sphereCollider->getRigidbody();
				Rigidbody *r2 = sphereCollider2->getRigidbody();
				glm::vec3 u = r1->getVelocity() - r2->getVelocity();
				glm::vec3 n = intersection.normal;
				glm::vec3 un = glm::dot(u, n)*n;
				glm::vec3 t = glm::normalize(u - un);

				float NdotV = glm::dot(-n, u);
				glm::vec3 ra = -n*sphereCollider->getRadius();
				glm::vec3 rb = n*sphereCollider2->getRadius();

				if (NdotV < 0.0f && glm::length(u) > 0.5f)
				{
					glm::mat3 K = r1->getInverseMass()*glm::mat3() + r2->getInverseMass()*glm::mat3();
					K -= glm::matrixCross3(ra)*r1->getInvI()*glm::matrixCross3(ra);
					K -= glm::matrixCross3(ra)*r2->getInvI()*glm::matrixCross3(rb);
					glm::vec3 J = glm::inverse(K)*(-0.1f*un - u);
					glm::vec3 Jn = glm::dot(J, n)*n;
					glm::vec3 Jt = J - Jn;
					float coef = 0.075f;

					if (glm::length(Jt) > coef*glm::length(Jn))
					{
						glm::vec3 ut = u - un;
						glm::vec3 t = glm::normalize(ut);
						glm::vec3 j = -((1.0f + 0.1f) * un) / (glm::dot(n, K * (n - coef*t)));

						J = j*n - coef*j*t;
					}

					glm::vec3 currentVelocity1 = r1->getVelocity();
					glm::vec3 currentVelocity2 = r2->getVelocity();
					glm::vec3 currentAngularVelocity1 = r1->getAngularVelocity();
					glm::vec3 currentAngularVelocity2 = r2->getAngularVelocity();
					glm::vec3 newVelocity1 = currentVelocity1 + r1->getInverseMass()*J;
					glm::vec3 newVelocity2 = currentVelocity2 - r2->getInverseMass()*J;
					glm::vec3 newAngularVelocity1 = currentAngularVelocity1 + r1->getInvI()* glm::cross(ra, J);
					glm::vec3 newAngularVelocity2 = currentAngularVelocity2 - r2->getInvI()* glm::cross(rb, J);

					r1->setVelocity(newVelocity1);
					r2->setVelocity(newVelocity2);
					r1->setAngularVelocity(newAngularVelocity1);
					r2->setAngularVelocity(newAngularVelocity2);

				}
				else if (NdotV < 0.00001f)
				{
					RRCollisionConstraint *constraint = new RRCollisionConstraint(sphereCollider->getRigidbody(), sphereCollider2->getRigidbody(), -intersection.normal, intersection.distance);

					_constraintSolver->addConstraint(constraint);
				}
			}
		}

		for (PlaneCollider *planeCollider : planeColliders)
		{
			Intersection intersection = planeCollider->checkCollision(sphereCollider);

			if (intersection.intersecting)
			{
				Rigidbody *r1 = sphereCollider->getRigidbody();
				glm::vec3 u = r1->getVelocity();
				glm::vec3 n = -intersection.normal;
				glm::vec3 un = glm::dot(u, n)*n;
				glm::vec3 t = glm::normalize(u - un);

				float NdotV = glm::dot(n, u);
				glm::vec3 ra = -n*sphereCollider->getRadius();

				if (NdotV < 0.0f && glm::length(u) > 0.5f)
				{
					glm::mat3 K = r1->getInverseMass()*glm::mat3();
					K -= glm::matrixCross3(ra)*r1->getInvI()*glm::matrixCross3(ra);
					glm::vec3 J = glm::inverse(K)*(-0.9f*un - u);
					glm::vec3 Jn = glm::dot(J, n)*n;
					glm::vec3 Jt = J - Jn;
					float coef = 0.45f;

					if (!(glm::length(Jt) <= coef*glm::length(Jn)))
					{
						glm::vec3 ut = u - un;
						glm::vec3 t = glm::normalize(ut);
						glm::vec3 j = -((1.0f + 0.9f) * un) / (glm::dot(n, K * (n - coef*t)));

						J = j*n - coef*j*t;
					}

					glm::vec3 currentVelocity1 = r1->getVelocity();
					glm::vec3 currentAngularVelocity1 = r1->getAngularVelocity();
					glm::vec3 newVelocity1 = currentVelocity1 + r1->getInverseMass()*J;
					glm::vec3 newAngularVelocity1 = currentAngularVelocity1 + r1->getInvI()* glm::cross(ra, J);

					r1->setVelocity(newVelocity1);
					r1->setAngularVelocity(newAngularVelocity1);
				}
				else if (NdotV < 0.00001f)
				{
					RCollisionConstraint *constraint = new RCollisionConstraint(sphereCollider->getRigidbody(), intersection.normal, intersection.distance);

					_constraintSolver->addConstraint(constraint);
				}
			}
		}
	}

	_constraintSolver->solve();
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
		/*if (!softBody->getDistanceConstraints()->empty())
		{
			_constraintSolver->solveConstraints(softBody->getDistanceConstraints());
		}*/

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

