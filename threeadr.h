/*
 * Implementace interpretu imperativniho jazyka IFJ07
 *
 * Autori:
 *  xfilom00 -- Filo Martin
 *  xkalab00 -- Kalab Jan
 *  xkobli00 -- Koblizek Ondrej
 *  xnovyo01 -- Novy Ondrej
 */


#include <string.h>
#include "avl.h"

#ifndef __THREEADR_H__
#define __THREEADR_H__

 
#define THREEADR_ALLOC 100

enum oper{
    TA_ADD = 0,
    TA_SUB,
    TA_LAB,
    TA_MUL,
    TA_WRITE,
    TA_READ,
    TA_SORT,
    TA_ASSIGN,
    TA_DIV,
    TA_OVER,
    TA_EQUAL,
    TA_NOT_EQUAL,
    TA_LESS,
    TA_LESS_EQUAL,
    TA_MORE,
    TA_MORE_EQUAL,
    TA_JUMP,
    TA_TRUEJUMP,
    TA_FALSEJUMP,
};

typedef struct {
 size_t operation;
 void *oper1;
 void *oper2;
 void *result;
 unsigned int last;
 int jump_label;
} Tthree_adr;

void ta_init(Tthree_adr **three_adr);
void ta_insert_last(size_t operation, Tnode_ptr *oper1, Tnode_ptr *oper2, Tnode_ptr *result, Tthree_adr **three_adr);
void ta_dispose_all(Tthree_adr **three_adr);
void ta_read_position(int position, size_t *operation, Tnode_ptr *oper1, Tnode_ptr *oper2, Tnode_ptr *result, Tthree_adr **three_adr);
int ta_get_last_position(Tthree_adr **three_adr);
void ta_define_jump(size_t operation, int jump_id, Tnode_ptr *oper1, Tthree_adr **three_adr);


#endif
