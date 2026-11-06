
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

    physicsWorld = object->worldController->GetPhysicsWorld();

    if (!physicsWorld || !physicsWorld->GetWorld())
    {
        physicsWorld = nullptr;
        return false;
    }

    btTransform transform;
    transform.setIdentity();

    transform.setOrigin(btVector3(
        object->position.x,
        object->position.y,
        object->position.z
    ));

    if (!CreateBody(transform))
    {
        physicsWorld = nullptr;
        return false;
    }

    return true;
}

bool RigidBody::CreateBody(const btTransform& transform)
{
    GameObject* object = GetAttachedGameObject();

    if (!object || !physicsWorld || !physicsWorld->GetWorld())
        return false;

    auto* collider = object->GetComponent<BoxCollider>();

    if (!collider)
        return false;

    collisionShape = std::make_unique<btBoxShape>(
        btVector3(
            std::max(0.001f, collider->width * 0.5f),
            std::max(0.001f, collider->height * 0.5f),
            std::max(0.001f, collider->depth * 0.5f)
        )
    );

    motionState = std::make_unique<btDefaultMotionState>(
        transform
    );

    const btScalar bodyMass =
        LockObject ? btScalar(0.0f)
                   : btScalar(std::max(0.0f, mass));

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

    physicsWorld->AddRigidBody(body.get());

    body->setFriction(friction);
    body->setRestitution(restitution);
    UpdateGravity();

    CacheProperties();

    lastSyncedPosition = transform.getOrigin();
    hasSyncedPosition = true;

    return true;
}

void RigidBody::Update()
{
    SerializableComponent<RigidBody>::Update();

    if (!body)
    {
        InitializePhysics();
        return;
    }

    GameObject* object = GetAttachedGameObject();

    if (!object || !object->GetComponent<BoxCollider>())
    {
        CleanUpPhysics();
        return;
    }

    SyncEditorTransform();

    if (NeedsRebuild())
    {
        RebuildPhysics();
        return;
    }

    ApplyProperties();
}

void RigidBody::ApplyProperties()
{
    if (!body)
        return;

    body->setFriction(friction);
    body->setRestitution(restitution);

    if (useGravity != lastUseGravity)
    {
        UpdateGravity();
        lastUseGravity = useGravity;
    }
}

void RigidBody::UpdateGravity()
{
    if (!body || !physicsWorld || !physicsWorld->GetWorld())
        return;

    if (useGravity)
    {
        body->setFlags(
            body->getFlags() & ~BT_DISABLE_WORLD_GRAVITY
        );

        body->setGravity(
            physicsWorld->GetWorld()->getGravity()
        );
    }
    else
    {
        body->setFlags(
            body->getFlags() | BT_DISABLE_WORLD_GRAVITY
        );

        body->setGravity(btVector3(0, 0, 0));
    }

    if (!body->isStaticOrKinematicObject())
        body->activate(true);
}

bool RigidBody::NeedsRebuild()
{
    GameObject* object = GetAttachedGameObject();

    if (!object)
        return false;

    auto* collider = object->GetComponent<BoxCollider>();

    if (!collider)
        return false;

    return mass != lastMass ||
           LockObject != lastLockObject ||
           collider->width != lastWidth ||
           collider->height != lastHeight ||
           collider->depth != lastDepth;
}

void RigidBody::CacheProperties()
{
    GameObject* object = GetAttachedGameObject();

    if (!object)
        return;

    auto* collider = object->GetComponent<BoxCollider>();

    if (!collider)
        return;

    lastMass = mass;
    lastLockObject = LockObject;
    lastUseGravity = useGravity;

    lastWidth = collider->width;
    lastHeight = collider->height;
    lastDepth = collider->depth;
}

void RigidBody::RebuildPhysics()
{
    if (!body)
    {
        InitializePhysics();
        return;
    }

    const btTransform transform = body->getWorldTransform();

    const bool wasDynamic =
        !body->isStaticOrKinematicObject();

    const btVector3 linearVelocity =
        wasDynamic ? body->getLinearVelocity()
                   : btVector3(0, 0, 0);

    const btVector3 angularVelocity =
        wasDynamic ? body->getAngularVelocity()
                   : btVector3(0, 0, 0);

    auto* world = physicsWorld;

    CleanUpPhysics();

    physicsWorld = world;

    if (!CreateBody(transform))
    {
        physicsWorld = nullptr;
        return;
    }

    if (wasDynamic && !body->isStaticOrKinematicObject())
    {
        body->setLinearVelocity(linearVelocity);
        body->setAngularVelocity(angularVelocity);
        body->activate(true);
    }
}

void RigidBody::CleanUpPhysics()
{
    if (body && physicsWorld && physicsWorld->GetWorld())
    {
        physicsWorld->RemoveRigidBody(body.get());
    }

    body.reset();
    motionState.reset();
    collisionShape.reset();

    physicsWorld = nullptr;

    lastSyncedPosition = btVector3(0, 0, 0);
    hasSyncedPosition = false;
}

void RigidBody::SyncEditorTransform()
{
    if (!body)
        return;

    GameObject* object = GetAttachedGameObject();

    if (!object)
        return;

    const btVector3 editorPosition(
        object->position.x,
        object->position.y,
        object->position.z
    );

    if (!hasSyncedPosition)
    {
        lastSyncedPosition = editorPosition;
        hasSyncedPosition = true;
        return;
    }

    const btVector3 difference =
        editorPosition - lastSyncedPosition;

    if (difference.length2() < 0.000001f)
        return;

    btTransform transform = body->getWorldTransform();
    transform.setOrigin(editorPosition);

    body->setWorldTransform(transform);
    body->setInterpolationWorldTransform(transform);

    if (body->getMotionState())
    {
        body->getMotionState()->setWorldTransform(transform);
    }

    body->setLinearVelocity(btVector3(0, 0, 0));
    body->setAngularVelocity(btVector3(0, 0, 0));
    body->clearForces();

    if (physicsWorld && physicsWorld->GetWorld())
    {
        physicsWorld->GetWorld()->updateSingleAabb(body.get());
    }

    body->activate(true);

    lastSyncedPosition = editorPosition;
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
        body->getMotionState()->getWorldTransform(transform);
    else
        transform = body->getWorldTransform();

    const btVector3 position = transform.getOrigin();

    object->position.x = position.x();
    object->position.y = position.y();
    object->position.z = position.z();

    lastSyncedPosition = position;
    hasSyncedPosition = true;
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