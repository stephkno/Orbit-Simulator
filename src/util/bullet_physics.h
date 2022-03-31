#pragma once

#include "btBulletDynamicsCommon.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "BulletDynamics/Dynamics/btSimpleDynamicsWorld.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "LinearMath/btAlignedAllocator.h"
#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"

#include <stdio.h>
#include <render/mesh.h>


/// This is a Hello World program for running a basic Bullet physics simulation

class Physics{
public:
	///-----includes_end-----

	int i;
	///-----initialization_start-----

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btDefaultCollisionConfiguration* collisionConfiguration;

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher;

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache;

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver;

	btDiscreteDynamicsWorld* dynamicsWorld;
  btAlignedObjectArray<btCollisionShape*> collisionShapes;

	static Physics * physics;

  Physics(){
		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collisionConfiguration);
		overlappingPairCache = new btDbvtBroadphase();
		solver = new btSequentialImpulseConstraintSolver;
		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
		dynamicsWorld->setGravity(btVector3(0, -10, 0));
  }
  ~Physics(){

  }
  static void Init(){
		physics = new Physics();
  }
  void Test(){
  	///-----initialization_end-----

  	//keep track of the shapes, we release memory at exit.
  	//make sure to re-use collision shapes among rigid bodies whenever possible!

  	///create a few basic rigid bodies

  	//the ground is a cube of side 100 at position y = -56.
  	//the sphere will hit it at y = -6, with center at -5
		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

		collisionShapes.push_back(groundShape);

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -56, 0));

		btScalar mass(0.);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);

		delete groundShape;
		delete myMotionState;
	}

  void AddBody(btCollisionShape * shape)
	{
		//create a dynamic rigidbody

		//btCollisionShape* shape = new btBoxShape(btVector3(1,1,1));
		//btCollisionShape* shape = new btSphereShape(btScalar(1.));
		collisionShapes.push_back(shape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(0.f);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic){
      shape->calculateLocalInertia(mass, localInertia);
    }

		startTransform.setOrigin(btVector3(0, 0, 0));

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		dynamicsWorld->addRigidBody(body);
	}

  void Step(float deltaTime){
	/// Do some simulation
	///-----stepsimulation_start-----
		dynamicsWorld->stepSimulation(deltaTime, 10);
  }
  void GetTransform(int i){
		int j = dynamicsWorld->getNumCollisionObjects();
		if(i>j){
			return;
		}
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
		btRigidBody* body = btRigidBody::upcast(obj);
		btTransform trans;
		if (body && body->getMotionState())
		{
			body->getMotionState()->getWorldTransform(trans);
		}
		else
		{
			trans = obj->getWorldTransform();
		}
		printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
	}

	///-----stepsimulation_end-----

	//cleanup in the reverse order of creation/initialization

	///-----cleanup_start-----
  void Finish(){
  	//remove the rigidbodies from the dynamics world and delete them
  	for (i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
  	{
  		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
  		btRigidBody* body = btRigidBody::upcast(obj);
  		if (body && body->getMotionState())
  		{
  			delete body->getMotionState();
  		}
  		dynamicsWorld->removeCollisionObject(obj);
  		delete obj;
  	}

  	//delete collision shapes
  	for (int j = 0; j < collisionShapes.size(); j++)
  	{
  		btCollisionShape* shape = collisionShapes[j];
  		collisionShapes[j] = 0;
  		delete shape;
  	}

  	//delete dynamics world
  	delete dynamicsWorld;

  	//delete solver
  	delete solver;

  	//delete broadphase
  	delete overlappingPairCache;

  	//delete dispatcher
  	delete dispatcher;

  	delete collisionConfiguration;

  	//next line is optional: it will be cleared by the destructor when the array goes out of scope
  	collisionShapes.clear();
  }
  glm::vec3 Raycast(glm::vec3 s, glm::vec3 e)
  {
    btVector3 Start = btVector3(s.x, s.y, s.z);
    btVector3 End = btVector3(e.x, e.y, e.z);
    //btVector3 Normal = btVector3(n.x, n.y, n.z);

    btCollisionWorld::ClosestRayResultCallback RayCallback(Start, End);
    //RayCallback.m_collisionFilterMask = FILTER_CAMERA;

    // Perform raycast
    dynamicsWorld->rayTest(Start, End, RayCallback);
    if(RayCallback.hasHit()) {
            btVector3 hit = RayCallback.m_hitPointWorld;
            //Normal = RayCallback.m_hitNormalWorld;
            glm::vec3 point = glm::vec3(hit.getX(), hit.getY(), hit.getZ());
            return point;
    }
    return glm::vec3(0.0f);
  }
};
