#include "Actor.h"
#include "StudentWorld.h"

#include "GameConstants.h"
#include "GraphObject.h"
#include "Compiler.h"

//===================================================ACTOR================================================
Actor::Actor(StudentWorld* world, int startX, int startY, Direction startDir, int imageID, int depth)
      :GraphObject(imageID, startX,startY, startDir, depth)
{
    m_world = world;
    m_haveMoved = false;
}
//Most of the functions below return values that are true for actors other than insects(with few exception)
// the name of the function really describes what the function does
bool Actor::isStunnablePoisonable() const
{
    return false;
}

bool Actor::haveMoved() const
{
    return m_haveMoved;
}

void Actor::setHaveMoved(bool nHaveMoved)
{
    m_haveMoved = nHaveMoved;
}
bool Actor::isDead() const
{
    return false;
}

bool Actor::blocksMovement() const
{
    return false;
}

void Actor::getBitten(int amt)
{
    return;
}

void Actor::getPoisoned()
{
    return;
}

void Actor::getStunned()
{
    return;
}

bool Actor::isEdible() const
{
    return false;
}


bool Actor::isPheromone(int colony) const
{
    return false;
}

bool Actor::isEnemy( int colony) const
{
    return false;
}

bool Actor::isDangerous(int colony) const
{
    return false;
}

bool Actor::isAntHill(int colony) const
{
    return false;
}

StudentWorld* Actor::getWorld() const
{
    return m_world;
}

//==============================================PEBBLE==============================================
Pebble::Pebble(StudentWorld* sw, int startX, int startY)
        :Actor(sw, startX, startY,GraphObject::right, IID_ROCK, 1)
{
}

void Pebble::doSomething()
{
    return;
}
//overrride blocksMovement to be true since it is Pebble
bool Pebble::blocksMovement() const
{
    return true;
}

//============================================ENERGYHOLDER===========================================

EnergyHolder::EnergyHolder(StudentWorld* sw, int startX, int startY, Direction startDir, int energy, int imageID, int depth)
        :Actor(sw, startX, startY, startDir, imageID, depth)
{
    m_energy = energy;
    m_colony = -1;
    m_compiler = nullptr;
    m_stateDead = false;
}

//Change the Compiler pointer to the argument passed in
void EnergyHolder::updateCompiler(Compiler* nComp)
{
    m_compiler = nComp;
}

//return the Compiler
Compiler* EnergyHolder::getCompliler() const
{
    return m_compiler;
}

//update the colony of this energy holder (essentially are for ants)
void EnergyHolder::updateColony(int nCol)
{
    m_colony = nCol;
}

//return the colony
int EnergyHolder::getColony() const
{
    return m_colony;
}

//Every round most Energyholder Objects lose 1 health and perform
// a check on their state whether they are dead or not
void EnergyHolder::doSomething()
{
    updateEnergy(-1);
    if(getEnergy() <= 0)
        setState(true);
}

bool EnergyHolder::isDead() const
{
    return m_stateDead;
}

void EnergyHolder::setState(bool nState)
{
    m_stateDead = nState;
}

int EnergyHolder::getEnergy() const
{
    return m_energy;
}

void EnergyHolder::updateEnergy(int amt)
{
    m_energy += amt;
}

//add a certain a mount of food onto this square
void EnergyHolder::addFood(int amt)
{
    if(amt <= 0)
        return;
    Food* temp = new Food(getWorld(), getX(), getY(), amt);
    getWorld() ->addActor(temp);
}

//Pick up a certain amount of food from this square if possible
int EnergyHolder::pickupFood(int amt)
{
    Actor* foodie = getWorld() ->getEdibleAt(getX(), getY());
    //if there is food
    if(foodie != nullptr)
    {
        //pick up all of it if the amount left is less than the amount
        // wanted
        EnergyHolder* nFoodie = static_cast<EnergyHolder*>(foodie);
        int foodAmount = nFoodie -> getEnergy();
        if( foodAmount <= amt)
        {
            nFoodie -> updateEnergy(-foodAmount);
            nFoodie -> setState(true);
            return foodAmount;
        }
        //otherwise pick up only as much food as it can carry
        else
        {
            nFoodie ->updateEnergy(-amt);
            return amt;
        }
    }
    return 0;
}

//Not only picking up the food but it will also eat this amount of food
int EnergyHolder::pickupAndEatFood(int amt)
{
    int amtPickedUp = pickupFood(amt);
    //eat the food
    updateEnergy(amtPickedUp);
    return amtPickedUp;
}

bool EnergyHolder::becomesFoodUponDeath() const
{
    return false;
}

