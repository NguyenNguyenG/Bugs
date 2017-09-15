#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;
class Compiler;
//========================================ACTOR===============================================
class Actor : public GraphObject
{
public:
    Actor(StudentWorld* world, int startX, int startY, Direction startDir, int imageID, int depth);
    
    //check if an object can be stunned
    virtual bool isStunnablePoisonable() const;
    
    //check if an actor has moved
    bool haveMoved() const;
    
    //set the value of hasMoved of an Actor
    void setHaveMoved(bool nHaveMoved);
    
    // Action to perform each tick.
    virtual void doSomething() = 0;
    
    // Is this actor dead?
    virtual bool isDead() const;
    
    // Does this actor block movement?
    virtual bool blocksMovement() const;
    
    // Cause this actor to be be bitten, suffering an amount of damage.
    virtual void getBitten(int amt);
    
    // Cause this actor to be be poisoned.
    virtual void getPoisoned();
    
    // Cause this actor to be be stunned.
    virtual void getStunned();
    
    // Can this actor be picked up to be eaten?
    virtual bool isEdible() const;
    
    // Is this actor detected by an ant as a pheromone?
    virtual bool isPheromone(int colony) const;
    
    // Is this actor an enemy of an ant of the indicated colony?
    virtual bool isEnemy(int colony) const;
    
    // Is this actor detected as dangerous by an ant of the indicated colony?
    virtual bool isDangerous(int colony) const;
    
    // Is this actor the anthill of the indicated colony?
    virtual bool isAntHill(int colony) const;
    
    // Get this actor's world.
    StudentWorld* getWorld() const;
private:
    StudentWorld* m_world;
    bool m_haveMoved;
};


//========================================PEBBLE===============================================
class Pebble : public Actor
{
public:
    Pebble(StudentWorld* sw, int startX, int startY);
    virtual void doSomething();
    //it does block movement so override
    virtual bool blocksMovement() const;
};


//========================================ENERGYHOLDER===============================================
class EnergyHolder : public Actor
{
public:
    EnergyHolder(StudentWorld* sw, int startX, int startY, Direction startDir, int energy, int imageID, int depth);
    virtual bool isDead() const;
    
    // Get this actor's amount of energy (for a Pheromone, same as strength).
    int getEnergy() const;
    
    // Adjust this actor's amount of energy upward or downward.
    void updateEnergy(int amt);
    
    // Add an amount of food to this actor's location.
    void addFood(int amt);
    
    // Have this actor pick up an amount of food.
    int pickupFood(int amt);
    
    // Have this actor pick up an amount of food and eat it.
    int pickupAndEatFood(int amt);
    
    // Does this actor become food when it dies?
    virtual bool becomesFoodUponDeath() const;
    
    //set the actor to state specified
    void setState(bool nState);
    
    virtual void doSomething();
    
    //change the Compiler
    void updateCompiler(Compiler* nComp);
    
    Compiler* getCompliler() const;
    
    //update the colony
    void updateColony(int nCol);
    
    int getColony() const;
private:
    int m_energy;
    int m_colony;
    Compiler* m_compiler;
    bool m_stateDead;
};



//========================================FOOD===============================================
class Food : public EnergyHolder
{
public:
    Food(StudentWorld* sw, int startX, int startY, int energy);
    virtual void doSomething();
    //override isEdible to return true
    virtual bool isEdible() const;
};



//========================================ANTHILL===============================================
class AntHill : public EnergyHolder
{
public:
    AntHill(StudentWorld* sw, int startX, int startY, int colony, Compiler* program);
    virtual void doSomething();
    //override the isAntHill to return accordingly
    virtual bool isAntHill(int colony) const;
    //return if the indicated colony is ant of this hill
    virtual bool isMyHill(int colony) const;
};


//========================================PHEROMONE===============================================
class Pheromone : public EnergyHolder
{
public:
    Pheromone(StudentWorld* sw, int startX, int startY, int colony, int imageID);
    virtual void doSomething();
    virtual bool isPheromone(int colony) const;
    
    // Increase the strength (i.e., energy) of this pheromone.
    void increaseStrength();
};


//========================================TRIGGERABLEACTOR===============================================
class TriggerableActor : public Actor
{
public:
    TriggerableActor(StudentWorld* sw, int x, int y, int imageID);
    virtual bool isDangerous(int colony) const;
};


//========================================WATERPOOL===============================================
class WaterPool : public TriggerableActor
{
public:
    WaterPool(StudentWorld* sw, int x, int y);
    virtual void doSomething();
};


