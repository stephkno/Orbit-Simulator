#pragma once
#include <ode/ode.h>
#include <ode/odemath.h>
#include <ode/collision.h>
#include <ode/objects.h>
#include <editor.h>
#include <render/mesh.h>
#include <glm/glm.hpp>

struct CollisionData {
  dWorldID world;
  dJointGroupID contact_group;
  CollisionData(dWorldID w, dJointGroupID c){
    world = w;
    contact_group = c;
  }
};

class Physics{
public:

  dReal density = 1.0;
  dReal radius = 0.3;
  double restitution = 0.9;
  double damping = 0.1;
  dReal starting_height = 10.0;
  dReal gravity_y = -9.81;
  const std::string name{"sphere1"};
  dWorldID world;
  dSpaceID space;
  dJointGroupID collision_contact_group;
  CollisionData * collision_data;

  Physics(){

  }
  ~Physics(){

  }
  void Init()
  {

    //
    // Initialize ODE, create the world, and create the collision space
    //   This must be done first.
    //

    dInitODE2(0);

    world = dWorldCreate();
    dWorldSetGravity(world, 0.0, gravity_y, 0.0);

    space = dSimpleSpaceCreate(0);
    collision_contact_group = dJointGroupCreate(0);
    collision_data = new CollisionData(world, collision_contact_group);

    dGeomID ground_geom = dCreatePlane(space, 0, 1, 0, 0);

    //
    // Create the sphere object
    //
    std::cout << "Begin physics" << std::endl;

  }
  /*
  void Test(){
    dTriMeshDataID TriMeshData;
    TriMeshData = dGeomTriMeshGetTriMeshDataID( 0 );

    // as long as dReal == floats
    dGeomTriMeshDataBuildSingle(TriMeshData,
                            // Vertices
                            v,
                            3*sizeof(dReal), (int) 0,
                            // Faces
                            i,
                            (int) 0, 3*sizeof(unsigned int),
                            // Normals
                            n);
  }
  */
  void CreateSphere(){
    // Rigid body for dynamics
    dBodyID sphere = dBodyCreate(world);
    dBodySetPosition(sphere, 0.0, starting_height, 0.0);

    // Mass
    dMass sphere_mass;
    dMassSetSphere(&sphere_mass, density, radius);
    dBodySetMass(sphere, &sphere_mass);

    // Geometry for collisions
    dGeomID sphere_geom = dCreateSphere(space, radius);
    dGeomSetBody(sphere_geom, sphere);

  }
  static void handle_collisions(void *data, dGeomID geom1, dGeomID geom2)
  {
    auto collision_data = static_cast<CollisionData*>(data);

    // Get the rigid bodies associated with the geometries
    dBodyID body1 = dGeomGetBody(geom1);
    dBodyID body2 = dGeomGetBody(geom2);

    // Maximum number of contacts to create between bodies (see ODE documentation)
    const int MAX_NUM_CONTACTS = 8;
    dContact contacts[MAX_NUM_CONTACTS];

    // Add collision joints
    std::cout  << geom1 << std::endl;
    std::cout  << geom2 << std::endl;
    std::cout  << &contacts[0].geom << std::endl;
    int numc = dCollide(geom1, geom2, MAX_NUM_CONTACTS, &contacts[0].geom, sizeof(dContact));
    std::cout  << "X" << std::endl;

    for (int i = 0; i < numc; ++i) {

      contacts[i].surface.mode = dContactSoftERP | dContactSoftCFM | dContactApprox1 |
          dContactSlip1 | dContactSlip2;

      contacts[i].surface.mu = 50.0;
      contacts[i].surface.soft_erp = 0.96;
      contacts[i].surface.soft_cfm = 2.00;

      // struct dSurfaceParameters {
      //      int mode;
      //      dReal mu;
      //      dReal mu2;
      //      dReal rho;
      //      dReal rho2;
      //      dReal rhoN;
      //      dReal bounce;
      //      dReal bounce_vel;
      //      dReal soft_erp;
      //      dReal soft_cfm;
      //      dReal motion1, motion2, motionN;
      //      dReal slip1, slip2;
      // };

      dJointID contact = dJointCreateContact(collision_data->world,
          collision_data->contact_group, &contacts[i]);

      dJointAttach(contact, body1, body2);
    }
  }
  void Step(float deltaTime){
    dSpaceCollide(space, &collision_data, &handle_collisions);
    dWorldStep(world, static_cast<dReal>(deltaTime));
    dJointGroupEmpty(collision_contact_group);
  }
  void GetTransform(dBodyID body){
    dBodyGetPosition(body);
  }


