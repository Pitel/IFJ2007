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

#include "threeadr.h"
#include "error.h"


/**
 * Initializes field of three address code
 * @param three_adr pointer on three address field
 */
void ta_init(Tthree_adr **three_adr)
{
	*three_adr = NULL;
}

/**
 * Inserts code at the end of threeAdr field. If it's necessary allocs memmory.
 * @param operation Type of operation
 * @param oper1	represents operand1
 * @param oper2 represents operand2
 * @param result represent operand result in three address code
 * @param three_adr pointer to three address code
 */
void ta_insert_last(size_t operation, Tnode_ptr *oper1, Tnode_ptr *oper2, Tnode_ptr *result, Tthree_adr **three_adr)
{
	int last = 0, adr_last = 0;  
 
	if(*three_adr == NULL)
		adr_last = 0;
	
	else
	{
		adr_last = three_adr[0]->last;
 
	}
	last = adr_last % THREEADR_ALLOC;

	if(last == 0) //pole je bud prazdne nebo plne, je treba alokovat
	{
		*three_adr = realloc(*three_adr, (adr_last+THREEADR_ALLOC) * sizeof(Tthree_adr));
		if(*three_adr == NULL)
		{
	  		error(ERROR_NO_MEMORY);
	  		return;
	  	}
	}

	(*three_adr)[adr_last].operation = operation;
	if(oper1 != NULL)
		(*three_adr)[adr_last].oper1 = *oper1;
	if(oper2 != NULL)
		(*three_adr)[adr_last].oper2 = *oper2;
	if(result != NULL)
		(*three_adr)[adr_last].result = *result;
	(*three_adr)[adr_last].jump_label = -1; // vsechny nelabel operace maji hodnotu -1
	(*three_adr)[0].last = adr_last+1;
}

/**
 * Free field of three address code
 * @param three_adr pointer to three address code
 */
void ta_dispose_all(Tthree_adr **three_adr)
{
	if(*three_adr == NULL)
		return;
	free(*three_adr);
	*three_adr = NULL;
}

/**
 * Reads three address code on position i in threeAdr array.
 * @param position Position of code in array
 * @param operation Type of operation
 * @param oper1 represents operand1
 * @param oper2 represents operand2
 * @param result represents operand result in three address code
 * @param three_adr pointer to three address code
 */
void ta_read_position(int position, size_t *operation, Tnode_ptr *oper1, Tnode_ptr *oper2, Tnode_ptr *result, Tthree_adr **three_adr)
{
	if(*three_adr == NULL)
	{
		error(ERROR_THREEADR_CODE);
		return;
	}

	if(position > three_adr[0]->last)
	{
		error(ERROR_THREEADR_CODE);
		return;
	}
 
	*operation = (*three_adr)[position].operation;
	*oper1 = (*three_adr)[position].oper1;
	*oper2 = (*three_adr)[position].oper2;
	*result = (*three_adr)[position].result;
}
/**
 * Returns last instuction from three address code field.
 * @param three_adr pointer to three address code
 */
int ta_get_last_position(Tthree_adr **three_adr)
{
	if(*three_adr == NULL)
	{
		error(ERROR_THREEADR_CODE);
		return -1;
	}
	return (*three_adr)[0].last - 1;
}

/*
 * Inserts Label or jump instruction
 * @param operation type of jump operation
 * @param jump_id jump or label identifier
 * @param oper1 logic for conditional jumps
 * @param three_adr three address code array
 */
void ta_define_jump(size_t operation, int jump_id, Tnode_ptr *oper1, Tthree_adr **three_adr)
{
	int insert = 0, last = 0;

	
	if(*three_adr == NULL)
	{
		insert = 0;
	}
	
	insert = (*three_adr)[0].last;
	last = insert % THREEADR_ALLOC;
	if(last == 0)
	{
		*three_adr = realloc(*three_adr, (insert + THREEADR_ALLOC)*sizeof(Tthree_adr));
		if(*three_adr == NULL)
		{
			error(ERROR_NO_MEMORY);
			return;
		}
	}

	(*three_adr)[insert].operation = operation;
	
	if(oper1 != NULL)
		(*three_adr)[insert].oper1 = *oper1;
	else
		(*three_adr)[insert].oper1 = NULL;
	(*three_adr)[insert].oper2 = NULL;
	(*three_adr)[insert].result = NULL;
	(*three_adr)[insert].jump_label = jump_id;
	(*three_adr)[0].last = insert + 1;
	return;
}