//========================================POISON===============================================
class Poison : public TriggerableActor
{
public:
    Poison(StudentWorld* sw, int x, int y);
    virtual void doSomething();
};


//========================================INSECT===============================================
class Insect : public EnergyHolder
{
public:
    Insect(StudentWorld* world, int startX, int startY, Direction startDir, int energy, int imageID);
    
    virtual void getBitten(int amt);
    virtual void getPoisoned();
    virtual void getStunned();
    virtual bool isEnemy(int colony) const;
    virtual bool becomesFoodUponDeath() const;

    // Set x,y to the coordinates of the spot one step in front of this insect.
    void getXYInFrontOfMe(int& x, int& y) const;
    
    // Move this insect one step forward if possible, and return true;
    // otherwise, return false without moving.
    virtual bool moveForwardIfPossible();
    
    // Increase the number of ticks this insect will sleep by the indicated amount.
    //void increaseSleepTicks(int amt);
    
    //this will do something that all insect do
    bool prelimStep();
    //return how many tick left untill stun is 0
    int stunLeft() const;
    //override if this object is stunnable
    virtual bool isStunnablePoisonable() const;
    //has it been stunned by water?
    bool stunnedByWater() const;
    //change value hasStunnedByWater
    void setStunnedByWater(bool nStun);
    
private:
    int m_stun;
    int m_stunnedByWater; 
};


//========================================ANT===============================================
class Ant : public Insect
{
public:
    Ant(StudentWorld* sw, int startX, int startY, int colony, Direction startDir, Compiler* program, int imageID);
    virtual void doSomething();
    virtual void getBitten(int amt);
    virtual bool isEnemy(int colony) const;
    virtual bool moveForwardIfPossible();
private:
    int m_foodStorage;
    bool m_prevBitten;
    bool m_prevBlocked;
    int m_lastRandInt;
    int m_instrCounter;
};


//========================================GRASSHOPPER===============================================
class Grasshopper : public Insect
{
public:
    Grasshopper(StudentWorld* sw, int startX, int startY, Direction startDir, int energy, int imageID);
    //get how many steps left until walking is done
    int getWalkDistance() const;
    //something that both adult and baby grasshopper do
    void postStepping();
private:
    int m_walkDistance;
};


//========================================BABYGRASSHOPPER===============================================
class BabyGrasshopper : public Grasshopper
{
public:
    BabyGrasshopper(StudentWorld* sw, int startX, int startY, Direction startDir);
    virtual void doSomething();
};


//========================================ADULTGRASSHOPPER===============================================
class AdultGrasshopper : public Grasshopper
{
public:
    AdultGrasshopper(StudentWorld* sw, int startX, int startY, Direction startDir);
    virtual void getBitten(int amt);
    virtual void doSomething();
    virtual bool isStunnablePoisonable() const;
};

#endif // ACTOR_H_






/*

colony: USCAnt // first line specifies the ant's name

// This program controls a single ant and causes it to move
// around the field and do things.
// This ant moves around randomly, picks up food if it
// happens to stumble upon it, eats when it gets hungry,
// and will drop food on its anthill if it happens to
// stumble back on its anthill while holding food.

// here's the ant's programming instructions, written
// in our 'Bugs' language

start:
emitPheromone
if i_was_blocked_from_moving then goto was_blocked
moveForward
if i_smell_pheromone_in_front_of_me then goto phero_in_front
emitPheromone
if i_am_standing_on_food then goto on_food

if i_am_hungry then goto eat_food
if i_am_standing_on_my_anthill then goto on_hill

goto start 	// jump back to the "start:" line

phero_in_front:
moveForward
if if i_smell_pheromone_in_front_of_me then goto phero_in_front
rotateClockwise
if if i_smell_pheromone_in_front_of_me then goto phero_in_front
rotateCounterClockwise
rotateCounterClockwise
if if i_smell_pheromone_in_front_of_me then goto phero_in_front
if i_am_standing_on_my_anthill then goto on_hill
if i_am_standing_on_food then goto on_food
goto start
was_blocked:
rotateClockwise
goto start
on_food:
pickUpFood
pickUpFood
pickUpFood
emitPheromone
rotateClockwise
rotateClockwise
if if i_smell_pheromone_in_front_of_me then goto phero_in_front
goto start 	// jump back to the "start:" line
eat_food:
eatFood          // assumes our ant has already picked up food
goto start 	// jump back to the "start:" line

on_hill:
dropFood    // feed the anthill's queen so she
rotateCounterClockwise   // can produce more ants for the colony
if if i_smell_pheromone_in_front_of_me then goto phero_in_front
goto start  // jump back to the "start:" line*/
