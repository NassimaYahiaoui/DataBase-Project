#include <cstring>
#include <ctime>
#include <iostream>

#include "global.h"
#include "struct/relation.h"
#include "file/heap.h"
#include "utils/utilities.h"

#include "DBMS.h"
#include "catalog/catalog.h"
#include "record/record.h"
#include "api/api.h"

using namespace std;

// //Select record. Return number of records selected
int Api::select(
    const char* relationName, const vector<string>* fieldName,
    const vector<int>* cond, const vector<string>* operand
)
{
    // //Get manager and table
    ManageCatalog* manageCatalog = DBMS::getCatalog();

    Relation* table = manageCatalog->getTable(relationName);
    if (table == NULL)
        return -1;

    int condCount = (int)cond->size();

    // //Check condition validity
    for (int i = 0; i < condCount; i++)
        if (
            cond->at(i) != COND_EQ &&
            cond->at(i) != COND_NE &&
            cond->at(i) != COND_LT &&
            cond->at(i) != COND_GT &&
            cond->at(i) != COND_LE &&
            cond->at(i) != COND_GE
        )
        {
            cerr << "ERROR: [Api::select] Unknown condition `" << cond->at(i) << "`!" << endl;
            return -1;
        }

    // //Check if field name exists
    for (int i = 0; i < condCount; i++)
    {
        short type = table->getColDataType(fieldName->at(i).c_str());
        if (type == TYPE_NULL)
            return -1;
    }

    // //Get select result
    vector<char*> record;
    vector<int> _;

    int selectCount = filter(
        relationName, fieldName, cond, operand, &record, &_
    );
    if (selectCount < 0)
        return -1;

    // //Print field name
    cout << endl;
    int countCol = table->fieldCount();
    for (int i = 0; i < countCol; i++)
        cout << table->getfieldName(i) << "\t";
    cout << endl << "----------------------------------------" << endl;

    // //Parse and print each record
    vector<char*> parsed;

    for (auto data : record)
    {
        table->recordToVec(data, &parsed);
        for (int i = 0; i < countCol; i++)
        {
            short type = table->getColDataType(table->getfieldName(i));
            
            if (type <= TYPE_CHAR)
                cout << parsed[i] << "\t";
            else if (type == TYPE_INT)
                cout << *(reinterpret_cast<int*>(parsed[i])) << "\t";
            else if (type == TYPE_FLOAT)
                cout << *(reinterpret_cast<float*>(parsed[i])) << "\t";
        }
        cout << endl;

        // //Clean up parsed record
        for (int i = 0; i < countCol; i++)
            delete[] parsed[i];
        parsed.clear();
    }
    cout << endl;

    return selectCount;
}

// //Insert record. Return true if success
bool Api::insert(const char* relationName, const vector<string>* value)
{
    // //Get manager and table
    ManageCatalog* manageCatalog = DBMS::getCatalog();
    ManageRecord* manageRecord = DBMS::getmanageRecord();
    ManageIndex* manageIndex = DBMS::getIndex();

    Relation* table = manageCatalog->getTable(relationName);
    if (table == NULL)
        return false;

    // //Parse data
    char* data = new char[table->calRecordLenth()];
    if (!table->copyVecRecord(value, data))
    {
        // //Parsing failed. Clean up
        delete[] data;
        return false;
    }

    // //Get insert result
    int res = manageRecord->insert(relationName, data);
    if (res < 0)
    {
        // //Insert failed. Clean up
        delete[] data;
        return false;
    }

    // //Insert data into indices
    vector<char*> vec;
    table->recordToVec(data, &vec);
    vector<Index*> indices;
    manageCatalog->getIndexByTable(relationName, &indices);

    for (auto index : indices)
        manageIndex->insert(
            index->getRelationName(), vec[table->getId(index->getfieldName())], res
        );

    for (auto t : vec)
        delete[] t;
    delete[] data;
    return true;
}

// //Delete record. Return number of records deleted
int Api::remove(
    const char* relationName, const vector<string>* fieldName,
    const vector<int>* cond, const vector<string>* operand
)
{
    // //Get manager and table
    ManageCatalog* manageCatalog = DBMS::getCatalog();
    ManageRecord* manageRecord = DBMS::getmanageRecord();
    ManageIndex* manageIndex = DBMS::getIndex();

    Relation* table = manageCatalog->getTable(relationName);
    if (table == NULL)
        return false;

    // //Get select result
    vector<char*> record;
    vector<int> table_ids;

    int selectCount = filter(
        relationName, fieldName, cond, operand, &record, &table_ids
    );
    if (selectCount < 0)
        return -1;

    // //Get delete result
    bool res = manageRecord->remove(relationName, &table_ids);
    if (!res)
        return -1;

    // //Delete data from indices
    vector<char*> vec;
    vector<Index*> indices;
    manageCatalog->getIndexByTable(relationName, &indices);

    for (auto data : record)
    {
        vec.clear();
        table->recordToVec(data, &vec);

        for (auto index : indices)
            manageIndex->remove(
                index->getRelationName(), vec[table->getId(index->getfieldName())]
            );

        for (auto t : vec)
            delete[] t;
    }

    return selectCount;
}

