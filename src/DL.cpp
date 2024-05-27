#include "DL.h"
#include "util.h"

std::string op_to_string(OP op) {
  switch(op) {
    case LT:
      return "<";
    case LE:
      return "<=";
    case GT:
      return ">";
    case GE:
      return ">=";
    case EQ:
      return "=";
    case NEQ:
      return "!=";
  }
  error("Cannot recognize operator");
  return "";
}

bool Variable::operator==(const Variable& other) const {
  return name == other.name;
}

bool Term::operator==(const Term& other) const {
  return v1 == other.v1 && v2 == other.v2;
}

bool Constraint::operator==(const Constraint& other) const {
  return t1 == other.t1 && op == other.op && c2 == other.c2; 
}
