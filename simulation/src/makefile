# Define compiler
CC=g++

# This will include headers from these directories
CFLAGS= -I ../../lib/Box2D -I ../../lib

# This is where all the source code is for the box2d library
BOX2D_CPP = ../../lib/Box2D/Collision/*.cpp \
            ../../lib/Box2D/Collision/Shapes/*.cpp \
 			../../lib/Box2D/Common/*.cpp \
 			../../lib/Box2D/Dynamics/*.cpp \
 			../../lib/Box2D/Dynamics/Contacts/*.cpp \
 			../../lib/Box2D/Dynamics/Joints/*.cpp \

# Add libraries here that are installed on the system
LIBS=-lsfml-graphics -lsfml-window -lsfml-system

# Tell us where the headers for the CPP files we're compiling are
IDIR = .

# Build run_sim
run_sim:
	$(CC) -o run_sim -w *.cpp $(BOX2D_CPP) $(CFLAGS) $(LIBS)