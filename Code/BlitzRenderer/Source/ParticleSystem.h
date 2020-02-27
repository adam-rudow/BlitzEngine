#pragma once

// Outer-Engine includes
#include <assert.h>
#include <vector>

// Sibling/Children includes
#include "Mesh.h"
#include "Components/Component.h"
#include "ParticleSystemTypesEnum.h"
#include "RendererMacros.h"

class Emitter;

class RENDERER_EXPORT ParticleSystem : public Component
{

public:

	// Constructor -------------------------------------------------------------
	ParticleSystem(/*class Game& game*/);

	void Initialize();

	virtual ~ParticleSystem() {}

	//virtual void addDefaultComponents();

	/*************************************************
	* Event Handlers
	**************************************************/
	virtual void Update(float frameTime);


	Vector3 mPosition;
	int mSystemTypeID;

	// Current number of running particle emitters
	unsigned mNumEmitters;

	// List of Emitters to manage
	//Array<Emitter> m_emitters;
	std::vector<Emitter*> mEmitters;
};

