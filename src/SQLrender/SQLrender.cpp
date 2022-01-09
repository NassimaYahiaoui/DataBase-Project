#include<windows.h>
#include <ctime>
#include<conio.h>
#include<stdlib.h>
#include <cstring>
#include <io.h>   // For access().
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().
#include <iostream>
#include <fstream>
#include <string>
#include "global.h"
#include "record/record.h"
#include "SQLrender/SQLrender.h"

using namespace std;

// //Constructor
SQLreader::SQLreader(bool _fromFile): fromFile(_fromFile)
{
    pointer = -1;
    queryCount = 0;
    exiting = false;
    tokenizer = new Token();
    api = new Api();
}

// //Destructor
SQLreader::~SQLreader()
{
    delete tokenizer;
    delete api;
}

// //Get  queries processed
int SQLreader::queriesProcessed() const
{
    return queryCount;
}

// //If user is exiting mini SQL
bool SQLreader::isExiting() const
{
    return exiting;
}


bool SQLreader::tokenVecEmpty() const
{
    return pointer == (int)tokens.size() - 1;
}

// //Execute MySQL statement
void SQLreader::execute(const char* sql){
	
    int endCount = tokenizer->getTokens(sql, &tokens, &db_tokens, &use_tokens, &type);
    queryCount += endCount;

    while (endCount--)
    {
        pointer++;
        if (type[pointer] != Token::TOKEN_IDENTIFIER && type[pointer] != Token::TOKEN_END){
            reportError("execute", "instruction");
        }else if (type[pointer] == Token::TOKEN_END) {}
        else if (tokens[pointer] == "select"){
            select();
        }else if (tokens[pointer] == "insert"){
            insert();
        }else if (tokens[pointer] == "delete"){
            remove();
        }else if (tokens[pointer] == "create"){
            create();
		}else if (tokens[pointer] == "use"){
			use();	
        }else if (tokens[pointer] == "drop"){
            drop();
        }else if (tokens[pointer] == "remove"){
            dropDatabase();
        }else if (tokens[pointer] == "exec" || tokens[pointer] == "execfile"){
            execfile();
        }else if (tokens[pointer] == "exit" || tokens[pointer] == "quit"){
            exit();
        }else
        {
            cerr << "ERROR: [SQLreader::execute] Unknown instruction '" << tokens[pointer] << "'." << endl;
            skipSQLStatement();
        }
    }
}


