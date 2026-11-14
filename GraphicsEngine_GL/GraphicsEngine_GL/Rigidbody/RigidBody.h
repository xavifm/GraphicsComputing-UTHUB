#pragma once

#include "Component/SerializableComponent.h"
#include <btBulletDynamicsCommon.h>
#include <memory>

#include "Globals.h"

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

    // Quan es true, elimina la friccio de contacte per lliscar per pendents.
    bool slideOnSlopes = true;

    // Acceleracio extra en pendents (0 = fisica normal).
    float slopeAcceleration = 2.0f;

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
    void SyncEditorTransform();

    void AddForce(const btVector3& force);
    void SetVelocity(const btVector3& velocity);

    void LockRigidbody(bool lock);

    Vector3D TransformDirection(const btVector3 &direction);

    void AddImpulse(const btVector3& impulse);

    btRigidBody* GetBody() const
    {
        return body.get();
    }

private:
    PhysicsWorld* physicsWorld = nullptr;

    std::unique_ptr<btBoxShape> collisionShape;
    std::unique_ptr<btCollisionShape> sphereShape;
    std::unique_ptr<btDefaultMotionState> motionState;
    std::unique_ptr<btRigidBody> body;

    float lastMass = -1.0f;
    float lastWidth = -1.0f;
    float lastHeight = -1.0f;
    float lastDepth = -1.0f;
    float lastRadius = -1.0f;

    bool lastLockObject = false;
    bool lastUseGravity = true;

    btVector3 lastSyncedPosition{0, 0, 0};
    btVector3 lastSyncedRotation{0, 0, 0};
    bool hasSyncedPosition = false;

    bool CreateBody(const btTransform& transform);
    void RebuildPhysics();
    void StopPhysics();

    void ApplyProperties();
    void ApplySlopeAcceleration();
    void UpdateGravity();
    void ReloadForces(bool stop);
    bool NeedsRebuild();
    void CacheProperties();

    template<class Self, class Visitor>
    static void VisitFields(Self& self, Visitor& visitor)
    {
        visitor("mass", self.mass);
        visitor("friction", self.friction);
        visitor("slideOnSlopes", self.slideOnSlopes);
        visitor("slopeAcceleration", self.slopeAcceleration);
        visitor("restitution", self.restitution);
        visitor("useGravity", self.useGravity);
        visitor("LockObject", self.LockObject);
    }
};
