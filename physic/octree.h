#ifndef OCTREE_H
#define OCTREE_H

#include "node.h"
#include <queue>
#include <array>

class Octree {
    public:
        //MAX >= MIN
        static const unsigned int MAX_OBJECTS = 20;
        static const unsigned int MIN_OBJECTS = 20;

        Octree();
        ~Octree();

        Node* getRoot() const;
        unsigned int getObjectCount() const;

        void addObject(PObject* obj);
        void addObject(Node* node, PObject* obj);
        void updateObject(PObject* obj);
        void removeObject(Node* node, PObject* obj);
        
        void updateNodeMerge(Node* node);
        void updateNodeSubdivision(Node* node);
        void updateRoot();

        void setRoot(Node* root) ;
        void print(Node* node) const;

    private:
        Node* m_root ;
        void addObjectToChildren(Node* node, PObject* obj);
        void addObjectOutsideWorld(PObject* obj);
        unsigned int m_objectCount ;

};

#endif