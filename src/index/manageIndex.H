#ifndef _INDEX_MANAGER_H
#define _INDEX_MANAGER_H

class ManageIndex
{
public:

    // //Find key in index. Return record id
    int find(const char* indexFiieldName, const char* key);

    // //Insert key into index. Return true if success
    bool insert(const char* indexFiieldName, const char* key, int value);

    // //Delete key from index. Return true if success
    bool remove(const char* indexFiieldName, const char* key);

    // //Create index. Return true if success
    bool createRelationIndex(const char* indexFiieldName);

    // //Drop index. Return true if success
    bool dropRelationIndex(const char* indexFiieldName);
};

#endif
