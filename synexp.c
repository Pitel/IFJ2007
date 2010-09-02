/*
 * Implementace interpretu imperativniho jazyka IFJ07
 *
 * Autori:
 *  xfilom00 -- Filo Martin
 *  xkalab00 -- Kalab Jan
 *  xkobli00 -- Koblizek Ondrej
 *  xnovyo01 -- Novy Ondrej
 */


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "synexp.h"
#include "error.h"
#include "stack.h"
#include "lex.h"
#include "avl.h"
#include "str.h"
#include "threeadr.h"

void syn_free_stack(Tstack *stack, Tstack_syntax *stack_data)
{
    assert(stack != NULL);
    assert(stack_data != NULL);
    while (stack_empty(stack) != true) {
	// Free stack
	stack_top(stack, (void *) &stack_data);
	token_free(&stack_data->stoken);
	free(stack_data);
	stack_pop(stack);
    }
}

char *syn_next_cons_name(size_t *cons_counter)
{
    assert(cons_counter != NULL);

    int n;
    char *cons_name;
    cons_name = malloc(STR_DEFAULT_SIZE * sizeof(char));
    if (cons_name == NULL) {
	error(ERROR_NO_MEMORY);
	return NULL;
    }
    n = sprintf(cons_name, "@%d", (int) *cons_counter);
    if (n < 0) {
	error(ERROR_SYNTAX);
	free(cons_name);
	return NULL;
    }
    (*cons_counter)++;
    return cons_name;
}

/**
 * Expression analyser
 *
 * @param stream Stream for loading next token
 * @param token Current token
 * @param symtab Symbol table (AVL root)
 * @param cons_counter number for original naming of item in symbol table
 */
