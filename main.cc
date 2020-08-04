#include <iostream>
#include <string>

#include "ast.hpp"

// void describe_func(NFunction &fn) {
//   std::cout << "[function id]=" + fn.id.name + "\n";
//   std::cout << "[function type]=";
//   get_node_type(fn.id);
//   std::cout << "[function parameters]={ ";
//   for (const auto &args : fn.arguments) {
//     std::cout << args->name << "-";
//   }
//   std::cout << "}\n";
//   std::cout << "[function block]={";
//   for (const auto &stm : fn.block.statements) {
//     for (const auto &expr : (stm->expressions)) {
//       switch (expr->node_type) {
//         case int_dtype:
//           std::cout << " " << expr->value().int_v << " ";
//           break;
//         case string_dtype:
//           std::cout << " " << expr->value().string_v << " ";
//           break;
//         case double_dtype:
//           std::cout << " " << expr->value().double_v << " ";
//           break;
//         default:
//           std::cout << " undefined ";
//           break;
//       }
//     }
//   }
//   std::cout << "}\n";
// }

int main() {
  // (1+2)**(3+2) Arithmetic Expression
  /*
++global_scope;
    int main (int foo, int bar) {
      return bar + foo * 2 ;
    }
--global_scope;
 */
  // int main:
  NIdentifierV2 *current_fn;
  NExpression *current_return_expr = nullptr;
  NFunctionV2 *ptr_to_main = nullptr;
  NIdentifierV2 *main_id = new NIdentifierV2("main", global_scope, int_dtype);
  current_fn = main_id;
  /* inside function incrementing scope */
  global_scope++;
  // (int foo, int bar){ :
  // int foo
  NIdentifierV2 *foo = new NIdentifierV2("foo", global_scope, int_dtype);
  NVariableDefinition *foo_definition = new NVariableDefinition(*foo);
  foo_definition->node_type = int_dtype;
  // variable list v2
  variable_listv2 *variable_list_args = new variable_listv2();
  variable_list_args->push_back(foo_definition);  // foo is pushed
  // int bar
  NIdentifierV2 *bar = new NIdentifierV2("bar", global_scope, int_dtype);
  NVariableDefinition *bar_definition = new NVariableDefinition(*bar);
  bar_definition->node_type = int_dtype;
  variable_list_args->push_back(bar_definition);  // bar is pushed
  // referencing  bar
  NIdentifierV2 *refBar = new NIdentifierV2("bar", global_scope);
  NExpression *exprBar = refBar;
  // get_node_type(*refBar);
  // get_node_type(*exprBar);
  NIdentifierV2 *refFoo = new NIdentifierV2("foo", global_scope);
  NExpression *exprFoo = refFoo;
  // get_node_type(*refBar);
  // get_node_type(*exprFoo);
  NInt *Int2 = new NInt(2);
  // MULT then ADD
  NExpression *ariMULT = new NArithmeticV2(*exprFoo, MULT, *Int2);
  ariMULT->node_type = exprFoo->node_type;
  NExpression *ariADD = new NArithmeticV2(*exprBar, ADD, *ariMULT);
  ariADD->node_type = exprBar->node_type;
  current_return_expr = ariMULT;
  /* return detected */
  NExpressionFromStatement *expression = new NExpressionFromStatement(ariADD);
  NBlockV2 *main_fn_block = new NBlockV2();
  main_fn_block->statements.push_back(expression);
  NFunctionV2 *main_fn =
      new NFunctionV2(int_dtype, *main_id, *variable_list_args, *main_fn_block,
                      current_return_expr);
  if (main_id->name == "main") ptr_to_main = main_fn;
  global_scope--;
  FunctionList *funcs = new FunctionList();
  funcs->push_back(main_fn);
  Program *mainprog = new Program;
  mainprog->list = funcs;
  mainprog->mainFn = ptr_to_main;
  // mainprog->interpreter();
  mainprog->describe_main_fn();
  return 0;
}