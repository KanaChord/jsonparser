#ifndef JSON_H
#define JSON_H

typedef enum
{
    INTEGER,
    FLOAT,
    STRING,
    OBJECT,
    ARRAY,
} jsnType;

typedef union value
{
    char *s;
    int i;
    float f;
    struct strjsn *o;
} jsnValue;

struct strjsn
{
    jsnType type;
    char *key;
    jsnValue value;
    struct strjsn *next;
};

struct strjsn *newJsn(char type);
void freeJsn(struct strjsn **objref);
long long jsnSize(struct strjsn *obj);
struct strjsn *jsntostr(char *jsn);
char *strtojsn(struct strjsn *obj);

#endif