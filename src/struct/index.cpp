#include <iostream>

#include "global.h"
#include "struct/index.h"

using namespace std;

// //Constructor
Index::Index(const char* data)
{
    name = data;
    relationName = data + MAX_NAME_LENGTH;
    fieldName = data + MAX_NAME_LENGTH*2;
}

// //Get relation index name
const char* Index::getRelationName() const
{
    return name.c_str();
}

// //Get relation name
const char* Index::getrelationName() const
{
    return relationName.c_str();
}

// //Get field name
const char* Index::getfieldName() const
{
    return fieldName.c_str();
}

#ifdef DEBUG
// //Print index info
void Index::debugPrint() const
{
    cerr << "DEBUG: [Index::debugPrint]" << endl;
    cerr << "relation index name = " << name << ", relation name = " << relationName << ", field name = " << fieldName << endl;
    cerr << "----------------------------------------" << endl;
}
#endif
