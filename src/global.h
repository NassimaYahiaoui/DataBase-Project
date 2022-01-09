#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <string>

using namespace std;

#define DEBUG

// //Bytes of a block node size
#define BLOCK_SIZE 4096

// //Set maximum length of value to store in the table
#define MAX_VALUE_LENGTH 256

// //The length of column, table
#define MAX_NAME_LENGTH 31

// //DBMS DData types
#define TYPE_NULL 0
#define TYPE_CHAR 255
#define TYPE_INT 256
#define TYPE_FLOAT 257

// //Conditions
#define COND_EQ 0
#define COND_NE 1
#define COND_LT 2
#define COND_GT 3
#define COND_LE 4
#define COND_GE 5

// //DEFINE block
struct Block
{
    string filename;
    int id;

    bool dirtyNode;
    bool pin;

    char content[BLOCK_SIZE];

    // //Constructor
    Block(const char* _filename, int _id): filename(_filename), id(_id)
    {
        dirtyNode = false;
        pin = false;
    }
};

#endif
