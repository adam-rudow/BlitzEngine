// DrawComponent.h
// Base class for all components that can be drawn by Renderer

#pragma once
#include "Components/Component.h"
#include "RenderUtility.h"
#include "RendererMacros.h"

struct Effect;
class VertexData;

class RENDERER_EXPORT DrawComponent : public Component
{
public:
	DrawComponent(/*class Game& game*/);

	virtual void Register() override;
	virtual void Unregister() override;
	
	virtual void Draw(class Renderer& render);

	void SetIsVisible(bool value) { mIsVisible = value; }
	bool IsVisible() const { return mIsVisible; }

	virtual VertexData* GetVertexData() { return mVertexBuffer; }

protected:
	
	//Effect* mEffect;
	
	// Shaders
	ShaderType mShaderType;
	bool mIsVisible;

	// Mask of which passes this component should be a part of
	UINT8 mDrawOrderMask;

	// Vertices to be rendered
	VertexData* mVertexBuffer;

};

