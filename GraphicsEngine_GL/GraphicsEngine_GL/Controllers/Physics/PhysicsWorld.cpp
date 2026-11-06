#include "PhysicsWorld.h"

#include <iostream>

PhysicsWorld::~PhysicsWorld()
{
    CleanUp();
}

bool PhysicsWorld::Init()
{
    if (dynamicsWorld)
        return true;

    collisionConfig =
        std::make_unique<btDefaultCollisionConfiguration>();

    dispatcher =
        std::make_unique<btCollisionDispatcher>(
            collisionConfig.get()
        );

    broadphase =
        std::make_unique<btDbvtBroadphase>();

    solver = std::make_unique<btSequentialImpulseConstraintSolver>();

    dynamicsWorld =
        std::make_unique<btDiscreteDynamicsWorld>(
            dispatcher.get(),
            broadphase.get(),
            solver.get(),
            collisionConfig.get()
        );

    SetGravity(0.0f, -9.81f, 0.0f);

    std::cout
        << "[PhysicsWorld] Bullet Physics initialized!"
        << std::endl;

    return true;
}

void PhysicsWorld::Update(float fixedDeltaTime)
{
    if (!dynamicsWorld || fixedDeltaTime <= 0.0f)
        return;

    dynamicsWorld->stepSimulation(
        fixedDeltaTime,
        0
    );
}

void PhysicsWorld::CleanUp()
{
    if (!dynamicsWorld)
        return;

    for (int i = dynamicsWorld->getNumCollisionObjects() - 1;
         i >= 0;
         --i)
    {
        btCollisionObject* object =
            dynamicsWorld->getCollisionObjectArray()[i];

        dynamicsWorld->removeCollisionObject(object);
    }

    dynamicsWorld.reset();

    solver.reset();
    broadphase.reset();
    dispatcher.reset();
    collisionConfig.reset();

    std::cout
        << "[PhysicsWorld] Physics cleaned up!"
        << std::endl;
}

void PhysicsWorld::SetGravity(float x, float y, float z)
{
    if (!dynamicsWorld)
        return;

    dynamicsWorld->setGravity(btVector3(x, y, z));
}

btVector3 PhysicsWorld::GetGravity() const
{
    if (!dynamicsWorld)
        return btVector3(0, 0, 0);

    return dynamicsWorld->getGravity();
}

void PhysicsWorld::AddRigidBody(btRigidBody* body)
{
    if (!dynamicsWorld || !body)
        return;

    dynamicsWorld->addRigidBody(body);
}

void PhysicsWorld::RemoveRigidBody(btRigidBody* body)
{
    if (!dynamicsWorld || !body)
        return;

    dynamicsWorld->removeRigidBody(body);
}
