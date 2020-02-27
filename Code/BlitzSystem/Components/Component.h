// Component.h
// Base class for all Components

#pragma once
#pragma warning(disable: 4251)

#include <memory>
#include <vector>
#include "Math/math.h"
//#include <Containers/TArray.h>
#include "../SystemDefines.h"
//#include "Game.h"

// Forward declaration
//class Actor;
//class Game;

//SYSTEM_EXT_TEMPLATE template class SYSTEM_EXPORT std::vector<Component*>;

class SYSTEM_EXPORT Component
{
public:
	Component(/*class Game& game*/);
	virtual ~Component();

	virtual void Update(float deltaTime);

	virtual void Register();
	virtual void Unregister();

	// This is called when the owner's world transform
	// changes
	virtual void OnUpdatedTransform();
	Matrix4 GetWorldTransform();

	Component* GetOwner() { return mOwner; }
	void SetOwner(Component* owner) { mOwner = owner; }

	float GetScale() { return mScale; }
	void SetScale(float scale);

	Quaternion GetRotation() { return mRotation; }
	void SetRotation(const Quaternion& rot);
	void AddRotation(const Quaternion& rot);

	//Vector3 GetWorldPosition();
	Vector3 GetPosition() const;// { return mPosition; }
	void SetPosition(const Vector3& pos);

	void AttachComponent(Component* comp);
	Component* GetChildComponent(unsigned index);

protected:
	void ComputeWorldTransform();

	//class Game& mGame;
	Component* mOwner;

	Matrix4 mWorldTransform;
	Vector3 mPosition;
	Quaternion mRotation;
	float mScale;
	bool isTransformDirty;

	std::vector<Component*> mComponents;

};



