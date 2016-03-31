#include "octree.h"
#include <iostream>
#include <queue>

using namespace std;
using namespace glm;

Octree::Octree()
{
    m_root = nullptr;
    m_objectCount = 0;
}
Octree::~Octree()
{
    if(m_root != nullptr)
        delete m_root;
}
void Octree::addObject(PObject* obj)
{
    //cout << "ajout d'un objet à la racine" << endl ;
    //cout << "racine : " << m_root << endl ;
    //Si le monde est vide, on le créé autour de l'objet.
    if(m_root == nullptr)
    {
        vec3 objSize = obj->getAABB().getSize() ;
        float size = 2.5f*std::max(std::max(objSize.x,objSize.y),objSize.z);
        m_root = new Node(obj->getPosition() - 1.5f*objSize, size, nullptr);
        vec3 pos = m_root->getPosition() ;
    }
    //On ajoute l'objet à la racine.
    addObject(m_root, obj);
}
void Octree::addObject(Node* n, PObject* obj)
{
    Node* node = n;
    //On cherche le noeud qui puisse contenir l'objet.
    //On commence sur le noeud courant et on remonte jusqu'à la racine.
    while(node != nullptr && node->getAABB().relativePosition(obj->getAABB()) != INSIDE)
    {
        node = node->getParent() ;
    }
    //Si on on a trouvé un parent capable de contenir l'objet.
    //Phase de redescente.
    if(node != nullptr)
    {   
        //cout << "noeud capable de contenir l'objet" << endl ;
        addObjectToChildren(node, obj);
        return;
    }
    //Sinon le monde n'est pas assez grand, on l'agrandi.
    //cout << "pas de noeud capable de contenir l'objet" << endl ;
    addObjectOutsideWorld(obj);
}

void Octree::addObjectToChildren(Node* node, PObject* obj)
{
    //cout << "ajout d'un objet DANS le noeud spécifié" << endl ;
    while(node->hasChildren())
    {
        //On calcule la position relative du noeud courant et de l'objet.
        int relPos = 0;

        //On regarde si l'object est dans la partie haute.
        relPos += (obj->getAABB().getBottomPosition() > node->getPosition().y + node->getSize()/2.) ? TOP : BOTTOM ;
        //S'il n'y est pas, on regarde s'il est aussi dans la partie basse
        if(!(relPos&TOP) && !(obj->getAABB().getTopPosition() < node->getPosition().y + node->getSize()/2.))
            break;

        //On regarde si l'object est dans la partie droite.
        relPos += (obj->getAABB().getLeftPosition() > node->getPosition().x + node->getSize()/2.) ? RIGHT : LEFT ;
        //S'il n'y est pas, on regarde s'il est aussi dans la partie gauche.
        if(!(relPos&RIGHT) && !(obj->getAABB().getRightPosition() < node->getPosition().x + node->getSize()/2.))
            break;

        //On regarde si l'object est dans la partie devant.
        relPos += (obj->getAABB().getBackPosition() > node->getPosition().z + node->getSize()/2.) ? FRONT : BACK ;
        //S'il n'y est pas, on regarde s'il est aussi dans la partie derriere.
        if(!(relPos&FRONT) && !(obj->getAABB().getFrontPosition() < node->getPosition().z + node->getSize()/2.))
            break;

        //Si il peut être contenu dans un des enfant.
        //On alloue l'enfant s'il n'existe pas.
        node->allocateChild(relPos);
        node = node->getChildren()[relPos] ;
    }
    //Si on est sur une feuille
    node->addObject(obj);
    updateNodeSubdivision(node);
    ++m_objectCount;
    //cout << "nombre d'éléments de " << node << " après insertion : " << node->getObjects().size() << endl ;
}

void Octree::updateObject(PObject* obj)
{
    //OPTIMIZE ME !!!!!!
    if(obj->getNode() == nullptr)
        return;

       //Sinon, on supprime l'objet du noeud et on l'insère dans un noeud proche.
       removeObject(obj->getNode(), obj);
       addObject(m_root, obj);
       //print(m_root);
}

void Octree::addObjectOutsideWorld(PObject* obj)
{
    //On part de la racine
    Node* node = m_root ;
    //Tant que l'objet ne peut pas être contenu dans la racine
    while(node->getAABB().relativePosition(obj->getAABB()) != INSIDE)
    {
        //On calcule la position relative du noeud courant et de l'objet.
        int relPos = 0;
        relPos += (obj->getAABB().getBottomPosition() < node->getPosition().y) ? TOP : BOTTOM ;
        relPos += (obj->getAABB().getLeftPosition() < node->getPosition().x) ? RIGHT : LEFT ;
        relPos += (obj->getAABB().getBackPosition() < node->getPosition().z) ? FRONT : BACK ;

        //On calcule la position de la nouvelle racine.
        vec3 pos(node->getPosition().x - ((relPos&RIGHT) ? node->getSize() : 0.f),
                    node->getPosition().y - ((relPos&TOP) ? node->getSize() : 0.f),
                 node->getPosition().z - ((relPos&FRONT) ? node->getSize() : 0.f));

        //La racine devient enfant de la nouvelle.
        Node* parent = new Node(pos, node->getSize()*2.f, nullptr);
        node->setParent(parent);
        parent->setChild(relPos, node) ;
        updateNodeMerge(parent);
        node = parent ;
    }
    m_root = node ;
    addObject(m_root, obj);
}

