#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>

#include <vector>
#include <list>

class Actor;
class Compiler;

class Coord
{
public:
    Coord(int r, int c);
    int getRow() const;
    int getCol() const;
private:
    int m_row;
    int m_col;
};

class StudentWorld : public GameWorld
{
public:
    
    StudentWorld(std::string assetDir);
    virtual ~StudentWorld();
    
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    // Can an insect move to x,y?
    bool canMoveTo(int x, int y) const;
    
    // Add an actor to the world
    void addActor(Actor* a);
    
    // If an item that can be picked up to be eaten is at x,y, return a
    // pointer to it; otherwise, return a null pointer.  (Edible items are
    // only ever going be food.)
    Actor* getEdibleAt(int x, int y) const;
    
    // If a pheromone of the indicated colony is at x,y, return a pointer
    // to it; otherwise, return a null pointer.
    Actor* getPheromoneAt(int x, int y, int colony) const;
    
    // Is an enemy of an ant of the indicated colony at x,y?
    bool isEnemyAt(int x, int y, int colony) const;
    
    // Is something dangerous to an ant of the indicated colony at x,y?
    bool isDangerAt(int x, int y, int colony) const;
    
    // Is the anthill of the indicated colony at x,y?
    bool isAntHillAt(int x, int y, int colony) const;
    
    // Bite an enemy of an ant of the indicated colony at me's location
    // (other than me; insects don't bite themselves).  Return true if an
    // enemy was bitten.
    bool biteEnemyAt(Actor* me, int colony, int biteDamage);
    
    // Poison all poisonable actors at x,y.
    bool poisonAllPoisonableAt(int x, int y);
    
    // Stun all stunnable actors at x,y.
    bool stunAllStunnableAt(int x, int y);
    
    // Record another ant birth for the indicated colony.
    void increaseScore(int colony);
    
    //get the clear location that the grasshopper can jump to
    void getClearLoc(Actor* actor, int des[], int size);
    
    void displayGameStat();
private:
    Compiler *compilerForEntrant0, *compilerForEntrant1, *compilerForEntrant2, *compilerForEntrant3;
    int m_score [4][2];
    std::vector<std::vector< Coord>> m_coordinate;
    std::list<Actor*> m_virtualField [VIEW_HEIGHT][VIEW_WIDTH];
    int tick;
    int m_winningAntIndex;
};

#endif // STUDENTWORLD_H_
