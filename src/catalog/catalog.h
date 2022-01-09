#ifndef _CATALOG_MANAGER_H
#define _CATALOG_MANAGER_H

#include <vector>
#include <string>
#include <unordered_map>

#include "file/heap.h"
#include "struct/relation.h"
#include "struct/index.h"

using namespace std;

class ManageCatalog
{
public:

    // //Constructor
    ManageCatalog();

    // //Destructor
    ~ManageCatalog();

    // //Get table by name
    Relation* getTable(const char* relationName) const;

    // //Create table. Return true if success
    bool createRelation(
        const char* relationName, const char* primary,
        const vector<string>* fieldName, const vector<short>* colDataType, vector<char>* fieldUniqueID
    );
	
	bool createDatabase(
        const char* dbName
    );

    // //Drop table. Return true if success
    bool dropRelation(const char* relationName);

    // //Get index by name
    Index* getIndex(const char* indexFiieldName) const;

    // //Get all indices by relation name
    void getIndexByTable(const char* relationName, vector<Index*>* vec);

    // //Get index by relation name and field name
    Index* getIndexByTableCol(const char* relationName, const char* fieldName);

    // //Create index. Return true if success
    bool createRelationIndex(const char* indexFiieldName, const char* relationName, const char* fieldName);

    // //Drop index. Return true if success
    bool dropRelationIndex(const char* indexFiieldName);

    // //Load table column info. Returns relation relation length of a record
    int loadTableColInfo(
        const char* relationName, vector<string>* fieldNameData,
        vector<short>* colDataType, vector<char>* fieldUniqueID
    );

#ifdef DEBUG
    // //Print all tables and indices info
    void debugPrint() const;
#endif

private:

    // //Table map
    unordered_map<string, Relation*> tableMap;
    unordered_map<string, int> tableIdMap;

    // //relation index name map
    unordered_map<string, Index*> indexMap;
    unordered_map<string, int> indexIdMap;

    // //relation name file
    Heap* relationNameFile;

    // //Index meta data file
    Heap* indexMetaFile;
};

#endif
