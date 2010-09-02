/*
 * Implementace interpretu imperativniho jazyka IFJ07
 *
 * Autori:
 *  xfilom00 -- Filo Martin
 *  xkalab00 -- Kalab Jan
 *  xkobli00 -- Koblizek Ondrej
 *  xnovyo01 -- Novy Ondrej
 */


#ifndef ERROR_H
#define ERROR_H


#ifndef NDEBUG
#define debug(...) do { printf("["); printf(__func__); printf(":"); printf("%d", __LINE__); printf("] "); printf( __VA_ARGS__); printf("\n"); } while (0);
#else
#define debug(...)
#endif


enum errors {
    ERROR_NO_ERROR = 0,
    ERROR_NO_MEMORY,
    ERROR_FOPEN,
    ERROR_STACK_POP,
    ERROR_STACK_TOP,
    ERROR_STACK_LOOKATNEXT,
    ERROR_LEX,
    ERROR_SYNTAX,
    ERROR_SEMANTIC,
    ERROR_INTERPRETER,
    ERROR_THREEADR_CODE
};

inline extern void error(int error_code);
inline extern int get_error();


#endif
