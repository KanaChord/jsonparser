#ifndef LEXER_H
#define LEXER_H

// will i even use those ?
#define TT_CBO '{'
#define TT_CBC '}'
#define TT_BO '['
#define TT_BC ']'
#define TT_INT 'i'    // /[0-9]+/
#define TT_FLOAT 'f'  // /[0-9]*\.[0-9]+/
#define TT_STRING 's' // /\".*\"/
#define TT_COMA ','
#define TT_DD ':'

struct tokenList
{
    char tokenType;
    char *value;
    struct tokenList *next;
};

void clearList(struct tokenList **reflist);
int isIn(char c, char *s);
struct tokenList *lexer(char *str);
#endif