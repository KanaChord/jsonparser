/*
    Thomas D. 2021-03
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "../inc/json.h"
#include "../inc/lexer.h"
#include "../inc/parser.h"

struct strjsn * newJsn(char type)
{
    struct strjsn * obj = NULL;
    obj = malloc(sizeof(struct strjsn));
    memset(obj, 0, sizeof(struct strjsn));
    obj->type = type;

    return obj;
}

void freeJsn(struct strjsn **objref)
{
    if (*objref != NULL)
    {
        if ((*objref)->type == OBJECT || (*objref)->type == ARRAY)
        {
            if ((*objref)->value.o != NULL)
                freeJsn(&(*objref)->value.o);
        }
        else if ((*objref)->type == STRING)
        {
            if ((*objref)->value.s != NULL)
                free(&(*objref)->value.s);
        }

        if ((*objref)->next != NULL)
            freeJsn(&(*objref)->next);

        free(*objref);
        *objref = NULL;
    }
}

long long jsnSize(struct strjsn *obj)
{
    long long size = 0;

    if (obj != NULL)
    {
        if (obj->key != NULL)
            size += strlen(obj->key) + 3;

        if (obj->type == OBJECT || obj->type == ARRAY)
        {
            size += jsnSize(obj->value.o) + 2;
        }
        else if (obj->type == STRING)
        {
            size += strlen(obj->value.s) + 2;
        }
        else if (obj->type == INTEGER)
        {
            int length = 1;
            int x = obj->value.i;
            while (x /= 10)
                length++;
            size += length;
        }
        else if (obj->type == FLOAT)
        {
            char a[100] = {0};
            sprintf(a, "%f", obj->value.f);
            size += strlen(a);
        }

        if (obj->next != NULL)
            size += jsnSize(obj->next) + 1;
    }

    return size;
}

struct strjsn *jsntostr(char *jsn)
{
    struct tokenList *list = NULL;
    struct tokenList *listCopy = NULL;
    struct strjsn *obj = NULL;

    listCopy = list = lexer(jsn);
    obj = parser(&listCopy);
    clearList(&list);

    return obj;
}

char *strtojsn(struct strjsn *obj)
{
    char *string = NULL;
    char *temp = NULL;
    long long size = jsnSize(obj);

    if (size > 0)
    {
        string = malloc(size + 1);
        memset(string, 0, size + 1);

        if (obj->key != NULL)
            sprintf(string, "\"%s\":", obj->key);

        if (obj->type == OBJECT)
        {
            temp = strtojsn(obj->value.o);
            sprintf(string + strlen(string), "{%s}", temp == NULL ? "" : temp);
        }
        else if (obj->type == ARRAY)
        {
            temp = strtojsn(obj->value.o);
            sprintf(string + strlen(string), "[%s]", temp == NULL ? "" : temp);
        }
        else if (obj->type == INTEGER)
        {
            sprintf(string + strlen(string), "%d", obj->value.i);
        }
        else if (obj->type == FLOAT)
        {
            sprintf(string + strlen(string), "%f", obj->value.f);
        }
        else if (obj->type == STRING)
        {
            sprintf(string + strlen(string), "\"%s\"", obj->value.s);
        }

        if (temp != NULL)
            free(temp), temp = NULL;

        if (obj->next != NULL)
        {
            temp = strtojsn(obj->next);
            sprintf(string + strlen(string), ",%s", temp);
        }
    }

    if (temp != NULL)
        free(temp);
    return string;
}