//============================================FOOD===========================================
Food::Food(StudentWorld* sw, int startX, int startY, int energy)
:EnergyHolder(sw, startX, startY, GraphObject::right, energy, IID_FOOD, 2)
{
}

bool Food::isEdible() const
{
    return true;
}

void Food::doSomething()
{
    return;
}

//============================================PHEROMONE===========================================
Pheromone::Pheromone(StudentWorld* sw, int startX, int startY, int colony, int imageID)
:EnergyHolder(sw, startX, startY, GraphObject::right, 256, imageID, 2)
{
    //has to update the colony from the Energy holder
    updateColony(colony);
}

void Pheromone::doSomething()
{
    //call the doSomething from Energyholder
    EnergyHolder::doSomething();
}

bool Pheromone::isPheromone(int colony) const
{
    return getColony() == colony;
}

void Pheromone::increaseStrength()
{
    //max strength can only be 768 so increase accordingly
    if(768 - getEnergy() <= 256)
    {
        updateEnergy(768 - getEnergy());
        return;
    }
    updateEnergy(256);
}


//============================================TriggerableAc===========================================
TriggerableActor::TriggerableActor(StudentWorld* sw, int x, int y, int imageID)
:Actor(sw, x, y, GraphObject::right, imageID, 2)
{
}

bool TriggerableActor::isDangerous(int colony) const
{
    return true;
}


//============================================WaterPool===========================================
WaterPool::WaterPool(StudentWorld* sw, int x, int y)
          :TriggerableActor(sw, x, y,IID_WATER_POOL)
{
}

//StudentWorld will have a better explaination for stunAllStunnableAt
void WaterPool::doSomething()
{
    getWorld() -> stunAllStunnableAt(getX(), getY());
}

//============================================POISON===========================================

Poison::Poison(StudentWorld* sw, int x, int y)
       :TriggerableActor(sw, x, y, IID_POISON)
{
}

//StudentWorld will have a better explaination for PoisonAllPoisonableAt
void Poison::doSomething()
{
    getWorld() -> poisonAllPoisonableAt(getX(), getY());
}

//============================================INSECT===========================================
Insect::Insect(StudentWorld* world, int startX, int startY, Direction startDir, int energy, int imageID)
:EnergyHolder(world, startX, startY, startDir, energy,imageID, 1)
{
    m_stunnedByWater = false;
    m_stun = 0;
}

//all insects are stunnable and poisonable except for Adult GH
bool Insect::isStunnablePoisonable() const
{
    return true;
}

bool Insect::stunnedByWater() const
{
    return m_stunnedByWater;
}

void Insect::setStunnedByWater(bool nStunn)
{
    m_stunnedByWater = nStunn;
}

//Something that all insect do
bool Insect::prelimStep()
{
    EnergyHolder::doSomething();
    //check if state is dead then add food to the square
    if(isDead())
    {
        addFood(100);
        return true;
    }
    //if stun is bigger than 0 then decrement and return
    if(m_stun > 0)
    {
        m_stun--;
        return true;
    }
    return false;
}

int Insect::stunLeft() const
{
    return m_stun;
}
//Override getBitten from the Actor class because Insects
// do get Bitten unlike rock or pheromone,etc.
void Insect::getBitten(int amt)
{
    updateEnergy(-amt);
    if(getEnergy() <= 0)
    {
        setState(true);
        addFood(100);
    }
}

void Insect::getPoisoned()
{
    getBitten(150);
}

void Insect::getStunned()
{
    m_stun += 2;
}

bool Insect::isEnemy(int colony) const
{
    return true;
}

bool Insect::becomesFoodUponDeath() const
{
    return true;
}

void Insect::getXYInFrontOfMe(int& x, int& y) const
{
    int x1 = getX();
    int y1 = getY();
    //get the location in front according to its
    // direction
    switch(getDirection())
    {
        case GraphObject::up:
            y1 += 1;
            break;
        case GraphObject::right:
            x1 += 1;
            break;
        case GraphObject::down:
            y1 -= 1;
            break;
        case GraphObject::left:
            x1 -= 1;
            break;
        default:
            break;
    }
    
    x = x1;
    y = y1;
}
//move to the square in front if possible
// if not return false and do not make any change
bool Insect::moveForwardIfPossible()
{
    int x = 0;
    int y = 0;
    getXYInFrontOfMe(x, y);
    if(getWorld() -> canMoveTo(x, y))
    {
        moveTo(x, y);
        return true;
    }
    return false;
}

