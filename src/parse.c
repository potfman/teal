#include"teal.h"

int group_add[] = {OP_ADD, OP_SUB};
int count_add = 2;

int group_mul[] = {OP_MUL, OP_DIV};
int count_mul = 2;

void expr();

void expr_factor() {
  if(this_ch('(')) {
    expr();
    exp_this_ch(')');
  } else {
    instr("push", exp_num());
  }
}

void expr_unary() {
  int o = these_op(group_add, count_add);
  expr_factor();
  if(o == OP_SUB) {
    instr("pop", REG_RAX);
    instrv("imul", REG_RAX, "-1");
    instr("push", REG_RAX);
  }
}

void expr_mul() {
  expr_unary();

  int o;
  while((o = these_op(group_mul, count_mul))) {
    expr_unary();
    instr("pop", REG_RDI);
    instr("pop", REG_RAX);

    if(o == OP_MUL) {
      instrr("imul", REG_RAX, REG_RDI);
    } else if (o == OP_DIV) {
      inst("cqo");
      instr("idiv", REG_RDI);
    } else {
      eputs("Unknown operator");
      sys_exit(1);
    }

    instr("push", REG_RAX);
  }
}

void expr_add() {
  expr_mul();

  int o;
  while((o = these_op(group_add, count_add))) {
    expr_mul();
    instr("pop", REG_RDI);
    instr("pop", REG_RAX);

    if(o == OP_ADD) {
      instrr("add", REG_RAX, REG_RDI);
    } else if (o == OP_SUB) {
      instrr("sub", REG_RAX, REG_RDI);
    } else {
      eputs("Unknown operator");
      sys_exit(1);
    }

    instr("push", REG_RAX);
  }
}

void expr() {
  expr_add();
}

void parse(char* buf) {
  init_lexer(buf);

  init_code();
  func("main");

  expr();

  instr("pop", REG_RAX);
  inst("ret");
}
