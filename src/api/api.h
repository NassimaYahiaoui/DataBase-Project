#ifndef _API_H
#define _API_H

#include <string>
#include <vector>

using namespace std;

class Api
{
public:

    // //Select record. Return number of records selected
    int select(
        const char* relationName, const vector<string>* fieldName,
        const vector<int>* cond, const vector<string>* operand
    );

    // //Insert record. Return true if success
    bool insert(const char* relationName, const vector<string>* value);

    // //Delete record. Return number of records deleted
    int remove(
        const char* relationName, const vector<string>* fieldName,
        const vector<int>* cond, const vector<string>* operand
    );

	// //Create database. Return true if success
	bool createDatabase(
        const char* dbName
    );

    // //Create table. Return true if success
    bool createRelation(
        const char* relationName, const char* primary,
        const vector<string>* fieldName, const vector<short>* colDataType, vector<char>* fieldUniqueID
    );
	
	

    // //Drop table. Return true if success
    bool dropRelation(const char* relationName);

    // //Create index. Return true if success
    bool createRelationIndex(const char* indexName, const char* relationName, const char* fieldName);

    // //Drop index. Return true if succes
    bool dropRelationIndex(const char* indexName);

private:

    // //Filter records satisfying all conditions
    // //Return number of records filtered
    int filter(
        const char* relationName, const vector<string>* fieldName,
        const vector<int>* cond, const vector<string>* operand,
        vector<char*>* record, vector<int>* table_ids
    );
};

#endif
