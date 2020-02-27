#include "ParticleSystem.h"

//#include "Game.h"
#include "Emitter.h"
#include "ParticleSystemTypesEnum.h"

//#include <math.h>


ParticleSystem::ParticleSystem(/*class Game& game*/) : Component(/*game*/)
{
	//TODO: Fix Particle systems
	//Game* game = S_CAST(Game*, IGame::Instance());
	//game->AddUpdateObject(this);
}

void ParticleSystem::Initialize()
{
	/*float min = 2.5;
	float max = 5.9;
	float dif = max - min;
	float frameTime = 0.00162f * 1000;
	int id = 1643;
	double offset;
	Vector2 seed(frameTime, id);*/
	//Vector2 r(
	//	23.1406926327792690,  // e^pi (Gelfond's constant)
	//	2.6651441426902251); // 2^sqrt(2) (Gelfond–Schneider constant)
	////float offset; 
	//double inner = (123456789 % (256 * (int)(seed.m_x * r.m_x + seed.m_y * r.m_y)));
	//inner /= 1000;
	//double cosRes = cosf(inner);
	//double fractPart = abs(modf(cosRes, &offset));

	//float result =  min + fractPart * dif;

	{
		Emitter *pEmitterTest = new Emitter(/*mGame, */FLAMES_YELLOW);
		AttachComponent(pEmitterTest);
		mEmitters.push_back(pEmitterTest);
	}

	/*
	{
		PE::Handle hEmitter("Emitter", sizeof(Emitter));
		Emitter *pEmitterTest = new(hEmitter) Emitter(*m_pContext, m_arena, hEmitter, NULL);
		addComponent(pEmitterTest);
		m_emitters.push_back(pEmitterTest);
		pEmitterTest->m_particleType = SMOKE;
	}*/

	/*{
		PE::Handle hEmitter("Emitter", sizeof(Emitter));
		Emitter *pEmitterTest = new(hEmitter) Emitter(*m_pContext, m_arena, hEmitter, NULL, FLAMES_RED);
		addComponent(pEmitterTest);
		m_emitters.push_back(pEmitterTest);
	}*/

	for (size_t i = 0; i < mEmitters.size(); ++i)
	{
		//mEmitters[i]->Initialize(32/*4096 * 100*/); // TODO: Set a way to define a number of particles for a specific type of emitter
		mEmitters[i]->Initialize(32, 3.5f);
	}
}

void ParticleSystem::Update(float frameTime)
{
	mSystemTypeID;
	// Update event generated, time to get our hands dirty
	for (size_t i = 0; i < mEmitters.size(); i++) {
		mEmitters[i]->Update(frameTime);

	}
}
