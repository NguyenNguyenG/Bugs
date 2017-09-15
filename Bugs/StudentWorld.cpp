#include "StudentWorld.h"
#include <string>
using namespace std;

#include "Compiler.h"
#include <list>
#include "GameConstants.h"
#include "Field.h"
#include "GraphObject.h"
#include "Actor.h"
#include <vector>
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>  //setw

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// used to keep track of all the coordinates that the adult grasshopper can
// jump to
Coord::Coord(int r, int c)
{
    m_row = r;
    m_col = c;
}

int Coord::getRow() const
{
    return m_row;
}

int Coord::getCol() const
{
    return m_col;
}


StudentWorld::StudentWorld(string assetDir)
: GameWorld(assetDir), m_coordinate(21)
{
    //getting all the coordinates that the adult grasshopper
    // can jump to
    int track = 0;
    for(int r = -10; r < 11; r++)
    {
        for(int c = 0; c < 11; c++)
        {
            if((r*r + c*c )<= 100)
            {
                if(r == 0 && c == 0)
                    continue;
                Coord temp(r,-c);
                m_coordinate[track].insert(m_coordinate[track].begin(),temp);
                if(c != 0)
                {
                    Coord temp2(r,c);
                    m_coordinate[track].push_back(temp2);
                }
            }
        }
        track++;
    }
    //set all score to 0
    for(int r =0; r < 4; r++)
    {
        m_score[r][0] = 0;
        m_score[r][1] = 0;
    }
    compilerForEntrant0 = compilerForEntrant1 = compilerForEntrant2 = compilerForEntrant3 = nullptr;
}


int StudentWorld::init()
{
    //parsing the field text file to get the world that the insects are in
    Field f;
    std::string fieldFile = getFieldFilename();
    
    std::string error;
    if (f.loadField(fieldFile, error) != Field::LoadResult::load_success)
    {
        setError(fieldFile + " " + error);
        return false; // something bad happened!
    }
    // otherwise the load was successful and you can access the
    // contents of the field – here’s an example
    
    string error2;
    vector<string> fileNames = getFilenamesOfAntPrograms();
    
    for(int r = 0; r < VIEW_HEIGHT; r++)
    {
        for(int c = 0; c < VIEW_WIDTH; c++)
        {
            //getting each actors in the map
            Field::FieldItem item = f.getContentsOf(c,r);
            GraphObject::Direction hoperDir; // note it’s x,y and not y,x!!!
            Actor* newActor = nullptr;
            //allocating each actor to a spot on the map
            switch(item)
            {
                case Field::FieldItem::rock:
                    newActor = new Pebble(this, c, r);
                    break;
                case Field::FieldItem::grasshopper:
                    hoperDir = static_cast<GraphObject::Direction>(randInt(GraphObject::up, GraphObject::left));
                    newActor = new BabyGrasshopper(this, c, r, hoperDir);
                    break;
                case Field::FieldItem::food:
                    newActor = new Food(this, c, r, 6000);
                    break;
                case Field::FieldItem::water:
                    newActor = new WaterPool(this, c, r);
                    break;
                case Field::FieldItem::poison:
                    newActor = new Poison(this, c , r);
                    break;
                case Field::Field::anthill0:
                    // check if the program even includes enough .bug files to
                    // create a new anthill and this check is performed for all other
                    // ant hill
                    compilerForEntrant0 = new Compiler;
                    if(fileNames.size() < 1)
                        break;
                    // if there is no error then create the new antHill
                    if(!compilerForEntrant0 -> compile(fileNames[0], error2))
                    {
                        setError(fileNames[0] + " " + error);
                        return  GWSTATUS_LEVEL_ERROR;
                    }
                    newActor = new AntHill(this, c, r, 0, compilerForEntrant0);
                    break;
                case Field::Field::anthill1:
                    compilerForEntrant1 = new Compiler;
                    if(fileNames.size() < 2)
                        break;
                    if(!compilerForEntrant1 -> compile(fileNames[1], error2))
                    {
                        setError(fileNames[1] + " " + error);
                        return  GWSTATUS_LEVEL_ERROR;
                    }
                    newActor = new AntHill(this, c, r, 1, compilerForEntrant1);
                    break;
                case Field::Field::anthill2:
                    compilerForEntrant2 = new Compiler;
                    if(fileNames.size() < 3)
                        break;
                    if(!compilerForEntrant2 -> compile(fileNames[2], error2))
                    {
                        setError(fileNames[2] + " " + error);
                        return  GWSTATUS_LEVEL_ERROR;
                    }
                    newActor = new AntHill(this, c, r, 2, compilerForEntrant2);
                    break;
                case Field::Field::anthill3:
                    compilerForEntrant3 = new Compiler;
                    if(fileNames.size() < 4)
                        break;
                    if(!compilerForEntrant3 -> compile(fileNames[3], error2))
                    {
                        setError(fileNames[3] + " " + error);
                        return  GWSTATUS_LEVEL_ERROR;
                    }
                    newActor = new AntHill(this, c, r, 3, compilerForEntrant3);
                    break;
                default:
                    break;
            }
            //add the actor pointer to the STL
            if(newActor != nullptr)
                m_virtualField[r][c].push_back(newActor);
        }
    }
    //set tick to 0
    tick = 0;
    return GWSTATUS_CONTINUE_GAME;
}