// //Create database. Return true if success
// //Create table. Return true if success
bool Api::createDatabase(
    const char* dbName
)
{
    // //Get manager
    ManageCatalog* manageCatalog = DBMS::getCatalog();
	//manageCatalog->createDatabase(dbName);
	
   
}

// //Create table. Return true if success
bool Api::createRelation(
    const char* relationName, const char* primary,
    const vector<string>* fieldName, const vector<short>* colDataType, vector<char>* fieldUniqueID
)
{
    // //Get manager
    ManageCatalog* manageCatalog = DBMS::getCatalog();
    ManageRecord* manageRecord = DBMS::getmanageRecord();

    // //Get create result
    if (manageCatalog->createRelation(
        relationName, primary, fieldName, colDataType, fieldUniqueID
    ))
    {
        manageRecord->createRelation(relationName);
        createRelationIndex(to_string(time(0)).c_str(), relationName, primary);
        return true;
    }
    else
        return false;
}

// //Drop table. Return true if success
bool Api::dropRelation(const char* relationName)
{
    // //Get manager
    ManageCatalog* manageCatalog = DBMS::getCatalog();
    ManageRecord* manageRecord = DBMS::getmanageRecord();

    // //Get drop result
    if (manageCatalog->dropRelation(relationName))
    {
        manageRecord->dropRelation(relationName);

        // //Drop all indices related to table
        vector<Index*> indices;
        manageCatalog->getIndexByTable(relationName, &indices);
        for (auto index : indices)
            dropRelationIndex(index->getRelationName());

        return true;
    }
    else
        return false;
}

// //Create index. Return true if success
bool Api::createRelationIndex(const char* indexFiieldName, const char* relationName, const char* fieldName)
{
    // //Get manager
    ManageCatalog* manageCatalog = DBMS::getCatalog();
    ManageIndex* manageIndex = DBMS::getIndex();

    // //Get create result
    if (manageCatalog->createRelationIndex(indexFiieldName, relationName, fieldName))
    {
        manageIndex->createRelationIndex(indexFiieldName);

        // //Add current records into index
        Heap* file = new Heap(("record/student/" + string(relationName)).c_str());
        Relation* table = manageCatalog->getTable(relationName);
        char* data = new char[table->calRecordLenth()];
        int id;
        while ((id = file->getNextRecord(data)) >= 0)
        {
            char dataOut[MAX_VALUE_LENGTH];
            table->getValue(fieldName, data, dataOut);
            manageIndex->insert(indexFiieldName, dataOut, id);
        }

        return true;
    }
    else
        return false;
}

// //Drop index. Return true if success
bool Api::dropRelationIndex(const char* indexFiieldName)
{
    // //Get manager
    ManageCatalog* manageCatalog = DBMS::getCatalog();
    ManageIndex* manageIndex = DBMS::getIndex();

    // //Get drop result
    if (manageCatalog->dropRelationIndex(indexFiieldName))
    {
        manageIndex->dropRelationIndex(indexFiieldName);
        return true;
    }
    else
        return false;
}

// //Filter records satisfying all conditions
// //Return number of records filtered
int Api::filter(
    const char* relationName, const vector<string>* fieldName,
    const vector<int>* cond, const vector<string>* operand,
    vector<char*>* record, vector<int>* table_ids
)
{
    // //Get managers
    ManageCatalog* manageCatalog = DBMS::getCatalog();
    ManageRecord* manageRecord = DBMS::getmanageRecord();
    ManageIndex* manageIndex = DBMS::getIndex();

    Relation* table = manageCatalog->getTable(relationName);
    int condCount = (int)cond->size();

    // //Try to use index
    for (int i = 0; i < condCount; i++)
    {
        if (cond->at(i) != COND_EQ)
            continue;
        Index* index = manageCatalog->getIndexByTableCol(
            relationName, fieldName->at(i).c_str()
        );
        if (index == NULL)
            continue;

        // //Use index to select
        short type = table->getColDataType(fieldName->at(i).c_str());
        char* key = Utilities::getStrData(operand->at(i).c_str(), type);
        if (key == NULL)
            return 0;
        int id = manageIndex->find(index->getRelationName(), key);

        int ret;
        if (id < 0)
            ret = 0;
        else
        {
            // //Record found. Check other conditions
            Heap* file = new Heap(("record/student/" + string(relationName)).c_str());
            const char* data = file->retrieveById(id);
            delete file;

            if (manageRecord->retriveRecord(
                data, relationName, fieldName, cond, operand
            ))
            {
                int recordLength = table->calRecordLenth();
                char* hit = new char[recordLength];
                memcpy(hit, data, recordLength);

                record->push_back(hit);
                table_ids->push_back(id);
                ret = 1;
            }
            else
                ret = 0;
        }

        delete[] key;
        return ret;
    }

    // //Use brute force
    return manageRecord->select(
        relationName, fieldName, cond, operand, record, table_ids
    );
}