  // Check ray collision against a space
  static void RayCallback(void *Data, dGeomID Geometry1, dGeomID Geometry2) {
      const int MAX_CONTACTS = 32;

      dReal *HitPosition = (dReal *)Data;
      // Check collisions
      dContact Contacts[MAX_CONTACTS];
      int Count = dCollide(Geometry1, Geometry2, MAX_CONTACTS, &Contacts[0].geom, sizeof(dContact));
      for(int i = 0; i < Count; i++) {

          // Check depth against current closest hit
          if(Contacts[i].geom.depth < HitPosition[3]) {
              dCopyVector3(HitPosition, Contacts[i].geom.pos);
              HitPosition[3] = Contacts[i].geom.depth;
          }
      }
  }
  void AddMesh(Mesh * mesh){

    int nVertices = mesh->vertices.size();
    int nIndices = mesh->indices.size();

    dVector3 vertices[nVertices];
    uint indices[nIndices];

    uint c;
    for(uint c = 0; c < mesh->vertices.size(); c++){
      std::cout << c << std::endl;
      glm::vec3 v = mesh->vertices.at(c).Position;
      vertices[c][0] = (double)v.x;
      vertices[c][1] = (double)v.y;
      vertices[c][2] = (double)v.z;
      c+=3;
    }
    c=0;
    for(uint i : mesh->indices){
      indices[c++] = i;
    }
    // create ode objects
    dGeomID plane;
    dTriMeshDataID triMesh;
    // set mesh data
    triMesh = dGeomTriMeshDataCreate();
    dGeomTriMeshDataBuildSimple(triMesh, (dReal*)vertices, nVertices, indices, nIndices);
    plane = dCreateTriMesh(space, triMesh, NULL, NULL, NULL);

    // set mesh data parameters
    dGeomSetData(plane, (void*)"Plane");
    dGeomSetPosition(plane, 0.0, 0.0, 0.0);
  }
  bool RayCast(glm::vec3 Origin, glm::vec3 End){

        dVector3 o;
        o[0] = Origin.x;
        o[1] = Origin.y;
        o[2] = Origin.z;

        dVector3 d;
        d[0] = End.x;
        d[1] = End.y;
        d[2] = End.z;

        return RaycastQuery(space, o, d);
  }

  // Performs raycasting on a space and returns the point of collision. Return false for no hit.
  static bool RaycastQuery(dSpaceID space, const dVector3 Start, const dVector3 End) {

    // Calculate direction
    dVector3 Direction;
    dSubtractVectors3(Direction, Start, End);

    // Get length
    dReal Length = dCalcVectorLength3(Direction);
    dReal InverseLength = dRecip(Length);

    // Normalize
    dScaleVector3(Direction, InverseLength);

    // Create ray
    dGeomID Ray = dCreateRay(0, Length);
    dGeomRaySet(Ray, Start[0], Start[1], Start[2], Direction[0], Direction[1], Direction[2]);

    // Check collisions
    dVector4 HitPosition;
    HitPosition[3] = dInfinity;
    dSpaceCollide2(Ray, (dGeomID)space, HitPosition, &RayCallback);

    // Cleanup
    dGeomDestroy(Ray);

    // Check for hit
    if(HitPosition[3] != dInfinity) {
        dCopyVector3((double*)End, HitPosition);
        return true;
    }

    return false;
  }
  //
  // Cleanup
  //
  void Finish(){
    // TODO:AJC other destroy
    dSpaceDestroy(space);
    dWorldDestroy(world);
    dCloseODE();
  }
};