void  SQLreader::dropDatabase()
{
    pointer++;
    if (type[pointer] != Token::TOKEN_IDENTIFIER)
    {
        reportError("Remove database", "database name");
        return;
    }

	
    // //Prepare create database information
    const char* dbName = tokens[pointer].c_str();
	string db = db_tokens[pointer].c_str();
	use_tokens[1] = dbName;
	

	 pointer++;
    if (type[pointer] != Token::TOKEN_END)
    {
        reportError("Remove database", "';'");
        return;
    }
	
    // //Do creation
    int tic, toc;
  
    tic = clock();
    toc = clock();
    
    // //Print execution time
	string dbFiler = "data/record/" + db;
	string dbFilec = "data/catalog/" + db;

	if ( access( dbFiler.c_str(), 0 ) == 0 ) {
		
        struct stat status;
        stat( dbFiler.c_str(), &status );

        if ( status.st_mode & S_IFDIR ) {

           cout <<"Database changed" << endl;
        }
    }
    else{
	
       cout <<"Database " << dbName << " does not exist. Query done in " << 1.0 * (toc-tic) / CLOCKS_PER_SEC << "s." << endl;

    }
}
void SQLreader::use()
{
    pointer++;
    if (type[pointer] != Token::TOKEN_IDENTIFIER)
    {
        reportError("Use database", "database name");
        return;
    }

	
    // //Prepare create database information
    const char* dbName = tokens[pointer].c_str();
	use_tokens[1] = dbName;
	string db = tokens[pointer].c_str();
	
	 pointer++;
    if (type[pointer] != Token::TOKEN_END)
    {
        reportError("Use database", "';'");
        return;
    }
	
    // //Do creation
    int tic, toc;
  
    tic = clock();
    toc = clock();
    
    // //Print execution time
	string dbFiler = "data/record/" + db;
	string dbFilec = "data/catalog/" + db;
	if ( access( dbFiler.c_str(), 0 ) == 0 ) {
		
        struct stat status;
        stat( dbFiler.c_str(), &status );

        if ( status.st_mode & S_IFDIR ) {

           cout <<"Database changed" << endl;
        }
    }
    else{
	
       cout <<"Database " << dbName << " does not exist. Query done in " << 1.0 * (toc-tic) / CLOCKS_PER_SEC << "s." << endl;

    }

}
// //Deal with select statement
void SQLreader::select()
{
    pointer++;
    if (tokens[pointer] != "*" || type[pointer] != Token::TOKEN_SYMBOL)
    {
        reportError("select", "'*'(DBMS does not support selecting specific columns)");
        return;
    }

    pointer++;
    if (tokens[pointer] != "from" || type[pointer] != Token::TOKEN_IDENTIFIER)
    {
        reportError("select", "'from'");
        return;
    }

    pointer++;
    if (type[pointer] != Token::TOKEN_IDENTIFIER)
    {
        reportError("select", "relation name");
        return;
    }

    // //Prepare select information
    const char* relationName = tokens[pointer].c_str();
    vector<string> fieldName;
    vector<int> cond;
    vector<string> operand;

    if (where(&fieldName, &cond, &operand))
    {
        // //Do selection
        int tic, toc, selectCount;
        tic = clock();
        selectCount = api->select(relationName, &fieldName, &cond, &operand);
        toc = clock();

        // //Print execution time
        if (selectCount >= 0 && !fromFile)
            cout << selectCount << " record(s) selected. Query done in " << 1.0 * (toc-tic) / CLOCKS_PER_SEC << "s." << endl;
    }
}

// //Deal with insert statement
void SQLreader::insert()
{
    pointer++;
    if (tokens[pointer] != "into" || type[pointer] != Token::TOKEN_IDENTIFIER)
    {
        reportError("insert", "'into'");
        return;
    }

    pointer++;
    if (type[pointer] != Token::TOKEN_IDENTIFIER)
    {
        reportError("insert", "relation name");
        return;
    }

    // //Prepare insert information
    const char* relationName = tokens[pointer].c_str();
    vector<string> value;

    pointer++;
    if (tokens[pointer] != "values" || type[pointer] != Token::TOKEN_IDENTIFIER)
    {
        reportError("insert", "'values'");
        return;
    }

    pointer++;
    if (tokens[pointer] != "(" || type[pointer] != Token::TOKEN_SYMBOL)
    {
        reportError("insert", "'('");
        return;
    }

    while (true)
    {
        pointer++;
        if (type[pointer] != Token::TOKEN_NUMBER && type[pointer] != Token::TOKEN_STRING_SINGLE && type[pointer] != Token::TOKEN_STRING_DOUBLE)
        {
            reportError("insert", "value");
            return;
        }
        value.push_back(tokens[pointer]);

        pointer++;
        if (tokens[pointer] == ")" && type[pointer] == Token::TOKEN_SYMBOL)
            break;
        else if (tokens[pointer] != "," || type[pointer] != Token::TOKEN_SYMBOL)
        {
            reportError("insert", "','");
            return;
        }
    }

    pointer++;
    if (type[pointer] != Token::TOKEN_END)
    {
        reportError("insert", "';'");
        return;
    }

    // //Do insertion
    int tic, toc;
    bool res;
    tic = clock();
    res = api->insert(relationName, &value);
    toc = clock();

    // //Print execution time
    if (res && !fromFile)
        cout << "1 record inserted. Query done in " << 1.0 * (toc-tic) / CLOCKS_PER_SEC << "s." << endl;
}

