#ifndef _DBMS_H
#define _DBMS_H

#include "buffer/buffer.h"
#include "catalog/catalog.h"
#include "record/record.h"
#include "index/manageIndex.h"

class DBMS
{
public:

    // //Init DBMS
    static void init();

    // //Clean up all system managers managers
    static void cleanUp();

    // //access buffer 
    static ManageStruct* getManageStruct();

    // //access catalog 
    static ManageCatalog* getCatalog();

    // //Obtain record 
    static ManageRecord* getmanageRecord();

    // //obtain index 
    static ManageIndex* getIndex();

private:

    // //Buffer manager
    static ManageStruct* manageBuffer;

    // //Catalog manager
    static ManageCatalog* manageCatalog;

    // //Record manager
    static ManageRecord* manageRecord;

    // //Index manager
    static ManageIndex* manageIndex;
};

#endif
