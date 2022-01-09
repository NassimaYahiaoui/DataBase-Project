#ifndef _BpointerEE_NODE_H
#define _BpointerEE_NODE_H

#include <vector>
#include <string>

using namespace std;

class BpointereeNode
{
public:

    // //Constructor
    BpointereeNode(const char* _filename, int _id, int _keyLength);
    BpointereeNode(const char* _filename, int _id, int _keyLength, bool _leafNode, int firstpointer);

    // //Destructor
    ~BpointereeNode();

    // //Get node size
    int getSize() const;

    // //Get key length
    int getKeyLength() const;

    // //If node is leafNode
    bool isleafNode() const;

    // //Get key
    const char* getKey(int pos) const;

    // //Get pointer
    int getPointer(int pos) const;

    // //Find key's position
    int findKeyPosition(const char* key) const;

    // //Set key at position
    void setKey(int pos, const char* key);

    // //Set pointer at position
    void setPointer(int pos, int pointer);

    // //Set the block as removed
    void markRemoved();

    // //Insert key-pointer after position
    void insert(int pos, const char* key, int pointer);

    // //Remove key-pointer at position
    void remove(int pos);

    // //Split into two nodes. Return new node
    BpointereeNode* split(int newId, char* newKey);

    // //Borrow a key from sibling. Return new parent key
    const char* borrow(BpointereeNode* sib, bool leftSib, const char* parentKey);

    // //Merge right sibling
    void mergeRight(BpointereeNode* sib, const char* parentKey);

private:

    // //Node filename
    string filename;

    // //Block id in file
    int id;

    // //Node size
    int size;

    // //Length of each key
    int keyLength;

    // //If node is leafNode
    bool leafNode;

    // //If node has been modified
    bool dirtyNode;

    // //If block has been removed
    bool diableRemove;
    
    // //Node keys
    vector<char*> keys;

    // //Node pointers
    vector<int> pointers;

};

#endif
