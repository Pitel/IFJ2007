
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

#include "lex.h"
#include "syntax.h"
#include "error.h"

/**
 * Process file by name and call syntax analyse
 *
 * @param file_name File name to process
 */
void process_file(char *file_name)
{
    assert(file_name != NULL);

    FILE *f;

    f = fopen(file_name, "r");
    if (f) {
	Tnode_ptr symtab;
	avl_init(&symtab);
	syntax(f, &symtab);
	fclose(f);
	avl_destroy_tree(&symtab);
    } else {
	error(ERROR_FOPEN);
    }
}

/**
 * Show help information
 */
void param_help()
{
    fprintf(stderr, "Usage: ifj2007 [FILE]\n");
}

/**
 * Main function
 */
int main(int argc, char **argv)
{
    int exit_code = 0;

    if (argc == 2) {
	// Process source file
	process_file(argv[1]);
	if (get_error()) {
	    if (get_error() == ERROR_LEX) {
		exit_code = 1;
		fprintf(stderr, "Error in lexical analyse\n");
	    } else if (get_error() == ERROR_SYNTAX) {
		exit_code = 2;
		fprintf(stderr, "Error in syntax analyse\n");
	    } else if (get_error() == ERROR_SEMANTIC) {
		exit_code = 3;
		fprintf(stderr, "Error in semantic analyse\n");
	    } else if (get_error() == ERROR_INTERPRETER) {
		exit_code = 4;
		fprintf(stderr, "Error in interpreter\n");
	    } else {
		// Other error
		exit_code = 255;
		switch (get_error()) {
		    case ERROR_NO_MEMORY:
			fprintf(stderr, "Not enough memory\n");
			break;
		    case ERROR_FOPEN:
			fprintf(stderr, "File opening error\n");
			break;
		    case ERROR_STACK_POP:
			fprintf(stderr, "Stack pop error\n");
			break;
		    case ERROR_STACK_TOP:
			fprintf(stderr, "Stack top error\n");
			break;
		    case ERROR_STACK_LOOKATNEXT:
			fprintf(stderr, "Stack lookatnext error\n");
			break;
		    case ERROR_THREEADR_CODE:
			fprintf(stderr, "Three addr code error\n");
			break;
		}
	    }
	}
    } else {
	// Show parameter help
	param_help();
    }

    exit(exit_code);
}
