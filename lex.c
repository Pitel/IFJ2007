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
#include <ctype.h>

#include "lex.h"
#include "str.h"
#include "error.h"
#include "keyword.h"
#include "avl.h"

/**
 * One char token conversion table
 */
const int char2token_type[] = {
    ['-'] = TOKEN_MINUS,
    ['+'] = TOKEN_PLUS,
    ['*'] = TOKEN_MULTIPLY,
    ['/'] = TOKEN_DIVIDE,
    ['^'] = TOKEN_OVER,
    ['$'] = TOKEN_DOLAR,
    ['{'] = TOKEN_BLOCK_BEGIN,
    ['}'] = TOKEN_BLOCK_END,
    ['('] = TOKEN_L_BRACKET,
    [')'] = TOKEN_R_BRACKET,
    ['<'] = TOKEN_LESS,
    ['>'] = TOKEN_MORE,
    ['='] = TOKEN_ASSIGN_OP,
    [';'] = TOKEN_SEMICOLON,
    [','] = TOKEN_COMMA,
    [255] = 0
};

/**
 * Is 'ch' LEX char (one char token)
 *
 * @param ch Char to test
 * @return True if 'ch' is LEX char
 */
inline int islexchar(char ch)
{
    if (char2token_type[(unsigned char)ch]) {
	return 1;
    }

    return 0;
}

/**
 * Do lexeme analyse on input stream and return token
 *
 * @param stream Stream to read from
 * @param token Pointer to destination token
 */
