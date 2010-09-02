/*
 * Implementace interpretu imperativniho jazyka IFJ07
 *
 * Autori:
 *  xfilom00 -- Filo Martin
 *  xkalab00 -- Kalab Jan
 *  xkobli00 -- Koblizek Ondrej
 *  xnovyo01 -- Novy Ondrej
 */


#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "stack.h"
#include "error.h"

/**
 * Stack Initialization
 *
 * @param *S is a pointer to stack of struct Tstack, for return stack
 */
void stack_init(Tstack *S)
{
    assert(S != NULL);

    S->top_ptr = NULL;
} // stack_init

/**
 * Push data to the stack
 *
 * @param *S is a pointer to stack of struct Tstack, for return stack
 * @param D is a pointer for data.
 */
void stack_push(Tstack *S, void **const D)
{
    assert(S != NULL);
    assert(D != NULL);

    Tstack_el newItem;
    if ((newItem = malloc(sizeof(struct Tstack_elS))) == NULL) {
	error(ERROR_NO_MEMORY);
    } else {
	newItem->data = *D;
	newItem->next_ptr = S->top_ptr;
	S->top_ptr = newItem;
    }
} // stack_push

/**
 * Pop data from the stack
 *
 * @param *S is a pointer to stack of struct Tstack, for return stack
 */
void stack_pop(Tstack *S)
{
    assert(S != NULL);

    if (S->top_ptr != NULL) {
	Tstack_el tmp;
	tmp = S->top_ptr;
	S->top_ptr = S->top_ptr->next_ptr;
	free(tmp);
    } else {
	error(ERROR_STACK_POP);
    }
} // stack_pop

/**
 * Return data from op of the stack
 *
 * @param *S is a pointer to stack of struct Tstack, for return stack
 * @param D is a pointer for data, for return data
 */
void stack_top(Tstack *S, void **D)
{
    assert(S != NULL);
    assert(D != NULL);

    if (S->top_ptr != NULL) {
	*D = S->top_ptr->data;
    } else {
	error(ERROR_STACK_TOP);
    }
} // stack_top

/**
 * Is stack empty
 *
 * @param *S is a pointer to stack of struct Tstack
 * @return bool, true if empty
 */
bool stack_empty(Tstack *S)
{
    assert(S != NULL);

    return (S->top_ptr == NULL);
} // stack_empty

/**
 * Count of elements
 * @param *S is a pointer to stack of struct Tstack
 */
int stack_count(Tstack *S)
{
    assert(S != NULL);

    int i = 0;
    Tstack_el tmp;
    tmp = S->top_ptr;
    while (tmp != NULL) {
	tmp = tmp->next_ptr;
	i++;
    }

    return i;
} // stack_count

/**
 * Free stack
 * @param *S is a pointer to stack of struct Tstack
 */
void stack_free(Tstack *S)
{
    assert(S != NULL);

    Tstack_el tmp, fr;
    tmp = S->top_ptr;
    while (tmp != NULL) {
	fr = tmp;
	tmp = tmp->next_ptr;
	free(fr);
    }
    S->top_ptr = NULL;
} // stack_free

/**
 * Return next data from the stack
 *
 * @param *S is a pointer to stack of struct Tstack, for return stack
 * @param D is a pointer for data, for return data
 */
void stack_lookatnext(Tstack *S, void **D)
{
    assert(S != NULL);
    assert(D != NULL);

    if (S->top_ptr != NULL) {
	if (S->top_ptr->next_ptr != NULL) {
	    *D = S->top_ptr->next_ptr->data;
	} else {
	    error(ERROR_STACK_LOOKATNEXT);
	}
    } else {
	error(ERROR_STACK_TOP);
    }
} // stack_lookatnext
