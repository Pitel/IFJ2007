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

#include "syntax.h"
#include "lex.h"
#include "synexp.h"
#include "avl.h"
#include "error.h"
#include "threeadr.h"
#include "interpreter.h"


/**
 * Returns 1 and get next token if given token type is expected token type, otherwise returns 0.
 *
 * @param type Expected token type
 * @param stream Stream for loading next token
 * @param token Current token
 */
short syn_accept(int type, FILE *stream, Tlex_token *token)
{
    assert(stream != NULL);
    assert(token != NULL);

    if (token->token_type == type) {
	token_free(token);
	lex(stream, token);
	if (get_error()) {
	    return 0;
	}
	return 1;
    }
    return 0;
}

/**
 * Returns 1 if syn_accept() accepts token (and gets new token), otherwise returns 0 and yell error.
 *
 * @param type Expected token type
 * @param stream Stream for loading next token
 * @param token Current token
 */
short syn_expect(int type, FILE *stream, Tlex_token *token)
{
    assert(stream != NULL);
    assert(token != NULL);

    if (syn_accept(type, stream, token)) {
	return 1;
    } else if (get_error()) {
	return 0;
    }
    error(ERROR_SYNTAX);
    return 0;
}

/**
 * Statements handler
 *
 * @param stream Stream for loading next token
 * @param token Current token
 * @param once Set to 0, if you don't care how many statement should be handled in a row, anything else will accept only one statement
 */
void syn_statement(FILE *stream, Tlex_token *token, short once,
		   Tnode_ptr *symtab, size_t *cons_counter,
		   Tthree_adr **ta, unsigned int *label);

/**
 * Variables declarations handler
 *
 * @param stream Stream for loading next token
 * @param token Current token
 */
void syn_declaration(FILE *stream, Tlex_token *token, Tnode_ptr *symtab,
		     size_t *cons_counter)
{
    assert(stream != NULL);
    assert(token != NULL);
    assert(symtab != NULL);
    assert(cons_counter != NULL);

    size_t type_tmp = token->token_type;
    Tdata_union value_tmp;
    Tnode_ptr variable = NULL;	// pomocna promenna pro hledani uzlu
    if (syn_accept(TOKEN_KW_INT, stream, token)	// int
	|| syn_accept(TOKEN_KW_DOUBLE, stream, token)	// double
	|| syn_accept(TOKEN_KW_STRING, stream, token)) {	// string
	if (get_error()) {
	    return;
	}
	Tstr symbol_tmp;
	str_init(&symbol_tmp);
	switch (type_tmp) {
	    case TOKEN_KW_INT:
		value_tmp.i = 0;
		type_tmp = SYMBOL_INT;
		break;
	    case TOKEN_KW_DOUBLE:
		value_tmp.d = 0;
		type_tmp = SYMBOL_DOUBLE;
		break;
	    case TOKEN_KW_STRING:
		str_init(&value_tmp.s);
		str_strcatChA(&value_tmp.s, "");
		type_tmp = SYMBOL_STRING;
		break;
	    default:
		error(ERROR_SYNTAX);
		return;
		break;
	}
	str_strcatChA(&symbol_tmp, token->lexeme.data);
	avl_search_node(*symtab, str_c_str(&token->lexeme), &variable);	// pokusim se jiz deklarovanou promenou
	if (variable != NULL) {
	    error(ERROR_SEMANTIC);
	    str_free(&symbol_tmp);
	    return;
	}
	if (syn_expect(TOKEN_SYMBOL, stream, token)) {	// id
	    if (syn_expect(TOKEN_SEMICOLON, stream, token)) {	// ;
		avl_insert(symtab, symbol_tmp.data, type_tmp, &value_tmp);
		syn_declaration(stream, token, symtab, cons_counter);	// next
		if (get_error()) {
		    return;
		}
	    } else {
		error(ERROR_SYNTAX);
		str_free(&symbol_tmp);
		return;
	    }
	} else {
	    error(ERROR_SYNTAX);
	    str_free(&symbol_tmp);
	    return;
	}
    }
}

