%{
#include "ast.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include <cmath>

// using namespace std;
extern FILE* yyin;
extern int yylex();
static std::map<std::string, int> vars;
extern int lineno;
inline void yyerror(const char *s) { std::cout << "LN["<<lineno<<"] "<<s<< "\n"; }
int fnc = 1;
void print_data_type(Dtype d, int lineno);

%}
// %#include <string>
%union { int token; std::string *str; Dtype t; Node *node;}

%token <str> identifier CDOUBLE CINT CSTRING
%token <token> RETURN DOUBLE INT STRING o_pow o_add o_sub o_mult o_div o_assign
%type  <str> id
%type  <t> dtype

%right o_assign
%left o_add o_sub
%left o_mult o_div
%left o_pow
%start program

%%

program:    functions {std::cout << "program\n";}
;

functions:  function
|           functions function
;

function:   dtype id {std::cout << "function "; print_data_type($1, lineno);} '(' fn_declare_args ')' block {std::cout << "function=" << fnc++ <<"\n";} 
;

id: identifier  {
                    std::cout <<lineno<<" $$ = new NIdentifier(*$1);\n\tIDENTIFIER[" << *$1 << "]\n";
                };

fn_declare_args:    fn_declare_args ',' fn_var_defs_args {
                        std::cout << lineno << " $1->push_back($<fn_var_def>3)\n";
                    }
|                   fn_var_defs_args {
                        std::cout << lineno << " $$ = new Fn_variable_list();$$->push_back($<var_def>1)\n";
                    }
|                   %empty{
                        std::cout << lineno << " $$ = new Fn_variable_list();\n";
                    }
;
fn_var_defs_args:   dtype var_defn {
                        // std::cout << "fn_var_defs_args "; 
                        // print_data_type($1, lineno);
                        std::cout << lineno << " $$ = $1; $$->setType($<t>1);\n";
                    }
;

var_defs:   dtype var_defn {
                // std::cout << "var_defs ";
                // print_data_type($1, lineno);
                std::cout << lineno << " $$ = new variable_list(); $$->setType($<t>1){setting type}; $$->push_back($<var_def>2);\n";
            }
|           var_defs ',' var_defn {
                std::cout << lineno <<" $1->push_back($<var_def>3)\n";
            };    
var_defn:   id o_assign expr {
                std::cout <<lineno <<" $$ = new VariableDefinition(*$1, *$3{expression})\n"; 
            }
|           id {
                std::cout << lineno <<" $$ = new VariableDefinition(*$1)\n";
            }; 

separator3: ';' ;

block:      '{' '}'
|           '{' stmts '}'
;

stmts:      stmts stmt
|           stmt
;
stmt:       exprs separator3
|           RETURN expr separator3
;

exprs:      var_defs
|           expr;

expr:       expr o_add expr
|           expr o_sub expr
|           expr o_mult expr
|           expr o_div expr
|           expr o_pow expr
|           '(' expr ')'
|           id '(' call_args ')'
|           CINT
|           CDOUBLE
|           CSTRING
|           id;

call_args:  expr
|           call_args ',' expr
|           %empty;


dtype:      INT {$$ = Dtype::int_dtype;}
|           DOUBLE {$$ = Dtype::double_dtype;} 
|           STRING {$$ = Dtype::string_dtype;}
;
%%
void print_data_type(Dtype d, int lineno) {
    if (d == double_dtype)
        std::cout << "LN[" << lineno << "]"<< "DOUBLE" << "\n";
    else if (d == int_dtype)
        std::cout << "LN[" << lineno << "]"<< "INT" << "\n";
    else if (d == string_dtype)
        std::cout << "LN[" << lineno << "]"<< "STRING" << "\n";
}

int main(int argc, char **argv) { 
    yyin = fopen(argv[1], "r");
    yyparse();
    fclose(yyin);
    return 0;    
}