#include <cstring>
#include <iostream>
#include <unordered_set>

#include "global.h"
#include "utils/utilities.h"
#include "catalog/catalog.h"

using namespace std;

// //Constructor
ManageCatalog::ManageCatalog()
{
    // //Load catalog file
    relationNameFile = new Heap("catalog/tables");
    indexMetaFile = new Heap("catalog/indices");

    // //Read catalog file
    int id;
    char tableData[MAX_NAME_LENGTH*2];
    char indexData[MAX_NAME_LENGTH*3];
    while ((id = relationNameFile->getNextRecord(tableData)) >= 0)
    {
        Relation* table = new Relation(tableData);
        tableIdMap[table->getRelationName()] = id;
        tableMap[table->getRelationName()] = table;
    }
    while ((id = indexMetaFile->getNextRecord(indexData)) >= 0)
    {
        Index* index = new Index(indexData);
        indexIdMap[index->getRelationName()] = id;
        indexMap[index->getRelationName()] = index;
    }
}

// //Destructor
ManageCatalog::~ManageCatalog()
{
    // //Clean up table and index pointers
    for (auto table : tableMap)
        delete table.second;
    for (auto index : indexMap)
        delete index.second;

    // //Clean up catalog file pointers
    delete relationNameFile;
    delete indexMetaFile;
}

// //Get table by name
Relation* ManageCatalog::getTable(const char* relationName) const
{
    if (tableMap.find(relationName) == tableMap.end())
    {
        cerr << "ERROR: [ManageCatalog::getTable] Table `" << relationName << "` does not exist!" << endl;
        return NULL;
    }
    return tableMap.at(relationName);
}

// //Create database. Return true if success
bool ManageCatalog::createDatabase(
    const char* dbName
)
{
    if (tableMap.find(dbName) != tableMap.end())
    {
        cerr << "ERROR: [ManageCatalog::createDatabase] Database `" << dbName << "` already exists!" << endl;
        return false;
    }


    Heap::createDBMSFile(("catalog/table_" + string(dbName)).c_str(), MAX_NAME_LENGTH + 3);
    Heap* tableDataFile = new Heap(("catalog/table_" + string(dbName)).c_str());



    return true;
}

// //Create table. Return true if success
bool ManageCatalog::createRelation(
    const char* relationName, const char* primary,
    const vector<string>* fieldName, const vector<short>* colDataType, vector<char>* fieldUniqueID
)
{
    if (tableMap.find(relationName) != tableMap.end())
    {
        cerr << "ERROR: [ManageCatalog::createRelation] Table `" << relationName << "` already exists!" << endl;
        return false;
    }

    int countCol = fieldName->size();

    // //Make sure each field name is unique
    unordered_set<string> fieldNameSet;
    for (auto name : *fieldName)
    {
        if (fieldNameSet.find(name) != fieldNameSet.end())
        {
            cerr << "ERROR: [ManageCatalog::createRelation] Duplicate field name `" << name << "`!" << endl;
            return false;
        }
        fieldNameSet.insert(name);
    }

    // //Make sure primary key is among field names
    if (fieldNameSet.find(primary) == fieldNameSet.end())
    {
        cerr << "ERROR: [ManageCatalog::createRelation] Cannot find relation primary key name `" << primary << "` in field names!" << endl;
        return false;
    }

    // //Set primary key to unique
    for (int i = 0; i < countCol; i++)
        if (primary == fieldName->at(i))
            fieldUniqueID->at(i) = 1;

    // //Write table data to catalog file
    char tableData[MAX_NAME_LENGTH*2];
    memcpy(tableData, relationName, MAX_NAME_LENGTH);
    memcpy(tableData + MAX_NAME_LENGTH, primary, MAX_NAME_LENGTH);
    int id = relationNameFile->insertRecord(tableData);

    // //Record table to map
    tableIdMap[relationName] = id;
    tableMap[relationName] = new Relation(tableData);

    // //Create table column data file
    Heap::createDBMSFile(("catalog/table_" + string(relationName)).c_str(), MAX_NAME_LENGTH + 3);
    Heap* tableDataFile = new Heap(("catalog/table_" + string(relationName)).c_str());

    for (int i = 0; i < countCol; i++)
    {
        char colData[MAX_NAME_LENGTH + 3];
        memcpy(colData, fieldName->at(i).c_str(), MAX_NAME_LENGTH);
        memcpy(colData + MAX_NAME_LENGTH, &(colDataType->at(i)), 2);
        memcpy(colData + MAX_NAME_LENGTH + 2, &(fieldUniqueID->at(i)), 1);
        tableDataFile->insertRecord(colData);
    }

    return true;
}

// //Drop table. Return true if success
bool ManageCatalog::dropRelation(const char* relationName)
{
    if (tableMap.find(relationName) == tableMap.end())
    {
        cerr << "ERROR: [ManageCatalog::dropRelation] Table `" << relationName << "` does not exist!" << endl;
        return false;
    }

    // //Delete table
    delete tableMap[relationName];
    relationNameFile->deleteRecord(tableIdMap[relationName]);
    tableIdMap.erase(relationName);
    tableMap.erase(relationName);

    // //Delete table column data file
    Utilities::deleteDBMSFile(("catalog/table_" + string(relationName)).c_str());

    return true;
}