void syn_statement(FILE *stream, Tlex_token *token, short once,
		   Tnode_ptr *symtab, size_t *cons_counter,
		   Tthree_adr **ta, unsigned int *label)
{
    assert(stream != NULL);
    assert(token != NULL);
    assert(symtab != NULL);
    assert(cons_counter != NULL);

    Tnode_ptr SynExp_Result = NULL;
    Tnode_ptr variable = NULL;	// pomocna promenna pro hledani uzlu
    if (token->token_type == TOKEN_EOF) {
	error(ERROR_SYNTAX);
	ta_dispose_all(ta);
	return;
    }
    avl_search_node(*symtab, str_c_str(&token->lexeme), &variable);	// pokusim se najit promennou
    if (syn_accept(TOKEN_SYMBOL, stream, token)) {	// id
	if (variable == NULL) {
	    error(ERROR_SEMANTIC);
	    ta_dispose_all(ta);
	    return;
	}
	syn_expect(TOKEN_ASSIGN_OP, stream, token);	// =
	if (get_error()) {
	    ta_dispose_all(ta);
	    return;
	}
	syn_expect(TOKEN_DOLAR, stream, token);	// $
	if (get_error()) {
	    ta_dispose_all(ta);
	    return;
	}
	SynExp_Result =
	    syntax_synexp(stream, token, symtab, cons_counter, ta);
	if (get_error()) {
	    ta_dispose_all(ta);
	    return;
	}
	syn_expect(TOKEN_SEMICOLON, stream, token);	// ;
	if (get_error()) {
	    ta_dispose_all(ta);
	    return;
	}
	assert(variable);
	assert(SynExp_Result);
	if (variable != NULL) {	// kdyz jsem ho nasel
	    if (SynExp_Result->type == variable->type) {
		ta_insert_last(TA_ASSIGN, &SynExp_Result, NULL, &variable,
			       ta);
		if (get_error()) {
		    ta_dispose_all(ta);
		    return;
		}
	    } else if (SynExp_Result->type == TOKEN_INT
		       && variable->type == TOKEN_DOUBLE) {
		ta_insert_last(TA_ASSIGN, &SynExp_Result, NULL, &variable,
			       ta);
		if (get_error()) {
		    ta_dispose_all(ta);
		    return;
		}
	    } else {
		error(ERROR_SEMANTIC);
		ta_dispose_all(ta);
		return;
	    }
	}
	if (once == 0) {
	    syn_statement(stream, token, 0, symtab, cons_counter, ta,
			  label);
	}
    } else if (syn_accept(TOKEN_KW_READ, stream, token)) {	// read
	if (get_error()) {
	    return;
	}
	avl_search_node(*symtab, str_c_str(&token->lexeme), &variable);	// pokusim se najit uzel
	if (variable == NULL) {
	    error(ERROR_SEMANTIC);
	    ta_dispose_all(ta);
	    return;
	}
	syn_expect(TOKEN_SYMBOL, stream, token);	// id
	if (get_error()) {
	    ta_dispose_all(ta);
	    return;
	}
	if (variable != NULL) {	// kdyz jsem ho nasel
	    ta_insert_last(TA_READ, NULL, NULL, &variable, ta);
	}
	variable = NULL;	// a vynulluj pomocnou promenou
	while (syn_accept(TOKEN_COMMA, stream, token)) {	// ,
	    avl_search_node(*symtab, str_c_str(&token->lexeme), &variable);	// pokusim se najit uzel
	    if (variable == NULL) {
		error(ERROR_SEMANTIC);
		ta_dispose_all(ta);
		return;
	    }
	    syn_expect(TOKEN_SYMBOL, stream, token);	// id
	    if (get_error()) {
		ta_dispose_all(ta);
		return;
	    }
	    if (variable != NULL) {	// kdyz jsem ho nasel
		ta_insert_last(TA_READ, NULL, NULL, &variable, ta);
	    }
	    variable = NULL;	// a vynulluj pomocnou promenou
	}
	syn_expect(TOKEN_SEMICOLON, stream, token);	// ;
	if (get_error()) {
	    ta_dispose_all(ta);
	    return;
	}
	if (once == 0) {
	    syn_statement(stream, token, 0, symtab, cons_counter, ta,
			  label);
	}
    } else if (syn_accept(TOKEN_KW_WRITE, stream, token)) {	// write
	if (get_error()) {
	    return;
	}
	syn_expect(TOKEN_DOLAR, stream, token);	// $
	if (get_error()) {
	    return;
	}
	SynExp_Result =
	    syntax_synexp(stream, token, symtab, cons_counter, ta);
	if (get_error()) {
	    ta_dispose_all(ta);
	    return;
	}
	ta_insert_last(TA_WRITE, &SynExp_Result, NULL, NULL, ta);
	if (get_error()) {
	    ta_dispose_all(ta);
	    return;
	}
	while (syn_accept(TOKEN_COMMA, stream, token)) {	// ,
	    syn_expect(TOKEN_DOLAR, stream, token);	// $
	    if (get_error()) {
		return;
	    }
	    SynExp_Result =
		syntax_synexp(stream, token, symtab, cons_counter, ta);
	    if (get_error()) {
		ta_dispose_all(ta);
		return;
	    }
	    if (get_error()) {
		return;
	    }
	    ta_insert_last(TA_WRITE, &SynExp_Result, NULL, NULL, ta);
	    if (get_error()) {
		return;
	    }
	}
	syn_expect(TOKEN_SEMICOLON, stream, token);	// ;
	if (get_error()) {
	    ta_dispose_all(ta);
	    return;
	}
	if (once == 0) {
	    syn_statement(stream, token, 0, symtab, cons_counter, ta,
			  label);
	}
    } else if (syn_accept(TOKEN_KW_SORT, stream, token)) {	// sort
	if (get_error()) {
	    return;
	}
	avl_search_node(*symtab, str_c_str(&token->lexeme), &variable);	// pokusim se najit uzel
	if (variable == NULL) {
	    error(ERROR_SEMANTIC);
	    ta_dispose_all(ta);
	    return;
	}
	syn_expect(TOKEN_SYMBOL, stream, token);	// id
	if (get_error()) {
	    ta_dispose_all(ta);
	    return;
	}
	syn_expect(TOKEN_SEMICOLON, stream, token);	// ;
	if (get_error()) {
	    ta_dispose_all(ta);
	    return;
	}
	if (variable != NULL && variable->type == TOKEN_STRING) {	// kdyz jsem ho nasel a sedi datove typy
	    ta_insert_last(TA_SORT, NULL, NULL, &variable, ta);
	} else {
	    error(ERROR_SEMANTIC);
	    ta_dispose_all(ta);
	    return;
	}
	if (once == 0) {
	    syn_statement(stream, token, 0, symtab, cons_counter, ta,
			  label);
	}
    } else if (syn_accept(TOKEN_KW_IF, stream, token)) {	// if
	assert(*label > 0);
	if (get_error()) {
	    return;
	}
	unsigned int else_label = *label;
	unsigned int endif_label = ++(*label);
	(*label)++;
	syn_expect(TOKEN_DOLAR, stream, token);	// $
	if (get_error()) {
	    ta_dispose_all(ta);
	    return;
	}
	SynExp_Result =
	    syntax_synexp(stream, token, symtab, cons_counter, ta);
	if (get_error()) {
	    ta_dispose_all(ta);
	    return;
	}
	// tady je v SynExp_Result->value.i -1
	ta_define_jump(TA_FALSEJUMP, else_label, &SynExp_Result, ta);
	syn_statement(stream, token, 1, symtab, cons_counter, ta, label);	// only once
	if (get_error()) {
	    ta_dispose_all(ta);
	    return;
	}
	ta_define_jump(TA_JUMP, endif_label, NULL, ta);
	syn_expect(TOKEN_KW_ELSE, stream, token);	// else
	if (get_error()) {
	    ta_dispose_all(ta);
	    return;
	}

	ta_define_jump(TA_LAB, else_label, NULL, ta);

	syn_statement(stream, token, 1, symtab, cons_counter, ta, label);	// one statement
	if (get_error()) {
	    return;
	}
	ta_define_jump(TA_LAB, endif_label, NULL, ta);
	if (once == 0) {
	    syn_statement(stream, token, 0, symtab, cons_counter, ta,
			  label);
	}
    } else if (syn_accept(TOKEN_KW_WHILE, stream, token)) {	// while
	if (get_error()) {
	    return;
	}
	unsigned int while_start = *label;
	unsigned int while_end = ++(*label);
	(*label)++;
	syn_expect(TOKEN_DOLAR, stream, token);	// $
	if (get_error()) {
	    return;
	}
	ta_define_jump(TA_LAB, while_start, NULL, ta);
	SynExp_Result =
	    syntax_synexp(stream, token, symtab, cons_counter, ta);
	if (get_error()) {
	    ta_dispose_all(ta);
	    return;
	}

	ta_define_jump(TA_FALSEJUMP, while_end, &SynExp_Result, ta);
	syn_statement(stream, token, 1, symtab, cons_counter, ta, label);	// statement
	if (get_error()) {
	    ta_dispose_all(ta);
	    return;
	}
	ta_define_jump(TA_JUMP, while_start, NULL, ta);
	ta_define_jump(TA_LAB, while_end, NULL, ta);
	if (once == 0) {
	    syn_statement(stream, token, 0, symtab, cons_counter, ta,
			  label);
	}
    } else if (syn_accept(TOKEN_BLOCK_BEGIN, stream, token)) {	// {
	if (get_error()) {
	    return;
	}
	syn_statement(stream, token, 0, symtab, cons_counter, ta, label);	// statement
	syn_expect(TOKEN_BLOCK_END, stream, token);	// }
	if (get_error()) {
	    return;
	}
	if (once == 0) {
	    syn_statement(stream, token, 0, symtab, cons_counter, ta, label);	// statement
	}
    }
}

