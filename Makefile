CXXFLAGS += -std=c++11
DEBUG_CXXFLAGS += -g -Wall -Werror
ifeq ($(OS),Windows_NT)
	TARGET = kerraria.exe
	CXXFLAGS += -I/mingw64/include/SDL2
	LDLIBS += -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lopengl32 -lglew32
else
	TARGET = kerraria.bin
	CXXFLAGS += -I/usr/include/SDL2
	DEBUG_CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
	DEBUG_LDFLAGS += -fsanitize=address
	LDLIBS += -lSDL2main -lSDL2 -lSDL2_image -lGL -lGLEW
endif
OBJECTS = \
	main.o \
	Debug.o \
	TestHandler.o \
	WindowEventHandler.o

all : debug

debug : CXXFLAGS += $(DEBUG_CXXFLAGS)
debug : LDFLAGS += $(DEBUG_LDFLAGS)
debug : $(TARGET)

release : CXXFLAGS += -O2
release : $(TARGET)

main.o : main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

Debug.o : Debug.cpp Debug.hpp
	$(CXX) $(CXXFLAGS) -c Debug.cpp

TestHandler.o : TestHandler.cpp TestHandler.hpp
	$(CXX) $(CXXFLAGS) -c TestHandler.cpp

WindowEventHandler.o : WindowEventHandler.cpp WindowEventHandler.hpp
	$(CXX) $(CXXFLAGS) -c WindowEventHandler.cpp

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(LDLIBS)

clean :
	rm -f -v *.o *.bin