// //Deal with delete statement
void SQLreader::remove()
{
    pointer++;
    if (tokens[pointer] != "from" || type[pointer] != Token::TOKEN_IDENTIFIER)
    {
        reportError("delete", "'from'");
        return;
    }

    pointer++;
    if (type[pointer] != Token::TOKEN_IDENTIFIER)
    {
        reportError("delete", "relation name");
        return;
    }

    // //Prepare delete information
    const char* relationName = tokens[pointer].c_str();
    vector<string> fieldName;
    vector<int> cond;
    vector<string> operand;

    if (where(&fieldName, &cond, &operand))
    {
        // //Do deletion
        int tic, toc, removeCount;
        tic = clock();
        removeCount = api->remove(relationName, &fieldName, &cond, &operand);
        toc = clock();

        // //Print execution time
        if (removeCount >= 0 && !fromFile)
            cout << removeCount << " record(s) deleted. Query done in " << 1.0 * (toc-tic) / CLOCKS_PER_SEC << "s." << endl;
    }
}

// //Deal with where statement. Return true if success
bool SQLreader::where(vector<string>* fieldName, vector<int>* cond, vector<string>* operand)
{
    pointer++;
    if (type[pointer] == Token::TOKEN_END)
        // //No condition
        return true;
    else if (tokens[pointer] != "where" || type[pointer] != Token::TOKEN_IDENTIFIER)
    {
        reportError("select", "'where'");
        return false;
    }

    // //With condition
    while (true)
    {
        pointer++;
        if (type[pointer] != Token::TOKEN_IDENTIFIER)
        {
            reportError("select", "field name");
            return false;
        }
        fieldName->push_back(tokens[pointer]);

        pointer++;
        if (type[pointer] != Token::TOKEN_OPERATOR)
        {
            reportError("select", "operator");
            return false;
        }
        int op = getDBMSOperatorType(tokens[pointer].c_str());
        if (op < 0)
        {
            cerr << "ERROR: [SQLreader::select] Unknown operator '" << tokens[pointer] << "'." << endl;
            skipSQLStatement();
            return false;
        }
        cond->push_back(op);

        pointer++;
        if (type[pointer] != Token::TOKEN_NUMBER && type[pointer] != Token::TOKEN_STRING_SINGLE && type[pointer] != Token::TOKEN_STRING_DOUBLE)
        {
            reportError("select", "value");
            return false;
        }
        operand->push_back(tokens[pointer]);

        pointer++;
        if (type[pointer] == Token::TOKEN_END)
            return true;
        else if (tokens[pointer] != "and" || type[pointer] != Token::TOKEN_IDENTIFIER)
        {
            reportError("select", "'and'(DBMS only supports conjunctive selection)");
            return false;
        }
    }
}

