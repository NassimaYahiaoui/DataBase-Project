#ifndef _BUFFER_MANAGER_H
#define _BUFFER_MANAGER_H

#include <string>
#include <unordered_map>

#include "global.h"

using namespace std;

// //Linked list node of data block. Used for LRU
struct Node
{
    Block* block;
    Node* pre;
    Node* nxt;

    // //Constructor
    Node(Block* _block): block(_block) {}

    // //Add to position after node
    void add(Node* node)
    {
        pre = node; nxt = node->nxt;
        node->nxt->pre = this; node->nxt = this;
    }

    // //Remove from linked list
    void remove() { pre->nxt = nxt; nxt->pre = pre;}

    // //Destructor
    ~Node() { remove();}
};

class ManageStruct
{
public:

    // //Max number of block
    static const int MAX_BLOCK_COUNT;

    // //Constructor
    ManageStruct();

    // //Destructor
    ~ManageStruct();

    // //Get the id-th block in file
    Block* getBlock(const char* filename, int id);

    // //Remove all block with filename(used when delete file)
    void removeNodeByFilename(const char* filename);

#ifdef DEBUG
    // //Print block filename and id
    void debugPrint() const;
#endif

private:

    // //Current block number
    int nodeCount;

    // //Dummy head and tail for linked list
    Node* lHead;
    Node* lTail;

    // //Block node map
    unordered_map<string, Node*> nodeMap;

    // //Delete node and its block from memory
    void deleteNode(Node* node, bool write = true);

    // //Load the id-th block from file
    Block* loadNode(const char* filename, int id);

    // //Write the id-th block of file
    void writeNode(const char* filename, int id);
};

#endif