// //Get index by name
Index* ManageCatalog::getIndex(const char* indexFiieldName) const
{
    if (indexMap.find(indexFiieldName) == indexMap.end())
    {
        cerr << "ERROR: [ManageCatalog::getIndex] Index `" << indexFiieldName << "` does not exist!" << endl;
        return NULL;
    }
    return indexMap.at(indexFiieldName);
}

// //Get all indices by relation name
void ManageCatalog::getIndexByTable(const char* relationName, vector<Index*>* vec)
{
    for (auto item : indexMap)
    {
        Index* index = item.second;
        if (strcmp(index->getrelationName(), relationName) == 0)
            vec->push_back(index);
    }
}

// //Get index by relation name and field name
Index* ManageCatalog::getIndexByTableCol(const char* relationName, const char* fieldName)
{
    for (auto item : indexMap)
    {
        Index* index = item.second;
        if (
            strcmp(index->getrelationName(), relationName) == 0 &&
            strcmp(index->getfieldName(), fieldName) == 0
        )
            return index;
    }
    return NULL;
}

// //Create index. Return true if success
bool ManageCatalog::createRelationIndex(const char* indexFiieldName, const char* relationName, const char* fieldName)
{
    if (indexMap.find(indexFiieldName) != indexMap.end())
    {
        cerr << "ERROR: [ManageCatalog::createRelationIndex] Index `" << indexFiieldName << "` already exists!" << endl;
        return false;
    }

    if (tableMap.find(relationName) == tableMap.end())
    {
        cerr << "ERROR: [ManageCatalog::createRelationIndex] Table `" << relationName << "` does not exist!" << endl;
        return false;
    }

    // //Check if fieldName exists, and if column is unique
    Relation* table = tableMap[relationName];
    char isUnique = table->getUnique(fieldName);
    if (isUnique == 0)
        cerr << "ERROR: [ManageCatalog::createRelationIndex] Column `" << fieldName << "` is not unique!" << endl;
    if (isUnique != 1)
        return false;

    // //Check if there is already an index with same relation name and field name
    Index* exist = getIndexByTableCol(relationName, fieldName);
    if (exist != NULL)
    {
        cerr << "ERROR: [ManageCatalog::createRelationIndex] Index with relation name `" << relationName << "` and field name `" << fieldName << "` already exists(relation index name `" << exist->getRelationName() << "`)!" << endl;
        return false;
    }

    // //Write index data to catalog file
    char indexData[MAX_NAME_LENGTH*3];
    memcpy(indexData, indexFiieldName, MAX_NAME_LENGTH);
    memcpy(indexData + MAX_NAME_LENGTH, relationName, MAX_NAME_LENGTH);
    memcpy(indexData + MAX_NAME_LENGTH*2, fieldName, MAX_NAME_LENGTH);
    int id = indexMetaFile->insertRecord(indexData);

    // //Record index into map
    indexIdMap[indexFiieldName] = id;
    indexMap[indexFiieldName] = new Index(indexData);

    return true;
}

// //Drop index. Return true if success
bool ManageCatalog::dropRelationIndex(const char* indexFiieldName)
{
    if (indexMap.find(indexFiieldName) == indexMap.end())
    {
        cerr << "ERROR: [ManageCatalog::dropRelationIndex] Index `" << indexFiieldName << "` does not exist!" << endl;
        return false;
    }

    // //Delete index
    delete indexMap[indexFiieldName];
    indexMetaFile->deleteRecord(indexIdMap[indexFiieldName]);
    indexIdMap.erase(indexFiieldName);
    indexMap.erase(indexFiieldName);

    return true;
}

// //Load table column info. Returns relation relation length of a record
int ManageCatalog::loadTableColInfo(
    const char* relationName, vector<string>* fieldName,
    vector<short>* colDataType, vector<char>* fieldUniqueID
)
{
    Heap* colFile = new Heap(("catalog/table_" + string(relationName)).c_str());

    int countCol = colFile->getRecordCount();
    char colData[MAX_NAME_LENGTH + 3];
    for (int i = 0; i < countCol; i++)
    {
        colFile->getNextRecord(colData);

        char nameData[MAX_NAME_LENGTH];
        memcpy(nameData, colData, MAX_NAME_LENGTH);
        fieldName->push_back(nameData);

        short type = *(reinterpret_cast<short*>(colData + MAX_NAME_LENGTH));
        colDataType->push_back(type);

        char unique = colData[MAX_NAME_LENGTH + 2];
        fieldUniqueID->push_back(unique);
    }

    delete colFile;
    return countCol;
}

#ifdef DEBUG
// //Print all tables and indices info
void ManageCatalog::debugPrint() const
{
    cerr << "DEBUG: [ManageCatalog::debugPrint] debugPrint begin" << endl;
    for (auto table : tableMap)
        table.second->debugPrint();
    for (auto index : indexMap)
        index.second->debugPrint();
    cerr << "DEBUG: [ManageCatalog::debugPrint] debugPrint end" << endl;
}
#endif
