#ifndef _TOKENIZER_H
#define _TOKENIZER_H

#include <vector>
#include <string>

using namespace std;

class Token
{
public:

   
    static const int TOKEN_INVALID;
    static const int TOKEN_IDLE;
    static const int TOKEN_END;
    static const int TOKEN_IDENTIFIER;
    static const int TOKEN_NUMBER;
    static const int TOKEN_STRING_SINGLE;
    static const int TOKEN_STRING_DOUBLE;
    static const int TOKEN_SYMBOL;
    static const int TOKEN_OPERATOR;

    int getTokens(const char* sql, vector<string>* tokens,vector<string>* db_tokens, vector<string>* use_tokens ,vector<int>* type);
};

#endif
