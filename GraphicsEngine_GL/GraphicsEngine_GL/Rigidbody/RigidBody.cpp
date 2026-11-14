
#include "RigidBody.h"

#include "../BoxCollider/BoxCollider.h"
#include "../SphereCollider/SphereCollider.h"
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

    btQuaternion initialRotation;
    initialRotation.setEulerZYX(
        btRadians(object->rotation.z),
        btRadians(object->rotation.y),
        btRadians(object->rotation.x)
    );
    transform.setRotation(initialRotation);

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

    // Un sol btRigidBody fa servir una forma activa, pero conservem dos punters separats.
    collisionShape.reset();
    sphereShape.reset();

    btCollisionShape* activeShape = nullptr;

    if (auto* box = object->GetComponent<BoxCollider>())
    {
        collisionShape = std::make_unique<btBoxShape>(
            btVector3(
                std::max(0.001f, box->width * 0.5f),
                std::max(0.001f, box->height * 0.5f),
                std::max(0.001f, box->depth * 0.5f)
            )
        );
        activeShape = collisionShape.get();
    }
    else if (auto* sphere = object->GetComponent<SphereCollider>())
    {
        sphereShape = std::make_unique<btSphereShape>(
            std::max(0.001f, sphere->radius)
        );
        activeShape = sphereShape.get();
    }
    else
    {
        return false;
    }

    motionState = std::make_unique<btDefaultMotionState>(
        transform
    );

    const btScalar bodyMass =
        LockObject ? btScalar(0.0f)
                   : btScalar(std::max(0.0f, mass));

    btVector3 inertia(0, 0, 0);

    if (bodyMass > 0.0f)
    {
        activeShape->calculateLocalInertia(
            bodyMass,
            inertia
        );
    }

    btRigidBody::btRigidBodyConstructionInfo info(
        bodyMass,
        motionState.get(),
        activeShape,
        inertia
    );

    body = std::make_unique<btRigidBody>(info);

    if (bodyMass > 0.0f)
    {
        body->setLinearFactor(btVector3(1, 1, 1));
        body->setAngularFactor(btVector3(1, 1, 1));

        body->setRollingFriction(0.0f);
        body->setSpinningFriction(0.0f);

        body->setDamping(0.0f, 0.0f);
        body->setSleepingThresholds(0.0f, 0.0f);

        body->activate(true);
    }

    physicsWorld->AddRigidBody(body.get());

    const btScalar contactFriction = sphereShape
        ? btScalar(std::max(0.8f, friction))
        : (slideOnSlopes ? btScalar(0.25f)
                         : btScalar(std::max(0.0f, friction)));
    body->setFriction(contactFriction);
    body->setRestitution(restitution);
    UpdateGravity();

    CacheProperties();

    lastSyncedPosition = transform.getOrigin();
    lastSyncedRotation = btVector3(
        object->rotation.x, object->rotation.y, object->rotation.z
    );
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

    if (!object || (!object->GetComponent<BoxCollider>() &&
                    !object->GetComponent<SphereCollider>()))
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
    ApplySlopeAcceleration();
}

void RigidBody::ApplyProperties()
{
    if (!body)
        return;

    const btScalar contactFriction = sphereShape
        ? btScalar(std::max(0.8f, friction))
        : (slideOnSlopes ? btScalar(0.25f)
                         : btScalar(std::max(0.0f, friction)));
    body->setFriction(contactFriction);
    body->setRestitution(restitution);

    if (useGravity != lastUseGravity)
    {
        UpdateGravity();
        lastUseGravity = useGravity;
    }
}

