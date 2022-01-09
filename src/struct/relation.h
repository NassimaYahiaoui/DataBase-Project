#ifndef _TABLE_H
#define _TABLE_H

#include <vector>
#include <string>

#include "global.h"

using namespace std;

class Relation
{
public:

    // //Constructor
    Relation(const char* data);

    // //Get relation name
    const char* getRelationName() const;

    // //Get relation primary key name
    const char* getPrimaryKey() const;

    // //Get relation relation length of a record
    int fieldCount();

    // //Get length of a record
    int calRecordLenth();

    // //Get field name by id
    const char* getfieldName(int id);

    // //Get relation value from table by field name. Return column type
    int getId(const char* fieldName);

    // //Get value from table by field name. Return column type
    short getValue(const char* fieldName, const char* dataIn, char* dataOut);

    // //Get the type of column by name
    short getColDataType(const char* fieldName);

    // //Get column by unique  name
    char getUnique(const char* fieldName);

    // //Check consistency(uniqueness) of new data and existing data
    // //Return -1 if consistent, else return column id of not unique column
    int checkDataConsistency(const char* data, const char* exist);

    
    bool recordToVec(const char* data, vector<char*>* vec);

    
    bool copyVecRecord(const vector<string>* vec, char* data);

#ifdef DEBUG
    // //Print table info
    void debugPrint();
#endif

private:

    // //relation name
    string name;

    // //Primary key
    string primary;

    // //relation relation length of a record
    int countCol;

    // //Length of a record
    int recordLength;

    // //field name
    vector<string> fieldNameList;

    // //Column type
    vector<short> colDataType;

    // //Are columns unique
    vector<char> fieldUniqueID;

    // //Starting position of each column in data
    vector<int> startPos;

    //Load catalog info into column
    void loadColData();
};

#endif
