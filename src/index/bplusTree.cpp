#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

#include "global.h"
#include "DBMS.h"
#include "buffer/buffer.h"
#include "index/bplusTreeNode.h"
#include "index/bplusTree.h"

using namespace std;

// //States
const int Bpointeree::BpointerEE_FAILED = -1;
const int Bpointeree::BpointerEE_NORMAL = 0;
const int Bpointeree::BpointerEE_ADD = 1;
const int Bpointeree::BpointerEE_REMOVE = 2;
const int Bpointeree::BpointerEE_CHANGE = 3;

// //Create B+ Tree file
void Bpointeree::createDBMSFile(const char* _filename, int _keyLength, int _order)
{
    // //Calculate order if not provided
    if (_order < 0)
        _order = (BLOCK_SIZE - 8) / (_keyLength + 4) + 1;

    // //Create file
    FILE* file = fopen(("data/" + string(_filename) + ".mdb").c_str(), "wb");
    int header[] = {_order, _keyLength, 0, -1, -1};
    fwrite(header, 4, 5, file);
    fclose(file);
}

// //Constructor
Bpointeree::Bpointeree(const char* _filename): filename(_filename)
{
    ManageStruct* manager = DBMS::getManageStruct();
    Block* header = manager->getBlock(_filename, 0);

    // //Get header information
    order = *(reinterpret_cast<int*>(header->content));
    keyLength = *(reinterpret_cast<int*>(header->content + 4));
    nodeCount = *(reinterpret_cast<int*>(header->content + 8));
    root = *(reinterpret_cast<int*>(header->content + 12));
    firstEmpty = *(reinterpret_cast<int*>(header->content + 16));

    key = new char[keyLength];
}

// //Destructor
Bpointeree::~Bpointeree()
{
    delete[] key;
}

// //Find value of key
int Bpointeree::find(const char* _key)
{
    memcpy(key, _key, keyLength);
    return root < 0 ? BpointerEE_FAILED : find(root);
}

// //Add key-value pair. Return true if success
bool Bpointeree::add(const char* _key, int _value)
{
    memcpy(key, _key, keyLength);
    value = _value;
    int res = root < 0 ? BpointerEE_ADD : add(root);

    if (res == BpointerEE_ADD)
    {
        // //Create new root
        int newRoot = getFirstEmpty();
        BpointereeNode* node = new BpointereeNode(filename.c_str(), newRoot, keyLength, root < 0, root < 0 ? -1 : root);
        node->insert(0, key, value);
        delete node;
        root = newRoot;
    }
    updateInformation();

    return res != BpointerEE_FAILED;
}

// //Remove key-value pair. Return true if success
bool Bpointeree::remove(const char* _key)
{
    memcpy(key, _key, keyLength);
    int res = root < 0 ? false : remove(root, 0, true, NULL);
    updateInformation();
    return res != BpointerEE_FAILED;
}

#ifdef DEBUG
// //Print tree structure
void Bpointeree::debugPrint()
{
    cerr << "DEBUG: [Bpointeree::debugPrint] Debug print start." << endl;
    cerr << "Node number = " << nodeCount << ", first empty = " << firstEmpty << endl;
    if (root >= 0)
    {
        cerr << "Root = " << root << endl;
        debugPrint(root);
    }
    else
        cerr << "Empty tree." << endl;
    cerr << "DEBUG: [Bpointeree::debugPrint] Debug print end." << endl;
}
#endif

// //Recursive function for finding value
int Bpointeree::find(int id)
{
    BpointereeNode* node = new BpointereeNode(filename.c_str(), id, keyLength);
    int pos = node->findKeyPosition(key);

    int ret = BpointerEE_FAILED;
    if (node->isleafNode())
    {
        // //Check if key is found
        if (pos > 0)
        {
            const char* k = node->getKey(pos);
            if (memcmp(key, k, keyLength) == 0)
                ret = node->getPointer(pos);
        }
    }
    else
        ret = find(node->getPointer(pos));

    delete node;
    return ret;
}

// //Recursive function for adding key-value pair
int Bpointeree::add(int id)
{
    BpointereeNode* node = new BpointereeNode(filename.c_str(), id, keyLength);
    int pos = node->findKeyPosition(key);

    int res = node->isleafNode() ? BpointerEE_ADD : add(node->getPointer(pos));
    int ret = BpointerEE_NORMAL;

    if (node->isleafNode() && pos > 0)
    {
        // //Check for duplicate
        const char* k = node->getKey(pos);
        if (memcmp(key, k, keyLength) == 0)
            res = BpointerEE_FAILED;
    }

    if (res == BpointerEE_FAILED)
        // //Duplicate key
        ret = BpointerEE_FAILED;
    else if (res == BpointerEE_ADD)
    {
        // //Add new key-value
        node->insert(pos, key, value);

        if (node->getSize() >= order)
        {
            // //Node full. Split
            int newId = getFirstEmpty();
            BpointereeNode* newNode = node->split(newId, key);
            value = newId;

            delete newNode;
            ret = BpointerEE_ADD;
        }
    }

    delete node;
    return ret;
}