void Octree::removeObject(Node* node, PObject* obj)
{
    node->removeObject(obj);
    updateNodeMerge(node);
    updateRoot();
    --m_objectCount;
}

void Octree::setRoot(Node* root)
{
    m_root = root;
}

Node* Octree::getRoot() const
{
    return m_root ;
}
void Octree::print(Node* node) const
{
    cout << endl ;
    queue<pair<Node*, bool>> q ;
    q.push({node, true});
    while(!q.empty())
    {
        Node* node ;
        bool endline = true;
        std::tie(node, endline) = q.front() ;
        q.pop();
        for(int i = 0 ; i < 8 ; i++)
        {
            if(node->getChildren()[i] != nullptr)
            {
                q.push({node->getChildren()[i], false});
            }
        }
        get<1>(q.back()) = true ;
        cout << node << "(objets:" << node->getObjects().size() << ", " << "enfants : " << node->m_childrenCount << " , " ;
        if(!node->hasChildren())
            cout << "not " ;
        cout << "divided) ; " ;
        if(endline)
            cout << endl ;
    }
    cout << endl << endl ;
}

unsigned int Octree::getObjectCount() const
{
    return m_objectCount;
}

void Octree::updateNodeMerge(Node* node)
{
    /**** HYPOTHESE POUR LA FUSION : LES ENFANTS SONT A JOUR *****/
    Node* current = node ;
    while(current != nullptr)
    {
        if(!current->hasChildren() && current->getObjectCount() == 0)
        {
            Node* n = current ;
            //parent existe ? (crash si supression du dernier object de la racine)
            current = current->getParent() ;
            current->setChild(n, nullptr) ;
            continue;
        }
        else if(current->hasChildren())
        {
            int objCount =  current->getObjectCount() ;
            for(int i = 0 ; i < 8 ; i++)
            {
                if(current->getChildren()[i] != nullptr)
                {
                    if(current->getChildren()[i]->hasChildren())
                        return;

                    objCount += current->getChildren()[i]->getObjectCount();
                }
            }

            if(objCount >= MIN_OBJECTS)
                return;

            //On fusionne.
            for(int i = 0 ; i < 8 ; i++)
            {
                if(current->getChildren()[i] != nullptr)
                {
                    for(auto it = current->getChildren()[i]->getObjects().begin() ; it != current->getChildren()[i]->getObjects().end() ; it++)
                    {
                        current->addObject(*it);
                    }
                    current->setChild(i, nullptr);
                }
            }
        }
        current = current->getParent();
    }
}

void Octree::updateNodeSubdivision(Node* node)
{
    queue<Node*> q ;
    q.push(node);
    array<bool, 8> editedNode = {false, false, false, false, false, false, false, false}; 

    while(!q.empty())
    {
        Node* current = q.front();
        q.pop();
        if(!current->hasChildren() && current->getObjectCount() > MAX_OBJECTS)
        {
            for(auto it = current->m_objects.begin() ; it != current->m_objects.end() ; it++)
            {
                //On calcule la position relative du noeud courant et de l'objet.
                int relPos = 0;

                //On regarde si l'object est dans la partie haute.
                relPos += ((*it)->getAABB().getBottomPosition() > current->getPosition().y + current->getSize()/2.f) ? TOP : BOTTOM ;
                //S'il n'y est pas, on regarde s'il est aussi dans la partie basse
                if(!(relPos&TOP) && !((*it)->getAABB().getTopPosition() < (current->getPosition().y + current->getSize()/2.f)))
                    continue;

                //On regarde si l'object est dans la partie droite.
                relPos += ((*it)->getAABB().getLeftPosition() > current->getPosition().x + current->getSize()/2.f) ? RIGHT : LEFT ;
                //S'il n'y est pas, on regarde s'il est aussi dans la partie gauche.
                if(!(relPos&RIGHT) && !((*it)->getAABB().getRightPosition() < (current->getPosition().x + current->getSize()/2.f)))
                    continue;

                //On regarde si l'object est dans la partie devant.
                relPos += ((*it)->getAABB().getBackPosition() > current->getPosition().z + current->getSize()/2.f) ? FRONT : BACK ;
                //S'il n'y est pas, on regarde s'il est aussi dans la partie derriere.
                if(!(relPos&FRONT) && !((*it)->getAABB().getFrontPosition() < (current->getPosition().z + current->getSize()/2.f)))
                    continue;

                //Si il peut être contenu dans un des enfant.
                //On alloue l'enfant s'il n'existe pas.
                current->allocateChild(relPos);                 

                //On l'ajoute.
                current->getChildren()[relPos]->addObject((*it));
                editedNode[relPos] = true ;

                //On l'enleve du noeud.
                current->m_objects.erase(it++);
            }
            for(int i = 0 ; i < 8 ; i++)
            {
                if(editedNode[i])
                {
                    q.push(current->getChildren()[i]);
                    editedNode[i] = false ;
                }
            }
        }
    }

}
void Octree::updateRoot()
{
    Node* temp ;
    while(m_root->m_childrenCount == 1 && m_root->getObjectCount() == 0)
    {
        for(int i = 0 ; i < 8 ; i++)
        {
            if(m_root->getChildren()[i] != nullptr)
            {
                temp = m_root->getChildren()[i] ;
                m_root->m_children[i] = nullptr ;
                delete m_root ;
                m_root = temp ;
                m_root->setParent(nullptr);
                break;
            }
        }
    }
}