#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include <vector>
#include <string>

#include "SQLrender/token.h"
#include "api/api.h"

using namespace std;

class SQLreader
{
public:

    // //Constructor
    SQLreader(bool _fromFile = false);

    // //Destructor
    ~SQLreader();

    // //Get  queries processed
    int queriesProcessed() const;

   
    bool isExiting() const;

    
    bool tokenVecEmpty() const;

    // //Execute MySQL statement
    void execute(const char* sql);

private:

    // //Token vector
    vector<string> tokens;
	
	// //Token vector
    vector<string> db_tokens;
	
	vector<string> use_tokens; 

    // //Token type vector
    vector<int> type;

    // //Current token pointer;
    int pointer;
	
	// //Current token pointer;
    int p;

    // // queries processed
    int queryCount;

   
    bool exiting;

    // //If is parsing MySQL statement from file
    bool fromFile;

    // //Tokenizer
    Token* tokenizer;

    // //Api
    Api* api;

	// //Deal with select statement
    void use();
	
    // //Deal with select statement
    void select();

    // //Deal with insert statement
    void insert();

    // //Deal with delete statement
    void remove();

    // //Deal with where statement. Return true if success
    bool where(vector<string>* fieldName, vector<int>* cond, vector<string>* operand);

    // //Deal with create statement table/index
    void create();
	
	// //Deal with create statement table
    void createDatabase();

    // //Deal with create statement table
    void createRelation();

    // //Deal with create statement index
    void createRelationIndex();

    // //Deal with drop table/index
    void drop();
	
	void dropDatabase();

    // //Deal with execfile
    void execfile();

    // //Deal with exit command
    void exit();

    // //Get DBMS operator type
    int getDBMSOperatorType(const char* op);

    // //Get column type
    short getNextcolDataType();

    // //Report error
    void reportError(const char* position, const char* expecting);

    // //Skip current statement
    void skipSQLStatement();
};

#endif
