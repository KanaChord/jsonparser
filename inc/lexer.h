#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TT_INT,     // /[0-9]+/
    TT_FLOAT,   // /[0-9]*\.[0-9]+/
    TT_STRING,  // /\".*\"/
    TT_CBO,
    TT_BO, 
    TT_CBC, 
    TT_BC, 
    TT_COMA, 
    TT_DD
} tokenType;

struct tokenList
{
    tokenType tokenType;
    char *value;
    struct tokenList *next;
};

void clearList(struct tokenList **reflist);
int isIn(char c, char *s);
struct tokenList *lexer(char *str);
#endif