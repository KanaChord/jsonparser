#ifndef JSON_H
#define JSON_H

#define OBJECT TT_CBO
#define ARRAY TT_BO
#define INTEGER TT_INT
#define FLOAT TT_FLOAT
#define STRING TT_STRING

struct strjsn
{
    char type;
    char *key;
    union value
    {
        char *s;
        int i;
        float f;
        struct strjsn *o;
    } value;
    struct strjsn *next;
};

struct strjsn * newJsn(char type);
void freeJsn(struct strjsn **objref);
long long jsnSize(struct strjsn *obj);
struct strjsn *jsntostr(char *jsn);
char *strtojsn(struct strjsn *obj);

#endif