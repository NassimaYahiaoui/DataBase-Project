#include <cstring>
#include <iostream>
#include <algorithm>

#include "global.h"
#include "DBMS.h"
#include "buffer/buffer.h"
#include "index/bplusTreeNode.h"

using namespace std;

// //Constructor(from file)
BpointereeNode::BpointereeNode(
    const char* _filename, int _id, int _keyLength
): filename(_filename), id(_id), keyLength(_keyLength)
{
    ManageStruct* manager = DBMS::getManageStruct();
    Block* block = manager->getBlock(_filename, id);
    char* data = block->content;

    size = *(reinterpret_cast<int*>(data));
    keys.push_back(NULL);
    pointers.push_back(*(reinterpret_cast<int*>(data + 4)));
    leafNode = pointers[0] < 0;
    diableRemove = false;

    int bias = 8;
    for (int i = 1; i <= size; i++)
    {
        char* k = new char[keyLength];
        memcpy(k, data + bias, keyLength);
        keys.push_back(k);
        pointers.push_back(*reinterpret_cast<int*>(data + bias + keyLength));
        bias += keyLength + 4;
    }
}

// //Constructor(construct an empty node)
BpointereeNode::BpointereeNode(
    const char* _filename, int _id, int _keyLength, bool _leafNode, int firstpointer
): filename(_filename), id(_id), keyLength(_keyLength), leafNode(_leafNode)
{
    size = 0;
    keys.push_back(NULL);
    pointers.push_back(firstpointer);
    dirtyNode = true;
    diableRemove = false;
}

// //Destructor
BpointereeNode::~BpointereeNode()
{
    // //Write modification to block
    if (dirtyNode && !diableRemove)
    {
        ManageStruct* manager = DBMS::getManageStruct();
        Block* block = manager->getBlock(filename.c_str(), id);
        char* data = block->content;

        
        memcpy(data, &size, 4);

     
        memcpy(data + 4, &pointers[0], 4);

        
        int bias = 8;
        for (int i = 1; i <= size; i++)
        {
            memcpy(data + bias, keys[i], keyLength);
            memcpy(data + bias + keyLength, &pointers[i], 4);
            bias += keyLength + 4;
        }

        block->dirtyNode = true;
    }

    // //Clean up keys
    for (auto k : keys)
        if (k != NULL)
            delete[] k;
}

// //Get node size
int BpointereeNode::getSize() const
{
    return size;
}

// //Get key length
int BpointereeNode::getKeyLength() const
{
    return keyLength;
}

// //If node is leafNode
bool BpointereeNode::isleafNode() const
{
    return leafNode;
}

// //Get key
const char* BpointereeNode::getKey(int pos) const
{
    if (pos > size || pos <= 0)
    {
        cerr << "ERROR: [BpointereeNode::getKey] Position " << pos << " out of range!" << endl;
        return NULL;
    }

    return keys[pos];
}

// //Get pointer
int BpointereeNode::getPointer(int pos) const
{
    if (pos > size || pos < 0)
    {
        cerr << "ERROR: [BpointereeNode::getKey] Position " << pos << " out of range!" << endl;
        return -1;
    }
    return pointers[pos];
}

// //Find key's position
int BpointereeNode::findKeyPosition(const char* key) const
{
    return upper_bound(
        keys.begin() + 1, keys.end(), key,
        [&](const char* a, const char* b) { return memcmp(a, b, keyLength) < 0;}
    ) - (keys.begin() + 1);
}

// //Set key at position
void BpointereeNode::setKey(int pos, const char* key)
{
    if (pos > size || pos <= 0)
    {
        cerr << "ERROR: [BpointereeNode::setKey] Position " << pos << " out of range!" << endl;
        return;
    }

    dirtyNode = true;
    memcpy(keys[pos], key, keyLength);
}

// //Set pointer at position
void BpointereeNode::setPointer(int pos, int pointer)
{
    if (pos > size || pos < 0)
    {
        cerr << "ERROR: [BpointereeNode::setPointer] Position " << pos << " out of range!" << endl;
        return;
    }

    dirtyNode = true;
    pointers[pos] = pointer;
}

// //Set block as removed
void BpointereeNode::markRemoved()
{
    diableRemove = true;
}

// //Insert key-pointer after position
void BpointereeNode::insert(int pos, const char* key, int pointer)
{
    if (pos > size || pos < 0)
    {
        cerr << "ERROR: [BpointereeNode::insert] Position " << pos << " out of range!" << endl;
        return;
    }

    dirtyNode = true;
    char* k = new char[keyLength];
    memcpy(k, key, keyLength);
    if (pos == size)
    {
        keys.push_back(k);
        pointers.push_back(pointer);
    }
    else
    {
        keys.insert(keys.begin() + pos + 1, k);
        pointers.insert(pointers.begin() + pos + 1, pointer);
    }
    size++;
}

// //Rmove key-pointer at position
void BpointereeNode::remove(int pos)
{
    if (pos > size || pos <= 0)
    {
        cerr << "ERROR: [BpointereeNode::insert] Position " << pos << " out of range!" << endl;
        return;
    }

    dirtyNode = true;
    if (pos == size)
    {
        keys.pop_back();
        pointers.pop_back();
    }
    else
    {
        keys.erase(keys.begin() + pos);
        pointers.erase(pointers.begin() + pos);
    }
    size--;
}

// //Split into two nodes. Return new node
BpointereeNode* BpointereeNode::split(int newId, char* newKey)
{
    dirtyNode = true;

    int pos = size/2 + (leafNode ? 0 : 1);
    memcpy(newKey, keys[size/2 + 1], keyLength);
    BpointereeNode* ret = new BpointereeNode(filename.c_str(), newId, keyLength, leafNode, leafNode ? -1 : pointers[pos]);

    // //Copy former half of keys-pointers to new node
    for (pos++; pos <= size; pos++)
        ret->insert(ret->getSize(), keys[pos], pointers[pos]);

    size /= 2;
    keys.resize(size + 1);
    pointers.resize(size + 1);

    return ret;
}

// //Borrow a key from sibling. Return new parent key
const char* BpointereeNode::borrow(BpointereeNode* sib, bool leftSib, const char* parentKey)
{
    dirtyNode = true;

    if (leftSib)
    {
        
        int sibSize = sib->getSize();
        const char* sibKey = sib->getKey(sibSize);
        int sibpointer = sib->getPointer(sibSize);
        sib->remove(sibSize);

        if (leafNode)
            insert(0, sibKey, sibpointer);
        else
        {
            int pointer = pointers[0];
            pointers[0] = sibpointer;
            insert(0, parentKey, pointer);
        }

        return sibKey;
    }
    else
    {
        // //Borrow from right sibling
        const char* sibKey = sib->getKey(1);
        int sibpointer0 = sib->getPointer(0);
        int sibpointer1 = sib->getPointer(1);
        sib->remove(1);

        if (leafNode)
        {
            insert(size, sibKey, sibpointer1);
            return sib->getKey(1);
        }
        else
        {
            sib->setPointer(0, sibpointer1);
            insert(size, parentKey, sibpointer0);
            return sibKey;
        }
    }
}

// //Merge right sibling
void BpointereeNode::mergeRight(BpointereeNode* sib, const char* parentKey)
{
    dirtyNode = true;

    int sibSize = sib->getSize();
    if (!leafNode)
        insert(size, parentKey, sib->getPointer(0));
    for (int i = 1; i <= sibSize; i++)
        insert(size, sib->getKey(i), sib->getPointer(i));
}