//============================================GrassHopper===========================================
Grasshopper::Grasshopper(StudentWorld* sw, int startX, int startY, Direction startDir, int energy, int imageID):Insect(sw, startX, startY, startDir, energy, imageID)
{
    m_walkDistance = randInt(2,10);
}

int Grasshopper::getWalkDistance() const
{
    return m_walkDistance;
}

//Something that all Grasshopper do
void Grasshopper::postStepping()
{
    //attemp to eat 200 food
    int pickUppedFood = pickupAndEatFood(200);
    // if it did eat then there is 50% chance of sleeping
    if(pickUppedFood > 0)
    {
        int temp = randInt(1, 2);
        if(temp == 1)
        {
            getStunned();
            return;
        }
    }
    
    //if walk distance is 0 then pick a new direction to walk
    if(m_walkDistance == 0)
    {
        m_walkDistance = randInt(2,10);
        GraphObject::Direction tempDir =
        static_cast<GraphObject::Direction>(randInt(GraphObject::up, GraphObject::left));
        setDirection(tempDir);
    }
    
    //attempt to walk 1 step in the current direction
    if(!moveForwardIfPossible())
        m_walkDistance = 0;
    else
    {
        m_walkDistance--;
    }
    getStunned();
}

//============================================BabyGrassHopper===========================================
BabyGrasshopper::BabyGrasshopper(StudentWorld* sw, int startX, int startY, Direction startDir)
:Grasshopper(sw, startX, startY, startDir, 500, IID_BABY_GRASSHOPPER)
{
}

void BabyGrasshopper::doSomething()
{
    StudentWorld* currWorld = getWorld();
    //do something that all insect do
    if(prelimStep())
    {
        return;
    }
    //if has more than or equal to 1600 units of energy then transform to an Adult grasshopper and add food
    // to the square
    if(getEnergy() >= 1600)
    {
        GraphObject::Direction tempDir = static_cast<GraphObject::Direction>(randInt(GraphObject::up, GraphObject::left));
        AdultGrasshopper* temp = new AdultGrasshopper(currWorld, getX(), getY(), tempDir);
        currWorld -> addActor(temp);
        addFood(100);
        setState(true);
        return;
    }
    // else do something that all Grasshopper do
    postStepping();
    
}


//============================================AdultGrassHopper===========================================
AdultGrasshopper::AdultGrasshopper(StudentWorld* sw, int startX, int startY, Direction startDir)
:Grasshopper(sw, startX, startY, startDir, 1600, IID_ADULT_GRASSHOPPER)
{
}

//override to return false
bool AdultGrasshopper::isStunnablePoisonable() const
{
    return false;
}

void AdultGrasshopper::doSomething()
{
    StudentWorld* currWorld = getWorld();
    //do something all insects do
    if(prelimStep())
    {
        return;
    }
    // check for chance of bitting if did bite something else
    // then go to sleep
    int random = randInt(1, 3);
    if(random == 1)
    {
        if(currWorld -> biteEnemyAt(this, -1, 50))
        {
            getStunned();
            return;
        }
    }
    
    //1/10 chance of jumping to another square,
    // if did jump then go to sleep
    int random2 = randInt(1, 10);
    if(random2 == 1)
    {
        int des[2];
        currWorld->getClearLoc(this, des, 2);
        if(des[0] > -1)
        {
            moveTo(des[0],des[1]);
            getStunned();
            return;
        }
    }
    //do something all grasshopper do
    postStepping();
}

void AdultGrasshopper::getBitten(int amt)
{
    // if not dead then there is 50% of bitting back other
    // insects at the same square
    Insect::getBitten(amt);
    if(isDead())
    {
        return;
    }
    int rand = randInt(0,1);
    if(rand == 1)
    {
        getWorld() -> biteEnemyAt(this, -1, 50);
    }
}
//======================================================AntHill=============================================================

AntHill::AntHill(StudentWorld* sw, int startX, int startY, int colony, Compiler* program)
:EnergyHolder(sw, startX, startY, GraphObject::right, 8999, IID_ANT_HILL, 2)
{
    updateColony(colony);
    updateCompiler(program);
}

bool AntHill::isAntHill(int colony) const
{
    return isMyHill(colony);
}

