/*
 *  Implementace interpretu imperativniho jazyka IFJ07
 *  
 *  Autori:
 *  xfilom00 -- Filo Martin
 *  xkalab00 -- Kalab Jan
 *  xkobli00 -- Koblizek Ondrej
 *  xnovyo01 -- Novy Ondrej
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "interpreter.h"
#include "error.h"
#include "threeadr.h"
#include "quicksort.h"
#include "str.h"
#include "lex.h"

/*
 * Scans three address code field and mark all jumps into special field
 * @param jumpArray field of jumps
 */
void inter_get_jumps(int **jump_array, Tthree_adr **three_adr)
{
    int i = 0, array_size = 0, last_adr = 0, jump_id = 0;
    int extra_realloc = 0, div_remain = 0;

    assert(*three_adr != NULL);
    last_adr = ta_get_last_position(three_adr);

    *jump_array = realloc(*jump_array, JUMP_ARRAY_ALLOC * sizeof(int));
    if (jump_array == NULL) {
	error(ERROR_NO_MEMORY);
	return;
    }
    array_size = JUMP_ARRAY_ALLOC;
    for (i = 0; i <= last_adr; i++) {
	//nasel navesti
	if ((*three_adr)[i].operation == TA_LAB) {
	    jump_id = (*three_adr)[i].jump_label;
	    if (jump_id >= array_size)	//skocil by za pole, prialokujeme pamet, aby se index pole rovnal jump_label;
	    {
		extra_realloc = jump_id - array_size;
		div_remain = extra_realloc % JUMP_ARRAY_ALLOC;
		div_remain = JUMP_ARRAY_ALLOC - div_remain;
		extra_realloc += div_remain;
		array_size = extra_realloc;
		*jump_array =
		    realloc(*jump_array, array_size * sizeof(int));
		if (*jump_array == NULL) {
		    error(ERROR_NO_MEMORY);
		    return;
		}

	    }
	    (*jump_array)[jump_id] = i;
	}


    }
    return;
}

/*
 * Dispose array with jumps
 */
void inter_dispose_jump_array(int *jump_array)
{
    free(jump_array);
    jump_array = NULL;
}

static int inter_charcmp(const void *p1, const void *p2)
{
    return (int) *(unsigned char *) p1 - (int) *(unsigned char *) p2;
}

/*
 * Reads and do whatever is in three address code field.
 */
