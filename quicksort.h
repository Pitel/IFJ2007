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

#ifndef QUICKSORT_H
#define QUICKSORT_H


void quicksort(void *array, size_t elements, size_t size, int (*comparator)(const void *, const void *));


#endif
