%{
#include "ast.hpp"
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
// NIdentifierV2 *current_fn = nullptr;
NExpression *current_return_expr = nullptr;
NFunctionV2 *ptr_to_main = nullptr;
Program *init_program  = nullptr;
%}
// %#include <string>
%union { 
    int token;
    std::string *str; 
    Dtype t; 
    Node *node;
    NIdentifierV2 *ident;
    NBlockV2 *block;
    NExpression *expr;
    NExpressionFromStatement *stmt;
    NFunctionV2* fn;
    NVariableDefinition *var_def;
    std::vector<NExpression*> *exprvec;
    std::vector<NVariableDefinition*> *var_def_vec;
    std::vector<NFunctionV2*> *fnvec;
    Program *prog;
}

%token <str> identifier CDOUBLE CINT CSTRING
%token <token> RETURN DOUBLE INT STRING o_pow o_add o_sub o_mult o_div o_assign
%type <ident> id
%type <expr> expr 
%type <stmt> stmt exprs
%type <block> stmts block
%type <fnvec> functions
%type <fn> function
%type <var_def> var_defn 
%type <var_def> fn_var_defs_args
%type <var_def_vec> var_defs 
%type <var_def_vec> fn_declare_args
%type <t> dtype
%type <prog> program
%type <exprvec> call_args

%right o_assign
%left o_add o_sub
%left o_mult o_div
%left o_pow
%start program

%%

program:    functions {
                $$ = new Program;
                $$->list = $1;
                $$->mainFn = ptr_to_main;
                // std::cout << "program\n";
                init_program = $$;
            }
;

functions:  function {
                $$ = new FunctionList();
                $$->push_back($1);
                // std::cout << "[FN]$$ = new FunctionList(); $$->push_back($<fn>1)\n";
            }
|           functions function{
                $1->push_back($2);
                // std::cout << "[FN]$1->push_back($<fn>2)\n";
            }
;

function:   dtype id {
                    $2->node_type = $1;
                    global_scope++;
                } '(' fn_declare_args ')' block {
                NFunctionV2* tmp = new NFunctionV2($1, *$2, *$5, *$7, current_return_expr);
                $$ = tmp;
                global_fn_table[$2->name] = tmp;
                if ($2->name == "main") ptr_to_main = $$;
                current_return_expr = nullptr;
                std::cout << "function=" << fnc++ <<"\n";
                // std::cout << "$$ = new NFunctionV2($1, *$2, *$4, *$6, current_return_expr);\n";
                } 
;

id: identifier  {
                    $$ = new NIdentifierV2(*$1, global_scope);
                    // std::cout <<lineno<<" $$ = new NIdentifierV2(*$1);["+$$->name+"]\n";
                    delete $1;
                };

fn_declare_args:    fn_declare_args ',' fn_var_defs_args {
                        $1->push_back($<var_def>3);
                        // std::cout << lineno << " $1->push_back($<var_def>$3);\n";
                    }
|                   fn_var_defs_args {
                        $$ = new variable_listv2();
                        $$->push_back($<var_def>1);
                        // std::cout << lineno << " $$ = new $$ = new variable_listv2();$$->push_back($<var_def>$1);\n";
                    }
|                   %empty{
                        $$ = new variable_listv2();
                        // std::cout << lineno << " fn $$ = new variable_listv2();\n";
                    }
;
fn_var_defs_args:   dtype var_defn {
                        $$ = $2;
                        $$->node_type = $1;
                        // std::cout << lineno << " $$ = $1; $$->node_type = dtype;\n";
                    }
;

var_defs:   dtype var_defn {
                $$ = new variable_listv2();
                $2->node_type = $1;
                $$->push_back($<var_def>2);
                // std::cout << lineno << " var_defS $$ = new variable_listv2(); $2->node_type = $1; $$->push_back($<var_def>2);\n";
            }
|           var_defs ',' var_defn {
                $1->push_back($<var_def>3);
                // std::cout << lineno <<" $1->push_back($<var_def>3)\n";
            };    