int StudentWorld::move()
{
    tick++;
    for(int r = 0; r < VIEW_HEIGHT; r++)
    {
        for(int c = 0 ; c < VIEW_WIDTH; c++)
        {
            std::list<Actor*>::iterator it = m_virtualField[r][c].begin();
            //looping through all actors at square to give them a chance to do something
            while(it != m_virtualField[r][c].end())
            {
                //if they are dead then delete it
                if(!((*it) -> isDead()))
                {
                    Actor* currActor = *it;
                    // if it has not moved before then it gets to do something
                    if(currActor -> haveMoved())
                    {
                        it++;
                        continue;
                    }
                    //storing the old location
                    int oldX = currActor -> getX();
                    int oldY = currActor ->getY();
                    
                    currActor -> doSomething();
                    //if it moves to a new spot, update the STL and set it states to have moved already
                    // also it has not been stunned by water (on a new square)
                    if(oldX != (*it) -> getX() || oldY != (*it) -> getY())
                    {
                        currActor -> setHaveMoved(true);
                        static_cast<Insect*>(currActor) -> setStunnedByWater(false);
                        m_virtualField[currActor -> getY()][currActor -> getX()].push_back(*it);
                        it = m_virtualField[r][c].erase(it);
                    }
                    
                    else
                        it++;
                }
                else
                {
                    delete (*it);
                    it = m_virtualField[r][c].erase(it);
                }
            }
        }
    }
    
    for(int r = 0; r < VIEW_HEIGHT; r++)
    {
        for(int c = 0; c < VIEW_WIDTH; c++)
        {
            list<Actor*>::iterator it = m_virtualField[r][c].begin();
            //deleting all dead actors or if they are not dead, set their state haveMoved to false
            // for the new tick
            while(it != m_virtualField[r][c].end())
            {
                if((*it) -> isDead())
                {
                    delete (*it);
                    it = m_virtualField[r][c].erase(it);
                }
                else
                {
                    (*it) ->setHaveMoved(false);
                    it++;
                }
            }
        }
    }
    
    displayGameStat();
    
    string error;
    if(tick == 2000)
    {
        // only if the anthill has produced at least 6 ants, then it can wins
        if(m_score[m_winningAntIndex][0] > 5)
        {
            Compiler c;
            //getting the name of the ant program that wins
            if(c.compile(getFilenamesOfAntPrograms()[m_winningAntIndex], error ))
                setWinner(c.getColonyName());
            return GWSTATUS_PLAYER_WON;
        }
        return GWSTATUS_NO_WINNER;
    }
    return  GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    //deleting all the actors that have not been deallocated earlier
    for(int r = 0; r < VIEW_HEIGHT; r++)
    {
        for(int c = 0; c < VIEW_WIDTH; c++)
        {
            while(!m_virtualField[r][c].empty())
            {
                //if(m_virtualField[r][c].front() -> isStunnablePoisonable())
                  //  delete(static_cast<EnergyHolder*>(m_virtualField[r][c].front()) ->getCompliler());
                delete (m_virtualField[r][c].front());
                m_virtualField[r][c].pop_front();
            }
        }
    }
}

StudentWorld::~StudentWorld()
{
    cleanUp();
    delete compilerForEntrant0;
    delete compilerForEntrant1;
    delete compilerForEntrant2;
    delete compilerForEntrant3;
}

