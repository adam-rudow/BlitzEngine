
#include "Emitter.h"

// Sibling/Children includes
#include "ParticleSystemManager.h"
#include "ParticleSystemTypesEnum.h"
//#include "GameObjectManager.h"
     
ImplSingleton(ParticleSystemManager)

ParticleSystemManager::ParticleSystemManager()
{
	// Generate a list of arrays
	for (int i = 0; i < PST_COUNT; i++) {
		// Maintain a list of lists
		mParticleSystems.push_back( std::map<int, ParticleSystem*>() );
	}
}

/*************************************************
* Member Methods
**************************************************/
void ParticleSystemManager::generateEmitter()
{
 //   Event_CREATE_EMITTER *pRealEvent = (Event_CREATE_EMITTER *)pEvt;
 //           
	//int type, ID;
	//		
	//type = pRealEvent->m_pSystemType;
	//ID = pRealEvent->m_pSystemID;
	//		
	//// Emitter instantiation
	//Handle hEmitter("Emitter", sizeof(Emitter));
	//Emitter *pEmitter = new(hEmitter) Emitter(*m_pContext, m_arena, hEmitter, NULL);
	//	
	//pEmitter->Initialize(pEvt);
	//// Remember that in the m_base 4x4 matrix, n is the direction!!!

	//if (m_particleSystems.size() > type) {
	//	// The type exists in the ParticleSystem vector
	//		
	//	// Check for type of given ID
	//	if (!(m_particleSystems[type].size() > ID)) {
	//		//The ID does not exist so add it (ie new particleSystem)
	//		Handle hParticleSystem("ParticleSystem", sizeof(ParticleSystem));
	//		ParticleSystem *pPSys = new(hParticleSystem) ParticleSystem(
	//			*m_pContext, m_arena, hParticleSystem);
	//		pPSys->addDefaultComponents();

	//		pPSys->m_systemTypeID = ID;
	//		pPSys->m_position = pRealEvent->m_base.getPos();

	//		RootSceneNode::Instance()->addComponent(pPSys);

	//		m_pContext->getGameObjectManager()->addComponent(pPSys);

	//		m_particleSystems[type][ID] = hParticleSystem;
	//	}
	//	// Add the emitters
	//	ParticleSystem *pSys = (ParticleSystem *)m_particleSystems[type][ID].getObject();
	//	pSys->addComponent(hEmitter);
	//	pSys->m_emitters.push_back(pEmitter);
	//}

}
