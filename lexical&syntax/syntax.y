%{
#include <stdio.h>
#include <stdlib.h>
#include "lexical.h"
#include "symtab.h"
#include "parse.h"
#include "token.h"

#define YYSTYPE Token
Token parse_ans;
extern int lineno;//import lineno from lexical
void yyerror(char * err_info);

%}

%%

program : declaration-list 		{$$ = make_program($1);}
					;
declaration-list : declaration-list declaration 		{}
					| declaration		{}
					;
declaration : var-declaration 		{}
					| fun-declaration			{}
					| enum-declaration		{}
					;
// a program consists of a list of declaration, including functions and variables

enum-declaration : ENUM LCB ID-list RCB SEMI
					;

var-declaration : type-specifier ID-list SEMI 			{} 
                                 | type-specifier ID LSB CONST_INT RSB SEMI		{} //array
								 | type-specifier MUL ID-list SEMI  {} //pointer
					; 
ID-list : ID COMMA ID-list 		{}
					| ID		{}
					;
type-specifier : INT 
					| DOUBLE 
					| BOOL 
					| CHAR 
					;
//declarations for variable and array

fuc-declaration : type-specifier ID LP paras RP compond-stmt
					;
paras : para-list 
					| VOID
					;
para-list : para-list COMMA para 
					| para
					;
para : type-specifier ID 
          			| type-specifier ADDR ID 
          			| type-specifier ID LSB RSB
					;
// function declaration and its definition
// pass the paras by value, reference and pointer

compond-stmt: LCB local-declaration stmt-list RCB
					;
//a compond statement {...}, contains some local statements 

local-declaration : local-declaration var-declaration 
					| //empty
					;
stmt-list : stmt-list stmt  
					| //empty
					;
//both nullable

stmt : 	expression-stmt 
					| compond-stmt 
					| selection-stmt 
					| iteration-stmt 
					| return-stmt
					;
//five kinds of statement

expression-stmt : expression SEMI 
					| SEMI 
					;
selection-stmt : if LP expression RP  stmt 
                    | if LP expression RP  stmt ELSE stmt
					;
iteration-stmt : WHILE LP expression RP stmt
					;
return-stmt : RETURN SEMI | RETURN expression SEMI
					;
// if else while and return 

expression : l-value ASSIGN expression 
					| simple-expression
					;
l-value : ID 
					| ADDR ID 
					| ID LSB expression RSB
					;
//left value contains id, pointer, array 

simple-expression : add-expression relop add-expression
                                        | add-expression
					;
relop : LE 
					| LT 
					| GE
					| GT 
					| EQUAL 
					| NE
					;
// if contain relop , return bool type

add-expression : add-expression addop term 
					| term
					;
addop : PLUS 
					| MINUS
					;
term : term mulop factor 
					| factor
					;
muiop : MUL 
					| DIV
					;
factor : LP expression RP 
					| l-value 
					| call 
					| literal-const
					;
literal-const : 
					CONST_INT 
					| CONST_DOUBLE 
					| CONST_CHAR 
					| TRUE 
					| FALSE
					;
// the factor can be a (expression) or left-value or a function call or a const-value

call : ID LP args RP
					;
args : arg-list 
					| //empty
					;
arg-list : arg-list COMMA expression 
					| expression
					;
//pass the args by id or an expression


%%
int main() 
{


	return 0;
}

void yyerror(char * err_info)
{
    fprintf(stderr, "error in line: %d: %s\n", lineno,err_info);
    exit(1);
}
