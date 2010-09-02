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
#include <stdio.h>
#include <stdlib.h>

#include "avl.h"
#include "error.h"
#include "lex.h"

/**
 * Init AVL
 * Inits *root at NULL.
 *
 * @param *root is a pointer to root node of the AVL tree, for return node
 */
void avl_init(Tnode_ptr *root)
{
    assert(root != NULL);

    *root = NULL;
} // avl_init

/**
 * Search Node
 *
 * @param root is a pointer to node of the AVL tree
 * @param K unique key of a finding node
 * @param found_node is a pointer to node of the AVL Tree, for return node 
 */
void avl_search_node(Tnode_ptr root, char *id, Tnode_ptr *found_node)
{
    assert(id != NULL);
    assert(found_node != NULL);

    if (root == NULL) {
	*found_node = NULL;
    } else {
	if (strcmp(root->id, id) != 0) {
	    if (strcmp(root->id, id) > 0) {
		avl_search_node(root->lptr, id, found_node);
	    } else {
		avl_search_node(root->rptr, id, found_node);
	    }
	} else {
	    *found_node = root;
	}
    }
} // avl_search_node

/**
 * Left Rotation
 * Call it also like simple LL rotation
 * Subfunction for Insert AVL.
 * Reconfiguration nodes into AVL tree after new node is inserted around the critical node.
 *
 * @param *parent_crit is a pointer to parent critical node of struct Tnode_ptr
 * @param *crit is a pointer to critical node of struct Tnode_ptr
 */
void avl_rot_ll(Tnode_ptr *parent_crit, Tnode_ptr *crit)
{
    assert(parent_crit != NULL);
    assert(crit != NULL);

    Tnode_ptr A;
    Tnode_ptr B;
    /* Reconfiguration of nodes */
    A = *crit;
    B = A->lptr;
    A->lptr = B->rptr;
    B->rptr = A;
    if (*parent_crit != NULL) {	// Connection to parent critical node
	if ((*parent_crit)->lptr == *crit)
	    (*parent_crit)->lptr = B;
	else
	    (*parent_crit)->rptr = B;
    } else {			// Root handling
	*crit = B;
    }
    /* Balance correction */
    A->balance = 0;
    B->balance = 0;
} // avl_rot_ll

/**
 * Right Rotation
 * Call it also like simple RR rotation
 * Subfunction for Insert AVL.
 * Reconfiguration nodes into AVL tree after new node is inserted around the critical node.
 *
 * @param *parent_crit is a pointer to parent critical node of struct Tnode_ptr
 * @param *crit is a pointer to critical node of struct Tnode_ptr
 */
void avl_rot_rr(Tnode_ptr *parent_crit, Tnode_ptr *crit)
{
    assert(parent_crit != NULL);
    assert(crit != NULL);

    Tnode_ptr A;
    Tnode_ptr B;
    /* Reconfiguration of nodes */
    A = *crit;
    B = A->rptr;
    A->rptr = B->lptr;
    B->lptr = A;
    if (*parent_crit != NULL) {	// Connection to parent critical node
	if ((*parent_crit)->lptr == *crit)
	    (*parent_crit)->lptr = B;
	else
	    (*parent_crit)->rptr = B;
    }
    /* Balance correction */
    A->balance = 0;
    B->balance = 0;
} // avl_rot_rr

/**
 * Double Left Rotation
 * Call it also like DLR rotation
 * Subfunction for Insert AVL.
 * Reconfiguration nodes into AVL tree after new node is inserted around the critical node.
 *
 * @param *parent_crit is a pointer to parent critical node of struct Tnode_ptr
 * @param *crit is a pointer to critical node of struct Tnode_ptr
 */