// //Deal with creata table/index
void SQLreader::create()
{

	pointer++;
    if (tokens[pointer] == "table" && type[pointer] == Token::TOKEN_IDENTIFIER){
        createRelation();
    }else if (tokens[pointer] == "index" && type[pointer] == Token::TOKEN_IDENTIFIER){
        createRelationIndex();
	}else if (tokens[pointer] == "database" && type[pointer] == Token::TOKEN_IDENTIFIER){
        createDatabase();
	}else{
        reportError("create", "'table' or 'index'  or 'database'");
	}
}
void SQLreader::createDatabase()
{
    pointer++;
    if (type[pointer] != Token::TOKEN_IDENTIFIER)
    {
        reportError("createDatabase", "database name");
        return;
    }
	
    const char* dbName = db_tokens[pointer].c_str();
	string db = db_tokens[pointer].c_str();
	use_tokens[1] = dbName;
	 pointer++;
    if (type[pointer] != Token::TOKEN_END)
    {
        reportError("createDatabase", "';'");
        return;
    }
	
    // //Do creation
    int tic, toc;
  
    tic = clock();
    toc = clock();
    
    // //Print execution time
	string dbFiler = "data/record/" + db;
	string dbFilec = "data/catalog/" + db;
	if ( access( dbFiler.c_str(), 0 ) == 0 ) {
		
        struct stat status;
        stat( dbFiler.c_str(), &status );

        if ( status.st_mode & S_IFDIR ) {

            cout <<"Database " << dbName << " already exists. Query done in " << 1.0 * (toc-tic) / CLOCKS_PER_SEC << "s." << endl;
        }
    }
    else{
        if (CreateDirectory (dbFiler.c_str(), NULL) && CreateDirectory (dbFilec.c_str(), NULL)){

             cout << "1 Database created. Query done in " << 1.0 * (toc-tic) / CLOCKS_PER_SEC << "s." << endl;
        }

    }

}
// //Deal with create statement table
void SQLreader::createRelation()
{
	int n = 0;
	n++;
	
	use_tokens[1];
	
	pointer++;
    if (type[pointer] != Token::TOKEN_IDENTIFIER)
    {
        reportError("createRelation", "relation name");
        return;
    }

    // //Prepare create table information
	// //Prepare create database information


   
    const char* relationName = tokens[pointer].c_str();
    const char* primary = NULL;
    vector<string> fieldName;
    vector<short> colDataType;
    vector<char> fieldUniqueID;
	//if (use_tokens.empty()) { 
		
    pointer++;
    if (tokens[pointer] != "(" || type[pointer] != Token::TOKEN_SYMBOL)
    {
        reportError("createRelation", "'('");
        return;
    }

    while (true)
    {
        pointer++;
        if (tokens[pointer] == "primary" && type[pointer] == Token::TOKEN_IDENTIFIER)
        {
            bool hasBracket = false;

            pointer++;
            if (tokens[pointer] != "key" || type[pointer] != Token::TOKEN_IDENTIFIER)
            {
                reportError("createRelation", "'key'");
                return;
            }

            pointer++;
            if (tokens[pointer] == "(" && type[pointer] == Token::TOKEN_SYMBOL)
                hasBracket = true;
            else if (type[pointer] == Token::TOKEN_IDENTIFIER)
            {
                if (primary != NULL)
                {
                    cerr << "ERROR: [SQLreader::createRelation] Multiple primary key definition." << endl;
                    skipSQLStatement();
                    return;
                }
                primary = tokens[pointer].c_str();
            }
            else
            {
                reportError("createRelation", "relation primary key name or '('");
                return;
            }

            if (hasBracket)
            {
                pointer++;
                if (type[pointer] != Token::TOKEN_IDENTIFIER)
                {
                    reportError("createRelation", "relation primary key name");
                    return;
                }
                else if (primary != NULL)
                {
                    cerr << "ERROR: [SQLreader::createRelation] Multiple primary key definition." << endl;
                    skipSQLStatement();
                    return;
                }
                primary = tokens[pointer].c_str();

                pointer++;
                if (tokens[pointer] != ")" || type[pointer] != Token::TOKEN_SYMBOL)
                {
                    reportError("createRelation", "')'");
                    return;
                }
            }
        }
        else if (type[pointer] == Token::TOKEN_IDENTIFIER)
        {
            fieldName.push_back(tokens[pointer]);

            int t = getNextcolDataType();
            if (t == TYPE_NULL)
                return;
            colDataType.push_back(t);

            if (tokens[pointer+1] == "unique" && type[pointer+1] == Token::TOKEN_IDENTIFIER)
            {
                pointer++;
                fieldUniqueID.push_back(1);
            }
            else
                fieldUniqueID.push_back(0);
        }
        else
        {
            reportError("createRelation", "field name or 'primary'");
            return;
        }

        pointer++;
        if (tokens[pointer] == ")" && type[pointer] == Token::TOKEN_SYMBOL)
            break;
        else if (tokens[pointer] != "," || type[pointer] != Token::TOKEN_SYMBOL)
        {
            reportError("createRelation", "','");
            return;
        }
    }

    pointer++;
    if (type[pointer] != Token::TOKEN_END)
    {
        reportError("createRelation", "';'");
        return;
    }

    if (primary == NULL)
    {
        cerr << "ERROR: [SQLreader::createRelation] No primary key definition!" << endl;
        return;
    }

    // //Do creation
    int tic, toc;
    bool res;
    tic = clock();
    res = api->createRelation(relationName, primary, &fieldName, &colDataType, &fieldUniqueID);
    toc = clock();
    
    // //Print execution time
    if (res && !fromFile){
        cout << "1 table created. Query done in " << 1.0 * (toc-tic) / CLOCKS_PER_SEC << "s." << endl;
	}
	//} else{
		
	//	 cout <<  "No database selected!" << endl;
	//	return;
		 
	//}
}

