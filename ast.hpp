#pragma once
#include <cmath>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>

typedef struct Value {
  long long int_v;
  double double_v;
  char *string_v;
  void *undef_v = nullptr;
} Value;

typedef enum AriOP { SUB, ADD, MULT, DIV, POW } AriOP;

typedef enum Dtype {
  int_dtype,
  double_dtype,
  string_dtype,
  void_dtype,
  undefined_dtype
} Dtype;

typedef struct ValueScope {
  Value value;
  int scope;
  Dtype type;
} ValueScope;

// typedef std::vector<NVariableDefinition *> variable_list;

class Node;
class NFunction;
class NFunctionV2;
class NStatement;
class NStatementV2;
class NExpression;
class NBlock;
class NBlockV2;
class NInt;
class NDouble;
class NString;
class NIdentifier;
class NIdentifierV2;
class NAssignment;
class NArithmetic;
class NArithmeticV2;
class NVariableDefinition;
class NExpressionFromStatement;

// typedef struct FunctionValue {
//   NFunction *fn;
// } FunctionValue;

static std::map<std::string, ValueScope> global_table;
static std::map<std::string, NFunctionV2 *> global_fn_table;
static int global_scope = 1;
// Value TMP;

typedef std::vector<NStatement *> statement_list;
typedef std::vector<NExpressionFromStatement *> statement_list_v2;
typedef std::vector<NExpression *> expression_list;
typedef std::vector<NIdentifier *> variable_list;
typedef std::vector<NVariableDefinition *> variable_listv2;
typedef std::vector<NVariableDefinition *> fn_variable_list;
typedef std::vector<NFunctionV2 *> FunctionList;

class Node {
 public:
};

class NStatement : public Node {
 public:
  expression_list &expressions;
  NStatement(expression_list &expr) : expressions(expr) {}
};

class NStatementV2 : public Node {};

class NBlock : public Node {
 public:
  statement_list &statements;
  NBlock(statement_list &stmt) : statements(stmt) {}
  // std::map<std::string, Value> block_table;
};

class NBlockV2 : public Node {
 public:
  statement_list_v2 statements;
  NBlockV2() {}
  // virtual Value code_value() {}
  // virtual Value value() {}
  // std::map<std::string, Value> block_table;
};

class NExpression : public Node {
 public:
  Dtype node_type;
  NExpression(Dtype dt = undefined_dtype) : node_type(dt) {}
  // virtual Value value(){};
  virtual Value code_value() = 0;
};

static void get_node_type(NExpression &n) {
  switch (n.node_type) {
    case int_dtype:
      std::cout << "{data-type: int}\n";
      break;
    case double_dtype:
      std::cout << "{data-type: double}\n";
      break;
    case string_dtype:
      std::cout << "{data-type: string}\n";
      break;
    default:
      std::cout << "{data-type: undefined}\n";
      break;
  }
}

class NInt : public NExpression {
 public:
  long long val;
  NInt(long long v) : val(v), NExpression(int_dtype) {}

  virtual Value code_value() {
    // std::cout << "passed: NInt code_value\n";
    Value res;
    res.int_v = val;
    return res;
  }
};

class NDouble : public NExpression {
 public:
  double val;
  NDouble(double v) : val(v), NExpression(double_dtype) {}

  virtual Value code_value() {
    Value res;
    res.double_v = val;
    return res;
  }
};

class NString : public NExpression {
 public:
  std::string val;
  NString(std::string v) : val(v), NExpression(string_dtype) {}
  Value res;

  virtual Value code_value() {
    res.string_v = (char *)val.c_str();
    return res;
  }
};

class NFunctionV2 : public Node {
 public:
  NExpression *return_exprs;
  Dtype fn_type;
  NIdentifierV2 &id;
  variable_listv2 &arguments;
  NBlockV2 &block;
  NFunctionV2(Dtype type, NIdentifierV2 &id, variable_listv2 &args,
              NBlockV2 &node_block, NExpression *return_exprs_ = nullptr)
      : fn_type(type),
        id(id),
        arguments(args),
        block(node_block),
        return_exprs(return_exprs_) {
    // global_fn_table[id.name] = this;
  }
};

