#include <cstring>
#include <iostream>
#include <string>

#include "global.h"
#include "DBMS.h"
#include "utils/utilities.h"

using namespace std;

// //Get data size of each type
int Utilities::getColDataTypeSize(short type)
{
    if (type == TYPE_NULL)
        return 0;
    else if (type < TYPE_CHAR)
        return type + 1;
    else if (type == TYPE_INT)
        return 4;
    else if (type == TYPE_FLOAT)
        return 4;
    else
    {
        cerr << "ERROR:  Unknown type " << type << "!" << endl;
        return 0;
    }
}

// //Check if a file exists
bool Utilities::checkFile(const char* filename)
{
    FILE* file = fopen(("data/" + string(filename) + ".mdb").c_str(), "rb");
    if (file)
    {
        fclose(file);
        return true;
    }
    return false;
}

// //Delete file
void Utilities::deleteDBMSFile(const char* filename)
{
    remove(("data/" + string(filename) + ".mdb").c_str());
    DBMS::getManageStruct()->removeNodeByFilename(filename);
}

// //Parse string to binary data according to type
char* Utilities::getStrData(const char* s, int type)
{
    char* key = NULL;
    int size = getColDataTypeSize(type);
    
    if (type <= TYPE_CHAR)
    {
        int len = strlen(s);
        if (len > type)
        {
            cerr << "ERROR: [Utils::getStrData] Expecting char(" << type << "), but found char(" << len << ")." << endl;
            return NULL;
        }
        
        key = new char[size]();
        memcpy(key, s, min(size, len + 1));
    }
    else if (type == TYPE_INT)
    {
        int value;
        if (sscanf(s, "%d", &value) < 1)
        {
            cerr << "ERROR:  Expecting int, but found '" << s << "'." << endl;
            return NULL;
        }
        
        key = new char[size]();
        memcpy(key, &value, size);
    }
    else if (type == TYPE_FLOAT)
    {
        float value;
        if (sscanf(s, "%f", &value) < 1)
        {
            cerr << "ERROR:  Expecting float, but found '" << s << "'." << endl;
            return NULL;
        }
        
        key = new char[size]();
        memcpy(key, &value, size);
    }
    
    return key;
}