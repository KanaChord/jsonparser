/*
    Thomas D. 2021-03
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "../inc/lexer.h"
#include "../inc/parser.h"
#include "../inc/json.h"

struct strjsn *getVerb(struct tokenList **list)
{
    struct strjsn *obj = NULL;

    if (*list != NULL && (*list)->tokenType == TT_STRING)
    {
        char *key = (*list)->value;
        *list = (*list)->next;
        if (*list != NULL && (*list)->tokenType == TT_DD)
        {
            *list = (*list)->next;
            obj = parser(list);
            if (obj != NULL)
            {
                obj->key = malloc(strlen(key) + 1);
                memcpy(obj->key, key, strlen(key) + 1);
                *list = (*list)->next;
                if (*list != NULL && (*list)->tokenType == TT_COMA)
                {
                    *list = (*list)->next;
                    obj->next = getVerb(list);
                }
            }
        }
    }

    return obj;
}

struct strjsn *parser(struct tokenList **list)
{
    struct strjsn *obj = NULL;
    struct strjsn **refobj = NULL;

    if (*list != NULL)
        if (isIn((*list)->tokenType, "{[fis"))
        {
            obj = newJsn((*list)->tokenType);

            switch (obj->type)
            {
            case INTEGER:
                obj->value.i = atoi((*list)->value);
                break;
            case FLOAT:
                obj->value.f = atof((*list)->value);
                break;
            case STRING:
                obj->value.s = malloc(strlen((*list)->value) + 1);
                memcpy(obj->value.s, (*list)->value, strlen((*list)->value) + 1);
                break;
            case OBJECT:
                *list = (*list)->next;
                obj->value.o = getVerb(list);
                if (*list == NULL || (*list)->tokenType != TT_CBC)
                {
                    printf("Unterminated object");
                    freeJsn(&obj);
                }
                break;
            case ARRAY:
                refobj = &(obj->value.o);
                do
                {
                    *list = (*list)->next;
                    if (*list == NULL || (*list)->tokenType == TT_BC)
                        break;
                    *refobj = parser(list);
                    refobj = &((*refobj)->next);
                    *list = (*list)->next;
                } while (*list != NULL && (*list)->tokenType == TT_COMA);
                if (*list == NULL || (*list)->tokenType != TT_BC)
                {
                    printf("Unterminated Array");
                    freeJsn(&obj);
                }
                break;
            }
        }
        else
        {
            printf("Expected object, array, string, float or int and got : %c", (*list)->tokenType);
        }

    return obj;
}