void interpreter(Tthree_adr **three_adr)
{
    assert(three_adr != NULL);

    int i = 0, j = 0, last = 0, *jump_array = NULL;
    Tnode_ptr op1, op2, result;
    size_t oper;

    if (*three_adr == NULL)	//jde o prazdnej program, coz je validni
	return;

    last = ta_get_last_position(three_adr);
    if (get_error())
	return;

    //Zanalyzovani skoku a ulozeni navesti do pole
    inter_get_jumps(&jump_array, three_adr);
    if (get_error()) {
	ta_dispose_all(three_adr);
	return;
    }
    //Vlastni interpretace
    for (i = 0; i <= last; i++) {
	ta_read_position(i, &oper, &op1, &op2, &result, three_adr);
	if (get_error())	//chyba pri cteni pozice i
	{
	    error(ERROR_INTERPRETER);
	    return;
	}
	switch (oper) {
	    case TA_ADD:
		if (result->type == TOKEN_INT) {
		    result->value.i = op1->value.i + op2->value.i;
		} else if (result->type == TOKEN_DOUBLE) {
		    if (op1->type == TOKEN_INT) {
			result->value.d = op1->value.i + op2->value.d;
		    } else if (op2->type == TOKEN_INT) {
			result->value.d = op1->value.d + op2->value.i;
		    } else {
			result->value.d = op1->value.d + op2->value.d;
		    }
		} else if (result->type == TOKEN_STRING) {
		    str_strcatStr(&result->value.s, &op1->value.s);
		    str_strcatStr(&result->value.s, &op2->value.s);
		}
		break;
	    case TA_ASSIGN:
		if (result->type == TOKEN_INT && op1->type == TOKEN_INT)
		    result->value.i = op1->value.i;
		else if (result->type == TOKEN_DOUBLE)
		    if (op1->type == TOKEN_INT) {
			result->value.d = op1->value.i;
		    } else {
			result->value.d = op1->value.d;
		} else if (result->type == TOKEN_STRING) {
		    str_free(&result->value.s);
		    str_init(&result->value.s);
		    str_strcatStr(&result->value.s, &op1->value.s);
		} else {
		    error(ERROR_INTERPRETER);
		    ta_dispose_all(three_adr);
		    free(jump_array);
		    return;
		}
		break;
	    case TA_DIV:
		if (op1->type == TOKEN_INT) {
		    if (op2->type == TOKEN_INT) {
			if (op2->value.i == 0) {
			    error(ERROR_INTERPRETER);
			    ta_dispose_all(three_adr);
			    free(jump_array);
			    return;
			} else {
			    result->value.d = op1->value.i / op2->value.i;
			}
		    } else {
			if (op2->value.d == 0.0) {
			    error(ERROR_INTERPRETER);
			    ta_dispose_all(three_adr);
			    free(jump_array);
			    return;
			} else {
			    result->value.d = op1->value.i / op2->value.d;
			}
		    }
		} else {
		    if (op2->type == TOKEN_INT) {
			if (op2->value.i == 0) {
			    error(ERROR_INTERPRETER);
			    ta_dispose_all(three_adr);
			    free(jump_array);
			    return;
			} else {
			    result->value.d = op1->value.d / op2->value.i;
			}
		    } else {
			if (op2->value.d == 0.0) {
			    error(ERROR_INTERPRETER);
			    ta_dispose_all(three_adr);
			    free(jump_array);
			    return;
			} else {
			    result->value.d = op1->value.d / op2->value.d;
			}
		    }
		}
		break;
	    case TA_EQUAL:
		if (op1->type == TOKEN_INT) {
		    if (op2->type == TOKEN_INT) {
			if (op1->value.i == op2->value.i) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    } else {
			if (op1->value.i == op2->value.d) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    }
		} else if (op1->type == TOKEN_DOUBLE) {
		    if (op2->type == TOKEN_DOUBLE) {
			if (op1->value.d == op2->value.d) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    } else {
			if (op1->value.d == op2->value.i) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    }
		} else if (op1->type == TOKEN_STRING) {
		    if (str_strcmpStr(&op1->value.s, &op2->value.s) == 0) {
			result->value.i = 1;
		    } else {
			result->value.i = 0;
		    }
		}
		break;
	    case TA_FALSEJUMP:
		if (op1->value.i == 0)
		    i = jump_array[(*three_adr)[i].jump_label];
		break;
	    case TA_JUMP:	//nepodmineny skok
		i = jump_array[(*three_adr)[i].jump_label];
		break;
	    case TA_LAB:	//na label se nic nedeje
		break;
	    case TA_NOT_EQUAL:
		if (op1->type == TOKEN_INT) {
		    if (op2->type == TOKEN_INT) {
			if (op1->value.i != op2->value.i) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    } else {
			if (op1->value.i != op2->value.d) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    }
		} else if (op1->type == TOKEN_DOUBLE) {
		    if (op2->type == TOKEN_DOUBLE) {
			if (op1->value.d != op2->value.d) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    } else {
			if (op1->value.d != op2->value.i) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    }
		} else if (op1->type == TOKEN_STRING) {
		    if (str_strcmpStr(&op1->value.s, &op2->value.s) != 0) {
			result->value.i = 1;
		    } else {
			result->value.i = 0;
		    }
		}
		break;
	    case TA_LESS:
		if (op1->type == TOKEN_INT) {
		    if (op2->type == TOKEN_INT) {
			if (op1->value.i < op2->value.i) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    } else {
			if (op1->value.i < op2->value.d) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    }
		} else if (op1->type == TOKEN_DOUBLE) {
		    if (op2->type == TOKEN_DOUBLE) {
			if (op1->value.d < op2->value.d) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    } else {
			if (op1->value.d < op2->value.i) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    }
		} else if (op1->type == TOKEN_STRING) {
		    if (str_strcmpStr(&op1->value.s, &op2->value.s) < 0) {
			result->value.i = 1;
		    } else {
			result->value.i = 0;
		    }
		}
		break;
	    case TA_LESS_EQUAL:
		if (op1->type == TOKEN_INT) {
		    if (op2->type == TOKEN_INT) {
			if (op1->value.i <= op2->value.i) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    } else {
			if (op1->value.i <= op2->value.d) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    }
		} else if (op1->type == TOKEN_DOUBLE) {
		    if (op2->type == TOKEN_DOUBLE) {
			if (op1->value.d <= op2->value.d) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    } else {
			if (op1->value.d <= op2->value.i) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    }
		} else if (op1->type == TOKEN_STRING) {
		    if (str_strcmpStr(&op1->value.s, &op2->value.s) <= 0) {
			result->value.i = 1;
		    } else {
			result->value.i = 0;
		    }
		}
		break;
	    case TA_MORE:
		if (op1->type == TOKEN_INT) {
		    if (op2->type == TOKEN_INT) {
			if (op1->value.i > op2->value.i) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    } else {
			if (op1->value.i > op2->value.d) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    }
		} else if (op1->type == TOKEN_DOUBLE) {
		    if (op2->type == TOKEN_DOUBLE) {
			if (op1->value.d > op2->value.d) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    } else {
			if (op1->value.d > op2->value.i) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    }
		} else if (op1->type == TOKEN_STRING) {
		    if (str_strcmpStr(&op1->value.s, &op2->value.s) > 0) {
			result->value.i = 1;
		    } else {
			result->value.i = 0;
		    }
		}
		break;
	    case TA_MORE_EQUAL:
		if (op1->type == TOKEN_INT) {
		    if (op2->type == TOKEN_INT) {
			if (op1->value.i >= op2->value.i) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    } else {
			if (op1->value.i >= op2->value.d) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    }
		} else if (op1->type == TOKEN_DOUBLE) {
		    if (op2->type == TOKEN_DOUBLE) {
			if (op1->value.d >= op2->value.d) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    } else {
			if (op1->value.d >= op2->value.i) {
			    result->value.i = 1;
			} else {
			    result->value.i = 0;
			}
		    }
		} else if (op1->type == TOKEN_STRING) {
		    if (str_strcmpStr(&op1->value.s, &op2->value.s) >= 0) {
			result->value.i = 1;
		    } else {
			result->value.i = 0;
		    }
		}
		break;
	    case TA_MUL:
		if (result->type == TOKEN_INT) {
		    result->value.i = op1->value.i * op2->value.i;
		} else if (result->type == TOKEN_DOUBLE) {
		    if (op1->type == TOKEN_INT) {
			result->value.d = op1->value.i * op2->value.d;
		    } else if (op2->type == TOKEN_INT) {
			result->value.d = op1->value.d * op2->value.i;
		    } else {
			result->value.d = op1->value.d * op2->value.d;
		    }
		}
		break;
	    case TA_OVER:
		if (op1->type == TOKEN_INT) {
		    if (op2->type == TOKEN_INT) {
			result->value.d = pow(op1->value.i, op2->value.i);
			if (errno != 0) {
			    error(ERROR_INTERPRETER);
			    ta_dispose_all(three_adr);
			    free(jump_array);
			    return;
			}
		    } else {
			result->value.d = pow(op1->value.i, op2->value.d);
			if (errno != 0) {
			    error(ERROR_INTERPRETER);
			    ta_dispose_all(three_adr);
			    free(jump_array);
			    return;
			}
		    }
		} else {
		    if (op2->type == TOKEN_INT) {
			result->value.d = pow(op1->value.d, op2->value.i);
			if (errno != 0) {
			    error(ERROR_INTERPRETER);
			    ta_dispose_all(three_adr);
			    free(jump_array);
			    return;
			}
		    } else {
			result->value.d = pow(op1->value.d, op2->value.d);
			if (errno != 0) {
			    error(ERROR_INTERPRETER);
			    ta_dispose_all(three_adr);
			    free(jump_array);
			    return;
			}
		    }
		}
		break;
	    case TA_READ:	//precte hodnotu ze stdin a vlozi do polozky avl stomu
		if (result->type == TOKEN_INT) {
		    if (scanf("%d", &result->value.i) != 1) {
			ta_dispose_all(three_adr);
			free(jump_array);
			error(ERROR_INTERPRETER);	//runtime error
			return;
		    }
		    getchar();
		}
		if (result->type == TOKEN_DOUBLE) {
		    if (scanf("%lf", &result->value.d) != 1) {
			ta_dispose_all(three_adr);
			free(jump_array);
			error(ERROR_INTERPRETER);	//runtime error
			return;
		    }
		    getchar();
		}
		if (result->type == TOKEN_STRING) {
		    str_free(&result->value.s);	//uvolni priupadny string a prepise jej novym
		    str_init(&result->value.s);
		    if (get_error()) {
			ta_dispose_all(three_adr);
			free(jump_array);
			return;
		    }
		    while ((j = getchar()) != '\n') {
			str_strcatCh(&result->value.s, j);
			if (get_error()) {
			    str_free(&result->value.s);
			    ta_dispose_all(three_adr);
			    free(jump_array);
			    return;
			}
		    }
		    str_shrink(&result->value.s);
		}
		break;
	    case TA_SORT:	//seradi quicksortem vstup
		if (result->type == TOKEN_STRING) {
		    quicksort(str_c_str(&result->value.s),
			      strlen(str_c_str(&result->value.s)),
			      sizeof(char), inter_charcmp);
		}
		break;
	    case TA_SUB:
		if (result->type == TOKEN_INT) {
		    result->value.i = op1->value.i - op2->value.i;
		} else if (result->type == TOKEN_DOUBLE) {
		    if (op1->type == TOKEN_INT) {
			result->value.d = op1->value.i - op2->value.d;
		    } else if (op2->type == TOKEN_INT) {
			result->value.d = op1->value.d - op2->value.i;
		    } else {
			result->value.d = op1->value.d - op2->value.d;
		    }
		} else {
		    error(ERROR_INTERPRETER);
		    ta_dispose_all(three_adr);
		    free(jump_array);
		    return;
		}
		break;
	    case TA_TRUEJUMP:
		if (op1->value.i == 1)
		    i = jump_array[(*three_adr)[i].jump_label];
		break;
	    case TA_WRITE:	//vypise hodnotu na stdout
		if (op1->type == TOKEN_INT)
		    printf("%d", op1->value.i);
		if (op1->type == TOKEN_DOUBLE)
		    printf("%f", op1->value.d);
		if (op1->type == TOKEN_STRING)
		    printf("%s", str_c_str(&op1->value.s));
		break;
	    default:
		error(ERROR_INTERPRETER);
		break;
	}
    }
    free(jump_array);
    ta_dispose_all(three_adr);
}