void AntHill::doSomething()
{
    //do something that all Energy holder do
    EnergyHolder::doSomething();
    if(isDead())
    {
        return;
    }
    //try to eat max 10000 units of food
    int pickedUpFood = pickupAndEatFood(10000);
    //if did eat then return
    if(pickedUpFood > 0)
        return;
    
    // if has more than 2000 units then will produce an ant
    if(getEnergy() >= 2000)
    {
        //getting the direciton
        GraphObject::Direction tempDir = static_cast<GraphObject::Direction>(randInt(GraphObject::up, GraphObject::left));
        int antID;
        //getting the colony for the ant according to the anthill
        switch(getColony())
        {
            case 0:
                antID = IID_ANT_TYPE0;
                break;
            case 1:
                antID = IID_ANT_TYPE1;
                break;
            case 2:
                antID = IID_ANT_TYPE2;
                break;
            case 3:
                antID = IID_ANT_TYPE3;
                break;
            default:
                antID = -1;
                break;
        }
        //adding an ant to the current square
        Ant* temp = new Ant(getWorld(), getX(), getY(), getColony(), tempDir, getCompliler(), antID);
        getWorld() ->addActor(temp);
        updateEnergy(-1500);
        getWorld() -> increaseScore(getColony());
    }
}

bool AntHill::isMyHill(int colony) const
{
    return getColony() == colony;
}


//======================================================Ant=============================================================

Ant::Ant(StudentWorld* sw, int startX, int startY, int colony, Direction startDir, Compiler* program, int imageID)
:Insect(sw, startX, startY, startDir, 1500, imageID )
{
    m_foodStorage = 0;
    m_prevBitten = false;
    updateCompiler(program);
    updateColony(colony);
    m_lastRandInt = 0;
    m_prevBlocked = false;
    m_instrCounter = 0;
}

