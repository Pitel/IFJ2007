/*
 * Implementace interpretu imperativniho jazyka IFJ07
 *
 * Autori:
 *  xfilom00 -- Filo Martin
 *  xkalab00 -- Kalab Jan
 *  xkobli00 -- Koblizek Ondrej
 *  xnovyo01 -- Novy Ondrej
 */


#include <stdbool.h>

#ifndef STACK_H
#define STACK_H


typedef struct Tstack_elS {
	void *data;
	struct Tstack_elS *next_ptr;
} *Tstack_el;

typedef struct {
	Tstack_el top_ptr;
} Tstack;

extern void stack_init(Tstack *S);
extern void stack_push(Tstack *S, void **const D);
extern void stack_pop(Tstack *S);
extern void stack_top(Tstack *S, void **D);
extern bool stack_empty(Tstack *S);
extern int stack_count(Tstack *S);
extern void stack_free(Tstack *S);
extern void stack_lookatnext(Tstack *S, void **D);


#endif
