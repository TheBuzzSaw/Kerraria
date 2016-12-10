CXXFLAGS += -std=c++11 -I/usr/include/SDL2
LIBS += -lSDL2main -lSDL2 -lSDL2_image -lGL -lGLEW
OBJECTS = \
	main.o \
	Debug.o \
	TestHandler.o \
	WindowEventHandler.o

all : debug

debug : CXXFLAGS += -g -Wall -fsanitize=address -fno-omit-frame-pointer
debug : LDFLAGS += -fsanitize=address
debug : kerraria.bin

release : CXXFLAGS += -O2
release : kerraria.bin

main.o : main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

Debug.o : Debug.cpp Debug.hpp
	$(CXX) $(CXXFLAGS) -c Debug.cpp

TestHandler.o : TestHandler.cpp TestHandler.hpp
	$(CXX) $(CXXFLAGS) -c TestHandler.cpp

WindowEventHandler.o : WindowEventHandler.cpp WindowEventHandler.hpp
	$(CXX) $(CXXFLAGS) -c WindowEventHandler.cpp

kerraria.bin : $(OBJECTS)
	$(CXX) -o kerraria.bin $(OBJECTS) $(LDFLAGS) $(LIBS)

clean :
	rm -f -v *.o *.bin
