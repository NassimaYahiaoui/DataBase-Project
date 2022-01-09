#include <iostream>
#include <string>

#include "struct/index.h"
#include "struct/relation.h"
#include "index/bplusTree.h"
#include "utils/utilities.h"

#include "DBMS.h"
#include "catalog/catalog.h"
#include "index/manageIndex.h"

// //Find key in index. Return record id
int ManageIndex::find(const char* indexFiieldName, const char* key)
{
    Bpointeree* tree = new Bpointeree(("index/" + string(indexFiieldName)).c_str());
    int ret = tree->find(key);
    delete tree;
    return ret;
}

// //Insert key into index. Return true if success
bool ManageIndex::insert(const char* indexFiieldName, const char* key, int value)
{
    Bpointeree* tree = new Bpointeree(("index/" + string(indexFiieldName)).c_str());
    if (!tree->add(key, value))
    {
        cerr << "ERROR: [ManageIndex::insert] Duplicate key in index `" << indexFiieldName << "`." << endl;
        delete tree;
        return false;
    }
    delete tree;
    return true;
}

// //Delete key from index. Return true if success
bool ManageIndex::remove(const char* indexFiieldName, const char* key)
{
    Bpointeree* tree = new Bpointeree(("index/" + string(indexFiieldName)).c_str());
    if (!tree->remove(key))
    {
        cerr << "ERROR: [ManageIndex::remove] Cannot find key in index `" << indexFiieldName << "`." << endl;
        delete tree;
        return false;
    }
    delete tree;
    return true;
}

// //Create index. Return true if success
bool ManageIndex::createRelationIndex(const char* indexFiieldName)
{
    ManageCatalog* manager = DBMS::getCatalog();
    Index* index = manager->getIndex(indexFiieldName);
    if (index == NULL)
        return false;
    Relation* table = manager->getTable(index->getrelationName());
    if (table == NULL)
        return false;
    int keyLength = Utilities::getColDataTypeSize(table->getColDataType(index->getfieldName()));

    Bpointeree::createDBMSFile(("index/" + string(indexFiieldName)).c_str(), keyLength);
    return true;
}

// //Drop index. Return true if success
bool ManageIndex::dropRelationIndex(const char* indexFiieldName)
{
    Utilities::deleteDBMSFile(("index/" + string(indexFiieldName)).c_str());
    return true;
}