// //Deal with create statement index
void SQLreader::createRelationIndex()
{
    pointer++;
    if (type[pointer] != Token::TOKEN_IDENTIFIER)
    {
        reportError("createRelationIndex", "relation index name");
        return;
    }

    // //Prepare create table information
    const char* indexFiieldName = tokens[pointer].c_str();
    const char* relationName;
    const char* fieldName;

    pointer++;
    if (tokens[pointer] != "on" || type[pointer] != Token::TOKEN_IDENTIFIER)
    {
        reportError("createRelationIndex", "'on'");
        return;
    }

    pointer++;
    if (type[pointer] != Token::TOKEN_IDENTIFIER)
    {
        reportError("createRelationIndex", "relation index name");
        return;
    }
    relationName = tokens[pointer].c_str();

    pointer++;
    if (tokens[pointer] != "(" || type[pointer] != Token::TOKEN_SYMBOL)
    {
        reportError("createRelationIndex", "'('");
        return;
    }

    pointer++;
    if (type[pointer] != Token::TOKEN_IDENTIFIER)
    {
        reportError("createRelationIndex", "field name");
        return;
    }
    fieldName = tokens[pointer].c_str();

    pointer++;
    if (tokens[pointer] != ")" || type[pointer] != Token::TOKEN_SYMBOL)
    {
        reportError("createRelationIndex", "')'");
        return;
    }

    pointer++;
    if (type[pointer] != Token::TOKEN_END)
    {
        reportError("createRelationIndex", "';'");
        return;
    }

    // //Do creation
    int tic, toc;
    bool res;
    tic = clock();
    res = api->createRelationIndex(indexFiieldName, relationName, fieldName);
    toc = clock();
    
    // //Print execution time
    if (res && !fromFile)
        cout << "1 index created. Query done in " << 1.0 * (toc-tic) / CLOCKS_PER_SEC << "s." << endl;
}

// //Deal with drop table/index

void SQLreader::drop()
{
	
    pointer++;
    if (tokens[pointer] == "table" && type[pointer] == Token::TOKEN_IDENTIFIER)
    {
        pointer++;
        if (type[pointer] != Token::TOKEN_IDENTIFIER)
        {
            reportError("drop", "relation name");
            return;
        }
        const char* relationName = tokens[pointer].c_str();

        pointer++;
        if (type[pointer] != Token::TOKEN_END)
        {
            reportError("drop", "';'");
            return;
        }

        // //Do drop table
        int tic, toc;
        bool res;
        tic = clock();
        res = api->dropRelation(relationName);
        toc = clock();

        // //Print execution time
        if (res && !fromFile)
            cout << "1 table dropped. Query done in " << 1.0 * (toc-tic) / CLOCKS_PER_SEC << "s." << endl;
    }
    else if (tokens[pointer] == "index" && type[pointer] == Token::TOKEN_IDENTIFIER)
    {
        pointer++;
        if (type[pointer] != Token::TOKEN_IDENTIFIER)
        {
            reportError("drop", "relation index name");
            return;
        }
        const char* indexFiieldName = tokens[pointer].c_str();

        pointer++;
        if (type[pointer] != Token::TOKEN_END)
        {
            reportError("drop", "';'");
            return;
        }

        // //Do drop index
        int tic, toc;
        bool res;
        tic = clock();
        res = api->dropRelationIndex(indexFiieldName);
        toc = clock();

        // //Print execution time
        if (res && !fromFile)
            cout << "1 index dropped. Query done in " << 1.0 * (toc-tic) / CLOCKS_PER_SEC << "s." << endl;
    }
    else
        reportError("drop", "'table' or 'index'");
}