class NIdentifier : public NExpression {
 public:
  std::string name;
  int scope;
  std::map<std::string, ValueScope> *sym_;
  std::map<std::string, NFunction *> *sym_f;
  /* 1 val, 0 for functions */
  int type_of_id;
  /* VALUES */
  NIdentifier(std::string &n, int scope,
              std::map<std::string, ValueScope> *st = nullptr,
              Dtype type = undefined_dtype)
      : name(n), sym_(st), scope(scope), NExpression(type) {
    type_of_id = 1;
  }
  NIdentifier(std::string &&n, int scope,
              std::map<std::string, ValueScope> *st = nullptr,
              Dtype type = undefined_dtype)
      : name(n), sym_(st), scope(scope), NExpression(type) {
    type_of_id = 1;
  }
  /* FUNCTIONS */
  NIdentifier(std::string &&n, int scope,
              std::map<std::string, NFunction *> *st = nullptr,
              Dtype type = undefined_dtype)
      : name(n), sym_f(st), scope(scope), NExpression(type) {
    type_of_id = 0;
  }
  NIdentifier(std::string &n, int scope,
              std::map<std::string, NFunction *> *st = nullptr,
              Dtype type = undefined_dtype)
      : name(n), sym_f(st), scope(scope), NExpression(type) {
    type_of_id = 0;
  }

  void setType(Dtype type) { node_type = type; }
  void setValue(Value val, Dtype type) {
    if (type == node_type && type_of_id == 1) {
      ValueScope tmp;
      tmp.scope = scope;
      tmp.value = val;
      (*sym_)[name] = tmp;
    } else {
      get_node_type(*this);
      fprintf(stderr, "Not the same value data types\n");
    }
  }
  // virtual Value value() {
  //   // if (type_of_id == 1)

  //   return ((*sym_)[name]).value;
  //   // else
  //   //   return (*sym_f)[name]->tmpval;
  // }
};

class NIdentifierV2 : public NExpression {
 public:
  std::string name;
  int scope;
  // ValueScope &val;
  /* VALUES */
  NIdentifierV2(std::string &n, int scope, Dtype type = undefined_dtype)
      : name(n), scope(scope), NExpression(type) {
    if (node_type != undefined_dtype) {
      std::cout << "save type\n";
      ValueScope tmp;
      tmp.scope = scope;
      tmp.type = node_type;
      /* value not defined yet */
      global_table[name] = tmp;
    }
    /* check if its already declared in scope */
    else if (global_table.find(n) != global_table.end()) {
      std::cout << "re type\n";
      node_type = global_table[name].scope == scope ? global_table[name].type
                                                    : undefined_dtype;
    }
    // val.value = sco;
  }
  NIdentifierV2(std::string &&n, int scope, Dtype type = undefined_dtype)
      : name(n), scope(scope), NExpression(type) {
    if (node_type != undefined_dtype) {
      // std::cout << "save type\n";
      ValueScope tmp;
      tmp.scope = scope;
      tmp.type = node_type;
      /* value not defined yet */
      global_table[name] = tmp;
    }
    /* check if its already declared in scope */
    else if (global_table.find(n) != global_table.end()) {
      // std::cout << "re type\n";
      node_type = global_table[name].scope == scope ? global_table[name].type
                                                    : undefined_dtype;
    }
  }

  void setType(Dtype type) { node_type = type; }
  void setValue(Value val, Dtype type) {
    ValueScope tmp;
    tmp.scope = scope;
    tmp.value = val;
    global_table[name] = tmp;
  }
  // virtual Value value() { return global_table[name].value; }
  virtual Value code_value() { return global_table[name].value; }
};

class NAssignmentV2 : public NExpression {
 public:
  NIdentifier &left;
  NExpression &right;
  NAssignmentV2(NIdentifier &l, NExpression &r)
      : left(l), right(r), NExpression(l.node_type) {}
  virtual Value code_value() { return right.code_value(); }
};

