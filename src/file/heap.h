#ifndef _HEAP_FILE_H
#define _HEAP_FILE_H

#include <string>
#include "global.h"

using namespace std;

class Heap
{
public:

    // //File beginning indicator
    static const int FILE_BEGIN;

    // //Create heap file
    static void createDBMSFile(const char* _filename, int _recordLength);

    // //Constructor
    Heap(const char* _filename);

    // //Get record number
    int getRecordCount() const;

    // //Read next record. Return id of the record
    int getNextRecord(char* data);

    // //Read id-th record
    const char* retrieveById(int id);

    // //Add record into file. Return id of the record
    int insertRecord(const char* data);

    // //Delete the id-th record. Return true if success
    bool deleteRecord(int id);

    // //Move pointer to after the id-th record
    void movePointer(int id);

private:

    // //Filename
    string filename;

    // //Length of a record
    int recordLength;

    // //Total number of records
    int recordCount;

    // //First empty record
    int firstEmpty;

    // //Number of records in a block
    int recordBlockCount;

    // //Current data block
    Block* block;

    // //Record pointer
    int pointer;

    // //Pointer bias in the block
    int bias;

    // //Update file header
    void updateInformation();

    // //Load id-th record to block
    void loadRecord(int id);
};

#endif
