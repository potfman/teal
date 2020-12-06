#include"teal.h"
#define MAX_BUFFER 50000
#define MAX_MARKER 100

char buf[MAX_BUFFER + 1];
char* cur;
char* marker[MAX_MARKER];
int tmp;

void init_lexer() {
  read_stdin(buf, MAX_BUFFER);
  cur = buf;

  int c = 0;
  while(c < MAX_MARKER) {
    marker[c] = NULL;
    c++;
  }

  tmp = 0;
}

int mark() {
  int c = 0;
  while(marker[c] != NULL && c != MAX_MARKER) {
    c++;
  }

  if(c == MAX_MARKER) {
    panic("Too many markers.");
  }

  marker[c] = cur;
  return c;
}

void jump(int c) {
  if(marker[c] == NULL) {
    panic("Use of unavailable marker.");
  }

  cur = marker[c];
  marker[c] = NULL;
}

void unmark(int c) {
  marker[c] = NULL;
}

int comment() {
  if(!(*cur == '/' && *(cur+1) == '/')) {
    return 0;
  }

  while(*cur != '\n') {
    cur++;
  }

  return 1;
}

void skip_space() {
  while(is_space(*cur) || comment()) {
    cur++;
  }
}

void panic_lex() {
  eput("Unexpected token: '");
  eputc(*cur);
  eputs("'.");
  sys_exit(1);
}

char lex_ch() {
  skip_space();
  char c = *cur;
  cur++;
  tmp = 1;
  return c;
}

char this_ch(char c) {
  if(lex_ch() == c) {
    return c;
  } else {
    cur-=tmp;
    return 0;
  }
}

char exp_this_ch(char c) {
  if(!this_ch(c)) {
    panic_lex();
  }
  return c;
}

int this_str(char* str) {
  skip_space();
  int len = length(str);
  if (str_cmp(cur, str, len)) {
    cur+=len;
    return 1;
  }

  return 0;
}

int exp_this_str(char* str) {
  int len = this_str(str);
  if(!len) {
    panic_lex();
    return 0;
  }

  return len;
}

int lex_num() {
  skip_space();
  return strtoi(cur, &cur);
}

int exp_num() {
  int i = lex_num();
  if(i == -1) {
    panic_lex();
  }
  return i;
}

int lex_ident() {
  skip_space();
  if(is_alpha(*cur)) {
    int len = identlen(cur);
    int id = lvar_get(cur, len, TY_I32);
    cur+=len;
    return id;
  } else {
    return 0;
  }
}

int exp_ident() {
  int i = lex_ident();
  if(!i) {
    panic_lex();
  }
  return i;
}

int at_eof() {
  skip_space();
  return !*cur;
}

void exp_eof() {
  if(!at_eof()) {
    panic_lex();
  }
}

int lex_op() {
  skip_space();
  tmp = 0;

  if(*cur == '+') {
    cur++;
    tmp++;
    if(*cur == '+') {
      cur++;
      tmp++;
      return OP_INC;
    } else if (*cur == '=') {
      cur++;
      tmp++;
      return OP_ADDASG;
    } else {
      return OP_ADD;
    }
  } else if (*cur == '-') {
    cur++;
    tmp++;
    if(*cur == '-') {
      cur++;
      tmp++;
      return OP_DEC;
    } else if (*cur == '=') {
      cur++;
      tmp++;
      return OP_SUBASG;
    } else {
      return OP_SUB;
    }
  } else if (*cur == '*') {
    cur++;
    tmp++;
    if(*cur == '=') {
      cur++;
      tmp++;
      return OP_MULASG;
    } else {
      return OP_MUL;
    }
  } else if (*cur == '/') {
    cur++;
    tmp++;
    if(*cur == '=') {
      cur++;
      tmp++;
      return OP_DIVASG;
    } else {
      return OP_DIV;
    }
  } else if (*cur == '%') {
    cur++;
    tmp++;
    if(*cur == '=') {
      cur++;
      tmp++;
      return OP_REMASG;
    } else {
      return OP_REM;
    }
  } else if (*cur == '=') {
    cur++;
    tmp++;
    if (*cur == '=') {
      cur++;
      tmp++;
      return OP_EQ;
    } else {
      return OP_ASG;
    }
  } else if(*cur == '!') {
    cur++;
    tmp++;
    if(*cur == '=') {
      cur++;
      tmp++;
      return OP_NEQ;
    } else {
      return OP_NOT;
    }
  } else if(*cur == '<') {
    cur++;
    tmp++;
    if (*cur == '=') {
      cur++;
      tmp++;
      return OP_LEQ;
    } else {
      return OP_LESS;
    }
  } else if(*cur == '>') {
    cur++;
    tmp++;
    if (*cur == '=') {
      cur++;
      tmp++;
      return OP_MEQ;
    } else {
      return OP_MORE;
    }
  } else if(*cur == '&') {
    cur++;
    tmp++;
    if (*cur == '&') {
      cur++;
      tmp++;
      return OP_AND;
    } else {
      cur--;
      tmp--;
      return OP_UNKNOWN;
    }
  } else if(*cur == '|') {
    cur++;
    tmp++;
    if (*cur == '|') {
      cur++;
      tmp++;
      return OP_OR;
    } else {
      cur--;
      tmp--;
      return OP_UNKNOWN;
    }
  } else {
    return OP_UNKNOWN;
  }
}

int exp_op() {
  int o = lex_op();
  if(!o) {
    panic_lex();
  }

  return o;
}

int this_op(int o) {
  if(lex_op() != o) {
    cur-=tmp;
    return 0;
  } else {
    return o;
  }
}

int these_op(int* os, int c) {
  int i = 0;
  while(i < c) {
    if(this_op(os[i])) {
      return os[i];
    }
    i++;
  }

  return 0;
}

int exp_this_op(int o) {
  if(!this_op(o)) {
    panic_lex();
  }

  return o;
}

int exp_these_op(int* os, int c) {
  int o = these_op(os, c);
  if(!o) {
    panic_lex();
  }

  return o;
}
