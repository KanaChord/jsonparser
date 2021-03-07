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

struct strjsn *newJsn(jsnType type)
{
    struct strjsn *obj = NULL;
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

struct strjsn *jsnGet(struct strjsn *obj, char *key)
{
    struct strjsn *res = NULL;
    if (obj != NULL && obj->type == OBJECT)
    {
        obj = obj->value.o;
        while (obj != NULL)
        {
            if (strcmp(key, obj->key) == 0)
            {
                res = obj;
                break;
            }
            obj = obj->next;
        }
    }

    return res;
}

struct strjsn *jsnArrayGet(struct strjsn *arr, int index)
{
    int i = 0;
    struct strjsn *res = NULL;
    if (arr != NULL && arr->type == ARRAY)
    {
        arr = arr->value.o;
        while (arr != NULL && i < index)
        {
            i++;
            arr = arr->next;
        }
        res = arr;
    }
    return res;
}

void jsnPush(struct strjsn *arr, struct strjsn *elem)
{
    if (arr != NULL && arr->type == ARRAY)
    {
        if (arr->value.o != NULL)
        {
            arr = arr->value.o;
            while (arr->next != NULL)
                arr = arr->next;
            arr->next = elem;
        }
        else
            arr->value.o = elem;
    }
}

struct strjsn *jsnPop(struct strjsn *arr)
{
    struct strjsn *res = NULL;
    struct strjsn *prev = NULL;

    if (arr != NULL && arr->type == ARRAY)
    {
        if (arr->value.o != NULL)
        {
            prev = arr;
            arr = arr->value.o;
            while (arr->next != NULL)
            {
                prev = arr;
                arr = arr->next;
            }
            prev->next = NULL;
            res = arr;
        }
        else
        {
            res = arr->value.o;
            arr->value.o = NULL;
        }
    }
    return res;
}

struct strjsn *jsnCpy(struct strjsn *obj, int deep)
{
    struct strjsn *res = NULL;
    if (obj != NULL)
    {
        res = newJsn(obj->type);
        if (obj->type == OBJECT || obj->type == ARRAY)
            res->value.o = jsnCpy(obj->value.o, 1);
        else if (obj->type == STRING)
        {
            res->value.s = malloc(strlen(obj->value) + 1);
            memset(res->value.s, 0, strlen(obj->value) + 1);
            memcpy(res->value.s, obj->value.s, strlen(obj->value.s));
        }
        else
        {
            res->value = obj->value;
        }
        if (deep == 1)
            res->next = jsnCpy(obj->next);
    }
    return res;
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
