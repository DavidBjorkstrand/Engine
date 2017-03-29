#pragma once

#include <iostream>

#include <glm/gtc/type_ptr.hpp>

using namespace std;

class Material
{
	public: 
		/*
		* Constructor
		*/
		Material(string name, glm::vec3 albedo, float roughness, float metallic);

		/*
		* Gets the name of the material
		*/
		string getName();

		/*
		* Gets the albedo of the material.
		*/
		glm::vec3 getAlbedo();

		/*
		* Gets the roughness of the material.
		*/
		float getRoughness();

		/*
		* Gets how metllic the matrial is. 
		*/
		float getMetallic();

	private:
		string _name;
		glm::vec3 _albedo;
		float _roughness;
		float _metallic;
};