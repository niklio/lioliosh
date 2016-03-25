#include <math.h>
#include "mpc.h"

#ifdef _WIN32

static char buffer[2048];

char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

void add_history(char* unused) {}

#else
#include <editline/readline.h>
#endif

enum { LVAL_ERR, LVAL_NUM, LVAL_OPER, LVAL_OEXPR, LVAL_QEXPR };

typedef struct lval {
  int type;
  double num;
  char* err;
  char* oper;
  int count;
  struct lval** cell;
} lval;

lval* lval_num(double x) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_NUM;
  v->num = x;
  return v;
}

lval* lval_err(char* m) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_ERR;
  v->err = malloc(strlen(m) + 1);
  strcpy(v->err, m);
  return v;
}

lval* lval_oper(char* o) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_OPER;
  v->oper = malloc(strlen(o) + 1);
  strcpy(v->oper, o);
  return v;
}

lval* lval_oexpr(void) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_OEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

/* A pointer to a new empty Qexpr lval */
lval* lval_qexpr(void) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_QEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

void lval_del(lval* v) {

  switch (v->type) {
    case LVAL_NUM: break;
    case LVAL_ERR: free(v->err); break;
    case LVAL_OPER: free(v->oper); break;
    
    /* If Qexpr or Oexpr then delete all elements inside */
    case LVAL_QEXPR:
    case LVAL_OEXPR:
      for (int i = 0; i < v->count; i++) {
        lval_del(v->cell[i]);
      }
      /* Also free the memory allocated to contain the pointers */
      free(v->cell);
    break;
  }
  
  free(v);
}

lval* lval_prepend(lval* x, lval* v) {
  v->count++;
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);
  memmove(&v->cell[1], &v->cell[0], sizeof(lval*) * (v->count));
  v->cell[0] = x;
  return v;
}

lval* lval_append(lval* v, lval* x) {
  v->count++;
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);
  v->cell[v->count-1] = x;
  return v;
}

lval* lval_pop(lval* v, int i) {
  lval* x = v->cell[i];
  memmove(&v->cell[i], &v->cell[i+1], sizeof(lval*) * (v->count-i-1));
  v->count--;
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);
  return x;
}

lval* lval_join(lval* x, lval* y) {

  while (y->count) {
    x = lval_append(x, lval_pop(y, 0));
  }

  lval_del(y);  
  return x;
}

lval* lval_take(lval* v, int i) {
  lval* x = lval_pop(v, i);
  lval_del(v);
  return x;
}

void lval_print(lval* v);

void lval_expr_print(lval* v, char open, char close) {
  putchar(open);
  for (int i = 0; i < v->count; i++) {
    
    lval_print(v->cell[i]);
    
    if (i != (v->count-1)) {
      putchar(' ');
    }
  }
  putchar(close);
}

void lval_print(lval* v) {
  switch (v->type) {
    case LVAL_NUM:      printf("%g", v->num); break;
    case LVAL_ERR:      printf("Error: %s", v->err); break;
    case LVAL_OPER:     printf("%s", v->oper); break;
    case LVAL_OEXPR:    lval_expr_print(v, '(', ')'); break;
    case LVAL_QEXPR:    lval_expr_print(v, '{', '}'); break;
  }
}

void lval_println(lval* v) { lval_print(v); putchar('\n'); }

#define LASSERT(args, cond, err) if (!(cond)) { lval_del(args); return lval_err(err); }

lval* lval_eval(lval* v);

lval* builtin_list(lval* a) {
  a->type = LVAL_QEXPR;
  return a;
}

lval* builtin_head(lval* a) {
  LASSERT(a, a->count == 1, "Function 'head' takes exactly 1 argument.");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'head' passed incorrect type.");
  LASSERT(a, a->cell[0]->count != 0, "Function 'head' passed {}.");
  
  lval* v = lval_take(a, 0);  
  while (v->count > 1) { lval_del(lval_pop(v, 1)); }
  return v;
}

lval* builtin_tail(lval* a) {
  LASSERT(a, a->count == 1, "Function 'tail' takes exactly 1 argument.");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'tail' passed incorrect type.");
  LASSERT(a, a->cell[0]->count != 0, "Function 'tail' passed {}.");

  lval* v = lval_take(a, 0);  
  lval_del(lval_pop(v, 0));
  return v;
}

lval* builtin_eval(lval* a) {
  LASSERT(a, a->count == 1, "Function 'eval' takes exactly 1 argument.");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'eval' passed incorrect type.");
  
  lval* x = lval_take(a, 0);
  x->type = LVAL_OEXPR;
  return lval_eval(x);
}

