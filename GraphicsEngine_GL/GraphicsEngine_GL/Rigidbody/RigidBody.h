
#pragma once

#include "Component/SerializableComponent.h"
#include <btBulletDynamicsCommon.h>
#include <memory>

namespace Assets {
    class RigidBody;
}

class PhysicsWorld;

class RigidBody : public SerializableComponent<RigidBody>
{
public:
    RigidBody();
    ~RigidBody();

    float mass = 1.0f;
    float friction = 0.5f;
    float restitution = 0.0f;

    bool useGravity = true;
    bool LockObject = false;

    [[nodiscard]] const char* GetTypeName() const override
    {
        return "RigidBody";
    }

    template<class Visitor>
    void Fields(Visitor& visitor)
    {
        VisitFields(*this, visitor);
    }

    template<class Visitor>
    void Fields(Visitor& visitor) const
    {
        VisitFields(*this, visitor);
    }

    void Update();

    bool InitializePhysics();
    void CleanUpPhysics();
    void SyncTransform();

    void AddForce(const btVector3& force);
    void AddImpulse(const btVector3& impulse);

    btRigidBody* GetBody() const
    {
        return body.get();
    }

private:
    PhysicsWorld* physicsWorld = nullptr;

    std::unique_ptr<btBoxShape> collisionShape;
    std::unique_ptr<btDefaultMotionState> motionState;
    std::unique_ptr<btRigidBody> body;

    float lastMass = -1.0f;
    float lastWidth = -1.0f;
    float lastHeight = -1.0f;
    float lastDepth = -1.0f;

    bool lastLockObject = false;
    bool lastUseGravity = true;

    bool CreateBody(const btTransform& transform);
    void RebuildPhysics();

    void ApplyProperties();
    void UpdateGravity();
    bool NeedsRebuild();
    void CacheProperties();

    template<class Self, class Visitor>
    static void VisitFields(Self& self, Visitor& visitor)
    {
        visitor("mass", self.mass);
        visitor("friction", self.friction);
        visitor("restitution", self.restitution);
        visitor("useGravity", self.useGravity);
        visitor("LockObject", self.LockObject);
    }
};