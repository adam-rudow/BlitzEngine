#pragma once

#define NOMINMAX

// Outer-Engine includes
#include <assert.h>
#include <vector>
#include <map>

// Inter-Engine includes
//#include "PrimeEngine/APIAbstraction/Effect/Effect.h"
//#include "PrimeEngine/ParticleSystem/Emitter.h"
#include "ParticleSystem.h"
#include "Components/Component.h"
#include "Utils/Utility.h"

// Sibling/Children includes
//#include "Mesh.h"

// Define stuff for debugging
#define _DEBUG_ 1

class ParticleSystem;


class RENDERER_EXPORT ParticleSystemManager// : public Component
{

	DeclSingleton(ParticleSystemManager)

	// Constructor -------------------------------------------------------------
	ParticleSystemManager();// {}
            
    virtual ~ParticleSystemManager() {}

	/*************************************************
	* Member Methods
	**************************************************/
	void generateEmitter();

	/*************************************************
	* Member variables
	**************************************************/
    std::vector< std::map<int, ParticleSystem*> > mParticleSystems;

private:
};