void RigidBody::ApplySlopeAcceleration()
{
    if (!body || !sphereShape || !physicsWorld ||
        !useGravity || slopeAcceleration <= 0.0f ||
        body->isStaticOrKinematicObject())
        return;

    btDiscreteDynamicsWorld* world = physicsWorld->GetWorld();
    if (!world)
        return;

    const btVector3 gravity = body->getGravity();
    if (gravity.length2() < btScalar(1e-8f))
        return;

    const btVector3 up = -gravity.normalized();
    btVector3 bestNormal(0, 0, 0);
    btScalar bestUpDot = btScalar(0.15f);

    btDispatcher* dispatcher = world->getDispatcher();
    for (int i = 0; i < dispatcher->getNumManifolds(); ++i)
    {
        btPersistentManifold* manifold =
            dispatcher->getManifoldByIndexInternal(i);

        const btCollisionObject* objectA =
            static_cast<const btCollisionObject*>(manifold->getBody0());
        const btCollisionObject* objectB =
            static_cast<const btCollisionObject*>(manifold->getBody1());

        if (objectA != body.get() && objectB != body.get())
            continue;

        for (int j = 0; j < manifold->getNumContacts(); ++j)
        {
            const btManifoldPoint& contact = manifold->getContactPoint(j);
            if (contact.getDistance() > btScalar(0.01f))
                continue;

            btVector3 normal = (objectA == body.get())
                ? contact.m_normalWorldOnB
                : -contact.m_normalWorldOnB;

            if (normal.length2() < btScalar(1e-8f))
                continue;

            normal.normalize();
            const btScalar upDot = normal.dot(up);

            if (upDot > bestUpDot && upDot < btScalar(0.999f))
            {
                bestUpDot = upDot;
                bestNormal = normal;
            }
        }
    }

    if (bestNormal.length2() < btScalar(0.5f))
        return;

    const btVector3 tangentGravity =
        gravity - bestNormal * gravity.dot(bestNormal);

    if (tangentGravity.length2() < btScalar(1e-8f))
        return;

    const btScalar inverseMass = body->getInvMass();
    if (inverseMass <= btScalar(0))
        return;

    body->applyCentralForce(
        tangentGravity * (btScalar(slopeAcceleration) / inverseMass)
    );
    body->activate(true);
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

    bool colliderModified = false;

    if (auto* box = object->GetComponent<BoxCollider>())
    {
        colliderModified = !collisionShape || sphereShape ||
            box->width != lastWidth ||
            box->height != lastHeight ||
            box->depth != lastDepth;
    }
    else if (auto* sphere = object->GetComponent<SphereCollider>())
    {
        colliderModified = !sphereShape || collisionShape ||
            sphere->radius != lastRadius;
    }
    else
    {
        return false;
    }

    return mass != lastMass || LockObject != lastLockObject || colliderModified;
}

void RigidBody::CacheProperties()
{
    GameObject* object = GetAttachedGameObject();

    if (!object)
        return;

    lastMass = mass;
    lastLockObject = LockObject;
    lastUseGravity = useGravity;

    if (auto* box = object->GetComponent<BoxCollider>())
    {
        lastWidth = box->width;
        lastHeight = box->height;
        lastDepth = box->depth;
    }
    else if (auto* sphere = object->GetComponent<SphereCollider>())
    {
        lastRadius = sphere->radius;
    }
}

void RigidBody::StopPhysics()
{
    if (!body)
        return;

    body->setLinearVelocity(btVector3(0, 0, 0));
    body->setAngularVelocity(btVector3(0, 0, 0));
    body->clearForces();
}

