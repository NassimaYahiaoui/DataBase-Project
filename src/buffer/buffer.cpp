#include <cstdio>
#include "buffer/buffer.h"
#include <iostream>

using namespace std;

// //Max number of block
const int ManageStruct::MAX_BLOCK_COUNT = 100;

// //Constructor
ManageStruct::ManageStruct()
{
    nodeCount = 0;

    // //Initialize dummy head and tail
    lHead = new Node(NULL);
    lTail = new Node(NULL);
    lHead->pre = lHead->nxt = lTail;
    lTail->pre = lTail->nxt = lHead;
}

// //Destructor
ManageStruct::~ManageStruct()
{
    // //Clean up linked list
    while (lHead->nxt != lTail)
        deleteNode(lHead->nxt);
    delete lHead;
    delete lTail;
}

// //Get the id-th block in file
Block* ManageStruct::getBlock(const char* filename, int id)
{
    string blockName = string(filename) + "`" + to_string(id);

    if (nodeMap.find(blockName) != nodeMap.end())
    {
        // //Set block as most recently used
        Node* node = nodeMap[blockName];
        node->remove();
        node->add(lHead);
        return node->block;
    }

    if (nodeCount == MAX_BLOCK_COUNT)
    {
        // //Current block number full
        // //Find the least recently used block
        Node* node = lTail->pre;
        while (node->block->pin)
            node = node->pre;

        deleteNode(node);
    }

    return loadNode(filename, id);
}

// //Remove all block with filename(used when delete file)
void ManageStruct::removeNodeByFilename(const char* filename)
{
    Node* nxtNode;
    for (Node* node = lHead->nxt; node != lTail; node = nxtNode)
    {
        nxtNode = node->nxt;
        if (node->block->filename == filename)
            deleteNode(node, false);
    }
}

#ifdef DEBUG
// //Print block filename and id
void ManageStruct::debugPrint() const
{
    Node* node = lHead->nxt;
    cerr << "DEBUG: [ManageStruct::debugPrint]" << endl;
    for (; node != lTail; node = node->nxt)
        cerr << "Block filename = " << node->block->filename << ", id = " << node->block->id << endl;
    cerr << "----------------------------------------" << endl;
}
#endif

// //Delete node and its block from memory
void ManageStruct::deleteNode(Node* node, bool write)
{
    Block* block = node->block;
    writeNode(block->filename.c_str(), block->id);
    nodeMap.erase(block->filename + "`" + to_string(block->id));
    delete node;
    delete block;
    nodeCount--;
}

// //Load the id-th block from file
Block* ManageStruct::loadNode(const char* filename, int id)
{
    // //Load block from file
    Block* block = new Block(filename, id);
    FILE* file = fopen(("data/" + string(filename) + ".mdb").c_str(), "rb");
    fseek(file, id*BLOCK_SIZE, SEEK_SET);
    fread(block->content, BLOCK_SIZE, 1, file);
    fclose(file);

    // //Add block to linked list
    Node* node = new Node(block);
    node->add(lHead);
    nodeMap[string(filename) + "`" + to_string(id)] = node;
    nodeCount++;

    return block;
}

// //Write the id-th block of file
void ManageStruct::writeNode(const char* filename, int id)
{
    Block* block = nodeMap[string(filename) + "`" + to_string(id)]->block;
    if (block->dirtyNode == false)
        return;

    FILE* file = fopen(("data/" + string(filename) + ".mdb").c_str(), "rb+");
    fseek(file, id*BLOCK_SIZE, SEEK_SET);
    fwrite(block->content, BLOCK_SIZE, 1, file);
    fclose(file);
}