/**
 * Main program flow handler
 *
 * @param stream Stream for loading next token
 */
void syntax(FILE *stream, Tnode_ptr *symtab)
{
    assert(stream != NULL);
    assert(symtab != NULL);

    Tthree_adr *ta;
    ta_init(&ta);

    Tlex_token token;
    lex(stream, &token);	// first token is ready
    if (get_error()) {
	return;
    }
    size_t cons_counter = 0;

    syn_declaration(stream, &token, symtab, &cons_counter);	// type id;
    if (get_error()) {
	ta_dispose_all(&ta);
	token_free(&token);
	return;
    }
    unsigned int label = 1;
    if (syn_accept(TOKEN_BLOCK_BEGIN, stream, &token)) {	// {
	if (get_error()) {
	    token_free(&token);
	    return;
	}
	syn_statement(stream, &token, 0, symtab, &cons_counter, &ta, &label);	// statement(s)
	syn_expect(TOKEN_BLOCK_END, stream, &token);	// }
	if (get_error()) {
	    token_free(&token);
	    return;
	}
    } else {
	if (get_error()) {
	    token_free(&token);
	    return;
	}
	error(ERROR_SYNTAX);
    }
    syn_expect(TOKEN_EOF, stream, &token);	// EOF
    if (get_error()) {
	token_free(&token);
	return;
    }
    token_free(&token);

    interpreter(&ta);
}