// //Deal with execfile
void SQLreader::execfile()
{
    pointer++;
    if (type[pointer] != Token::TOKEN_STRING_SINGLE && type[pointer] != Token::TOKEN_STRING_DOUBLE)
    {
        reportError("execfile", "a string as filename");
        return;
    }
    const char* filename = tokens[pointer].c_str();

    pointer++;
    if (type[pointer] != Token::TOKEN_END)
    {
        reportError("execfile", "';'");
        return;
    }

    if (fromFile)
    {
        cerr << "ERROR: [SQLreader::execfile] Cannot do 'execfile' instruction when executing from file." << endl;
        return;
    }

    // //Read from file
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "ERROR: [SQLreader::execfile] Cannot load file " << filename << "!" << endl;
        return;
    }
    string line, content = "";
    while (!file.eof())
    {
        getline(file, line);
        content += line + '\n';
    }

    // //Execute file
    int tic, toc;
    SQLreader* interpreter = new SQLreader(true);
    tic = clock();
    interpreter->execute(content.c_str());
    toc = clock();

    // //Print execution time
    cout << interpreter->queriesProcessed() << " queries done in " << 1.0 * (toc-tic) / CLOCKS_PER_SEC << "s." << endl;

    delete interpreter;
}

// //Deal with exit command
void SQLreader::exit()
{
    pointer++;
    if (type[pointer] != Token::TOKEN_END)
    {
        reportError("exit", "';'");
        return;
    }

    if (fromFile)
        cerr << "ERROR: [SQLreader::exit] Cannot do 'exit' instruction when executing from file." << endl;
    else
    {
        cout << "Bye~ :)" << endl;
        exiting = true;
    }
}

// //Get DBMS operator type
int SQLreader::getDBMSOperatorType(const char* op)
{
    string s = op;
    if (s == "=")
        return COND_EQ;
    else if (s == "<>")
        return COND_NE;
    else if (s == "<")
        return COND_LT;
    else if (s == ">")
        return COND_GT;
    else if (s == "<=")
        return COND_LE;
    else if (s == ">=")
        return COND_GE;
    else
        return -1;
}

// //Get column type
short SQLreader::getNextcolDataType()
{
    pointer++;
    if (tokens[pointer] == "char" && type[pointer] == Token::TOKEN_IDENTIFIER)
    {
        pointer++;
        if (tokens[pointer] != "(" || type[pointer] != Token::TOKEN_SYMBOL)
        {
            reportError("getNextcolDataType", "'('");
            return TYPE_NULL;
        }

        pointer++;
        int len = stoi(tokens[pointer]);
        if (type[pointer] != Token::TOKEN_NUMBER || len <= 0 || len > TYPE_CHAR)
        {
            reportError("getNextcolDataType", "1~255");
            return TYPE_NULL;
        }

        pointer++;
        if (tokens[pointer] != ")" || type[pointer] != Token::TOKEN_SYMBOL)
        {
            reportError("getNextcolDataType", "')'");
            return TYPE_NULL;
        }

        return len;
    }
    else if (tokens[pointer] == "int" && type[pointer] == Token::TOKEN_IDENTIFIER)
        return TYPE_INT;
    else if (tokens[pointer] == "float" && type[pointer] == Token::TOKEN_IDENTIFIER)
        return TYPE_FLOAT;
    else
    {
        reportError("getNextcolDataType", "'char', 'int' or 'float'(DBMS only supports these three data types)");
        return TYPE_NULL;
    }
}

// //Report error
void SQLreader::reportError(const char* position, const char* expecting)
{
    cerr << "ERROR: [SQLreader::" << position << "] Expecting " << expecting << ", but found '" << tokens[pointer] << "'." << endl;
    skipSQLStatement();
}

// //Skip current statement
void SQLreader::skipSQLStatement()
{
    if (pointer < 0)
        pointer = 0;
    for (; type[pointer] != Token::TOKEN_END; pointer++);
}
