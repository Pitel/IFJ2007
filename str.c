/*
 * Implementace interpretu imperativniho jazyka IFJ07
 *
 * Autori:
 *  xfilom00 -- Filo Martin
 *  xkalab00 -- Kalab Jan
 *  xkobli00 -- Koblizek Ondrej
 *  xnovyo01 -- Novy Ondrej
 */


#include <stdlib.h>
#include <assert.h>

#include "str.h"
#include "error.h"

/**
 * Initialize string
 *
 * @param str String
 */
void str_init(Tstr *str)
{
    assert(str != NULL);

    char *data;

    data = malloc(STR_DEFAULT_SIZE);
    if (data) {
	str->size = STR_DEFAULT_SIZE;
	str->len = 0;
	str->data = data;
	str->data[0] = '\0';
    } else {
	error(ERROR_NO_MEMORY);
    }
}

/**
 * Destroy string
 *
 * @param str String
 */
void str_free(Tstr *str)
{
    assert(str != NULL);

    if (str->data) {
	str->len = 0;
	str->size = 0;
	free(str->data);
	str->data = NULL;
    }
}

/**
 * Return C char*
 *
 * @param str String
 * @return C char*
 */
inline char *str_c_str(Tstr *str)
{
    assert(str != NULL);

    return str->data;
}

/**
 * Internal strcat to concate char* to Str only len2 chars
 *
 * @param str Destionation string
 * @param ch Source char*
 * @param len2 Length of ch
 */
void _str_strcat(Tstr *str, char *ch, size_t len2)
{
    assert(str != NULL);
    assert(ch != NULL);

    size_t newsize = str->size;
    size_t newlen = str->len + len2;

    while (newlen >= newsize) {
	newsize *= 2;
    }

    if (newsize != str->size) {
	str->data = realloc(str->data, newsize);
	if (str->data) {
	    str->size = newsize;
	} else {
	    error(ERROR_NO_MEMORY);
	    return;
	}
    }

    strncat(str->data + str->len, ch, len2);
    str->len = newlen;
    str->data[str->len] = '\0';
}

/**
 * Strcat - concate char to Str
 *
 * @param str Destionation string
 * @param ch Source char
 */
inline void str_strcatCh(Tstr *str, char ch)
{
    assert(str != NULL);

    _str_strcat(str, &ch, 1);
}

/**
 * Strcat - concate char* to Str
 *
 * @param str Destionation string
 * @param ch Source char*
 */
inline void str_strcatChA(Tstr *str, char *ch)
{
    assert(str != NULL);
    assert(ch != NULL);

    _str_strcat(str, ch, strlen(ch));
}

/**
 * Strcat - concate Str to Str
 *
 * @param str Destionation string
 * @param str2 Source string
 */
inline void str_strcatStr(Tstr *str, Tstr *str2)
{
    assert(str != NULL);
    assert(str2 != NULL);

    _str_strcat(str, str_c_str(str2), str_strlen(str2));
}

/**
 * Shrink string - free unused string memory
 *
 * @param str String for shrinking
 */
void str_shrink(Tstr *str)
{
    assert(str != NULL);

    if (str->size > str->len + 1) {
	str->data = realloc(str->data, str->len + 1);
	if (str->data) {
	    str->size = str->len + 1;
	} else {
	    error(ERROR_NO_MEMORY);
	}
    }
}

/**
 * Return string length
 *
 * @param str String
 * @return Length of string
 */
inline size_t str_strlen(Tstr *str)
{
    assert(str != NULL);

    return str->len;
}

/**
 * Strcmp - compare string and char*
 *
 * @param str String
 * @param ch Char*
 * @return Compare output
 */
int str_strcmpChA(Tstr *str, char *ch)
{
    assert(str != NULL);
    assert(ch != NULL);

    return strcmp(str->data, ch);
}

/**
 * Strcmp - compare two string
 *
 * @param str String
 * @param str2 String
 * @return Compare output
 */
inline int str_strcmpStr(Tstr *str, Tstr *str2)
{
    assert(str != NULL);
    assert(str2 != NULL);

    return str_strcmpChA(str, str2->data);
}
