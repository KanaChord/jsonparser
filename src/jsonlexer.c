/*
    Thomas D. 2021-03
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "../inc/lexer.h"

int isIn(char c, char *s)
{
    while (*s != '\0')
    {
        if (c == *s)
            return 1;
        s++;
    }
    return 0;
}

void append(struct tokenList **list, char type, char *value)
{
    struct tokenList *token = malloc(sizeof(struct tokenList));
    struct tokenList *ref = *list;

    token->tokenType = type;
    if (value != NULL)
    {
        token->value = malloc(strlen(value) + 1);
        memcpy(token->value, value, strlen(value) + 1);
    }
    else
    {
        token->value = NULL;
    }
    token->next = NULL;

    if (*list != NULL)
    {
        while (ref->next != NULL)
            ref = ref->next;
        ref->next = token;
    }
    else
    {
        *list = token;
    }
}

void clearList(struct tokenList **reflist)
{
    if (*reflist != NULL)
    {
        if ((*reflist)->next != NULL)
            clearList(&((*reflist)->next));
        if ((*reflist)->value != NULL)
        {
            free((*reflist)->value);
            (*reflist)->value = NULL;
        }
        free(*reflist);
        *reflist = NULL;
    }
}

void printList(struct tokenList *list)
{
    struct tokenList *token = list;
    while (token != NULL)
    {
        printf("( '%c': '%s' ) ", token->tokenType, token->value);
        token = token->next;
    }
}

int isFloat(char **str, char **value)
{
    char *ptr = *str;
    int dotCount = 0;
    while (isIn(**str, "0123456789."))
    {
        if (**str == '.')
        {
            if (isIn(*(*str + 1), "0123456789") == 0)
            {
                return 0;
            }
            if (dotCount == 0)
                dotCount++;
            else
                break;
        }
        (*str)++;
    }
    *value = malloc(*str - ptr + 1);
    memset(*value, 0, *str - ptr + 1);
    memcpy(*value, ptr, *str - ptr);
    (*str)--;
    return dotCount;
}

char *extractString(char **str)
{
    char *ptr = (*str)++;
    while (*(*str + 1) != '\"')
        if (**str == '\0')
            return NULL;
        else
            (*str)++;

    char *value = malloc(*str - ptr + 1);
    memset(value, 0, *str - ptr + 1);
    memcpy(value, ptr + 1, *str - ptr);

    (*str)++;
    return value;
}

struct tokenList *lexer(char *str)
{
    // init list
    struct tokenList *list = NULL;

    // iterate through list
    while (*str != '\0')
    {
        if (isIn(*str, " \t\n"))
        { /*pass*/
        }
        else if (isIn(*str, "0123456789."))
        {
            char *value = NULL;
            int ret = isFloat(&str, &value);
            if (value != NULL)
            {
                append(&list, ret ? TT_FLOAT : TT_INT, value);
                free(value);
            }
            else
            {
                printf("Illegal character : .");
                return NULL;
            }
        }
        else if (isIn(*str, "{}[]:,"))
        {
            append(&list, *str, NULL);
        }
        else if (*str == '\"')
        {
            char *value = extractString(&str);
            if (value != NULL)
            {
                append(&list, 's', value);
                free(value);
            }
            else
            {
                printf("Non terminated string");
                return NULL;
            }
        }
        else
        {
            printf("Invalid character : %c", *str);
            return NULL;
        }
        str++;
    }

    return list;
}