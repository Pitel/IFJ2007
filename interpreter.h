/*
 * Implementace interpretu imperativniho jazyka IFJ07
 *
 * Autori:
 *  xfilom00 -- Filo Martin
 *  xkalab00 -- Kalab Jan
 *  xkobli00 -- Koblizek Ondrej
 *  xnovyo01 -- Novy Ondrej
 */

#ifndef __INTERPRET_H__
#define __INTERPRET_H__


#include "threeadr.h"

#define JUMP_ARRAY_ALLOC 100 //segments for allocation

void interpreter(Tthree_adr **three_adr);
void inter_get_jumps(int **jump_array, Tthree_adr **three_adr);


#endif