//check if it can moves to this spot = basically checking if there is a pebble here
bool StudentWorld::canMoveTo(int x, int y) const
{
    if(m_virtualField[y][x].size() == 0)
        return true;
    list<Actor*>::const_iterator it = m_virtualField[y][x].begin();
    return !((*it) -> blocksMovement());
}


void StudentWorld::addActor(Actor *a)
{
    int x = a ->getX();
    int y = a -> getY();
    // if the actor is a food actor
    if(a ->isEdible() && !m_virtualField[y][x].empty())
    {
        //check if there is another food actor here
        list<Actor*>::iterator it = m_virtualField[y][x].begin();
        while(it != m_virtualField[y][x].end())
        {
            if((*it) -> isEdible())
            {
                //simply add the amount to existing Food actor if there is one
                static_cast<EnergyHolder*>(*it) ->
                updateEnergy(static_cast<EnergyHolder*>(a)->getEnergy());
                return;
            }
            it++;
        }
    }
    //otherwise make a new Food actor at this location
    m_virtualField[y][x].push_back(a);
}

Actor* StudentWorld::getEdibleAt(int x, int y) const
{
    if(m_virtualField[y][x].size() == 0)
        return nullptr;
    list<Actor*>::const_iterator it = m_virtualField[y][x].begin();
    while(it != m_virtualField[y][x].end())
    {
        Actor* curr = *it;
        // if there is a food actor here and it is not dead then return
        // its pointer
        if(curr -> isEdible() && !(curr -> isDead()))
        {
            return *it;
        }
        else
            it++;
    }
    //there was no food actor here
    return nullptr;
}

bool StudentWorld::poisonAllPoisonableAt(int x, int y)
{
    if(m_virtualField[y][x].size() == 0)
        return false;
    bool flag = false;
    list<Actor*>::iterator it = m_virtualField[y][x].begin();
    //poison all of the actors that can be poisoned
    while(it != m_virtualField[y][x].end())
    {
        if((*it) -> isStunnablePoisonable() )
        {
            (*it) ->getPoisoned();
            flag = true;
        }
        it++;
    }
    //return true/false whether something was poisoned
    return flag;
}

Actor* StudentWorld::getPheromoneAt(int x, int y, int colony) const
{
    if(m_virtualField[y][x].size() == 0)
        return nullptr;
    list<Actor*>::const_iterator it = m_virtualField[y][x].begin();
    //looping throug the actors at the location and if there a pheromone of the
    // indicated colony, return the pointer
    while(it != m_virtualField[y][x].end())
    {
        if((*it) ->isPheromone(colony))
            return (*it);
        it++;
    }
    //there was no pheromone of the indicated colony here
    return nullptr;
}
bool StudentWorld::stunAllStunnableAt(int x, int y)
{
    if(m_virtualField[y][x].size() == 0)
        return false;
    bool flag = false;
    list<Actor*>::iterator it = m_virtualField[y][x].begin();
    //looping through all the actors at the current location and
    // stun them if they are stunnable
    while(it != m_virtualField[y][x].end())
    {
        if((*it) -> isStunnablePoisonable())
        {
            if(!(static_cast<Insect*>(*it) -> stunnedByWater()))
            {
                //prevent the insects being stunned over and over again by water
                static_cast<Insect*>(*it) ->setStunnedByWater(true);
                (*it)->getStunned();
                flag = true;
            }
        }
        it++;
    }
    return flag;
}

bool StudentWorld::biteEnemyAt(Actor* me, int colony, int biteDamage)
{
    vector<int> bitableInsect;
    int counter = 0;
    int x = me ->getX();
    int y = me ->getY();
    list<Actor*>::iterator it = m_virtualField[y][x].begin();
    //getting all of the indexes of the bitable enemy at this location
    while(it != m_virtualField[y][x].end())
    {
        if(*it == me)
        {
            it++;
            counter++;
            continue;
        }
        Actor* curr = *it;
        if(curr -> isEnemy(colony) && !(curr ->isDead()))
        {
            bitableInsect.push_back(counter);
        }
        counter++;
        it++;
    }
    // there was no enemy to bite
    if(bitableInsect.empty())
        return false;
    else
    {
        //picking a random actor to bite
        int random = randInt(0, bitableInsect.size() - 1);
        counter = 0;
        it = m_virtualField[y][x].begin();
        while(counter < bitableInsect[random])
        {
            it++;
            counter++;
        }
        //the actor get bitten
        (*it) ->getBitten(biteDamage);
        return true;
    }
}

