#ifndef _RECORD_MANAGER_H
#define _RECORD_MANAGER_H

#include <vector>
#include <string>

using namespace std;

class ManageRecord
{
public:

    
    int select(
        const char* relationName, const vector<string>* fieldName,
        const vector<int>* cond, const vector<string>* operand,
        vector<char*>* record, vector<int>* table_ids
    );

    // //Insert record into table
    int insert(const char* relationName, const char* data);

    
    bool remove(const char* relationName, const vector<int>* table_ids);

    // //Create table. Return true if success
    bool createRelation(const char* relationName);

    // //Drop table. Return true if success
    bool dropRelation(const char* relationName);

    // //Select  records where
    bool retriveRecord(
        const char* record, const char* relationName,
        const vector<string>* fieldName, const vector<int>* cond,
        const vector<string>* operand
    );

private:

    //Compare string records
    bool charCmp(const char* a, const char* b, int op);

    //Compare int records
    bool intCmp(const char* a, const char* b, int op);

    // ////Compare float records
    bool floatCmp(const char* a, const char* b, int op);
};

#endif
