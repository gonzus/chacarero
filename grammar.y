%include {
#include "parser.h"
}

%token_type { char* }

%extra_argument { AST *ast }

%parse_accept { fprintf(stderr, "The parser has completed successfully\n"); }

%syntax_error { fprintf(stderr, "Parser syntax error\n"); }

%parse_failure { fprintf(stderr, "Parser failure\n"); }

%start_symbol program

// UNARY OPERATOR
unary_operator ::= ADD.
unary_operator ::= SUB.
unary_operator ::= BNOT.
unary_operator ::= LNOT.

// ASSIGNMENT OPERATOR
assignment_operator ::= EQ.

// PRIMARY EXPRESSION
primary_expression ::= NAME.
primary_expression ::= INT_LITERAL.
primary_expression ::= STRING_LITERAL.
primary_expression ::= LPAREN expression RPAREN.

// POSTFIX EXPRESSION
postfix_expression ::= primary_expression.
postfix_expression ::= postfix_expression LBRACKET expression RBRACKET.
postfix_expression ::= postfix_expression LPAREN RPAREN.
postfix_expression ::= postfix_expression DOT NAME.
postfix_expression ::= postfix_expression ARROW NAME.
postfix_expression ::= postfix_expression ADDADD.
postfix_expression ::= postfix_expression SUBSUB.

// UNARY EXPRESSION
unary_expression ::= postfix_expression.
unary_expression ::= ADDADD unary_expression.
unary_expression ::= SUBSUB unary_expression.
unary_expression ::= unary_operator cast_expression.

// CAST EXPRESSION
cast_expression ::= unary_expression.

// MULTIPLICATIVE EXPRESSION
multiplicative_expression ::= cast_expression.
multiplicative_expression ::= multiplicative_expression MUL cast_expression.
multiplicative_expression ::= multiplicative_expression DIV cast_expression.
multiplicative_expression ::= multiplicative_expression MOD cast_expression.

// ADDITIVE EXPRESSION
additive_expression ::= multiplicative_expression.
additive_expression ::= additive_expression ADD multiplicative_expression.
additive_expression ::= additive_expression SUB multiplicative_expression.

// SHIFT EXPRESSION
shift_expression ::= additive_expression.

// RELATIONAL EXPRESSION
relational_expression ::= shift_expression.
relational_expression ::= relational_expression LLT shift_expression.
relational_expression ::= relational_expression LGT shift_expression.
relational_expression ::= relational_expression LLE shift_expression.
relational_expression ::= relational_expression LGE shift_expression.

// EQUALITY EXPRESSION
equality_expression ::= relational_expression.
equality_expression ::= equality_expression LEQ relational_expression.
equality_expression ::= equality_expression LNE relational_expression.

// BIT AND EXPRESSION
and_expression ::= equality_expression.
and_expression ::= and_expression BAND equality_expression.

// BIT EXCLUSIVE OR EXPRESSION
exclusive_or_expression ::= and_expression.
exclusive_or_expression ::= exclusive_or_expression BXOR and_expression.

// BIT INCLUSIVE OR EXPRESSION
inclusive_or_expression ::= exclusive_or_expression.
inclusive_or_expression ::= inclusive_or_expression BOR exclusive_or_expression.

// LOGICAL AND EXPRESSION
logical_and_expression ::= inclusive_or_expression.
logical_and_expression ::= logical_and_expression LAND inclusive_or_expression.

// LOGICAL OR EXPRESSION
logical_or_expression ::= logical_and_expression.
logical_or_expression ::= logical_or_expression LOR logical_and_expression.

// CONDITIONAL EXPRESSION
conditional_expression ::=	logical_or_expression.
conditional_expression ::=	logical_or_expression QUESTION expression COLON conditional_expression.

// ASSIGNMENT EXPRESSION
assignment_expression ::= conditional_expression.
assignment_expression ::= unary_expression assignment_operator assignment_expression.

// EXPRESSION
expression ::= assignment_expression.

// EXPRESSION STATEMENT
expression_statement ::= expression SEMICOLON.
expression_statement ::= SEMICOLON.

// STATEMENT
statement ::= expression_statement.

// STATEMENT LIST
statement_list ::= statement.
statement_list ::= statement_list statement.

// PROGRAM
program ::= statement_list END_TOKEN. { fprintf(stderr, "Constructed the program symbol\n"); }
program ::= unused.

// UNUSED TERMINALS
unused ::= LBRACE.
unused ::= RBRACE.
unused ::= COMMA.
