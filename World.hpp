#ifndef World_hpp
#define World_hpp

class World
{
public:
    World();
    World(World&&) = delete;
    World(const World&&) = delete;
    ~World();

    World& operator=(World&&) = delete;
    World& operator=(const World&) = delete;
};

#endif
