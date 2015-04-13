#ifndef VERLET_H
#define VERLET_H

#include <glm/glm.hpp>
#include <vector>
#include "link.h"
#include <QHash>

class Ellipsoid;
class Graphics;
class Entity;
class Verlet
{
public:
    Verlet();
    ~Verlet();
    glm::vec3 rayTraceSize = glm::vec3(.4,.4,.4);

    int getSize(){return numPoints;}
    glm::vec3 getPoint(const int& id){return _pos[id];}

    //Fix point at specified index to its _pos
    void createPin(int index);
    //Sets point at specified index to given pos
    void setPos(int index, const glm::vec3& pos);

    //Per update, called in VerletManager's onTick:
    //Set acceleration of points
    void applyForce(const glm::vec3& force);
    //Update pos + prevPos
    void verlet(float seconds);
    //Solve individual constraints
    void boxConstraint(const glm::vec3& _boxMin, const glm::vec3& _boxMax);
    void linkConstraint();
    void pinConstraint();

    void onDraw(Graphics *g);
    glm::vec3 collide(Entity* e);

    //Map from indices to links, for tearing
    QHash<int, QList<Link> > link_map;
    void removeLink(Link l);
    void removeLink(int id);

protected:
    //Creates new point (at index numPoints) w/ given position
    void createPoint(const glm::vec3& pos);
    //Specify indices of two pre-existing points
    void createLink(int a, int b);
    private:
    //Actual number of points
    int numPoints = 0;
    //Maximum points, for declaring arrays
    static const int NUM = 300000;
    //Points
    glm::vec3 _pos[NUM];
    glm::vec3 _prevPos[NUM];
    glm::vec3 _acc[NUM];

    //Constraints
    typedef struct{
        int index;
        glm::vec3 pinPos;
    }Pin;
    std::vector<Pin> pins;
    std::vector<Link> links;
};

#endif // VERLET_H