void StudentWorld::getClearLoc(Actor* actor, int des[], int size)
{
    vector<Coord> temp;
    int x = actor ->getX();
    int y = actor -> getY();
    for(int i = 0; i < 21; i++)
    {
        for(unsigned int k = 0; k < m_coordinate[i].size(); k++ )
        {
            //looping through and storing all the possible coordinates
            // that the Adult grasshopper can jump to in a vector
            int newX =m_coordinate[i][k].getCol() + x ;
            int newY = m_coordinate[i][k].getRow() + y;
            if( newY >= 1 && newY <= 62 && newX >= 1 && newX <= 62)
            {
                if(canMoveTo(newX, newY))
                {
                    temp.push_back(Coord(newY,newX));
                }
            }
        }
    }
    // if there is no location then signal that there is no location to jump to
    if(temp.empty())
    {
        des[0] = -1;
    }
    else
    {
        //pick a random coordinate to jump to
        int random = randInt(0, temp.size() - 1);
        des[0] = temp[random].getCol();
        des[1] = temp[random].getRow();
    }
}

bool StudentWorld::isEnemyAt(int x, int y, int colony) const
{
    if(m_virtualField[y][x].size() == 0)
        return false;
    //looping through and if an enemy is found then return true
    list<Actor*>::const_iterator it = m_virtualField[y][x].begin();
    while(it != m_virtualField[y][x].end())
    {
        if((*it) -> isEnemy(colony))
        {
            return true;
        }
        it++;
    }
    //no enemy was found
    return false;
}

bool StudentWorld::isAntHillAt(int x, int y, int colony) const
{
    if(m_virtualField[y][x].size() == 0)
        return false;
    //looping through and check if the ant's anthill is here
    list<Actor*>::const_iterator it = m_virtualField[y][x].begin();
    while(it != m_virtualField[y][x].end())
    {
        if((*it) -> isAntHill(colony))
        {
            return true;
        }
        it++;
    }
    return false;
}

bool StudentWorld::isDangerAt(int x, int y, int colony) const
{
    //check if the location has no actor
    if(m_virtualField[y][x].size() == 0)
        return false;
    //check if there is an enemy
    if(isEnemyAt(x, y, colony))
        return true;
    //addtionally check if there is pool or poison which are considered to be dangerous
    list<Actor*>::const_iterator it = m_virtualField[y][x].begin();
    while(it != m_virtualField[y][x].end())
    {
        if((*it) -> isDangerous(colony))
        {
            return true;
        }
        it++;
    }
    //no danger
    return false;
}

void StudentWorld::increaseScore(int colony)
{
    //store the score and record the time
    m_score[colony][0]++;
    m_score[colony][1] = tick;
}

void StudentWorld::displayGameStat()
{
    vector<string> fileNames = getFilenamesOfAntPrograms();
    int indexOfGreatest = 0;
    //getting the index of the winning ant
    for(int i = 1; i < 4; i++)
    {
        //if the score is bigger then index of greatest get changed
        if(m_score[indexOfGreatest][0] < m_score[i][0])
            indexOfGreatest = i;
        else if(m_score[indexOfGreatest][0] == m_score[i][0])
        {
            // if the score is equal we look at the time each ant colony
            // got the score
            if(m_score[i][1] < m_score[indexOfGreatest][1])
                indexOfGreatest = i;
        }
    }
    m_winningAntIndex = indexOfGreatest;
    ostringstream text;
    string error;
    //formatting the text to the desired style
    text << "Ticks:";
    text << setw(5) << 2000 - tick << " - ";
    for(int i = 0; i < 4; i++)
    {
        if(m_score[i][0] > 0 )
        {
            Compiler c;
            if(c.compile(fileNames[i], error))
            {
                text << c.getColonyName();
                //check if the score is bigger than 5 only then can it be winning
                if(i == indexOfGreatest && m_score[i][0] > 5)
                    text << "*";
                text << ": ";
                text.fill('0');
                text << setw(2) << m_score[i][0];
                text << "  ";
            }
        }
    }
    setGameStatText(text.str());
    
}