class NArithmeticV2 : public NExpression {
 public:
  AriOP op;
  NExpression &left;
  NExpression &right;
  NArithmeticV2(NExpression &lhs, AriOP op, NExpression &rhs)
      : left(lhs), right(rhs), op(op), NExpression(lhs.node_type) {}
  virtual Value code_value() {
    Value res;
    switch (op) {
      case SUB:
        if (left.node_type == int_dtype) {
          if (right.node_type == double_dtype)
            res.int_v = left.code_value().int_v - right.code_value().double_v;
          else
            res.int_v = left.code_value().int_v - right.code_value().int_v;
        } else if (left.node_type == double_dtype) {
          if (right.node_type == int_dtype)
            res.double_v =
                left.code_value().double_v - right.code_value().int_v;
          else
            res.double_v =
                left.code_value().double_v - right.code_value().double_v;
        }
        break;
      case ADD:
        if (left.node_type == int_dtype) {
          if (right.node_type == double_dtype)
            res.int_v = left.code_value().int_v + right.code_value().double_v;
          else
            res.int_v = left.code_value().int_v + right.code_value().int_v;

        } else if (left.node_type == double_dtype) {
          if (right.node_type == int_dtype)
            res.double_v =
                left.code_value().double_v + right.code_value().int_v;
          else
            res.double_v =
                left.code_value().double_v + right.code_value().double_v;
        } else if (left.node_type == string_dtype) {
          char *tmp = new char;
          strcat(tmp, left.code_value().string_v);
          strcat(tmp, right.code_value().string_v);
          res.string_v = tmp;
        }
        break;
      case MULT:
        if (left.node_type == int_dtype) {
          if (right.node_type == double_dtype)
            res.int_v = left.code_value().int_v * right.code_value().double_v;
          else
            res.int_v = left.code_value().int_v * right.code_value().int_v;
        } else if (left.node_type == double_dtype) {
          if (right.node_type == int_dtype)
            res.double_v =
                left.code_value().double_v * right.code_value().int_v;
          else
            res.double_v =
                left.code_value().double_v * right.code_value().double_v;
        }
        break;
      case DIV:
        if (left.node_type == int_dtype) {
          if (right.node_type == double_dtype)
            res.int_v = left.code_value().int_v / right.code_value().double_v;
          else
            res.int_v = left.code_value().int_v / right.code_value().int_v;
        } else if (left.node_type == double_dtype) {
          if (right.node_type == int_dtype)
            res.double_v =
                left.code_value().double_v / right.code_value().int_v;
          else
            res.double_v =
                left.code_value().double_v / right.code_value().double_v;
        }
        break;
      case POW:
        if (left.node_type == int_dtype) {
          if (right.node_type == double_dtype)
            res.int_v =
                pow(left.code_value().int_v, right.code_value().double_v);
          else
            res.int_v = pow(left.code_value().int_v, right.code_value().int_v);
        } else if (left.node_type == double_dtype) {
          if (right.node_type == int_dtype)
            res.double_v =
                pow(left.code_value().double_v, right.code_value().int_v);
          else
            res.double_v =
                pow(left.code_value().double_v, right.code_value().double_v);
        }
        break;
      default:
        std::cout << "?? Arithmetic\n";
        break;
    }
    return res;
  }
};

class NVariableDefinition : public NExpression {
 public:
  /* we still dont know id type */
  NIdentifierV2 &id;
  NExpression *assignmentExpr;
  NVariableDefinition(NIdentifierV2 &id_, NExpression *asig = nullptr,
                      Dtype type = undefined_dtype)
      : id(id_), assignmentExpr(asig), NExpression(type) {
    if (asig != nullptr) {
      node_type = assignmentExpr->node_type;
      id.setType(assignmentExpr->node_type);
      id.setValue(assignmentExpr->code_value(), assignmentExpr->node_type);
    }
  }
  /* if there is expression that is added at later point */
  void setTypeValueFromExpression() {
    ValueScope tmp;
    tmp.value = assignmentExpr->code_value();
    tmp.type = assignmentExpr->node_type;
    tmp.scope = id.scope;
    node_type = assignmentExpr->node_type;
    id.setType(assignmentExpr->node_type);
    id.setValue(assignmentExpr->code_value(), assignmentExpr->node_type);
    global_table[id.name] = tmp;
  }
  void setType_lst(Dtype type) {
    node_type = type;
    id.setType(node_type);
  }
  void setExpression(NExpression *asig) { assignmentExpr = asig; }
  // virtual Value value() { return id.value(); }
  virtual Value code_value() {
    Value garbage;
    if (assignmentExpr) {
      void setTypeValueFromExpression();
      return id.code_value();
    }
    return garbage;
  }
};

class NExpressionFromStatement : public NStatementV2 {
 public:
  NExpression *expression;
  variable_listv2 *variabledefinitions;
  NExpressionFromStatement(NExpression *expression) : expression(expression){};
  NExpressionFromStatement(variable_listv2 *vars) : variabledefinitions(vars) {}
};

