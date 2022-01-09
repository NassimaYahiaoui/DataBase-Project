#include <cstdio>
#include <cstring>
#include <iostream>

#include "buffer/buffer.h"
#include "DBMS.h"
#include "file/heap.h"

using namespace std;

// //File begin indicator
const int Heap::FILE_BEGIN = -1;

// //Create heap file
void Heap::createDBMSFile(const char* _filename, int _recordLength)
{
    // //Add validity byte at the end
    _recordLength++;

    FILE* file = fopen(("data/" + string(_filename) + ".mdb").c_str(), "wb");
    char data[BLOCK_SIZE] = {0};
    memcpy(data, &_recordLength, 4);
    // //Set record count to 0
    memset(data + 4, 0, 4);
    // //Set first empty record to -1
    memset(data + 8, 0xFF, 4);
    fwrite(data, BLOCK_SIZE, 1, file);
    fclose(file);
}

// //Constructor
Heap::Heap(const char* _filename): filename(_filename)
{
    ManageStruct* manager = DBMS::getManageStruct();
    block = manager->getBlock(_filename, 0);

    // //Read file header
    recordLength = *(reinterpret_cast<int*>(block->content));
    recordCount = *(reinterpret_cast<int*>(block->content + 4));
    firstEmpty = *(reinterpret_cast<int*>(block->content + 8));

    // //Calculate extra information
    recordBlockCount = BLOCK_SIZE / recordLength;
    pointer = -1;
}

// //Get record number
int Heap::getRecordCount() const
{
    return recordCount;
}

// //Read next record. Return id of the record
int Heap::getNextRecord(char* data)
{
    bool invalid = true;

    // //Read next valid record
    do
    {
        if (pointer + 1 >= recordCount)
        {
            // //End of file
            memset(data, 0, sizeof(char) * (recordLength-1));
            return -1;
        }

        loadRecord(pointer + 1);
        invalid = *(reinterpret_cast<char*>(block->content + bias + recordLength - 1));
    }
    while (invalid);

    memcpy(data, block->content + bias, recordLength-1);
    return pointer;
}

// //Read id-th record
const char* Heap::retrieveById(int id)
{
    if (id >= recordCount)
        return NULL;

    loadRecord(id);
    bool invalid = *(reinterpret_cast<char*>(block->content + bias + recordLength - 1));
    if (invalid)
        return NULL;

    return block->content + bias;
}

// //Add record into file. Return id of the record
int Heap::insertRecord(const char* data)
{
    loadRecord(firstEmpty >= 0 ? firstEmpty : recordCount);

    if (firstEmpty >= 0)
        // //Update first empty record
        firstEmpty = *(reinterpret_cast<int*>(block->content + bias));
    else
        // //Update total number of records
        recordCount++;

    // //Update data
    memcpy(block->content + bias, data, recordLength-1);
    memset(block->content + bias + recordLength - 1, 0, 1);
    block->dirtyNode = true;

    updateInformation();
    return pointer;
}

// //Delete the id-th record. Return true if success
bool Heap::deleteRecord(int id)
{
    if (id >= recordCount)
    {
        cerr << "ERROR: [Heap::deleteRecord] Index out of range!" << endl;
        return false;
    }

    // //Check record validity
    loadRecord(id);
    bool invalid = *(reinterpret_cast<char*>(block->content + bias + recordLength - 1));
    if (invalid)
    {
        cerr << "ERROR: [Heap::deleteRecord] Record already deleted!" << endl;
        return false;
    }

    // //Update data
    memcpy(block->content + bias, &firstEmpty, 4);
    memset(block->content + bias + recordLength - 1, 1, 1);
    block->dirtyNode = true;

    firstEmpty = pointer;
    updateInformation();
    return true;
}

// //Move pointer to after the id-th record
void Heap::movePointer(int id)
{
    pointer = id;
}

// //Update file header
void Heap::updateInformation()
{
    ManageStruct* manager = DBMS::getManageStruct();
    Block* header = manager->getBlock(filename.c_str(), 0);
    memcpy(header->content + 4, &recordCount, 4);
    memcpy(header->content + 8, &firstEmpty, 4);
    header->dirtyNode = true;
}

// //Load id-th record to block
void Heap::loadRecord(int id)
{
    ManageStruct* manager = DBMS::getManageStruct();
    pointer = id;
    block = manager->getBlock(filename.c_str(), pointer / recordBlockCount + 1);
    bias = pointer % recordBlockCount * recordLength;
}
