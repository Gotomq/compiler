#ifndef AST
#define AST
typedef union Value {
  long long int_v;
  double double_v;
  char *string_v;
} Value;

typedef enum Dtype {
  int_dtype,
  double_dtype,
  string_dtype,
  void_dtype,
  undefined_dtype
} Dtype;
class Node {};
#endif