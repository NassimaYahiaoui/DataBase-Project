#ifndef _INDEX_H
#define _INDEX_H

#include <string>

using namespace std;

class Index
{
public:

    // //INDEX Constructor
    Index(const char* data);

    // //Get relation name
    const char* getRelationName() const;

    // //Get relation name
    const char* getrelationName() const;

    // //Get field name
    const char* getfieldName() const;

#ifdef DEBUG
    // //Print index info
    void debugPrint() const;
#endif

private:

    // //relation index name
    string name;

    // //relation name
    string relationName;

    // //field name
    string fieldName;
};

#endif
