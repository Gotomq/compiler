// Online C++ compiler to run C++ online.
// Write C++ code in this online editor and run it.
#include <cmath>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

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
std::map<std::string, Value> sym;

class Node;
class NFunctionDeclaration;
class NStatement;
class NExpression;
class NBlock;
class NInt;
class NDouble;
class NString;
class NIdentifier;
class NAssignment;
class NArithmetic;

class Node {};

class NFunction : public Node {
 public:
  Dtype fn_type;
  NIdentifier &id;
  NExpression *assignmentExpr;
};

class NStatement : public Node {};

class NExpression : public Node {
 public:
  Dtype node_type;
  NExpression(Dtype dt = undefined_dtype) : node_type(dt) {}
  virtual Value value() = 0;
};
void get_node_type(NExpression &n);

class NBlock : public NExpression {
 public:
  std::vector<NStatement *> statements;
};

class NInt : public NExpression {
 public:
  long long val;
  NInt(long long v) : val(v), NExpression(int_dtype) {}
  virtual Value value() {
    Value res;
    res.int_v = val;
    return res;
  }
};

class Ndouble : public NExpression {
 public:
  double val;
  Ndouble(double v) : val(v), NExpression(double_dtype) {}
  virtual Value value() {
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
  virtual Value value() {
    res.string_v = (char *)val.c_str();
    return res;
  }
};

class NIdentifier : public NExpression {
 public:
  std::string name;
  //   Value& val;
  std::map<std::string, Value> *sym_;
  //   Value res;
  //   NIdentifier(std::string &&n, dtype type, std::map<std::string, Value>
  //   &st)
  //       : name(n), NExpression(type), sym_(st) {}
  NIdentifier(std::string &n, std::map<std::string, Value> *st = nullptr,
              Dtype type = undefined_dtype)
      : name(n), sym_(st), NExpression(type) {}
  void setType(Dtype type) { node_type = type; }
  void setValue(Value val, Dtype type) {
    if (type == node_type)
      (*sym_)[name] = val;
    else {
      get_node_type(*this);
      fprintf(stderr, "Not the same value data types\n");
    }
  }
  virtual Value value() { return (*sym_)[name]; }
};

class NAssignment : public NExpression {
 public:
  NIdentifier &left;
  NExpression &right;
  //   NAssignment(NIdentifier &l, NExpression &r) : left(l), right(r) {
  //     left.setValue(right.value(), right.node_type);
  //   }
  NAssignment(NIdentifier &l, NExpression &r) : left(l), right(r) {
    left.setValue(right.value(), right.node_type);
  }
  virtual Value value() { return left.value(); }
};

class NArithmetic : public NExpression {
 public:
  NExpression &left, &right;
  AriOP OP;
  Value res;
  NArithmetic(AriOP op, NExpression &l, NExpression &r)
      : OP(op), left(l), right(r), NExpression(l.node_type) {}
  ~NArithmetic() {
    if (node_type == string_dtype) delete[] res.string_v;
  }
  virtual Value value() {
    switch (OP) {
      case SUB:
        if (left.node_type == int_dtype) {
          if (right.node_type == double_dtype)
            res.int_v = left.value().int_v - right.value().double_v;
          else
            res.int_v = left.value().int_v - right.value().int_v;
        } else if (left.node_type == double_dtype) {
          if (right.node_type == int_dtype)
            res.double_v = left.value().double_v - right.value().int_v;
          else
            res.double_v = left.value().double_v - right.value().double_v;
        }
        break;
      case ADD:
        if (left.node_type == int_dtype) {
          if (right.node_type == double_dtype)
            res.int_v = left.value().int_v + right.value().double_v;
          else
            res.int_v = left.value().int_v + right.value().int_v;
        } else if (left.node_type == double_dtype) {
          if (right.node_type == int_dtype)
            res.double_v = left.value().double_v + right.value().int_v;
          else
            res.double_v = left.value().double_v + right.value().double_v;
        } else if (left.node_type == string_dtype) {
          //   std::string *tmp = new std::string(left.value().string_v);
          //   tmp->append(right.value().string_v);
          //   res.string_v = (char *)tmp->c_str();
          char *tmp = new char;
          strcat(tmp, left.value().string_v);
          strcat(tmp, right.value().string_v);
          res.string_v = tmp;
        }
        break;
      case MULT:
        if (left.node_type == int_dtype) {
          if (right.node_type == double_dtype)
            res.int_v = left.value().int_v * right.value().double_v;
          else
            res.int_v = left.value().int_v * right.value().int_v;
        } else if (left.node_type == double_dtype) {
          if (right.node_type == int_dtype)
            res.double_v = left.value().double_v * right.value().int_v;
          else
            res.double_v = left.value().double_v * right.value().double_v;
        }
        break;
      case DIV:
        if (left.node_type == int_dtype) {
          if (right.node_type == double_dtype)
            res.int_v = left.value().int_v / right.value().double_v;
          else
            res.int_v = left.value().int_v / right.value().int_v;
        } else if (left.node_type == double_dtype) {
          if (right.node_type == int_dtype)
            res.double_v = left.value().double_v / right.value().int_v;
          else
            res.double_v = left.value().double_v / right.value().double_v;
        }
        break;
      case POW:
        if (left.node_type == int_dtype) {
          if (right.node_type == double_dtype)
            res.int_v = pow(left.value().int_v, right.value().double_v);
          else
            res.int_v = pow(left.value().int_v, right.value().int_v);
        } else if (left.node_type == double_dtype) {
          if (right.node_type == int_dtype)
            res.double_v = pow(left.value().double_v, right.value().int_v);
          else
            res.double_v = pow(left.value().double_v, right.value().double_v);
        }
        break;
      default:
        std::cout << "?\n";
        break;
    }
    return res;
  };
};

void get_node_type(NExpression &n) {
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

// void Interpreter(Node &node) {}
// long long Interpreter(NInt &node) { return node.val; }
// double Interpreter(Ndouble &node) { return node.val; }

int main() {
  // (1+2)**(3+2) Arithmetic Expression
  Ndouble *var1 = new Ndouble(1);
  Ndouble *var2 = new Ndouble(2);

  Ndouble *var3 = new Ndouble(3);
  Ndouble *var4 = new Ndouble(2);

  NArithmetic *var5 = new NArithmetic(ADD, *var1, *var2);
  NArithmetic *var6 = new NArithmetic(ADD, *var3, *var4);

  NArithmetic *var7 = new NArithmetic(POW, *var5, *var6);
  std::cout << "Expression: " << var7->value().double_v << "\n";

  //   IDENTIFIER DECLARATION
  std::string varname = "id1";
  NIdentifier id1(varname, &sym);
  id1.setType(double_dtype);
  NAssignment asg1(id1, *var7);
  //   IDENTIFIER ASSIGNMENT
  std::cout << "Assignment: " + id1.name << "=" << id1.value().double_v << "\n";

  //   INCREMENT VARIABLE
  NInt dbl1(2);
  NArithmetic incr(POW, id1, dbl1);
  NAssignment asg2(id1, incr);
  std::cout << "Assignment: " + id1.name << "=" << id1.value().double_v << "\n";

  delete var1, var2, var3, var4, var5, var6, var7;
  return 0;
}