lval* builtin_join(lval* a) {

  for (int i = 0; i < a->count; i++) {
    LASSERT(a, a->cell[i]->type == LVAL_QEXPR, "Function 'join' passed incorrect type.");
  }
  
  lval* x = lval_pop(a, 0);
  
  while (a->count) {
    x = lval_join(x, lval_pop(a, 0));
  }
  
  lval_del(a);
  return x;
}

lval* builtin_max(lval* a) {
    LASSERT(a, a->cell[0]->type == LVAL_NUM, "Function 'max' passed incorrect type.");

    int max = a->cell[0]->num;
    for (int i = 1; i < a->count; i++) {
        if (max < a->cell[i]->num) {
            max = a->cell[i]->num;
        }
    }
    return lval_num(max);
}

lval* builtin_min(lval* a) {
    LASSERT(a, a->cell[0]->type == LVAL_NUM, "Function 'min' passed incorrect type.");

    int min = a->cell[0]->num;
    for (int i = 1; i < a->count; i++) {
        if (min > a->cell[i]->num) {
            min = a->cell[i]->num;
        }
    }
    return lval_num(min);
}

lval* builtin_cons(lval* a) {
    LASSERT(a, a->count == 2, "Function 'cons' takes exactly 2 argument");
    LASSERT(a, a->cell[0]->type == LVAL_NUM, "Function 'cons' passed incorrect type.");
    LASSERT(a, a->cell[1]->type == LVAL_QEXPR, "Function 'cons' passed incorrect type.");
    return lval_prepend(a->cell[0], a->cell[1]);
}

lval* builtin_add(lval* a) {
    LASSERT(a, a->cell[0]->type == LVAL_NUM, "Function 'add' passed incorrect type.");
    lval* x = lval_pop(a, 0);
    while (a->count > 0) {
        lval* y = lval_pop(a, 0);
        x->num += y->num;
        lval_del(y);
    }
    lval_del(a);
    return x;
}

lval* builtin_sub(lval* a) {
    LASSERT(a, a->cell[0]->type == LVAL_NUM, "Function 'sub' passed incorrect type.");
    lval* x = lval_pop(a, 0);
    if (a->count == 0) {
        x->num = -x->num;
        return x;
    }
    while (a->count > 0) {
        lval* y = lval_pop(a, 0);
        x->num -= y->num;
        lval_del(y);
    }
    lval_del(a);
    return x;
}

lval* builtin_mul(lval* a) {
    LASSERT(a, a->cell[0]->type == LVAL_NUM, "Function 'mul' passed incorrect type.");
    lval* x = lval_pop(a, 0);
    while (a->count > 0) {
        lval* y = lval_pop(a, 0);
        x->num *= y->num;
        lval_del(y);
    }
    lval_del(a);
    return x;
}

lval* builtin_div(lval* a) {
    LASSERT(a, a->cell[0]->type == LVAL_NUM, "Function 'div' passed incorrect type.");
    lval* x = lval_pop(a, 0);
    while (a->count > 0) {
        lval* y = lval_pop(a, 0);
        if (y->num == 0) {
            lval_del(x); lval_del(y);
            x = lval_err("Division by zero.");
            break;
        }
        x->num /= y->num;
    }
    lval_del(a);
    return x;
}

lval* builtin_mod(lval* a) {
    LASSERT(a, a->cell[0]->type == LVAL_NUM, "Function 'mod' passed incorrect type.");
    lval* x = lval_pop(a, 0);
    while (a->count > 0) {
        lval* y = lval_pop(a, 0);
        if (y->num == 0) {
            lval_del(x); lval_del(y);
            x = lval_err("Division by zero.");
        }
        x->num = fmod((fmod(x->num, y->num) + y->num), y->num);
    }
    lval_del(a);
    return x;
}

lval* builtin_exp(lval* a) {
    LASSERT(a, a->cell[0]->type == LVAL_NUM, "Function 'exp' passed incorrect type.");
    lval* x = lval_pop(a, 0);
    while (a->count > 0) {
        lval* y = lval_pop(a, 0);
        x->num = powf(x->num, y->num);
    }
    lval_del(a);
    return x;
}

