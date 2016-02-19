#include "collisions.h"
#include "../glm/gtx/norm.hpp"
#include "../glm/gtx/transform.hpp"

using namespace glm ;
using namespace std ;

float square(float f)
{
    return f * f;
}

bool collideSphereWithSphere(PSphere* obj1, PSphere* obj2)
{
    return distance2(obj1->getPosition(), obj2->getPosition()) <= (obj1->getRadius() + obj2->getRadius()) * (obj1->getRadius() + obj2->getRadius()) ;
}

vector<tuple<vec3,vec3>> collisionPointsSphereWithSphere(PSphere* obj1, PSphere* obj2)
{
    //On retourne la moyenne des centres ponderees par les rayons.
    //La normale est définie comme le vecteur formé par les deux centres.
    //Suppose que les spheres sont en collision (c'est le cas, la broadphase est exacte pour les solides de base).
    vec3 point((obj1->getRadius() * obj1->getPosition() + obj2->getRadius() * obj2->getPosition()) / (obj1->getRadius() + obj2->getRadius()));
    vec3 normal(obj2->getPosition()- obj1->getPosition());
    return vector<tuple<vec3,vec3>>({make_tuple(point, normal)});
}

bool collideSphereWithBox(PSphere* s, PBox* b)
{
    // calculating s's position relative to this
    glm::vec3 sphPos = b->getLocalPoint(s->getPosition());

    float xc = fabsf(sphPos.x);
    float yc = fabsf(sphPos.y);
    float zc = fabsf(sphPos.z);
    float r = s->getRadius();
    if (xc <= b->getWidth() / 2.0)
    {
        if (yc <= b->getHeight() / 2.0)
            return zc - r <= b->getDepth() / 2.0;
        if (zc <= b->getDepth() / 2.0)
            return yc - r <= b->getHeight() / 2.0;
        else if (yc <= b->getHeight() / 2.0 + r)
            return square(yc - (b->getHeight() / 2.0)) + square(zc - (b->getDepth() / 2.0)) <= square(r);
        else
            return false;
    }
    else if (yc <= b->getHeight() / 2.0)
    {
        if (zc <= b->getDepth() / 2.0)
            return xc - r <= b->getWidth() / 2.0;
        else if (zc <= b->getDepth() / 2.0 + r)
            return square(xc - (b->getWidth() / 2.0)) + square(zc - (b->getDepth() / 2.0)) <= square(r);
        else
            return false;
    }
    else if (zc <= b->getDepth() / 2.0)
    {
        if (xc <= b->getWidth() / 2.0 + r)
            return square(xc - (b->getWidth() / 2.0)) + square(yc - (b->getHeight() / 2.0)) <= square(r);
        else
            return false;
    }
    else if ((xc <= b->getWidth() / 2.0 + r) && (yc <= b->getHeight() / 2.0 + r) && (zc <= b->getDepth() / 2.0 + r))
        return square(xc - (b->getWidth() / 2.0)) + square(yc - (b->getHeight() / 2.0)) + square(zc - (b->getDepth() / 2.0)) <= square(r);
    else
        return false;
}

vector<tuple<vec3,vec3>> collisionPointsSphereWithBox(PSphere* obj1, PBox* obj2)
{
    return vector<tuple<vec3,vec3>>();
}

bool collideSphereWithMesh(PSphere* obj1, PMesh* obj2)
{
    return false;
}

vector<tuple<vec3,vec3>> collisionPointsSphereWithMesh(PSphere* obj1, PMesh* obj2)
{
    return vector<tuple<vec3,vec3>>();
}

bool collideBoxWithBox(PBox* obj1, PBox* obj2)
{
    return false;
}

vector<tuple<vec3,vec3>> collisionPointsBoxWithBox(PBox* obj1, PBox* obj2)
{
    return vector<tuple<vec3,vec3>>();
}

bool collideBoxWithMesh(PBox* obj1, PMesh* obj2)
{
    return false;
}

vector<tuple<vec3,vec3>> collisionPointsBoxWithMesh(PBox* obj1, PMesh* obj2)
{
    return vector<tuple<vec3,vec3>>();
}

bool collideMeshWithMesh(PMesh* obj1, PMesh* obj2)
{
    return false;
}

vector<tuple<vec3,vec3>> collisionPointsMeshWithMesh(PMesh* obj1, PMesh* obj2)
{
    return vector<tuple<vec3,vec3>>();
}

