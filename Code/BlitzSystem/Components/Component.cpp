//#include "PreCompiledHeader.h"
#include "Component.h"

Component::Component()//class Game& game) : mGame(game)
{
	isTransformDirty = false;
	mScale = 1.0f;
	mOwner = NULL;
}

Component::~Component()
{

}

void Component::Update(float deltaTime)
{

}

void Component::Register()
{
	
}

void Component::Unregister()
{
}

void Component::OnUpdatedTransform()
{

}

void Component::AttachComponent(Component* comp)
{
	comp->SetOwner(this);
	mComponents.push_back(comp);

	comp->ComputeWorldTransform();
}

Component* Component::GetChildComponent(unsigned index)
{
	if (index < mComponents.size())
	{
		return mComponents.at(index);
	}
	else
	{
		return NULL;
	}
}

Matrix4 Component::GetWorldTransform()
{
	if (isTransformDirty)
		ComputeWorldTransform();

	return mWorldTransform;
}

void Component::SetScale(float scale)
{
	mScale = scale;
	ComputeWorldTransform();
}

void Component::SetRotation(const Quaternion& rot)
{
	mRotation = rot;
	ComputeWorldTransform();
}

void Component::SetPosition(const Vector3& pos)
{ 
	mPosition = pos; 
	ComputeWorldTransform(); 
}

Vector3 Component::GetPosition() const
{
	return mWorldTransform.GetTranslation();
}

void Component::ComputeWorldTransform()
{
	mWorldTransform = Matrix4::CreateScale(mScale) * Matrix4::CreateFromQuat(mRotation) * Matrix4::CreateTranslation(mPosition);

	if (mOwner)
	{
		mWorldTransform = mWorldTransform * mOwner->GetWorldTransform();
	}

	for (auto& comp : mComponents)
	{
		comp->ComputeWorldTransform();
	}
}

void Component::AddRotation(const Quaternion& rot)
{
	mRotation = Concatenate(mRotation, rot);
	ComputeWorldTransform();
}