void avl_rot_dlr(Tnode_ptr *parent_crit, Tnode_ptr *crit)
{
    assert(parent_crit != NULL);
    assert(crit != NULL);

    Tnode_ptr A;
    Tnode_ptr B;
    Tnode_ptr C;
    A = *crit;
    B = A->lptr;
    C = B->rptr;
    /* Reconfiguration of nodes */
    B->rptr = C->lptr;
    A->lptr = C->rptr;
    C->lptr = B;
    C->rptr = A;
    if (*parent_crit != NULL) {	// Connection to parent critical node
	if ((*parent_crit)->lptr == *crit)
	    (*parent_crit)->lptr = C;
	else
	    (*parent_crit)->rptr = C;
    }
    /* Balance correction */
    if (C->balance == 1) {
	B->balance = -1;
	A->balance = 0;
    } else {
	if (C->balance == -1) {
	    B->balance = 0;
	    A->balance = 1;
	} else {
	    B->balance = 0;
	    A->balance = 0;
	}
    }
    C->balance = 0;
} // avl_rot_dlr

/**
 * Double Right Rotation
 * Call it also like DRL rotation
 * Subfunction for Insert AVL.
 * Reconfiguration nodes into AVL tree after new node is inserted around the critical node.
 *
 * @param *parent_crit is a pointer to parent critical node of struct Tnode_ptr
 * @param *crit is a pointer to critical node of struct Tnode_ptr
 */
void avl_rot_drl(Tnode_ptr *parent_crit, Tnode_ptr *crit)
{
    assert(parent_crit != NULL);
    assert(crit != NULL);

    Tnode_ptr A;
    Tnode_ptr B;
    Tnode_ptr C;
    A = *crit;
    B = A->rptr;
    C = B->lptr;
    /* Reconfiguration of nodes */
    B->lptr = C->rptr;
    A->rptr = C->lptr;
    C->rptr = B;
    C->lptr = A;
    if (*parent_crit != NULL) {	// Connection to parent critical node
	if ((*parent_crit)->lptr == *crit)
	    (*parent_crit)->lptr = C;
	else
	    (*parent_crit)->rptr = C;
    }
    /* Balance correction */
    if (C->balance == 1) {
	B->balance = -1;
	A->balance = 0;
    } else {
	if (C->balance == -1) {
	    B->balance = 0;
	    A->balance = 1;
	} else {
	    B->balance = 0;
	    A->balance = 0;
	}
    }
    C->balance = 0;
} // avl_rot_drl

/**
 * Create Node
 * Subfunction for Insert AVL.
 *
 * @param *node is a pointer to new node of struct Tnode_ptr, for return new node
 * @param K unique key to new node
 * @param value data for new node 
 */
void avl_createnode(Tnode_ptr *node, char *id, size_t type,
		    Tdata_union *value)
{
    assert(node != NULL);
    assert(id != NULL);
    assert(value != NULL);

    Tnode_ptr newNode;
    if ((newNode = malloc(sizeof(struct Tnode))) == NULL)
	error(ERROR_NO_MEMORY);
    else {
	newNode->balance = 0;
	newNode->id = id;
	newNode->type = type;
	switch (newNode->type) {
	    case SYMBOL_INT:
		newNode->value.i = value->i;
		break;
	    case SYMBOL_DOUBLE:
		newNode->value.d = value->d;
		break;
	    case SYMBOL_STRING:
		newNode->value.s = value->s;
		break;
	    default:
		break;
	}
	newNode->lptr = NULL;
	newNode->rptr = NULL;
	*node = newNode;
    }
} // avl_createnode

/**
 * Insert AVL
 * Not existed node in tree is inserted into and if is necessary tree is rebalanced.
 * Existed node is updated with new data.
 *
 * @param *root is a pointer to root node of struct Tnode_ptr, for return tree
 * @param K unique key
 * @param value data 
 */
