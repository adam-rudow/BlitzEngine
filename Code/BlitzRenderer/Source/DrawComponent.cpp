#include "DrawComponent.h"
//#include "Game.h"
//#include "PreCompiledHeader.h"
#include "Renderer.h"
#include "VertexData.h"

DrawComponent::DrawComponent(/*class Game& game*/) : Component(/*game*/)
{
	mIsVisible = true;
}

void DrawComponent::Register()
{
	//Super
	Renderer::Instance()->AddComponent(this, mDrawOrderMask);
}

void DrawComponent::Unregister()
{
	//Super::Unregister();
	Renderer::Instance()->RemoveComponent(this, mDrawOrderMask);
}

void DrawComponent::Draw(class Renderer& render)
{
	// Do frustum culling here?
}

