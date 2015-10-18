#include <string>
/*
 *  CLASS DEFINITIONS:
 */


#define LINE_LEN 100
class fstring {
public:
   char c[LINE_LEN];

   fstring();
};

/*
 *   EXPR
 */

class expr {
public:
   expr *down=nullptr, *next=nullptr, *back=nullptr;
   std::string symbol;
   union {
      double f;
      int i;
      fstring *s;
   } value;
   int pos = 0;

   expr( std::string);
   expr( const char *);
   void append( expr *);
   expr *copy();
   expr *find( const char *);
   void init_head();
   int hd();
   int is( const char *);
   int is2( const char *, const char *);
   int is3( const char *, const char *, const char *);
};

/*
 *   FORMS
 */

#define MAX_NUM_FORMS 10
#define MAX_NUM_VARS_IN_FORMS 10

class forms {
   struct {
      int num_args;
      char oldf[LINE_LEN];
      char newf[LINE_LEN];
   } f[MAX_NUM_FORMS];

   double x_vars[MAX_NUM_VARS_IN_FORMS];
   int num_vars;

public:

   int defined( int i);
   void display( int i);
   void init();
   void remove( int i);
   void set( int i, char *fs);

   void clear_vars();
   void add_var( double x);
   void type_vars( int i);
};


/*
 *   STEPS
 */

#define MAX_NUM_STEPS 1000

class step {
public:
   int step_number;
   char line[LINE_LEN];
   expr *e;

   void clear();
   void display();
   void move( step *s);
   void set( int number, char *t, expr *f);
};

class steps {
   step st[MAX_NUM_STEPS];
   int num_steps;
public:

   void init();
   void insert( int step_number, char *s, expr *e); 
   step * remove( step *s);
   void remove_all();

   step *get( int step_number);
   step *get_next( int step_number);
   step *next( step *s);
};

/*
 *   ELEMENTS
 */

class element {
public:
   element *next;
   int i1, i2;
   double value;

   element();
};

/*
 *   VARS
 */

#define MAX_NUM_VARS 52
class vars {
   char formula[LINE_LEN];
public:
   int name;
   int type;
   int num_args;
   double value;
   expr *f;
   element *e_list;
   fstring line;

   vars();
   void init( int name);
   int get_value( double *x, char *prompt);
   void set_value( double x);
   void set_formula( expr *e, char *s);
   element *find_element( int j1, int j2, int demand_switch, char *prompt);
   void clear();
   void display();
};

/* Functions */

void alter_function( expr *f, int num_args, char *var_list);
void delete_exp( expr *e);
int demand( char *prompt, double *x);
int eval( expr *s, double *x_result, int *i_result);
void eval_error( const char *s);
int eval_iter( expr *s, expr *t, double *x_result, int *i_result,
               const std::string &symbol);
element *eval_ref( expr *s, int demand_switch, char *prompt);
vars *find_var( int name);
void init_eval();
void init_steps();
void init_vars();
void join( expr *e1, expr *e2);
void join_down( expr *e1, expr *e2);
int parse( int goal, expr *e);
void parse_error( int pos, const char *message);
int read_line( const char *prompt);
int scan();
void short_display( expr *e, char *s);
expr *split( expr *e);
expr *split_at( expr *e);
expr *split_down( expr *e);
void tree_print( int level, expr *e);
void trim( char *s);

/*  Parse goals. */

#define COMMAND       1
#define SUBCOMMAND    2
#define ITERATION     3
#define RANGE         4
#define PREDLIST      5
#define PREDICATE     6
#define EXPLIST       7
#define EXPRESSION    8
#define SUBEXPRESSION 9

