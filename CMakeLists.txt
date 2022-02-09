cmake_minimum_required(VERSION 3.12)

# Turn on debugging symbols
set(CMAKE_BUILD_TYPE Debug)

# Use C++20
set(CMAKE_CXX_STANDARD 20)

# project name
project(ArmyAntSim)

# specify directory for src includes
include_directories(${PROJECT_SOURCE_DIR}/simulation/src)

# specify top library directory
include_directories(${PROJECT_SOURCE_DIR}/lib)

# specify directories for box2d includes
include_directories(${PROJECT_SOURCE_DIR}/lib/Box2D)
include_directories(${PROJECT_SOURCE_DIR}/lib/Box2D/Collision)
include_directories(${PROJECT_SOURCE_DIR}/lib/Box2D/Collision/Shapes)
include_directories(${PROJECT_SOURCE_DIR}/lib/Box2D/Common)
include_directories(${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics)
include_directories(${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Joints)
include_directories(${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Contacts)
include_directories(${PROJECT_SOURCE_DIR}/lib/Box2D/Rope)

# add the library
add_library(Box2D STATIC
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Common/b2StackAllocator.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Common/b2BlockAllocator.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Common/b2StackAllocator.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Common/b2BlockAllocator.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Common/b2Timer.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Common/b2Draw.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Common/b2Settings.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Common/b2Math.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Collision/Shapes/b2ChainShape.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Collision/Shapes/b2PolygonShape.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Collision/Shapes/b2EdgeShape.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Collision/Shapes/b2CircleShape.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Collision/b2CollideEdge.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Collision/b2TimeOfImpact.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Collision/b2CollidePolygon.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Collision/b2CollideCircle.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Collision/b2BroadPhase.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Collision/b2DynamicTree.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Collision/b2Collision.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Collision/b2Distance.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Rope/b2Rope.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/b2Fixture.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/b2ContactManager.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/b2Island.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/b2Body.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Joints/b2RopeJoint.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Joints/b2GearJoint.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Joints/b2PulleyJoint.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Joints/b2MouseJoint.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Joints/b2PrismaticJoint.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Joints/b2RevoluteJoint.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Joints/b2FrictionJoint.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Joints/b2MotorJoint.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Joints/b2WeldJoint.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Joints/b2Joint.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Joints/b2DistanceJoint.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Joints/b2WheelJoint.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/b2World.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Contacts/b2ContactSolver.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Contacts/b2Contact.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Contacts/b2PolygonContact.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Contacts/b2CircleContact.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.cpp
    ${PROJECT_SOURCE_DIR}/lib/Box2D/Dynamics/b2WorldCallbacks.cpp
    )

# set up the Box2D library
set_target_properties(Box2D PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# add executable
add_executable(ArmyAntSim simulation/src/Config.cpp simulation/src/Demo.cpp simulation/src/helpers.cpp simulation/src/main.cpp simulation/src/RobotController.cpp simulation/src/Robot.cpp simulation/src/CliffTerrain.cpp simulation/src/BoxTerrain.cpp simulation/src/Ramp.cpp simulation/src/V2BLTerrain.cpp simulation/src/VStepper.cpp simulation/src/Vterrain.cpp simulation/src/DefaultTerrain.cpp simulation/src/IslandTerrain.cpp simulation/src/FlatTerrain.cpp)
# link the Box2D library to the ArmyAntSim
target_link_libraries(ArmyAntSim Box2D sfml-audio sfml-network sfml-window sfml-graphics sfml-system stdc++fs)
