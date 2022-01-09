#include <cstring>
#include <iostream>
#include <string>

#include "global.h"
#include "utils/utilities.h"
#include "SQLrender/SQLrender.h"
#include "index/bplusTree.h"
#include "DBMS.h"

using namespace std;

// //Managers
ManageStruct* DBMS::manageBuffer = NULL;
ManageCatalog* DBMS::manageCatalog = NULL;
ManageRecord* DBMS::manageRecord = NULL;
ManageIndex* DBMS::manageIndex = NULL;

// //Init DBMS
void DBMS::init()
{
    // //Check if catalog/tables.mdb exists
    if (!Utilities::checkFile("catalog/tables"))
        Heap::createDBMSFile("catalog/tables", MAX_NAME_LENGTH*2);

    // //Check if catalog/indices.mdb exists
    if (!Utilities::checkFile("catalog/indices"))
        Heap::createDBMSFile("catalog/indices", MAX_NAME_LENGTH*3);

    // //Init managers
    manageBuffer = new ManageStruct();
    manageCatalog = new ManageCatalog();
    manageRecord = new ManageRecord();
    manageIndex = new ManageIndex();
}

// //Clean up managers
void DBMS::cleanUp()
{
    delete manageBuffer;
    delete manageCatalog;
    delete manageRecord;
    delete manageIndex;
}

// //Get buffer manager
ManageStruct* DBMS::getManageStruct()
{
    return manageBuffer;
}

// //Get catalog manager
ManageCatalog* DBMS::getCatalog()
{
    return manageCatalog;
}

// //Get record manager
ManageRecord* DBMS::getmanageRecord()
{
    return manageRecord;
}

// //Get index manager
ManageIndex* DBMS::getIndex()
{
    return manageIndex;
}

// //Main function
int main()
{
    DBMS::init();
    SQLreader* interpreter = new SQLreader();
    
    string sql;
    while (!interpreter->isExiting())
    {
        if (interpreter->tokenVecEmpty())
            cout << endl << "DBMS> ";
        else
            cout << "    ...> ";

        getline(cin, sql);
        interpreter->execute(sql.c_str());
    }
    
    delete interpreter;
    DBMS::cleanUp();
    return 0;
}