void RigidBody::RebuildPhysics()
{
    if (!body)
    {
        InitializePhysics();
        return;
    }

    if (!physicsWorld || !physicsWorld->GetWorld())
        return;

    btDiscreteDynamicsWorld* world =
        physicsWorld->GetWorld();

    const btTransform transform =
        body->getWorldTransform();

    const bool wasDynamic =
        !body->isStaticOrKinematicObject();

    const btVector3 linearVelocity =
        wasDynamic && !LockObject
            ? body->getLinearVelocity()
            : btVector3(0, 0, 0);

    const btVector3 angularVelocity =
        wasDynamic && !LockObject
            ? body->getAngularVelocity()
            : btVector3(0, 0, 0);

    PhysicsWorld* savedWorld = physicsWorld;

    CleanUpPhysics();

    physicsWorld = savedWorld;

    if (!CreateBody(transform))
    {
        physicsWorld = nullptr;
        return;
    }

    if (!LockObject &&
        !body->isStaticOrKinematicObject())
    {
        body->setLinearFactor(btVector3(1, 1, 1));
        body->setAngularFactor(btVector3(1, 1, 1));

        body->setLinearVelocity(linearVelocity);
        body->setAngularVelocity(angularVelocity);
    }

    world->updateSingleAabb(body.get());

    const int numObjects =
        world->getNumCollisionObjects();

    for (int i = 0; i < numObjects; ++i)
    {
        btCollisionObject* collisionObject =
            world->getCollisionObjectArray()[i];

        btRigidBody* otherBody =
            btRigidBody::upcast(collisionObject);

        if (!otherBody)
            continue;

        if (otherBody == body.get())
            continue;

        if (otherBody->isStaticOrKinematicObject())
            continue;

        otherBody->activate(true);
    }

    if (!body->isStaticOrKinematicObject())
        body->activate(true);

    lastSyncedPosition = transform.getOrigin();
    if (GameObject* object = GetAttachedGameObject())
        lastSyncedRotation = btVector3(
            object->rotation.x, object->rotation.y, object->rotation.z
        );
    hasSyncedPosition = true;
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
    sphereShape.reset();

    physicsWorld = nullptr;

    lastSyncedPosition = btVector3(0, 0, 0);
    lastSyncedRotation = btVector3(0, 0, 0);
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
        object->position.x, object->position.y, object->position.z
    );
    const btVector3 editorRotation(
        object->rotation.x, object->rotation.y, object->rotation.z
    );

    if (!hasSyncedPosition)
    {
        lastSyncedPosition = editorPosition;
        lastSyncedRotation = editorRotation;
        hasSyncedPosition = true;
        return;
    }

    constexpr btScalar epsilon2 = btScalar(0.000001);
    const bool positionChanged =
        (editorPosition - lastSyncedPosition).length2() > epsilon2;
    const bool rotationChanged =
        (editorRotation - lastSyncedRotation).length2() > epsilon2;

    if (!positionChanged && !rotationChanged)
        return;

    btTransform transform = body->getWorldTransform();

    if (positionChanged)
        transform.setOrigin(editorPosition);

    if (rotationChanged)
    {
        btQuaternion rotation;
        rotation.setEulerZYX(
            btRadians(editorRotation.z()),
            btRadians(editorRotation.y()),
            btRadians(editorRotation.x())
        );
        transform.setRotation(rotation);
    }

    body->setWorldTransform(transform);
    body->setInterpolationWorldTransform(transform);
    if (body->getMotionState())
        body->getMotionState()->setWorldTransform(transform);

    body->setLinearVelocity(btVector3(0, 0, 0));
    body->setAngularVelocity(btVector3(0, 0, 0));
    body->clearForces();

    if (physicsWorld && physicsWorld->GetWorld())
    {
        auto* world = physicsWorld->GetWorld();
        world->updateSingleAabb(body.get());

        btBroadphaseProxy* proxy = body->getBroadphaseHandle();
        if (proxy)
        {
            auto* pairCache = world->getBroadphase()->getOverlappingPairCache();
            btBroadphasePairArray& pairs = pairCache->getOverlappingPairArray();

            for (int i = 0; i < pairs.size(); ++i)
            {
                const btBroadphasePair& pair = pairs[i];
                btBroadphaseProxy* otherProxy = nullptr;

                if (pair.m_pProxy0 == proxy)
                    otherProxy = pair.m_pProxy1;
                else if (pair.m_pProxy1 == proxy)
                    otherProxy = pair.m_pProxy0;

                if (!otherProxy)
                    continue;

                auto* collisionObject =
                    static_cast<btCollisionObject*>(otherProxy->m_clientObject);
                btRigidBody* otherBody = btRigidBody::upcast(collisionObject);

                if (otherBody && otherBody != body.get() &&
                    !otherBody->isStaticOrKinematicObject())
                    otherBody->activate(true);
            }
        }
    }

    body->activate(true);
    lastSyncedPosition = editorPosition;
    lastSyncedRotation = editorRotation;
    hasSyncedPosition = true;
}

void RigidBody::SyncTransform()
{
    if (!body || body->isStaticOrKinematicObject())
        return;

    GameObject* object = GetAttachedGameObject();
    if (!object)
        return;

    const btTransform& transform = body->getWorldTransform();
    const btVector3 position = transform.getOrigin();

    object->position.x = position.x();
    object->position.y = position.y();
    object->position.z = position.z();

    btScalar angleZ, angleY, angleX;
    transform.getRotation().getEulerZYX(angleZ, angleY, angleX);

    object->rotation.x = btDegrees(angleX);
    object->rotation.y = btDegrees(angleY);
    object->rotation.z = btDegrees(angleZ);

    lastSyncedPosition = position;
    lastSyncedRotation = btVector3(
        object->rotation.x, object->rotation.y, object->rotation.z
    );
    hasSyncedPosition = true;
}

void RigidBody::AddForce(const btVector3& force)
{
    if (!body || body->isStaticOrKinematicObject())
        return;

    body->activate(true);
    body->applyCentralForce(force);
}

void RigidBody::SetVelocity(const btVector3 &velocity)
{
    if (!body || body->isStaticOrKinematicObject())
        return;

    body->activate(true);
    body->setLinearVelocity(velocity);
}

void RigidBody::LockRigidbody(bool lock)
{

    if (LockObject == lock)
        return;

    LockObject = lock;
    RebuildPhysics();
}

Vector3D RigidBody::TransformDirection(const btVector3& direction)
{
    btQuaternion rotation = body->getWorldTransform().getRotation();
    btVector3 result = quatRotate(rotation, direction);

    return Vector3D(result.x(), result.y(), result.z());
}

void RigidBody::AddImpulse(const btVector3& impulse)
{
    if (!body || body->isStaticOrKinematicObject())
        return;

    body->activate(true);
    body->applyCentralImpulse(impulse);
}