lval* builtin(lval* a, char* func) {
  if (strcmp("list", func) == 0) { return builtin_list(a); }
  if (strcmp("head", func) == 0) { return builtin_head(a); }
  if (strcmp("tail", func) == 0) { return builtin_tail(a); }
  if (strcmp("join", func) == 0) { return builtin_join(a); }
  if (strcmp("eval", func) == 0) { return builtin_eval(a); }

  if (strcmp("cons", func) == 0) { return builtin_cons(a); }

  if (strcmp("max", func) == 0) { return builtin_max(a); }
  if (strcmp("min", func) == 0) { return builtin_min(a); }
  if (strcmp("add", func) == 0 || strcmp("+", func) == 0) { return builtin_add(a); }
  if (strcmp("sub", func) == 0 || strcmp("-", func) == 0) { return builtin_sub(a); }
  if (strcmp("mul", func) == 0 || strcmp("*", func) == 0) { return builtin_mul(a); }
  if (strcmp("div", func) == 0 || strcmp("/", func) == 0) { return builtin_div(a); }
  if (strcmp("mod", func) == 0 || strcmp("%", func) == 0) { return builtin_mod(a); }
  if (strcmp("exp", func) == 0 || strcmp("^", func) == 0) { return builtin_exp(a); }

  lval_del(a);
  return lval_err("Unknown Function");
}

lval* lval_eval_oexpr(lval* v) {
  for (int i = 0; i < v->count; i++) {
    v->cell[i] = lval_eval(v->cell[i]);
  }
  
  for (int i = 0; i < v->count; i++) {
    if (v->cell[i]->type == LVAL_ERR) { return lval_take(v, i); }
  }
  
  if (v->count == 0) { return v; }
  
  if (v->count == 1) { return lval_take(v, 0); }
  
  lval* f = lval_pop(v, 0);
  if (f->type != LVAL_OPER) {
    lval_del(f); lval_del(v);
    return lval_err("Expression expected an operator.");
  }
  
  /* Call builtin with operator */
  lval* result = builtin(v, f->oper);
  lval_del(f);
  return result;
}

lval* lval_eval(lval* v) {
  if (v->type == LVAL_OEXPR) { return lval_eval_oexpr(v); }
  return v;
}

lval* lval_read_num(mpc_ast_t* t) {
  errno = 0;
  double x = strtod(t->contents, NULL);
  return errno != ERANGE ? lval_num(x) : lval_err("invalid number");
}

lval* lval_read(mpc_ast_t* t) {
  
  if (strstr(t->tag, "number")) { return lval_read_num(t); }
  if (strstr(t->tag, "operator")) { return lval_oper(t->contents); }
  
  lval* x = NULL;
  if (strcmp(t->tag, ">") == 0) { x = lval_oexpr(); }
  if (strstr(t->tag, "oexpr"))  { x = lval_oexpr(); }
  if (strstr(t->tag, "qexpr"))  { x = lval_qexpr(); }
  
  for (int i = 0; i < t->children_num; i++) {
    if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
    if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
    if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
    if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
    if (strcmp(t->children[i]->tag,  "regex") == 0) { continue; }
    x = lval_append(x, lval_read(t->children[i]));
  }
  
  return x;
}

int main(int argc, char** argv) {
  
  mpc_parser_t* Number      = mpc_new("number");
  mpc_parser_t* Operator    = mpc_new("operator");
  mpc_parser_t* Oexpr       = mpc_new("oexpr");
  mpc_parser_t* Qexpr       = mpc_new("qexpr");
  mpc_parser_t* Expr        = mpc_new("expr");
  mpc_parser_t* Lang        = mpc_new("lang");
  
  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                                               \
        number      : /-?[0-9]+[\\.[0-9]*]?/ ;                                      \
        operator    : \"list\" | \"head\" | \"tail\" | \"eval\" | \"join\"          \
                    | \"cons\"                                                      \
                    | \"min\" | \"max\" | \"add\" | \"sub\"                         \
                    | \"mul\" | \"div\" | \"mod\" | \"exp\"                                   \
                    | '+' | '-' | '*' | '/' |  '%' | '^' ;                          \
        oexpr       : '(' <expr>* ')' ;                                             \
        qexpr       : '{' <expr>* '}' ;                                             \
        expr        : <number> | <operator> | <oexpr> | <qexpr> ;                   \
        lang        : /^/ <expr>* /$/ ;                                             \
    ",
    Number, Operator, Oexpr, Qexpr, Expr, Lang);
  
  puts("Lioliosh Version 0.0.1");
  puts("Press Ctrl+c to Exit\n");
  
  while (1) {
  
    char* input = readline(">>> ");
    add_history(input);
    
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lang, &r)) {
      lval* x = lval_eval(lval_read(r.output));
      lval_println(x);
      lval_del(x);
      mpc_ast_delete(r.output);
    } else {    
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    
    free(input);
    
  }
  
  mpc_cleanup(6, Number, Operator, Oexpr, Qexpr, Expr, Lang);
  
  return 0;
}