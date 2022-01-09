#include <cstdio>
#include <iostream>
#include <cstring>
#include "utils/utilities.h"
#include "DBMS.h"
#include "struct/relation.h"

using namespace std;

// //Relation Constructor
Relation::Relation(const char* data)
{
    name = data;
    primary = data + MAX_NAME_LENGTH;
    countCol = 0;
    recordLength = 0;
}

// //Get relatio name
const char* Relation::getRelationName() const
{
    return name.c_str();
}

// //Get relation primary key name
const char* Relation::getPrimaryKey() const
{
    return primary.c_str();
}

// //Get relation relation length of a record
int Relation::fieldCount()
{
    if (countCol == 0)
        loadColData();
    return countCol;
}

// //Get length of a record
int Relation::calRecordLenth()
{
    if (countCol == 0)
        loadColData();
    return recordLength;
}

// //Get field name by id
const char* Relation::getfieldName(int id)
{
    if (countCol == 0)
        loadColData();
    if (id >= countCol)
    {
        cerr << "ERROR: [Table::getfieldName] Column id " << id << " too large!" << endl;
        return NULL;
    }
    return fieldNameList[id].c_str();
}

// //Get relation value from table by field name. Return column type
int Relation::getId(const char* fieldName)
{
    if (countCol == 0)
        loadColData();
    for (int i = 0; i < countCol; i++)
        if (fieldNameList[i] == fieldName)
            return i;
    return -1;
}

// //Get value from table by field name. Return column type
short Relation::getValue(const char* fieldName, const char* dataIn, char* dataOut)
{
    if (countCol == 0)
        loadColData();

    int id = getId(fieldName);
    if (id < 0)
    {
        cerr << "ERROR: [Table::getValue] Table `" << name << "` has no field named `" << fieldName << "`!" << endl;
        return TYPE_NULL;
    }
    memcpy(dataOut, dataIn + startPos[id], Utilities::getColDataTypeSize(colDataType[id]));
    return colDataType[id];
}

// //Get the type of column by name
short Relation::getColDataType(const char* fieldName)
{
    if (countCol == 0)
        loadColData();

    int id = getId(fieldName);
    if (id < 0)
    {
        cerr << "ERROR: [Table::getColDataType] Table `" << name << "` has no field named `" << fieldName << "`!" << endl;
        return TYPE_NULL;
    }
    return colDataType[id];
}

// //Get column by unique  name
char Relation::getUnique(const char* fieldName)
{
    if (countCol == 0)
        loadColData();

    int id = getId(fieldName);
    if (id < 0)
    {
        cerr << "ERROR: [Table::getUnique] Table `" << name << "` has no field named `" << fieldName << "`!" << endl;
        return -1;
    }
    return fieldUniqueID[id];
}


int Relation::checkDataConsistency(const char* data, const char* exist)
{
    if (countCol == 0)
        loadColData();

    for (int i = 0; i < countCol; i++)
    {
        if (!fieldUniqueID[i])
            continue;

        int j;
        for (j = 0; j < Utilities::getColDataTypeSize(colDataType[i]); j++)
            if (data[startPos[i] + j] != exist[startPos[i] + j])
                break;
        if (j >= Utilities::getColDataTypeSize(colDataType[i]))
            return i;
    }
    return -1;
}


bool Relation::recordToVec(const char* data, vector<char*>* vec)
{
    if (countCol == 0)
        loadColData();

    for (int i = 0; i < countCol; i++)
    {
        int size = Utilities::getColDataTypeSize(colDataType[i]);
        char* value = new char[size];
        memcpy(value, data + startPos[i], size);
        vec->push_back(value);
    }
    return true;
}


bool Relation::copyVecRecord(const vector<string>* vec, char* data)
{
    if (countCol == 0)
        loadColData();

    if ((int)vec->size() != countCol)
    {
        cerr << "ERROR: [Table::copyVecRecord] Value number mismatch. Expecting " << countCol << " values, but found " << vec->size() << " values." << endl;
        return false;
    }

    for (int i = 0; i < countCol; i++)
    {
        char* key = Utilities::getStrData(vec->at(i).c_str(), colDataType[i]);
        int size = Utilities::getColDataTypeSize(colDataType[i]);
        
        if (key == NULL)
            return false;
        memcpy(data + startPos[i], key, size);
        delete[] key;
    }
    return true;
}

#ifdef DEBUG
void Relation::debugPrint()
{
    if (countCol == 0)
        loadColData();

    cerr << "DEBUG: [Table::debugPrint]" << endl;
    cerr << "relation name = " << name << ", column count = " << countCol << ", primary = " << primary << ", record length = " << recordLength << endl;
    for (int i = 0; i < countCol; i++)
        cerr << "Column = " << fieldNameList[i] << ", type = " << colDataType[i] << ", unique = " << (fieldUniqueID[i] ? '1' : '0') << endl;
    cerr << "----------------------------------------" << endl;
}
#endif

//Load catalog info into column
void Relation::loadColData()
{
    // //Get column data
    countCol = DBMS::getCatalog()->loadTableColInfo(name.c_str(), &fieldNameList, &colDataType, &fieldUniqueID);

    // //Calculate starting position every column
    for (int i = 0; i < countCol; i++)
    {
        if (i > 0)
            startPos.push_back(startPos[i-1] + Utilities::getColDataTypeSize(colDataType[i-1]));
        else
            startPos.push_back(0);
    }
    recordLength = startPos[countCol-1] + Utilities::getColDataTypeSize(colDataType[countCol-1]);
}