var_defn:   id o_assign expr {
                $$ = new NVariableDefinition(*$1, $3);
                // std::cout <<lineno <<" var_defN $$ = new NVariableDefinition(*$1, *$3{expression})\n"; 
            }
|           id {
                $$ = new NVariableDefinition(*$1);
                // std::cout << lineno <<" var_defN $$ = new NVariableDefinition(*$1)\n";
            }; 

separator3: ';' ;

block:      '{' '}'{
                $$ = new NBlockV2();
                // std::cout << "$$ = new NBlockV2();\n"; 
                global_scope--;
            }
|           '{' stmts '}'{
                $$ = $2;
                // std::cout << "$$ = $2 [BLOCK]\n";
                global_scope--;
            }
;

stmts:      stmts stmt {
                $1->statements.push_back($2);
                // std::cout << "$1->statements.push_back($<stmt>2);\n";
            }
|           stmt{
                $$ = new NBlockV2(); 
                $$->statements.push_back($1);
                // std::cout << "$$ = new NBlockV2(); $$->statements.push_back($1)\n";
            }
;

stmt:       exprs separator3 {
                $$ = $1;
                // std::cout << "\t$$ = new NExpressionFromStatement(*$1);\n";
            }
|           RETURN expr separator3 {
                current_return_expr = $2;
                $$ = new NExpressionFromStatement($2);
                // std::cout << "\t$$ = new NExpressionFromStatement($2);\n\t {current function being defined assign an expression}\n";
                // std::cout << lineno << " return list\n";
            }
;

exprs:      var_defs {
                $$ = new NExpressionFromStatement($1);
            }
|           expr {
                $$ = new NExpressionFromStatement($1);
            };

expr:       expr o_add expr {
                $$ = new NArithmeticV2(*$1, ADD, *$3);
                // std::cout << lineno <<" $$ NArithmeticV2($1,ADD,$3);\n";
                }
|           expr o_sub expr{
                $$ = new NArithmeticV2(*$1, SUB, *$3);
                // std::cout << lineno <<" $$ NArithmeticV2($1,SUB,$3);\n";
                }
|           expr o_mult expr{
                $$ = new NArithmeticV2(*$1, MULT, *$3);
                // std::cout << lineno <<" $$ NArithmeticV2($1,MULT,$3);\n";
                }
|           expr o_div expr{
                $$ = new NArithmeticV2(*$1, DIV, *$3);
                // std::cout << lineno <<" $$ NArithmeticV2($1,DIV,$3);\n";
                }
|           expr o_pow expr{
                $$ = new NArithmeticV2(*$1, POW, *$3);
                // std::cout << lineno <<" $$ NArithmeticV2($1,POW,$3);\n";
                }
|           '(' expr ')' {$$ = $2;}
|           id '(' call_args ')' {
                $$ = new NCallFunction(*$1, *$3);
            }
|           CINT    {
                $$ = new NInt(atol($1->c_str())); 
                // std::cout <<"\t" << $$->code_value().int_v << " int value\n";
                delete $1; 
            }
|           CDOUBLE {
                $$ = new NDouble(atof($1->c_str())); 
                // std::cout <<"\t" <<$$->code_value().double_v << " double value\n";
                delete $1; 
            }
|           CSTRING {
                $$ = new NString(*$1); 
                // std::cout <<"\t" <<$$->code_value().string_v << " string value\n";
                delete $1; 
            }
|           id      {
                $<ident>$ = $1; 
                // std::cout << "\t$<ident>$ = $1\n";
            };

call_args:  expr {$$ = new expression_list(); $$->push_back($1);}
|           call_args ',' expr {$1->push_back($3);}
|           %empty {$$ = new expression_list();};


dtype:      INT    {$$ = Dtype::int_dtype;   }
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
    std::cout << "🚀> Execute main function\n";
    init_program->init_main();
    fclose(yyin);
    return 0;    
}