class NCallFunction : public NExpression {
 public:
  NIdentifierV2 &id;
  expression_list &argument_values;
  // NFunctionV2 *ref;
  NFunctionV2 *fn;
  NCallFunction(NIdentifierV2 &id_, expression_list &args)
      : id(id_), argument_values(args) {}
  // NCallFunction(NIdentifierV2 &id_) : id(id_) {}
  virtual Value code_value() {
    Value undefined;
    /* Check if function needs parameters */
    fn = global_fn_table[id.name];
    // std::cout << "function reached ... " + fn->id.name << "\n";
    if (argument_values.size() > 0) {
      // std::cout << "number of args: " << fn->arguments.size() << "\n";
      /* setting expression to function arguments */
      auto args_expr = argument_values.begin();
      for (auto &args_fn : fn->arguments) {
        args_fn->assignmentExpr = *args_expr;
        // args_fn->setTypeValueFromExpression();
        // std::cout << fn->arguments[0]->id.code_value().int_v << "\n";
        args_expr++;
      }
    }
    /* Executing function */
    NExpression *returnexpr = fn->return_exprs;
    auto &statements = fn->block.statements;
    for (auto &stmt : statements) {
      NExpression *expr;
      variable_listv2 *var_definitions;
      expr = stmt->expression;
      var_definitions = stmt->variabledefinitions;
      if (expr) {
        if (expr == returnexpr) {
          // std::cout << "reached return\n";
          return returnexpr->code_value();
        } else {
          Value temp = expr->code_value();
          show_value(expr->node_type, temp);
        }
      }
      if (var_definitions) {
        /* executing var_definitions */
        for (auto &def : *var_definitions) {
          NExpression *hasExpr = def->assignmentExpr;
          if (hasExpr) {
            Value result = def->code_value();
            show_value(def->id.node_type, result);
          } else {
            def->code_value();
          }
        }
      }
    }
    std::cout << "reached undefined\n";
    return undefined;
  }
  void show_value(Dtype type, Value &val) {
    switch (type) {
      case int_dtype:
        std::cout << val.int_v << "\n";
        break;
      case double_dtype:
        std::cout << val.double_v << "\n";
        break;
      case string_dtype:
        std::cout << val.string_v << "\n";
        break;
      case undefined_dtype:
        std::cout << val.undef_v << "\n";
        break;
      default:
        break;
    }
  }
};
class Program {
 public:
  FunctionList *list;
  NFunctionV2 *mainFn;
  void init_main() {
    /* Main function does not have parameters
     Executing statements */
    auto &statements = mainFn->block.statements;
    for (auto &stmt : statements) {
      /* Execute expressions and define variables only if not null */
      NExpression *expr;
      variable_listv2 *var_definitions;
      expr = stmt->expression;
      var_definitions = stmt->variabledefinitions;
      if (expr != nullptr) {
        /* show type of exprssion */
        get_node_type(*expr);
        Value result = expr->code_value();
        show_value(expr->node_type, result);
      } else if (var_definitions != nullptr) {
        /* there can be many definitions with the same type */
        for (auto &def : *var_definitions) {
          /* checking if it has an assignment expression */
          // std::cout << "var[" + def->id.name + "]\n";
          NExpression *hasExpr = def->assignmentExpr;
          // std::cout << hasExpr->code_value().int_v << "\n";
          // get_node_type(*hasExpr);
          // std::cout << hasExpr->code_value().int_v << "\n";
          if (hasExpr) {
            Value result = def->code_value();
            show_value(def->id.node_type, result);
          } else {
            def->code_value();
          }
        }
      }
    }
  }
  void show_value(Dtype type, Value &val) {
    switch (type) {
      case int_dtype:
        std::cout << val.int_v << "\n";
        break;
      case double_dtype:
        std::cout << val.double_v << "\n";
        break;
      case string_dtype:
        std::cout << val.string_v << "\n";
        break;
      case undefined_dtype:
        std::cout << val.undef_v << "\n";
        break;
      default:
        break;
    }
  }
  void describe_main_fn() {
    // main arguments
    std::cout << "main args\n";
    for (const auto &mn : mainFn->arguments) {
      std::cout << mn->id.name << " : ";
      get_node_type(mn->id);
    }
    for (auto &args : mainFn->arguments) {
      std::cout << args->id.name << "="
                << global_table[args->id.name].value.int_v << "\n";
    }
  }
};