// //Recursive function for deleting key-value pair
int Bpointeree::remove(int id, int sibId, bool leftSib, const char* parentKey)
{
    BpointereeNode* node = new BpointereeNode(filename.c_str(), id, keyLength);
    BpointereeNode* sib = NULL;
    if (id != root)
        sib = new BpointereeNode(filename.c_str(), sibId, keyLength);
    int pos = node->findKeyPosition(key);

    int res;
    if (node->isleafNode())
        res = BpointerEE_FAILED;
    else
    {
        int nxtId = node->getPointer(pos);
        int nxtSib = node->getPointer(pos > 0 ? pos - 1 : pos + 1);
        const char* nxtParentKey = node->getKey(pos > 0 ? pos : pos + 1);
        res = remove(nxtId, nxtSib, pos > 0, nxtParentKey);
    }

    if (node->isleafNode())
    {
        // //Check if key is found
        if (pos > 0)
        {
            const char* k = node->getKey(pos);
            if (memcmp(key, k, keyLength) == 0)
                res = BpointerEE_REMOVE;
        }
    }

    int ret = BpointerEE_NORMAL;
    if (res == BpointerEE_FAILED)
        // //Key not found
        ret = BpointerEE_FAILED;
    else if (res == BpointerEE_CHANGE)
        // //Change key
        node->setKey(pos > 0 ? pos : pos + 1, key);
    else if (res == BpointerEE_REMOVE)
    {
        // //Delete key
        node->remove(pos > 0 ? pos : pos + 1);

        if (id == root)
        {
            if (node->getSize() == 0)
            {
                root = node->getPointer(0);
                removeNode(id);
                node->markRemoved();
            }
        }
        else
        {
            int lim = (order+1)/2 - 1;
            if (node->getSize() < lim)
            {
                if (sib->getSize() > lim)
                {
                    // //Borrow key from sibling
                    const char* k = node->borrow(sib, leftSib, parentKey);
                    memcpy(key, k, keyLength);
                    ret = BpointerEE_CHANGE;
                }
                else
                {
                    // //Merge sibling
                    if (leftSib)
                    {
                        sib->mergeRight(node, parentKey);
                        removeNode(id);
                        node->markRemoved();
                    }
                    else
                    {
                        node->mergeRight(sib, parentKey);
                        removeNode(sibId);
                        sib->markRemoved();
                    }
                    ret = BpointerEE_REMOVE;
                }
            }
        }
    }

    delete node;
    if (sib != NULL)
        delete sib;
    return ret;
}

// //Get first empty block id
int Bpointeree::getFirstEmpty()
{
    if (firstEmpty < 0)
        return ++nodeCount;

    int ret = firstEmpty;
    ManageStruct* manager = DBMS::getManageStruct();
    Block* block = manager->getBlock(filename.c_str(), firstEmpty);
    firstEmpty = *(reinterpret_cast<int*>(block->content));
    return ret;
}

// //Remove block in file
void Bpointeree::removeNode(int id)
{
    ManageStruct* manager = DBMS::getManageStruct();
    Block* block = manager->getBlock(filename.c_str(), id);
    memcpy(block->content, &firstEmpty, 4);
    firstEmpty = id;
}

// //Update header information
void Bpointeree::updateInformation()
{
    ManageStruct* manager = DBMS::getManageStruct();
    Block* block = manager->getBlock(filename.c_str(), 0);

    memcpy(block->content + 8, &nodeCount, 4);
    memcpy(block->content + 12, &root, 4);
    memcpy(block->content + 16, &firstEmpty, 4);

    block->dirtyNode = true;
}

#ifdef DEBUG
// //Recursive function for tree structure printing
void Bpointeree::debugPrint(int id)
{
    BpointereeNode* node = new BpointereeNode(filename.c_str(), id, keyLength);

    cerr << "Block id = " << id << ", isleafNode = " << node->isleafNode() << endl;
    cerr << "Keys:";
    for (int i = 1; i <= node->getSize(); i++)
    {
        cerr << " ";
        const char* k = node->getKey(i);
        for (int j = 0; j < keyLength; j++)
        {
            cerr << (int)k[j];
            if (j < keyLength-1)
                cerr << "~";
        }
    }
    cerr << endl;
    cerr << "Pointers: ";
    for (int i = 0; i <= node->getSize(); i++)
        cerr << " " << node->getPointer(i);
    cerr << endl;

    if (!node->isleafNode())
        for (int i = 0; i <= node->getSize(); i++)
            debugPrint(node->getPointer(i));

    delete node;
}
#endif