void avl_insert(Tnode_ptr * root, char *id, size_t type,
		Tdata_union * value)
{
    assert(root != NULL);
    assert(id != NULL);
    assert(value != NULL);

    bool repeat, found;
    Tnode_ptr actNode, critNode, parent_crit, parentNode;
    critNode = *root;		// Initialization
    actNode = NULL;
    parent_crit = NULL;
    parentNode = NULL;
    if (*root == NULL) {
	avl_createnode(root, id, type, value);	// Make a balanced root
    } else {			// Initialization of tree path
	repeat = true;
	critNode = *root;
	actNode = critNode;
	parentNode = NULL;
	while (repeat) {
	    if (strcmp(actNode->id, id) == 0) {	// Equality found
		actNode->type = type;	// Rewriting data
		actNode->value = *value;
		found = true;
		repeat = false;
	    } else {		// Equality not found
		if (actNode->balance != 0) {	// Registration of potential critical node
		    parent_crit = parentNode;
		    critNode = actNode;
		}
		if (strcmp(actNode->id, id) > 0) {	// Go left
		    if (actNode->lptr == NULL) {	// Will insert to the left
			found = false;
			repeat = false;
		    } else {	// Shift to the left
			parentNode = actNode;
			actNode = actNode->lptr;
		    }
		} else {	// Go Right
		    if (actNode->rptr == NULL) {	// Will insert to te right
			found = false;
			repeat = false;
		    } else {	// Shift to the right
			parentNode = actNode;
			actNode = actNode->rptr;
		    }
		}		// Else Go Right
	    }			// Else Equality not found
	}			// While
	/* New node insertion */
	if (!found) {
	    repeat = true;
	    actNode = critNode;
	    while (repeat) {
		if (strcmp(actNode->id, id) > 0) {	// Left version
		    actNode->balance = actNode->balance - 1;	// Balance correction
		    if (actNode->lptr == NULL) {	// Connect new balanced node to the actNode->lptr and stop while iteration
			avl_createnode(&actNode->lptr, id, type, value);
			repeat = false;
		    } else {	// Shift to the left
			actNode = actNode->lptr;
		    }
		} else {	// Right version
		    actNode->balance = actNode->balance + 1;	// Balance correction
		    if (actNode->rptr == NULL) {	// Connect new balanced node to the actNode->rptr and stop while iteration
			avl_createnode(&actNode->rptr, id, type, value);
			repeat = false;
		    } else {	// Shift to the right
			actNode = actNode->rptr;
		    }
		}		// Else right version
	    }			// While
	    /* Node reconfiguration with rotation */
	    if (parentNode != NULL) {
		if (critNode->balance == -2) {
		    if (critNode->lptr->balance == -1) {
			if (parent_crit == NULL) {	// Root handling
			    *root = critNode->lptr;
			}
			avl_rot_ll(&parent_crit, &critNode);
		    } else {
			if (parent_crit == NULL) {	// Root handling
			    *root = critNode->lptr->rptr;
			}
			avl_rot_dlr(&parent_crit, &critNode);
		    }
		}
		if (critNode->balance == 2) {
		    if (critNode->rptr->balance == 1) {
			if (parent_crit == NULL) {	// Root handling
			    *root = critNode->rptr;
			}
			avl_rot_rr(&parent_crit, &critNode);
		    } else {
			if (parent_crit == NULL) {	// Root handling
			    *root = critNode->rptr->lptr;
			}
			avl_rot_drl(&parent_crit, &critNode);
		    }
		}
	    }			// If (parentNode != NULL)
	}			// If (!found)        
    }				// Else initialization of tree path
} // avl_insert

/**
 * Destroy Tree
 * Delete whole tree.
 *
 * @param *node is a pointer to root node of struct Tnode_ptr, for return empty node
 */
void avl_destroy_tree(Tnode_ptr *root)
{
    assert(root != NULL);

    if (*root != NULL) {
	avl_destroy_tree(&(*root)->lptr);
	avl_destroy_tree(&(*root)->rptr);
	free((*root)->id);
	if ((*root)->type == SYMBOL_STRING) {
	    str_free(&(*root)->value.s);
	}
	free(*root);
	*root = NULL;
    }
} // avl_destroy_tree
