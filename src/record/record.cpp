#include <cstring>
#include <iostream>

#include "global.h"
#include "struct/relation.h"
#include "file/heap.h"
#include "utils/utilities.h"

#include "DBMS.h"
#include "catalog/catalog.h"
#include "record/record.h"

using namespace std;


int ManageRecord::select(
    const char* relationName, const vector<string>* fieldName,
    const vector<int>* cond, const vector<string>* operand,
    vector<char*>* record, vector<int>* table_ids
)
{
    
    ManageCatalog* manager = DBMS::getCatalog();
    Relation* table = manager->getTable(relationName);
    if (table == NULL)
        return -1;

    Heap* file = new Heap(("record/student/" + string(relationName)).c_str());
    int recordLength = table->calRecordLenth();


    int id, hitCount = 0;
    char* dataIn = new char[recordLength];

    while ((id = file->getNextRecord(dataIn)) >= 0)
        // //Check all conditions
        if (retriveRecord(
            dataIn, relationName, fieldName, cond, operand
        ))
        {
            char* hit = new char[recordLength];
            memcpy(hit, dataIn, recordLength);

            record->push_back(hit);
            table_ids->push_back(id);
            hitCount++;
        }

    delete[] dataIn;
    delete file;
    return hitCount;
}

// //Insert record into table
int ManageRecord::insert(const char* relationName, const char* data)
{
    
    ManageCatalog* manager = DBMS::getCatalog();
    Relation* table = manager->getTable(relationName);
    if (table == NULL)
        return -1;
    Heap* file = new Heap(("record/student/" + string(relationName)).c_str());

    
    char* exist = new char[table->calRecordLenth()];
    while (file->getNextRecord(exist) >= 0)
    {
        int colId;
        if ((colId = table->checkDataConsistency(data, exist)) >= 0)
        {
            // //Uniqueness violated! Clean up
            cerr << "ERROR: [ManageRecord::insert] Duplicate values in unique column `" << table->getfieldName(colId) << "` of table `" << relationName << "`!" << endl;
            delete[] exist;
            delete file;
            return -1;
        }
    }

    // //Insert data
    int ret = file->insertRecord(data);

    delete[] exist;
    delete file;
    return ret;
}


bool ManageRecord::remove(const char* relationName, const vector<int>* table_ids)
{
    Heap* file = new Heap(("record/student/" + string(relationName)).c_str());
    for (int i = 0; i < (int)table_ids->size(); i++)
        file->deleteRecord(table_ids->at(i));
    delete file;
    return true;
}

// //Create table.
bool ManageRecord::createRelation(const char* relationName)
{
    ManageCatalog* manager = DBMS::getCatalog();
    Relation* table = manager->getTable(relationName);
    if (table == NULL)
        return false;
    Heap::createDBMSFile(("record/student/" + string(relationName)).c_str(), table->calRecordLenth());
    return true;
}

// //Drop table
bool ManageRecord::dropRelation(const char* relationName)
{
    Utilities::deleteDBMSFile(("record/student/" + string(relationName)).c_str());
    return true;
}

// //Select  records where
bool ManageRecord::retriveRecord(
    const char* record, const char* relationName,
    const vector<string>* fieldName, const vector<int>* cond,
    const vector<string>* operand
)
{
    ManageCatalog* manager = DBMS::getCatalog();
    Relation* table = manager->getTable(relationName);
    if (table == NULL)
        return false;

    int condCount = fieldName->size();

    for (int i = 0; i < condCount; i++)
    {
        char dataOut[MAX_VALUE_LENGTH];
        short type = table->getValue(fieldName->at(i).c_str(), record, dataOut);

        if (type <= TYPE_CHAR)
        {
            // //Char type
            if (!charCmp(dataOut, operand->at(i).c_str(), cond->at(i)))
                return false;
        }
        else if (type == TYPE_INT)
        {
            // //Int type
            if (!intCmp(dataOut, operand->at(i).c_str(), cond->at(i)))
                return false;
        }
        else if (type == TYPE_FLOAT)
        {
            // //Float type
            if (!floatCmp(dataOut, operand->at(i).c_str(), cond->at(i)))
                return false;
        }
    }

    return true;
}

//Compare string records
bool ManageRecord::charCmp(const char* a, const char* b, int op)
{
    if (op == COND_EQ)
        return strcmp(a, b) == 0;
    else if (op == COND_NE)
        return strcmp(a, b) != 0;
    else if (op == COND_LT)
        return strcmp(a, b) < 0;
    else if (op == COND_GT)
        return strcmp(a, b) > 0;
    else if (op == COND_LE)
        return strcmp(a, b) <= 0;
    else if (op == COND_GE)
        return strcmp(a, b) >= 0;
    else
        return false;
}

//Compare int records
bool ManageRecord::intCmp(const char* a, const char* b, int op)
{
    const int left = *(reinterpret_cast<const int*>(a));
    int right;
    sscanf(b, "%d", &right);

    if (op == COND_EQ)
        return left == right;
    else if (op == COND_NE)
        return left != right;
    else if (op == COND_LT)
        return left < right;
    else if (op == COND_GT)
        return left > right;
    else if (op == COND_LE)
        return left <= right;
    else if (op == COND_GE)
        return left >= right;
    else
        return false;
}

// ////Compare float records
bool ManageRecord::floatCmp(const char* a, const char* b, int op)
{
    const float left = *(reinterpret_cast<const float*>(a));
    float right;
    sscanf(b, "%f", &right);

    if (op == COND_EQ)
        return left == right;
    else if (op == COND_NE)
        return left != right;
    else if (op == COND_LT)
        return left < right;
    else if (op == COND_GT)
        return left > right;
    else if (op == COND_LE)
        return left <= right;
    else if (op == COND_GE)
        return left >= right;
    else
        return false;
}