void Ant::doSomething()
{
    // do someting that all insects do
    if(prelimStep())
       return;
    
    int counter = 0;
    bool flag = false;
    bool flag2 = false;
    
    Compiler::Command cmd;
    Pheromone* pherToAdd = nullptr;
    int pherImageID;
    Compiler* currCompiler = getCompliler();
    for(;;)
    {
        int x,y;
        // if there is syntax error then return and set ant to dead
        if (!(currCompiler -> getCommand(m_instrCounter,cmd)))
        {
            setState(true);
            return;
        }
        //if more than 10 commands have been ran return
        if(counter > 9)
            return;
        //performing switch case on the command
        switch (cmd.opcode)
        {
            //move forward if possible and update the ant's state
            case Compiler::moveForward:
                moveForwardIfPossible();
                flag = true;
                break;
            //attempt to eat up to 100 food in storage
            case Compiler::eatFood:
                if(m_foodStorage <= 100)
                {
                    updateEnergy(m_foodStorage);
                    m_foodStorage = 0;
                }
                else
                {
                    updateEnergy(100);
                    m_foodStorage -= 100;
                }
                flag = true;
                break;
            //attempt to drop food that the ant hold onto the square
            case Compiler::dropFood:
                addFood(m_foodStorage);
                m_foodStorage = 0;
                flag = true;
                break;
            // attempt to bite another enemy
            case Compiler::bite:
                getWorld() -> biteEnemyAt(this, getColony(), 15);
                flag = true;
                break;
            // attempt to pick up up to 400 units of food or
            // however much left in the capacity and store it
            case Compiler::pickupFood:
                if(1800 - m_foodStorage <= 400)
                {
                    m_foodStorage += pickupFood(1800 - m_foodStorage);
                }
                else
                    m_foodStorage += pickupFood(400);
                flag = true;
                break;
            // release pheromone onto the square
            case Compiler::emitPheromone:
                pherToAdd = static_cast<Pheromone*>(getWorld() -> getPheromoneAt(getX(), getY(), getColony()));
                if(pherToAdd != nullptr)
                {
                    pherToAdd -> increaseStrength();
                }
                else
                {
                    switch (getColony())
                    {
                        case 0:
                            pherImageID = IID_PHEROMONE_TYPE0;
                            break;
                        case 1:
                            pherImageID = IID_PHEROMONE_TYPE1;
                            break;
                        case 2:
                            pherImageID = IID_PHEROMONE_TYPE2;
                            break;
                        case 3:
                            pherImageID = IID_PHEROMONE_TYPE3;
                            break;
                        default:
                            pherImageID = -1;
                            break;
                    }
                    pherToAdd = new Pheromone(getWorld(), getX(), getY(), getColony(), pherImageID);
                    getWorld() -> addActor(pherToAdd);
                }
                flag = true;
                break;
            case Compiler::faceRandomDirection:
                setDirection(static_cast<GraphObject::Direction>(randInt(GraphObject::up, GraphObject::left)));
                flag = true;
                break;
            case Compiler::rotateClockwise:
                switch(getDirection())
                {
                    case GraphObject::up:
                        setDirection(right);
                        break;
                    case GraphObject::right:
                        setDirection(down);
                        break;
                    case GraphObject::down:
                        setDirection(left);
                        break;
                    case GraphObject::left:
                        setDirection(up);
                        break;
                    default:
                        break;
                }
                flag = true;
                break;
            case Compiler::rotateCounterClockwise:
                switch(getDirection())
                {
                case GraphObject::up:
                    setDirection(left);
                    break;
                case GraphObject::right:
                    setDirection(up);
                    break;
                case GraphObject::down:
                    setDirection(right);
                    break;
                case GraphObject::left:
                    setDirection(down);
                    break;
                default:
                    break;
                }
                flag = true;
                break;
                
            case Compiler::generateRandomNumber:
                if(stoi(cmd.operand1) == 0)
                    m_lastRandInt = 0;
                else
                    m_lastRandInt = randInt(0, stoi(cmd.operand1) - 1);
                flag2 = false;
                break;
                
            case Compiler::goto_command:
                m_instrCounter = stoi(cmd.operand1);
                flag2 = true;
                break;
            case Compiler::if_command:
                switch(static_cast<Compiler::Condition>(stoi(cmd.operand1)))
                {
                    case Compiler::last_random_number_was_zero:
                        if(m_lastRandInt == 0)     //check the lastrandInt memeber variable
                        {
                            flag2 = true;
                            m_instrCounter = stoi(cmd.operand2);
                        }
                        break;
                    case Compiler::i_am_carrying_food:
                        if(m_foodStorage > 0)   //checking food storage
                        {
                            flag2 = true;
                            m_instrCounter = stoi(cmd.operand2);
                        }
                        break;
                    case Compiler::i_am_hungry:
                        if(getEnergy() <= 25)   //check if energy is below 25
                        {
                            flag2 = true;
                            m_instrCounter = stoi(cmd.operand2);
                        }
                        break;
                    case Compiler::i_am_standing_with_an_enemy:
                        if(getWorld() -> isEnemyAt(getX(), getY(), getColony())) // check if an enemy is at the square
                        {
                            flag2 = true;
                            m_instrCounter = stoi(cmd.operand2);
                        }
                        break;
                    case Compiler::i_am_standing_on_food:
                        if((getWorld() ->getEdibleAt(getX(), getY())) != nullptr) //check if there is food
                        {
                            flag2 = true;
                            m_instrCounter = stoi(cmd.operand2);
                        }
                        break;
                    case Compiler::i_am_standing_on_my_anthill:
                        if(getWorld() -> isAntHillAt(getX(), getY(), getColony())) // check if it is standing on its hill
                        {
                            flag2 = true;
                            m_instrCounter = stoi(cmd.operand2);
                        }
                        break;
                    case Compiler::i_smell_pheromone_in_front_of_me:
                        getXYInFrontOfMe(x, y);     //get location in front and check if there is pheromone of its colony
                        if(getWorld() -> getPheromoneAt(x, y, getColony()) != nullptr)
                        {
                            flag2 = true;
                            m_instrCounter = stoi(cmd.operand2);
                        }
                        break;
                    case Compiler::i_smell_danger_in_front_of_me:
                        getXYInFrontOfMe(x, y);        //get location in front and check if there is danger in front of it
                        if(getWorld() -> isDangerAt(x, y, getColony()))
                        {
                            flag2 = true;
                            m_instrCounter = stoi(cmd.operand2);
                        }
                        break;
                    case Compiler::i_was_bit:
                        if(m_prevBitten) //check if it was bit on this square
                        {
                            flag2 = true;
                            m_instrCounter= stoi(cmd.operand2);
                        }
                        break;
                    case Compiler::i_was_blocked_from_moving:
                        if(m_prevBlocked) //check if there is a rock in front of it
                        {
                            flag2 = true;
                            m_instrCounter = stoi(cmd.operand2);
                        }
                        break;
                    default:
                        break;
                }
            default:
                break;
        }
        //if the flag is true then the ant attempt to change its external state
        // so we incremenet instruction counter and return
        if(flag)
        {
            m_instrCounter++;
            return;
        }
        counter++;
        //indicate that the ant already move its instruction counter so we wont increment
        // it again
        if(!flag2)
        {
            m_instrCounter++;
        }
    }
    
}

bool Ant::moveForwardIfPossible()
{
    //update the ant state accordingly if the ant can move in
    // the current direction
    if(Insect::moveForwardIfPossible())
    {
        m_prevBlocked = false;
        m_prevBitten = false;
        return true;
    }
    else
        m_prevBlocked = true;
    return false;
}

bool Ant::isEnemy(int colony) const
{
    return !(getColony() == colony);
}

void Ant::getBitten(int amt)
{
    m_prevBitten = true;
    Insect::getBitten(amt);
}
























