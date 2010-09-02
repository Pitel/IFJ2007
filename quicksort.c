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

#include "quicksort.h"

/**
 * Returns median of three items
 *
 * @param *array Pointer to array with items
 * @param size Size of item
 * @param a Index of first item
 * @param b Index of second item
 * @param c Index of third item
 * @param *comparator Pointer at comparing function
 */
inline static size_t qs_med3(void *array, size_t size, size_t a, size_t b,
			     size_t c,
			     int (*comparator) (const void *, const void *))
{
    assert(array != NULL);
    assert(comparator != NULL);
    if ((*comparator) ((char *) array + a * size, (char *) array + b * size) <= 0) {
	if ((*comparator) ((char *) array + b * size, (char *) array + c * size) <= 0) {
	    return b;
	} else {
	    return c;
	}
    } else {
	if ((*comparator) ((char *) array + a * size, (char *) array + c * size) <= 0) {
	    return a;
	} else {
	    return c;
	}
    }
}

/**
 * Returns pivot. If array has 3 or less elements, returns the middle one, if it has more than 3, returns median.
 *
 * @param *array Pointer to array with items
 * @param elements Number of items in array
 * @param size Size of item
 * @param *comparator Pointer at comparing function
 */
inline static size_t qs_pivot(void *array, size_t elements, size_t size,
			      int (*comparator) (const void *, const void *))
{
    assert(array != NULL);
    assert(comparator != NULL);

    if (elements < 4) {
	return elements / 2;
    } else {
	return qs_med3(array, size, 0, elements / 2, elements - 1, comparator);
    }
}

/**
 * Swaps two items in any array
 *
 * @param *array Pointer to array
 * @param size Size of array item
 * @param a Index of item to swap
 * @param b Index of item to swap
 */
inline static void qs_swap(void *array, size_t size, size_t a, size_t b)
{
    assert(array != NULL);

    char *bytes = array;
    for (size_t i = 0; i < size; i++) {
	char temp = bytes[a * size + i];
	bytes[a * size + i] = bytes[b * size + i];
	bytes[b * size + i] = temp;
    }
}

/**
 * Quicksort… is sorting quickly.
 *
 * @param *array Array to sort
 * @param elements Number of items in array
 * @param size Size of array items
 * @param *comparator Pointer at comparing function
 */
void quicksort(void *array, size_t elements, size_t size,
	       int (*comparator) (const void *, const void *))
{
    assert(array != NULL);
    assert(comparator != NULL);

    if (elements <= 1) {
	return;
    }
    size_t pivot = qs_pivot(array, elements, size, comparator);

    // partition
    qs_swap(array, size, pivot, elements - 1);	// move middle to the end
    size_t left = 0, right = elements - 1;
    while (left < right) {
	while (left < right
	       && (*comparator) ((char *) array + left * size,
				 (char *) array + (elements - 1) * size) <= 0) {
	    left++;
	}
	while (left < right
	       && (*comparator) ((char *) array + right * size,
				 (char *) array + (elements - 1) * size) >= 0) {
	    right--;
	}
	if (left < right) {
	    qs_swap(array, size, left, right);
	}
    }
    qs_swap(array, size, left, elements - 1);	// move middle back

    // recursion call on left and right side
    quicksort(array, left, size, comparator);
    quicksort((void *) ((char *) array + right * size), elements - right,
	      size, comparator);
}
