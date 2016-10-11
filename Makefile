CXX=g++ -std=c++11
CXXFLAGS=`pkg-config opencv --cflags`
LDLIBS=-lglut -lGL -lGLU `pkg-config opencv --libs`