Tnode_ptr syntax_synexp(FILE *stream, Tlex_token *token,
			Tnode_ptr *symtab, size_t *cons_counter,
			Tthree_adr **ta)
{
    assert(stream != NULL);
    assert(token != NULL);
    assert(symtab != NULL);
    assert(cons_counter != NULL);

    size_t precedence_table[][SYN_TABLE_SIZE] = {
//	   1 ,  2 ,  3 ,  4 ,  5 ,  6 ,  7 ,  8 ,  9 , 10 , 11 , 12 , 13 , 14 , 15 , 16 , 17 , 18 , 19 , 20 , 21 , 22 , 23 , 24 , 25 , 26
/*1*/	{ P_E, P_E, P_E, P_E, P_M, P_M, P_M, P_M, P_M, P_E, P_M, P_E, P_E, P_M, P_M, P_M, P_M, P_M, P_M, P_E, P_M, P_E, P_E, P_E, P_E, P_E } ,
/*2*/	{ P_E, P_E, P_E, P_E, P_M, P_M, P_M, P_M, P_M, P_E, P_M, P_E, P_E, P_M, P_M, P_M, P_M, P_M, P_M, P_E, P_M, P_E, P_E, P_E, P_E, P_E } ,
/*3*/	{ P_E, P_E, P_E, P_E, P_M, P_M, P_M, P_M, P_M, P_E, P_M, P_E, P_E, P_M, P_M, P_M, P_M, P_M, P_M, P_E, P_M, P_E, P_E, P_E, P_E, P_E } ,
/*4*/	{ P_E, P_E, P_E, P_E, P_M, P_M, P_M, P_M, P_M, P_E, P_M, P_E, P_E, P_M, P_M, P_M, P_M, P_M, P_M, P_E, P_M, P_E, P_E, P_E, P_E, P_E } ,
/*5*/	{ P_L, P_L, P_L, P_L, P_M, P_M, P_L, P_L, P_L, P_E, P_M, P_E, P_E, P_M, P_M, P_M, P_M, P_M, P_M, P_L, P_M, P_E, P_E, P_E, P_E, P_E } ,
/*6*/	{ P_L, P_L, P_L, P_L, P_M, P_M, P_L, P_L, P_L, P_E, P_M, P_E, P_E, P_M, P_M, P_M, P_M, P_M, P_M, P_L, P_M, P_E, P_E, P_E, P_E, P_E } ,
/*7*/	{ P_L, P_L, P_L, P_L, P_M, P_M, P_M, P_M, P_L, P_E, P_M, P_E, P_E, P_M, P_M, P_M, P_M, P_M, P_M, P_L, P_M, P_E, P_E, P_E, P_E, P_E } ,
/*8*/	{ P_L, P_L, P_L, P_L, P_M, P_M, P_M, P_M, P_L, P_E, P_M, P_E, P_E, P_M, P_M, P_M, P_M, P_M, P_M, P_L, P_M, P_E, P_E, P_E, P_E, P_E } ,
/*9*/	{ P_L, P_L, P_L, P_L, P_M, P_M, P_M, P_M, P_L, P_E, P_M, P_E, P_E, P_M, P_M, P_M, P_M, P_M, P_M, P_L, P_M, P_E, P_E, P_E, P_E, P_E } ,
/*10*/	{ P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E } ,
/*11*/	{ P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_E, P_Q, P_E, P_E, P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_E, P_E, P_E, P_E, P_E, P_E } ,
/*12*/	{ P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E } ,
/*13*/	{ P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E } ,
/*14*/	{ P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_E, P_M, P_E, P_E, P_M, P_M, P_L, P_L, P_L, P_L, P_L, P_M, P_E, P_E, P_E, P_E, P_E } ,
/*15*/	{ P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_E, P_M, P_E, P_E, P_M, P_M, P_L, P_L, P_L, P_L, P_L, P_M, P_E, P_E, P_E, P_E, P_E } ,
/*16*/	{ P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_E, P_M, P_E, P_E, P_M, P_M, P_M, P_M, P_M, P_M, P_L, P_M, P_E, P_E, P_E, P_E, P_E } ,
/*17*/	{ P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_E, P_M, P_E, P_E, P_M, P_M, P_M, P_M, P_M, P_M, P_L, P_M, P_E, P_E, P_E, P_E, P_E } ,
/*18*/	{ P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_E, P_M, P_E, P_E, P_M, P_M, P_M, P_M, P_M, P_M, P_L, P_M, P_E, P_E, P_E, P_E, P_E } ,
/*19*/	{ P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_E, P_M, P_E, P_E, P_M, P_M, P_M, P_M, P_M, P_M, P_L, P_M, P_E, P_E, P_E, P_E, P_E } ,
/*20*/	{ P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_E, P_E, P_E, P_E, P_L, P_L, P_L, P_L, P_L, P_L, P_L, P_EQ,P_E, P_E, P_E, P_E, P_E } ,
/*21*/	{ P_E, P_E, P_E, P_E, P_M, P_M, P_M, P_M, P_M, P_E, P_M, P_E, P_E, P_M, P_M, P_M, P_M, P_M, P_M, P_E, P_M, P_E, P_E, P_E, P_E, P_E } ,
/*22*/	{ P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E } ,
/*23*/	{ P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E } ,
/*24*/	{ P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E } ,
/*25*/	{ P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E } ,
/*26*/	{ P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E, P_E } ,
    };

    Tstack stack;
    stack_init(&stack);
    Tstack_syntax *stack_data = NULL;
    char *cons_name_tmp;
    Tnode_ptr operand1 = NULL;
    Tnode_ptr operand2 = NULL;
    Tdata_union value_tmp;
    Tnode_ptr SearchSymbol = NULL;
    Tnode_ptr Result = NULL;
    size_t type_tmp = 0;
    size_t type_tmp2 = 0;

    stack_data = malloc(sizeof(*stack_data));
    if (stack_data == NULL) {
	error(ERROR_NO_MEMORY);
	return NULL;
    }
    str_init(&stack_data->stoken.lexeme);
    str_strcatCh(&stack_data->stoken.lexeme, '$');
    stack_data->stoken.token_type = TOKEN_DOLAR;
    stack_data->symtab_ptr = NULL;
    stack_push(&stack, (void *) &stack_data);
    do {
	if (stack_data->stoken.token_type == TOKEN_SYNEXP_E) {	// jump over TOKEN_SYNTAX_E
	    stack_lookatnext(&stack, (void *) &stack_data);
	}
	switch (precedence_table
		[(size_t) stack_data->stoken.token_type -
		 1][(size_t) token->token_type - 1]) {
	    case P_EQ:
		stack_data = malloc(sizeof(*stack_data));
		if (stack_data == NULL) {	// TODO: Cleanup stack, test it
		    error(ERROR_NO_MEMORY);
		    syn_free_stack(&stack, stack_data);
		    return NULL;
		}
		stack_data->stoken.lexeme = token->lexeme;
		stack_data->stoken.token_type = token->token_type;
		stack_push(&stack, (void *) &stack_data);
		lex(stream, token);
		if (get_error()) {
		    syn_free_stack(&stack, stack_data);
		    return NULL;
		}
		break;
	    case P_L:		// Copy and push
		stack_data = malloc(sizeof(*stack_data));
		if (stack_data == NULL) {	// TODO: Cleanup stack, test it
		    error(ERROR_NO_MEMORY);
		    syn_free_stack(&stack, stack_data);
		    return NULL;
		}
		stack_data->stoken.lexeme = token->lexeme;
		stack_data->stoken.token_type = token->token_type;
		stack_push(&stack, (void *) &stack_data);
		lex(stream, token);
		if (get_error()) {
		    syn_free_stack(&stack, stack_data);
		    return NULL;
		}
		break;
	    case P_M:
		stack_top(&stack, (void *) &stack_data);
		SearchSymbol = NULL;
		switch (stack_data->stoken.token_type) {
		    case TOKEN_SYMBOL:
			SearchSymbol = NULL;
			avl_search_node(*symtab,
					stack_data->stoken.lexeme.data,
					&SearchSymbol);
			if (SearchSymbol == NULL) {
			    error(ERROR_SEMANTIC);
			    syn_free_stack(&stack, stack_data);
			    return NULL;
			} else {
			    str_free(&(stack_data)->stoken.lexeme);
			    stack_pop(&stack);
			    stack_data->stoken.token_type = TOKEN_SYNEXP_E;
			    stack_data->symtab_ptr = SearchSymbol;
			    stack_push(&stack, (void *) &stack_data);
			}
			break;
		    case TOKEN_INT:
			cons_name_tmp = syn_next_cons_name(cons_counter);
			Tdata_union integer;
			integer.i =
			    (int) strtol(stack_data->stoken.lexeme.data,
					 NULL, 10);
			avl_insert(symtab, cons_name_tmp, SYMBOL_INT,
				   &integer);
			avl_search_node(*symtab, cons_name_tmp,
					&SearchSymbol);
			str_free(&(stack_data)->stoken.lexeme);
			stack_pop(&stack);
			stack_data->stoken.token_type = TOKEN_SYNEXP_E;
			stack_data->symtab_ptr = SearchSymbol;
			stack_push(&stack, (void *) &stack_data);
			break;
		    case TOKEN_DOUBLE:
			cons_name_tmp = syn_next_cons_name(cons_counter);
			Tdata_union Double;
			Double.d =
			    strtod(stack_data->stoken.lexeme.data, NULL);
			avl_insert(symtab, cons_name_tmp, SYMBOL_DOUBLE,
				   &Double);
			avl_search_node(*symtab, cons_name_tmp,
					&SearchSymbol);
			str_free(&(stack_data)->stoken.lexeme);
			stack_pop(&stack);
			stack_data->stoken.token_type = TOKEN_SYNEXP_E;
			stack_data->symtab_ptr = SearchSymbol;
			stack_push(&stack, (void *) &stack_data);
			break;
		    case TOKEN_STRING:
			cons_name_tmp = syn_next_cons_name(cons_counter);
			Tdata_union String;
			str_init(&String.s);
			str_strcatStr(&String.s,
				      &(stack_data->stoken.lexeme));
			avl_insert(symtab, cons_name_tmp, SYMBOL_STRING,
				   &String);
			avl_search_node(*symtab, cons_name_tmp,
					&SearchSymbol);
			str_free(&(stack_data)->stoken.lexeme);
			stack_pop(&stack);
			stack_data->stoken.token_type = TOKEN_SYNEXP_E;
			stack_data->symtab_ptr = SearchSymbol;
			stack_push(&stack, (void *) &stack_data);
			break;
		    case TOKEN_R_BRACKET:
			str_free(&(stack_data)->stoken.lexeme);
			free(stack_data);
			stack_pop(&stack);
			stack_top(&stack, (void *) &stack_data);
			if (stack_data->stoken.token_type ==
			    TOKEN_SYNEXP_E) {
			    operand1 = stack_data->symtab_ptr;
			    free(stack_data);
			    stack_pop(&stack);
			    stack_top(&stack, (void *) &stack_data);
			    if (stack_data->stoken.token_type ==
				TOKEN_L_BRACKET) {
				str_free(&(stack_data)->stoken.lexeme);
				stack_pop(&stack);
				stack_data->stoken.token_type =
				    TOKEN_SYNEXP_E;
				stack_data->symtab_ptr = operand1;
				stack_push(&stack, (void *) &stack_data);
			    } else {	// otestovat
				error(ERROR_SYNTAX);
				syn_free_stack(&stack, stack_data);
				return NULL;
			    }
			} else {	// otestovat
			    error(ERROR_SYNTAX);
			    syn_free_stack(&stack, stack_data);
			    return NULL;
			}
			break;
		    case TOKEN_SYNEXP_E:
			operand2 = stack_data->symtab_ptr;
			free(stack_data);
			stack_pop(&stack);
			stack_top(&stack, (void *) &stack_data);
			switch (stack_data->stoken.token_type) {
			    case TOKEN_DIVIDE:
				str_free(&(stack_data)->stoken.lexeme);
				free(stack_data);
				stack_pop(&stack);
				stack_top(&stack, (void *) &stack_data);
				if (stack_data->stoken.token_type ==
				    TOKEN_SYNEXP_E) {
				    operand1 = stack_data->symtab_ptr;
				    cons_name_tmp =
					syn_next_cons_name(cons_counter);
				    switch (operand1->type) {
					case TOKEN_INT:
					case TOKEN_DOUBLE:
					    if (operand2->type !=
						TOKEN_STRING) {
						value_tmp.d = 0;
						type_tmp = SYMBOL_DOUBLE;
					    } else {
						error(ERROR_SEMANTIC);
						free(cons_name_tmp);
						syn_free_stack(&stack,
							       stack_data);
						return NULL;
					    }
					    break;
					case TOKEN_STRING:
					    error(ERROR_SEMANTIC);
					    free(cons_name_tmp);
					    syn_free_stack(&stack,
							   stack_data);
					    return NULL;
					    break;
					default:
					    break;
				    }
				    stack_pop(&stack);
				    stack_data->stoken.token_type =
					TOKEN_SYNEXP_E;
				    avl_insert(symtab, cons_name_tmp,
					       type_tmp, &value_tmp);
				    avl_search_node(*symtab, cons_name_tmp,
						    &SearchSymbol);
				    stack_data->symtab_ptr = SearchSymbol;
				    if (SearchSymbol == NULL) {
					error(ERROR_SYNTAX);
					syn_free_stack(&stack, stack_data);
					return NULL;
				    }
				    ta_insert_last(TA_DIV, &operand1,
						   &operand2,
						   &SearchSymbol, ta);
				    stack_push(&stack,
					       (void *) &stack_data);
				    if (get_error()) {
					syn_free_stack(&stack, stack_data);
					return NULL;
				    }
				} else {
				    error(ERROR_SYNTAX);
				    syn_free_stack(&stack, stack_data);
				    return NULL;
				}
				break;
			    case TOKEN_EQUAL:
			    case TOKEN_NOT_EQUAL:
			    case TOKEN_LESS:
			    case TOKEN_LESS_EQUAL:
			    case TOKEN_MORE:
			    case TOKEN_MORE_EQUAL:
				type_tmp2 = stack_data->stoken.token_type;
				str_free(&(stack_data)->stoken.lexeme);
				free(stack_data);
				stack_pop(&stack);
				stack_top(&stack, (void *) &stack_data);
				if (stack_data->stoken.token_type ==
				    TOKEN_SYNEXP_E) {
				    operand1 = stack_data->symtab_ptr;
				    cons_name_tmp =
					syn_next_cons_name(cons_counter);
				    switch (operand1->type) {
					case TOKEN_INT:
					    if (operand2->type !=
						SYMBOL_STRING) {
						value_tmp.i = -1;
						type_tmp = SYMBOL_INT;
					    } else {
						error(ERROR_SEMANTIC);
						free(cons_name_tmp);
						syn_free_stack(&stack,
							       stack_data);
						return NULL;
					    }
					    break;
					case TOKEN_DOUBLE:
					    if (operand2->type !=
						SYMBOL_STRING) {
						value_tmp.i = -1;
						type_tmp = SYMBOL_INT;
					    } else {
						error(ERROR_SEMANTIC);
						free(cons_name_tmp);
						syn_free_stack(&stack,
							       stack_data);
						return NULL;
					    }
					    break;
					case TOKEN_STRING:
					    if (operand2->type ==
						SYMBOL_STRING) {
						value_tmp.i = -1;
						type_tmp = SYMBOL_INT;
					    } else {
						error(ERROR_SEMANTIC);
						free(cons_name_tmp);
						syn_free_stack(&stack,
							       stack_data);
						return NULL;
					    }
					    break;
					default:
					    break;
				    }
				    stack_pop(&stack);
				    stack_data->stoken.token_type =
					TOKEN_SYNEXP_E;
				    avl_insert(symtab, cons_name_tmp,
					       type_tmp, &value_tmp);
				    avl_search_node(*symtab, cons_name_tmp,
						    &SearchSymbol);
				    stack_data->symtab_ptr = SearchSymbol;
				    if (SearchSymbol == NULL) {
					error(ERROR_SYNTAX);
					syn_free_stack(&stack, stack_data);
					return NULL;
				    }
				    switch (type_tmp2) {
					case TOKEN_EQUAL:
					    ta_insert_last(TA_EQUAL,
							   &operand1,
							   &operand2,
							   &SearchSymbol,
							   ta);
					    break;
					case TOKEN_NOT_EQUAL:
					    ta_insert_last(TA_NOT_EQUAL,
							   &operand1,
							   &operand2,
							   &SearchSymbol,
							   ta);
					    break;
					case TOKEN_LESS:
					    ta_insert_last(TA_LESS,
							   &operand1,
							   &operand2,
							   &SearchSymbol,
							   ta);
					    break;
					case TOKEN_LESS_EQUAL:
					    ta_insert_last(TA_LESS_EQUAL,
							   &operand1,
							   &operand2,
							   &SearchSymbol,
							   ta);
					    break;
					case TOKEN_MORE:
					    ta_insert_last(TA_MORE,
							   &operand1,
							   &operand2,
							   &SearchSymbol,
							   ta);
					    break;
					case TOKEN_MORE_EQUAL:
					    ta_insert_last(TA_MORE_EQUAL,
							   &operand1,
							   &operand2,
							   &SearchSymbol,
							   ta);
					    break;
				    }
				    stack_push(&stack,
					       (void *) &stack_data);
				} else {
				    error(ERROR_SYNTAX);
				    syn_free_stack(&stack, stack_data);
				    return NULL;
				}
				break;
			    case TOKEN_MINUS:
				str_free(&(stack_data)->stoken.lexeme);
				free(stack_data);
				stack_pop(&stack);
				stack_top(&stack, (void *) &stack_data);
				if (stack_data->stoken.token_type ==
				    TOKEN_SYNEXP_E) {
				    operand1 = stack_data->symtab_ptr;
				    cons_name_tmp =
					syn_next_cons_name(cons_counter);
				    switch (operand1->type) {
					case TOKEN_INT:
					    if (operand2->type ==
						TOKEN_INT) {
						value_tmp.i = 0;
						type_tmp = SYMBOL_INT;
					    } else if (operand2->type ==
						       TOKEN_DOUBLE) {
						value_tmp.d = 0;
						type_tmp = SYMBOL_DOUBLE;
					    } else {
						error(ERROR_SEMANTIC);
						free(cons_name_tmp);
						syn_free_stack(&stack,
							       stack_data);
						return NULL;
					    }
					    break;
					case TOKEN_DOUBLE:
					    if (operand2->type !=
						TOKEN_STRING) {
						value_tmp.d = 0;
						type_tmp = SYMBOL_DOUBLE;
					    } else {
						error(ERROR_SEMANTIC);
						free(cons_name_tmp);
						syn_free_stack(&stack,
							       stack_data);
						return NULL;
					    }
					    break;
					case TOKEN_STRING:
					    error(ERROR_SEMANTIC);
					    free(cons_name_tmp);
					    syn_free_stack(&stack,
							   stack_data);
					    return NULL;
					    break;
					default:
					    break;
				    }
				    stack_pop(&stack);
				    stack_data->stoken.token_type =
					TOKEN_SYNEXP_E;
				    avl_insert(symtab, cons_name_tmp,
					       type_tmp, &value_tmp);
				    avl_search_node(*symtab, cons_name_tmp,
						    &SearchSymbol);
				    stack_data->symtab_ptr = SearchSymbol;
				    if (SearchSymbol == NULL) {
					error(ERROR_SYNTAX);
					syn_free_stack(&stack, stack_data);
					return NULL;
				    }
				    ta_insert_last(TA_SUB, &operand1,
						   &operand2,
						   &SearchSymbol, ta);
				    stack_push(&stack,
					       (void *) &stack_data);
				} else {
				    error(ERROR_SYNTAX);
				    syn_free_stack(&stack, stack_data);
				    return NULL;
				}
				break;
			    case TOKEN_MULTIPLY:
				str_free(&(stack_data)->stoken.lexeme);
				free(stack_data);
				stack_pop(&stack);
				stack_top(&stack, (void *) &stack_data);
				if (stack_data->stoken.token_type ==
				    TOKEN_SYNEXP_E) {
				    operand1 = stack_data->symtab_ptr;
				    cons_name_tmp =
					syn_next_cons_name(cons_counter);
				    switch (operand1->type) {
					case TOKEN_INT:
					    if (operand2->type ==
						TOKEN_INT) {
						value_tmp.i = 0;
						type_tmp = SYMBOL_INT;
					    } else if (operand2->type ==
						       TOKEN_DOUBLE) {
						value_tmp.d = 0;
						type_tmp = SYMBOL_DOUBLE;
					    } else {
						error(ERROR_SEMANTIC);
						free(cons_name_tmp);
						syn_free_stack(&stack,
							       stack_data);
						return NULL;
					    }
					    break;
					case TOKEN_DOUBLE:
					    if (operand2->type !=
						TOKEN_STRING) {
						value_tmp.d = 0;
						type_tmp = SYMBOL_DOUBLE;
					    } else {
						error(ERROR_SEMANTIC);
						free(cons_name_tmp);
						syn_free_stack(&stack,
							       stack_data);
						return NULL;
					    }
					    break;
					case TOKEN_STRING:
					    error(ERROR_SEMANTIC);
					    free(cons_name_tmp);
					    syn_free_stack(&stack,
							   stack_data);
					    return NULL;
					default:
					    break;
				    }
				    stack_pop(&stack);
				    stack_data->stoken.token_type =
					TOKEN_SYNEXP_E;
				    avl_insert(symtab, cons_name_tmp,
					       type_tmp, &value_tmp);
				    avl_search_node(*symtab, cons_name_tmp,
						    &SearchSymbol);
				    stack_data->symtab_ptr = SearchSymbol;
				    if (SearchSymbol == NULL) {	// LADENI
					error(ERROR_SYNTAX);
					syn_free_stack(&stack, stack_data);
					return NULL;
				    }
				    ta_insert_last(TA_MUL, &operand1,
						   &operand2,
						   &SearchSymbol, ta);
				    stack_push(&stack,
					       (void *) &stack_data);
				} else {
				    error(ERROR_SYNTAX);
				    syn_free_stack(&stack, stack_data);
				    return NULL;
				}
				break;
			    case TOKEN_OVER:
				str_free(&(stack_data)->stoken.lexeme);
				free(stack_data);
				stack_pop(&stack);
				stack_top(&stack, (void *) &stack_data);
				if (stack_data->stoken.token_type ==
				    TOKEN_SYNEXP_E) {
				    operand1 = stack_data->symtab_ptr;
				    cons_name_tmp =
					syn_next_cons_name(cons_counter);
				    switch (operand1->type) {
					case TOKEN_INT:
					case TOKEN_DOUBLE:
					    if (operand2->type !=
						TOKEN_STRING) {
						value_tmp.d = 0;
						type_tmp = SYMBOL_DOUBLE;
					    } else {
						error(ERROR_SEMANTIC);
						free(cons_name_tmp);
						syn_free_stack(&stack,
							       stack_data);
						return NULL;
					    }
					    break;
					case TOKEN_STRING:
					    error(ERROR_SEMANTIC);
					    free(cons_name_tmp);
					    syn_free_stack(&stack,
							   stack_data);
					    return NULL;
					    break;
					default:
					    break;
				    }
				    stack_pop(&stack);
				    stack_data->stoken.token_type =
					TOKEN_SYNEXP_E;
				    avl_insert(symtab, cons_name_tmp,
					       type_tmp, &value_tmp);
				    avl_search_node(*symtab, cons_name_tmp,
						    &SearchSymbol);
				    if (get_error()) {
					error(ERROR_SYNTAX);
					syn_free_stack(&stack, stack_data);
					return NULL;
				    }
				    stack_data->symtab_ptr = SearchSymbol;
				    ta_insert_last(TA_OVER, &operand1,
						   &operand2,
						   &SearchSymbol, ta);
				    stack_push(&stack,
					       (void *) &stack_data);
				} else {
				    error(ERROR_SYNTAX);
				    syn_free_stack(&stack, stack_data);
				    return NULL;
				}
				break;
			    case TOKEN_PLUS:
				str_free(&(stack_data)->stoken.lexeme);
				free(stack_data);
				stack_pop(&stack);
				stack_top(&stack, (void *) &stack_data);
				if (stack_data->stoken.token_type ==
				    TOKEN_SYNEXP_E) {
				    operand1 = stack_data->symtab_ptr;
				    cons_name_tmp =
					syn_next_cons_name(cons_counter);
				    switch (operand1->type) {
					case TOKEN_INT:
					    if (operand2->type ==
						TOKEN_INT) {
						value_tmp.i = 0;
						type_tmp = SYMBOL_INT;
					    } else if (operand2->type ==
						       TOKEN_DOUBLE) {
						value_tmp.d = 0;
						type_tmp = SYMBOL_DOUBLE;
					    } else {
						error(ERROR_SEMANTIC);
						free(cons_name_tmp);
						syn_free_stack(&stack,
							       stack_data);
						return NULL;
					    }
					    break;
					case TOKEN_DOUBLE:
					    if (operand2->type !=
						TOKEN_STRING) {
						value_tmp.d = 0;
						type_tmp = SYMBOL_DOUBLE;
					    } else {
						error(ERROR_SEMANTIC);
						free(cons_name_tmp);
						syn_free_stack(&stack,
							       stack_data);
						return NULL;
					    }
					    break;
					case TOKEN_STRING:
					    if (operand2->type ==
						TOKEN_STRING) {
						str_init(&value_tmp.s);
						type_tmp = SYMBOL_STRING;
					    } else {
						error(ERROR_SEMANTIC);
						free(cons_name_tmp);
						syn_free_stack(&stack,
							       stack_data);
						return NULL;
					    }
					    break;
					default:
					    break;
				    }
				    stack_pop(&stack);
				    stack_data->stoken.token_type =
					TOKEN_SYNEXP_E;
				    avl_insert(symtab, cons_name_tmp,
					       type_tmp, &value_tmp);
				    avl_search_node(*symtab, cons_name_tmp,
						    &SearchSymbol);
				    if (get_error()) {
					error(ERROR_SYNTAX);
					syn_free_stack(&stack, stack_data);
					return NULL;
				    }
				    stack_data->symtab_ptr = SearchSymbol;
				    ta_insert_last(TA_ADD, &operand1,
						   &operand2,
						   &SearchSymbol, ta);
				    stack_push(&stack,
					       (void *) &stack_data);
				} else {
				    error(ERROR_SYNTAX);
				    syn_free_stack(&stack, stack_data);
				    return NULL;
				}
				break;
			    default:
				error(ERROR_SYNTAX);
				syn_free_stack(&stack, stack_data);
				return NULL;
				break;
			}
			break;
		    default:
			error(ERROR_SYNTAX);
			syn_free_stack(&stack, stack_data);
			return NULL;
			break;
		}
	    case P_Q:		// end of do-while
		break;
	    case P_E:		// continue in default
	    default:		// Unexpected Error, this status will not come. Maybe replace by case P_Q.
		error(ERROR_SYNTAX);
		syn_free_stack(&stack, stack_data);
		return NULL;
		break;
	}
    } while (token->token_type != TOKEN_DOLAR
	     || stack_data->stoken.token_type != TOKEN_DOLAR);

    stack_top(&stack, (void *) &stack_data);
    if (stack_data->symtab_ptr == NULL) {
	error(ERROR_SYNTAX);
	token_free(&(stack_data)->stoken);
	free(stack_data);
	stack_pop(&stack);	// dealloc $ in stack
	token_free(token);	// dealloc $ in buffer
	return NULL;
    }
    Result = stack_data->symtab_ptr;
    free(stack_data);
    stack_pop(&stack);		// dealloc last exp
    stack_top(&stack, (void *) &stack_data);
    token_free(&(stack_data)->stoken);
    free(stack_data);
    stack_pop(&stack);		// dealloc $ in stack
    token_free(token);		// dealloc $ in buffer
    lex(stream, token);		// token for syntax.c
    if (get_error()) {
	return NULL;
    }
    return Result;
}
