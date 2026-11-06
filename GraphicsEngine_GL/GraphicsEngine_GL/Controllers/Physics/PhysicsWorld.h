#include <btBulletDynamicsCommon.h>
#include <memory>

class PhysicsWorld
{
public:
    PhysicsWorld() = default;
    ~PhysicsWorld();

    bool Init();

    void Update(float fixedDeltaTime);

    void CleanUp();

    void SetGravity(float x, float y, float z);
    btVector3 GetGravity() const;

    void AddRigidBody(btRigidBody* body);
    void RemoveRigidBody(btRigidBody* body);

    btDiscreteDynamicsWorld* GetWorld() const
    {
        return dynamicsWorld.get();
    }

private:
    std::unique_ptr<btDefaultCollisionConfiguration> collisionConfig;
    std::unique_ptr<btCollisionDispatcher> dispatcher;
    std::unique_ptr<btDbvtBroadphase> broadphase;
    std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
    std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;
};