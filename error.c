/*
 * Implementace interpretu imperativniho jazyka IFJ07
 *
 * Autori:
 *  xfilom00 -- Filo Martin
 *  xkalab00 -- Kalab Jan
 *  xkobli00 -- Koblizek Ondrej
 *  xnovyo01 -- Novy Ondrej
 */


#include "error.h"

static int _error_code = ERROR_NO_ERROR;

/**
 * Sets error state to error code
 *
 * @param errorCode Error code
 */
inline void error(int error_code)
{
    _error_code = error_code;
}

/*
 * Return error state (code)
 *
 * @return Error code
 */
inline int get_error()
{
    return _error_code;
}
