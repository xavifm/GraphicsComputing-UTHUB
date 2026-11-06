
#include "RigidBody.h"
#include "../BoxCollider/BoxCollider.h"
#include "Object/GameObject/GameObject.h"
#include "Controllers/Worlds/WorldController.h"

#include <algorithm>

RigidBody::RigidBody()
{
}

RigidBody::~RigidBody()
{
    CleanUpPhysics();
}

bool RigidBody::InitializePhysics()
{
    if (body)
        return true;

    GameObject* object = GetAttachedGameObject();

    if (!object || !object->worldController)
        return false;

    auto* physics =
        object->worldController->GetPhysicsWorld();

    if (!physics || !physics->GetWorld())
        return false;

    BoxCollider* collider =
        object->GetComponent<BoxCollider>();

    if (!collider)
        return false;

    const float halfWidth =
        std::max(0.001f, collider->width * 0.5f);

    const float halfHeight =
        std::max(0.001f, collider->height * 0.5f);

    const float halfDepth =
        std::max(0.001f, collider->depth * 0.5f);

    collisionShape = std::make_unique<btBoxShape>(
        btVector3(halfWidth, halfHeight, halfDepth)
    );

    btTransform transform;
    transform.setIdentity();

    transform.setOrigin(btVector3(
        object->position.x,
        object->position.y,
        object->position.z
    ));

    motionState = std::make_unique<btDefaultMotionState>(transform);

    const btScalar bodyMass = std::max(0.0f, mass);

    btVector3 inertia(0, 0, 0);

    if (bodyMass > 0.0f)
    {
        collisionShape->calculateLocalInertia(
            bodyMass,
            inertia
        );
    }

    btRigidBody::btRigidBodyConstructionInfo info(
        bodyMass,
        motionState.get(),
        collisionShape.get(),
        inertia
    );

    body = std::make_unique<btRigidBody>(info);

    body->setFriction(friction);
    body->setRestitution(restitution);

    physics->AddRigidBody(body.get());
    physicsWorld = physics;

    if (!useGravity)
    {
        body->setGravity(btVector3(0, 0, 0));
        body->setFlags(
            body->getFlags() | BT_DISABLE_WORLD_GRAVITY
        );
    }

    if (LockObject)
    {
        body->setLinearVelocity(btVector3(0, 0, 0));
        body->setAngularVelocity(btVector3(0, 0, 0));

        body->setLinearFactor(btVector3(0, 0, 0));
        body->setAngularFactor(btVector3(0, 0, 0));

        body->clearForces();
    }
    else
    {
        body->setLinearFactor(btVector3(1, 1, 1));
        body->setAngularFactor(btVector3(1, 1, 1));
    }

    return true;
}

void RigidBody::Update()
{
    SerializableComponent<RigidBody>::Update();

    //paràmetres s'han d'actualitzar cada frame

    if (!body)
        InitializePhysics();

    if (body)
    {
        body->setFriction(friction);
        body->setRestitution(restitution);
    }
}

void RigidBody::CleanUpPhysics()
{
    if (body && physicsWorld)
    {
        physicsWorld->RemoveRigidBody(body.get());
    }

    body.reset();
    motionState.reset();
    collisionShape.reset();

    physicsWorld = nullptr;
}

void RigidBody::SyncTransform()
{
    if (!body || body->isStaticOrKinematicObject())
        return;

    GameObject* object = GetAttachedGameObject();

    if (!object)
        return;

    btTransform transform;

    if (body->getMotionState())
    {
        body->getMotionState()->getWorldTransform(
            transform
        );
    }
    else
    {
        transform = body->getWorldTransform();
    }

    const btVector3 position = transform.getOrigin();

    object->position.x = position.x();
    object->position.y = position.y();
    object->position.z = position.z();
}

void RigidBody::AddForce(const btVector3& force)
{
    if (!body || body->isStaticOrKinematicObject())
        return;

    body->activate(true);
    body->applyCentralForce(force);
}

void RigidBody::AddImpulse(const btVector3& impulse)
{
    if (!body || body->isStaticOrKinematicObject())
        return;

    body->activate(true);
    body->applyCentralImpulse(impulse);
}