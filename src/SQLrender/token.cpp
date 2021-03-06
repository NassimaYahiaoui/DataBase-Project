#include <cstdio>
#include <iostream>
#include "SQLrender/token.h"

using namespace std;


const int Token::TOKEN_INVALID = -1;
const int Token::TOKEN_IDLE = 0;
const int Token::TOKEN_END = 1;
const int Token::TOKEN_IDENTIFIER = 2;
const int Token::TOKEN_NUMBER = 3;
const int Token::TOKEN_STRING_SINGLE = 4;
const int Token::TOKEN_STRING_DOUBLE = 5;
const int Token::TOKEN_SYMBOL = 6;
const int Token::TOKEN_OPERATOR = 7;

int Token::getTokens(const char* sql, vector<string>* tokens, vector<string>* db_tokens, vector<string>* use_tokens,vector<int>* type)
{
    int endCount = 0;
    int state = TOKEN_IDLE;
    int cursor = 0;
    string token = "";

    while (true)
    {
        bool moveCursor = true;
        char c = sql[cursor];
        if (state != TOKEN_STRING_SINGLE && state != TOKEN_STRING_DOUBLE && c >= 'A' && c <= 'Z')
            c += 'a' - 'A';

        if (state == TOKEN_IDLE)
        {
            if (c == ';')
            {
                state = TOKEN_END;
                moveCursor = false;
            }
            else if ((c >= 'a' && c <= 'z') || c == '_')
            {
                state = TOKEN_IDENTIFIER;
                moveCursor = false;
            }
            else if ((c >= '0' && c <= '9') || c == '+' || c == '-' || c == '.')
            {
                state = TOKEN_NUMBER;
                moveCursor = false;
            }
            else if (c == '\'')
                state = TOKEN_STRING_SINGLE;
            else if (c == '"')
                state = TOKEN_STRING_DOUBLE;
            else if (c == ',' || c == '(' || c == ')' || c == '*')
            {
                state = TOKEN_SYMBOL;
                moveCursor = false;
            }
            else if (c == '=' || c == '<' || c == '>')
            {
                state = TOKEN_OPERATOR;
                moveCursor = false;
            }
            else if (c == ' ' || c == '\n' || c == '\t') {}
            else if (c == 0)
                break;
            else
            {
                cerr << "ERROR: [Token::getTokens] Unknown character '" << c << "'!" << endl;
                tokens->push_back(token);
				db_tokens->push_back(token);
				use_tokens->push_back(token);
                type->push_back(TOKEN_INVALID);
                token.clear();
            }
        }
        else if (state == TOKEN_END)
        {
            token += c;

            tokens->push_back(token);
			db_tokens->push_back(token);
			use_tokens->push_back(token);
            type->push_back(TOKEN_END);
            token.clear();
            endCount++;

            state = TOKEN_IDLE;
        }
        else if (state == TOKEN_IDENTIFIER)
        {
            if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_')
                token += c;
            else
            {
                tokens->push_back(token);
				db_tokens->push_back(token);
				use_tokens->push_back(token);
                type->push_back(TOKEN_IDENTIFIER);
                token.clear();

                state = TOKEN_IDLE;
                moveCursor = false;
            }
        }
        else if (state == TOKEN_NUMBER)
        {
            if ((c >= '0' && c <= '9') || c == '+' || c == '-' || c == '.')
                token += c;
            else
            {
                tokens->push_back(token);
				db_tokens->push_back(token);
				use_tokens->push_back(token);
                type->push_back(TOKEN_NUMBER);
                token.clear();

                state = TOKEN_IDLE;
                moveCursor = false;
            }
        }
        else if (state == TOKEN_STRING_SINGLE || state == TOKEN_STRING_DOUBLE)
        {
            if ((c == '\'' && state == TOKEN_STRING_SINGLE) || (c == '"' && state == TOKEN_STRING_DOUBLE))
            {
                tokens->push_back(token);
				db_tokens->push_back(token);
				use_tokens->push_back(token);
                type->push_back(state);
                token.clear();

                state = TOKEN_IDLE;
            }
            else if (c == '\n' || c == 0)
            {
                cerr << "ERROR: [Token::getTokens] New line symbol reached when reading string!" << endl;
                tokens->push_back(token);
				db_tokens->push_back(token);
				use_tokens->push_back(token);
                type->push_back(TOKEN_INVALID);
                token.clear();
                state = TOKEN_IDLE;
            }
            else
                token += c;
        }
        else if (state == TOKEN_SYMBOL)
        {
            token += c;

            tokens->push_back(token);
			db_tokens->push_back(token);
			use_tokens->push_back(token);
            type->push_back(TOKEN_SYMBOL);
            token.clear();

            state = TOKEN_IDLE;
        }
        else if (state == TOKEN_OPERATOR)
        {
            if (c == '=' || c == '<' || c == '>')
                token += c;
            else
            {
                tokens->push_back(token);
				db_tokens->push_back(token);
				use_tokens->push_back(token);
                type->push_back(TOKEN_OPERATOR);
                token.clear();

                state = TOKEN_IDLE;
                moveCursor = false;
            }
        }

        if (moveCursor)
            cursor++;
    }

    return endCount;
}
