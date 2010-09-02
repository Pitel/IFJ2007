/*
 * Implementace interpretu imperativniho jazyka IFJ07
 *
 * Autori:
 *  xfilom00 -- Filo Martin
 *  xkalab00 -- Kalab Jan
 *  xkobli00 -- Koblizek Ondrej
 *  xnovyo01 -- Novy Ondrej
 */


#include<stdbool.h>
#include <string.h>

#include "lex.h"

#ifndef AVL_H
#define AVL_H


typedef union {
	int i;
	double d;
	Tstr s;
} Tdata_union;

enum symbol_types {
	SYMBOL_INT = 2,
	SYMBOL_DOUBLE,
	SYMBOL_STRING,
};

typedef struct Tnode {
	int balance;
	char *id;
	size_t type;
	Tdata_union value;
	struct Tnode *lptr;
	struct Tnode *rptr;
} *Tnode_ptr;

extern void avl_init(Tnode_ptr *root);
extern void avl_search_node(Tnode_ptr root, char *id, Tnode_ptr *found_node);
extern void avl_insert(Tnode_ptr *root, char *id, size_t type, Tdata_union *value);
extern void avl_destroy_tree(Tnode_ptr *root);


#endif
