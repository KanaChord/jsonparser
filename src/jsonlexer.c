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
    char *r = s;
    if (s != NULL)
        while (*s != '\0')
        {
            if (c == *s)
                return (s - r + 1);
            s++;
        }
    return 0;
}

void append(struct tokenList **list, tokenType type, char *value)
{
    struct tokenList *token = malloc(sizeof(struct tokenList));
    struct tokenList *ref = *list;

    token->tokenType = type;
    token->value = value;
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
    char *a[] = {
        "TT_INT",
        "TT_FLOAT",
        "TT_STRING",
        "TT_CBO",
        "TT_BO",
        "TT_CBC",
        "TT_BC",
        "TT_COMA",
        "TT_DD"};
    while (list != NULL)
    {
        printf("( %s: '%s' ) ", a[list->tokenType], list->value);
        list = list->next;
    }
}

#ifndef _POSIX_VERSION

char *strndup(char *s, int size)
{
    char *c = malloc(size + 1);
    memcpy(c, s, size);
    c[size] = '\0';

    return c;
}

#endif

char *getIntFloat(char **str)
{
    char *value = NULL;
    char *ptr = *str;
    int dotCount = 0;

    while (isIn(**str, "0123456789.") && dotCount <= 1)
    {
        if (**str == '.')
            dotCount++;
        (*str)++;
    }

    if (*(*str - 1) != '.')
        value = strndup(ptr, *str - ptr);

    return value;
}

char *extractString(char **str)
{
    char *value = NULL;
    char *ptr = ++(*str);
    while (**str != '\"' && **str != '\0')
        (*str)++;

    if (**str != '\0')
        value = strndup(ptr, *str - ptr);

    return value;
}

struct tokenList *lexer(char *str)
{
    struct tokenList *list = NULL;

    while (*str != '\0')
    {
        char *value = NULL;
        tokenType type = 0xFF;

        if (isIn(*str, " \t\n"))
            /* pass */;
        else
        {
            if (isIn(*str, "0123456789."))
            {
                char *value = getIntFloat(&str);
                type = isIn('.', value) ? TT_FLOAT : TT_INT;
            }
            else if (*str == '\"')
            {
                type = TT_STRING;
                value = extractString(&str);
            }
            else if (isIn(*str, "{[}],:"))
                type = isIn(*str, "{[}],:") + 2;

            if (type <= 2 && value == NULL || type == 0xFF)
            {
                clearList(&list);
                break;
            }

            append(&list, type, value);
        }
        str++;
    }

    return list;
}

int main()
{
    char input[100] = "{\"hello\":\"world\"}";
    struct tokenList *list = NULL;
    char *string;

    while (1)
    {
        printf("json> ");
        fgets(input, 100, stdin);
        list = lexer(input);
        printList(list);
        printf("\n");
        memset(input, 0, 100);
    }

    return 0;
}