void lex(FILE *stream, Tlex_token *token)
{
    assert(stream != NULL);
    assert(token != NULL);

    int ch = 0;
    int state = LSTATE_INIT;
    int again = 0;

    token->token_type = TOKEN_EOF;
    token->lexeme.data = NULL;

    while (state != LSTATE_END && (again || (ch = fgetc(stream)) != EOF)) {
	if (again) {
	    again = 0;
	}
	switch (state) {
	    // Init state
	    case LSTATE_INIT:
		// Ignore white-space in init state
		if isspace(ch) {
		    break;
		}

		switch (ch) {
		    case '/':
			state = LSTATE_SLASH;
			break;
		    case '<':
			state = LSTATE_COMPARE_LESS;
			str_init(&token->lexeme);
			if (get_error()) {
			    return;
			}
			str_strcatCh(&token->lexeme, (char)ch);
			if (get_error()) {
			    str_free(&token->lexeme);
			    return;
			}
			break;
		    case '>':
			state = LSTATE_COMPARE_MORE;
			str_init(&token->lexeme);
			if (get_error()) {
			    return;
			}
			str_strcatCh(&token->lexeme, (char)ch);
			if (get_error()) {
			    str_free(&token->lexeme);
			    return;
			}
			break;
		    case '=':
			state = LSTATE_COMPARE_EQUAL;
			str_init(&token->lexeme);
			if (get_error()) {
			    return;
			}
			str_strcatCh(&token->lexeme, (char)ch);
			if (get_error()) {
			    str_free(&token->lexeme);
			    return;
			}
			break;
		    case '!':
			state = LSTATE_COMPARE_NOT;
			str_init(&token->lexeme);
			if (get_error()) {
			    return;
			}
			str_strcatCh(&token->lexeme, (char)ch);
			if (get_error()) {
			    str_free(&token->lexeme);
			    return;
			}
			break;
		    case '"':
			state = LSTATE_QUOTE;
			str_init(&token->lexeme);
			if (get_error()) {
			    return;
			}
			break;
		    default:
			str_init(&token->lexeme);
			if (get_error()) {
			    return;
			}
			if (islexchar(ch)) {
			    str_strcatCh(&token->lexeme, (char)ch);
			    if (get_error()) {
				str_free(&token->lexeme);
				return;
			    }
			    token->token_type = char2token_type[ch];
			    state = LSTATE_END;
			} else {
			    if (isalpha(ch)) {
				state = LSTATE_SYMBOL;
			    } else if (isdigit(ch)) {
				state = LSTATE_INT;
			    } else {
				error(ERROR_LEX);
				str_free(&token->lexeme);
				return;
			    }
			    again = 1;
			}
			break;
		}
		break;

	    // Symbol state
	    case LSTATE_SYMBOL:
		if (isalnum(ch)) {
		    str_strcatCh(&token->lexeme, (char)ch);
		    if (get_error()) {
			str_free(&token->lexeme);
			return;
		    }
		} else {
		    int i = is_keyword(str_c_str(&token->lexeme));
		    if (i) {
			token->token_type = i;
		    } else {
			token->token_type = TOKEN_SYMBOL;
		    }

		    again = 1;
		    state = LSTATE_END;
		}
		break;

	    // Integer state
	    case LSTATE_INT:
		if (isdigit(ch)) {
		    str_strcatCh(&token->lexeme, (char)ch);
		    if (get_error()) {
			str_free(&token->lexeme);
			return;
		    }
		} else if (ch == '.') {
		    str_strcatCh(&token->lexeme, (char)ch);
		    if (get_error()) {
			str_free(&token->lexeme);
			return;
		    }
		    state = LSTATE_DOUBLE;
		} else if (ch == 'e' || ch == 'E') {
		    str_strcatCh(&token->lexeme, (char)ch);
		    if (get_error()) {
			str_free(&token->lexeme);
			return;
		    }
		    state = LSTATE_DOUBLE_EXP;
		} else if (islexchar(ch) || isspace(ch)) {
		    token->token_type = TOKEN_INT;
		    state = LSTATE_END;
		    again = 1;
		} else {
		    error(ERROR_LEX);
		    str_free(&token->lexeme);
		    return;
		}
		break;

	    // Double state
	    case LSTATE_DOUBLE:
		if (isdigit(ch)) {
		    state = LSTATE_DOUBLE_P;
		    str_strcatCh(&token->lexeme, (char)ch);
		    if (get_error()) {
			str_free(&token->lexeme);
			return;
		    }
		} else {
		    error(ERROR_LEX);
		    str_free(&token->lexeme);
		    return;
		}
		break;

	    // Double state plus
	    case LSTATE_DOUBLE_P:
		if (ch == 'e' || ch == 'E') {
		    state = LSTATE_DOUBLE_EXP;
		    str_strcatCh(&token->lexeme, (char)ch);
		    if (get_error()) {
			str_free(&token->lexeme);
			return;
		    }
		} else if (isdigit(ch)) {
		    str_strcatCh(&token->lexeme, (char)ch);
		    if (get_error()) {
			str_free(&token->lexeme);
			return;
		    }
		} else if (islexchar(ch) || isspace(ch)) {
		    token->token_type = TOKEN_DOUBLE;
		    state = LSTATE_END;
		    again = 1;
		} else {
		    error(ERROR_LEX);
		    str_free(&token->lexeme);
		    return;
		}
		break;

	    // Double exponent state
	    case LSTATE_DOUBLE_EXP:
		if (ch == '+' || ch == '-') {
		    str_strcatCh(&token->lexeme, (char)ch);
		    if (get_error()) {
			str_free(&token->lexeme);
			return;
		    }
		    state = LSTATE_DOUBLE_EXP_SIGN;
		} else if (isdigit(ch)) {
		    state = LSTATE_DOUBLE_EXP_P;
		    str_strcatCh(&token->lexeme, (char)ch);
		    if (get_error()) {
			str_free(&token->lexeme);
			return;
		    }
		} else {
		    error(ERROR_LEX);
		    str_free(&token->lexeme);
		    return;
		}
		break;

	    // Double exponent plus state
	    case LSTATE_DOUBLE_EXP_P:
		if (isdigit(ch)) {
		    state = LSTATE_DOUBLE_EXP_P;
		    str_strcatCh(&token->lexeme, (char)ch);
		    if (get_error()) {
			str_free(&token->lexeme);
			return;
		    }
		} else if (islexchar(ch) || isspace(ch)) {
		    token->token_type = TOKEN_DOUBLE;
		    state = LSTATE_END;
		    again = 1;
		} else {
		    error(ERROR_LEX);
		    str_free(&token->lexeme);
		    return;
		}
		break;

	    // Double exponent sign state
	    case LSTATE_DOUBLE_EXP_SIGN:
		if (isdigit(ch)) {
		    state = LSTATE_DOUBLE_EXP_SIGN_P;
		    str_strcatCh(&token->lexeme, (char)ch);
		    if (get_error()) {
			str_free(&token->lexeme);
			return;
		    }
		} else {
		    error(ERROR_LEX);
		    str_free(&token->lexeme);
		    return;
		}
		break;

	    // Double exponent sign plus state
	    case LSTATE_DOUBLE_EXP_SIGN_P:
		if (isdigit(ch)) {
		    str_strcatCh(&token->lexeme, (char)ch);
		    if (get_error()) {
			str_free(&token->lexeme);
			return;
		    }
		} else if (islexchar(ch) || isspace(ch)) {
		    token->token_type = TOKEN_DOUBLE;
		    state = LSTATE_END;
		    again = 1;
		} else {
		    error(ERROR_LEX);
		    str_free(&token->lexeme);
		    return;
		}
		break;

	    // Compare less state
	    case LSTATE_COMPARE_LESS:
		state = LSTATE_END;
		if (ch == '=') {
		    token->token_type = TOKEN_LESS_EQUAL;
		    str_strcatCh(&token->lexeme, (char)ch);
		    if (get_error()) {
			str_free(&token->lexeme);
			return;
		    }
		} else {
		    token->token_type = TOKEN_LESS;
		    again = 1;
		}
		break;

	    // Compare more state
	    case LSTATE_COMPARE_MORE:
		state = LSTATE_END;
		if (ch == '=') {
		    token->token_type = TOKEN_MORE_EQUAL;
		    str_strcatCh(&token->lexeme, (char)ch);
		    if (get_error()) {
			str_free(&token->lexeme);
			return;
		    }
		} else {
		    token->token_type = TOKEN_MORE;
		    again = 1;
		}
		break;

	    // Compare equal state
	    case LSTATE_COMPARE_EQUAL:
		state = LSTATE_END;
		if (ch == '=') {
		    token->token_type = TOKEN_EQUAL;
		    str_strcatCh(&token->lexeme, (char)ch);
		    if (get_error()) {
			str_free(&token->lexeme);
			return;
		    }
		} else {
		    token->token_type = TOKEN_ASSIGN_OP;
		    again = 1;
		}
		break;

	    // Compare not state
	    case LSTATE_COMPARE_NOT:
		state = LSTATE_END;
		if (ch == '=') {
		    token->token_type = TOKEN_NOT_EQUAL;
		    str_strcatCh(&token->lexeme, (char)ch);
		    if (get_error()) {
			str_free(&token->lexeme);
			return;
		    }
		} else {
		    token->token_type = TOKEN_EXCLAM;
		    again = 1;
		}
		break;

	    // Quote state
	    case LSTATE_QUOTE:
		switch (ch) {
		    case '\\':
			state = LSTATE_EQUOTE;
			break;
		    case '"':
			state = LSTATE_END;
			token->token_type = TOKEN_STRING;
			break;
		    default:
			str_strcatCh(&token->lexeme, (char)ch);
			if (get_error()) {
			    str_free(&token->lexeme);
			    return;
			}
			break;
		}
		break;

	    // Escape quote state
	    case LSTATE_EQUOTE:
		switch (ch) {
		    case 'n':
			str_strcatCh(&token->lexeme, '\n');
			if (get_error()) {
			    str_free(&token->lexeme);
			    return;
			}
			break;
		    case 't':
			str_strcatCh(&token->lexeme, '\t');
			if (get_error()) {
			    str_free(&token->lexeme);
			    return;
			}
			break;
		    default:
			str_strcatCh(&token->lexeme, (char)ch);
			if (get_error()) {
			    str_free(&token->lexeme);
			    return;
			}
			break;
		}

		state = LSTATE_QUOTE;
		break;

	    // Slash state
	    case LSTATE_SLASH:
		switch (ch) {
		    case '*':
			state = LSTATE_COMMENT;
			break;
		    default:
			str_init(&token->lexeme);
			if (get_error()) {
			    return;
			}
			str_strcatCh(&token->lexeme, '/');
			if (get_error()) {
			    str_free(&token->lexeme);
			    return;
			}

			state = LSTATE_END;
			token->token_type = TOKEN_DIVIDE;
			again = 1;
			break;
		}
		break;

	    // Comment state
	    case LSTATE_COMMENT:
		switch (ch) {
		    case '*':
			state = LSTATE_COMMENT_END;
			break;
		}
		break;

	    // Comment end state
	    case LSTATE_COMMENT_END:
		switch (ch) {
		    case '/':
			state = LSTATE_INIT;
			break;
		    default:
			state = LSTATE_COMMENT;
			break;
		}
		break;

	    // Unknow state
	    default:
		error(ERROR_LEX);
		token_free(token);
		return;
		break;
	}
    }

    if (token->lexeme.data != NULL) {
	str_shrink(&token->lexeme);
	if (get_error()) {
	    return;
	}
    }

    if (again) {
	ungetc(ch, stream);
    } else {
	if (token->token_type == TOKEN_EOF &&
		state != LSTATE_END && state != LSTATE_INIT) {
	    error(ERROR_LEX);
	    token_free(token);
	}
    }
}

/**
 * Free token memory
 *
 * @param *token Pointer to token
 */
void token_free(Tlex_token *token)
{
    if (token->lexeme.data != NULL) {
	str_free(&(token->lexeme));
    }
}
