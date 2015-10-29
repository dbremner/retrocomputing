#include <stdio.h>
#include <string.h>
#include "joss.hpp"

extern int DEBUG;
extern char input_line[];
extern vars V[MAX_NUM_VARS];
extern "C" void exit(int);

/*
 *  MEMBER FUNCTIONS
 */


/*
 *  EXPR
 */

expr::expr( std::string s) :
   symbol{s}
{
   value.f = 0.0f;
    
} 
 
expr::expr( const char *s) :
   symbol{s}
{
   value.f = 0.0;
}


void
expr::append( expr *e)
{
   e->next = next;
   e->back = this;
   e->next->back = e->back->next = e;
}


expr *
expr::copy()
{
   expr *e, *f, *g;

   if (!hd())
   {
      printf( "Error, copy must be called on the head node.\n");
      exit(0);
   }
   
   e = new expr( "head");
   e->init_head();

   for (f = next; !f->hd(); f = f->next)
   {
      g = new expr( f->symbol);

      /*  Copy the fields. */

      g->pos = f->pos;
      if (g->is( "string") || g->is( "name"))
      {
         if (f->value.s == nullptr)
            f->value.s = g->value.s;
         else
         {
            g->value.s = new fstring();
            strcpy( g->value.s->c, f->value.s->c);
         }
      }
      else if (g->is( "number"))
         g->value.f = f->value.f;
      else
         g->value.i = f->value.i;

      /*  Copy the subtree. */

      if (f->down != nullptr)
         g->down = f->down->copy();

      e->back->append( g);
   }
   return e;
}


expr *
expr::find( const char *s)
{
   expr *p;

   for (p = this;  !p->hd(); p = p->next)
   {
      if (p->is( s)) return(p);
   }
   return nullptr;
}


void
expr::init_head()
{
   symbol = "head";
   down = nullptr;
   next = back = this;
}


int
expr::hd()
{
   return symbol == "head";
}


int
expr::is( const char *s)
{
   if (symbol == s)
      return(1);
   else return(0);
}

int
expr::is2( const char *s1, const char *s2)
{
   if ((symbol == s1)
    && (next->symbol == s2))
      return(1);
   else return(0);
}

int
expr::is3( const char *s1, const char *s2, const char *s3)
{
   if (( symbol == s1)
    && ( next->symbol == s2)
    && ( next->next->symbol == s3) )
      return(1);
   else return(0);
}

fstring::fstring()
{
   c[0] = '\0';
}


/*
 *  FORMS
 */


int
forms::defined( int i)
{
   if (f[i-1].oldf[0] == '\0') return 0;
   return 1;
}


void
forms::display( int i)
{
   printf( "     Form %d: %c%s%c\n", i, '"', f[i-1].oldf, '"');
}


void
forms::init()
{
   for (int i=0; i<MAX_NUM_FORMS; i++)
   {
      f[i].num_args = 0;
      f[i].oldf[0] = '\0';
      f[i].newf[0] = '\0';
   }

   for (int i=0; i<MAX_NUM_VARS_IN_FORMS; i++)
      x_vars[i] = 0.0;

   num_vars = 0;
}


void
forms::remove( int i)
{
   f[i-1].num_args = 0;
   f[i-1].oldf[0] = '\0';
   f[i-1].newf[0] = '\0';
}


void
forms::set( int i, char *fs)
{
   f[i-1].num_args = 0;
   strcpy( f[i-1].oldf, fs);
   char *s = f[i-1].newf;

   for (char *t = fs; *t != '\0'; t++)
   {
      if (*t == '\\')
      {
         if (*(t+1) == '\\')
         {
            *s++ = *t++;
            continue;
         }
         if (*(t+1) == 'n')
         {
            *s++ = '\015';
            *s++ = '\012';
            t++;
            continue;
         }
         if (*(t+1) == 'f')
         {
            *s++ = '\014';
            t++;
            continue;
         }
         if (*(t+1) == 't')
         {
            *s++ = '\011';
            t++;
            continue;
         }
      }

      if (*t == '%' && *(t+1) == '%')
      {
         *s++ = *t++;
         continue;
      }

      if (*t == '%')
        f[i-1].num_args++;
      *s++ = *t;
   }
   *s = '\0';
}


void
forms::clear_vars()
{
   num_vars = 0;
}


void
forms::add_var( double x)
{
   x_vars[num_vars++] = x;
}


void
forms::type_vars( int i)
{
   if (f[i-1].num_args != num_vars)
   {
      printf( "     Error in typing in form %d.\n", i);
      printf( "     %d arg(s) expected, %d arg(s) given.\n",
              f[i-1].num_args, num_vars);
      return;
   }

   switch( num_vars)
   {
      case 0:  printf( f[i-1].newf);
               break;

      case 1:  printf( f[i-1].newf, x_vars[0]);
               break;

      case 2:  printf( f[i-1].newf, x_vars[0], x_vars[1]);
               break;

      case 3:  printf( f[i-1].newf, x_vars[0], x_vars[1], x_vars[2]);
               break;

      case 4:  printf( f[i-1].newf, x_vars[0], x_vars[1], x_vars[2],
                       x_vars[3]);
               break;

      case 5:  printf( f[i-1].newf, x_vars[0], x_vars[1], x_vars[2],
                       x_vars[3], x_vars[4]);
               break;

      case 6:  printf( f[i-1].newf, x_vars[0], x_vars[1], x_vars[2],
                       x_vars[3], x_vars[4], x_vars[5]);
               break;

      case 7:  printf( f[i-1].newf, x_vars[0], x_vars[1], x_vars[2],
                       x_vars[3], x_vars[4], x_vars[5], x_vars[6]);
               break;

      default:
      case 8:  printf( f[i-1].newf, x_vars[0], x_vars[1], x_vars[2],
                       x_vars[3], x_vars[4], x_vars[5], x_vars[6],
                       x_vars[7]);
               break;
   }
   num_vars = 0;
}


/*
 *   STEPS
 */


void
step::clear()
{
   step_number = 0;
   line[0] = '\0';
   if (e != nullptr)
   {
      delete_exp( e);
      e = nullptr;
   }
}


void
step::display()
{
   static char step_string[20];

   snprintf( step_string, sizeof(step_string), "%-.3f", ((float) step_number) / 1000.0);
   int len = strlen( step_string);

   char *s = &step_string[len-1];
   while (*s == '0') *s-- = ' ';

   printf( "     %s %s\n", step_string, line);
}


void
step::move( step *s)
{
   step_number = s->step_number;
   strcpy( line, s->line);
   e = s->e;

   s->step_number = 0;
   s->line[0] = '\0';
   s->e = nullptr;
}


void
step::set( int number, char *t, expr *f)
{
   step_number = number;
   if (t == nullptr)
      line[0] = '\0';
   else strcpy( line, t);
   e = f;
}


void
steps::init()
{
   for (int i=0; i<MAX_NUM_STEPS; i++)
   {
      st[i].set( 0, nullptr, nullptr);
   }
   num_steps = 0;
}


void
steps::insert( int step_number, char *s, expr *e)
{
   int i;

   for (i=0; i<num_steps; i++)
      if (st[i].step_number >= step_number) break;

   if (i < num_steps && st[i].step_number == step_number)
   {
      st[i].clear();
   }
   else
   {
      if (num_steps >= MAX_NUM_STEPS-1)
      {
         printf( "Step table overflow.");
         exit(0);
      }

      for (int j = num_steps-1; j >= i; j--)
         st[j+1].move( &st[j]);

      num_steps++;
   }
   st[i].set( step_number, s, e);
}


step *
steps::remove( step *s)
{
   int j = s - st;
   if (j < 0 || j >= num_steps) return nullptr;

   st[j].clear();

   for (int i = j+1; i<num_steps; i++)
      st[i-1].move( &st[i]);

   num_steps--;

   if (j < num_steps) return &st[j];
   else return nullptr;
}


void
steps::remove_all()
{
   for (int i=0; i<num_steps; i++) st[i].clear();
   num_steps = 0;
}


step *
steps::get( int step_number)
{
   for (int i=0; i<num_steps; i++)
   {
      if (st[i].step_number < step_number) continue;
      if (st[i].step_number > step_number) break;
      return &st[i];
   }
   return( nullptr);
}


step *
steps::get_next( int step_number)
{
   for (int i=0; i<num_steps; i++)
   {
      if (st[i].step_number > step_number) return &st[i];
   }
   return( nullptr);
}


step *
steps::next( step *s)
{
   int i = s - st;
   if (i<0 || i >= num_steps-1)
      return nullptr;
   return &st[i+1];
}


/*
 *    VARS
 */


vars::vars()
{
   formula[0] = '\0';
}


void
vars::init( int var_name)
{
   formula[0] = '\0';

   name = var_name;
   type = -1;
   value = 0.0;
   num_args = 0;

   f = nullptr;
   e_list = nullptr;
}


int
vars::get_value( double *x_result, char *prompt)
{
   static char line[LINE_LEN];

   *x_result = 0.0;

   if (type == 0)
   {
      *x_result = value;
      return 1;
   }

   if (type == -1)
   {
      if (prompt == nullptr)
      {
         snprintf( line, sizeof(line), "     %c = ", name);
         prompt = line;
      }
      if (!demand( prompt, x_result)) return 0;
      value = *x_result;
      type = 0;
      return 1;
   }

   if (type == 1 || type == 2)
      printf( "The variable %c is an array.\n");
   else if (type == 3)
      printf( "The variable %c is a defined function.\n");
   return 0;
}


void
vars::set_value( double x)
{
   type = 0;
   value = x;
}


void
vars::set_formula( expr *e, char *s)
{
   type = 3;
   strcpy( formula, s);
   f = e;
}


element *
vars::find_element( int j1, int j2, int demand_switch, char *prompt)
{
   element *t, *u, *v;
   double x;
   static char line[LINE_LEN];

   /*
    *  demand_switch = 0:   Find the element entry, and create
    *                       new element if necessary.
    *  demand_switch = 1:   In addition, if the element is new,
    *                       get its value from the user.
    *  demand_switch = 2:   Get the value from the user, whether it
    *                       it is new or not.
    */

   int new_element = 1;
   for (v = e_list, t = nullptr;
        v != nullptr;
        t = v, v = v->next)
   {
      if (v->i1 < j1) continue;
      if (v->i1 > j1) break;
      if (v->i2 < j2) continue;
      if (v->i2 > j2) break;

      new_element = 0;
      break;
   }

   if ((new_element && demand_switch == 1) || demand_switch == 2)
   {
      if (prompt == nullptr)
      {
         if (type == 1)
            snprintf( line, sizeof(line), "     %c[%d] = ", name, j1);
         else
            snprintf( line, sizeof(line), "     %c[%d,%d] = ", name, j1, j2);
         prompt = line;
      }

      if (!demand( prompt, &x)) return nullptr;
   }

   if (new_element)
   {
      u = new element();
      u->value = x;
      u->next = v;
      u->i1 = j1;
      u->i2 = j2;
      if (t == nullptr)
        e_list = u;
      else
        t->next = u;
      v = u;
   }

   if (demand_switch == 2 || (demand_switch == 1 && new_element))
      v->value = x;
   return v;
}


void
vars::clear()
{
   element *e1, *e2;

   for (e1 = e_list, e_list = nullptr;
        e1 != nullptr;
        e1 = e2)
   {
      e2 = e1->next;
      e1->next = nullptr;
      delete e1;
   }

   if (f != nullptr)
   {
      delete_exp( f);
      f = nullptr;
   }

   type = (-1);
   value = 0.0;
   formula[0] = '\0';
}


void
vars::display()
{
   switch( type)
   {
      case -1:
      default:
         return;

      case 0:
         printf( "     %c = %lg\n", name, value);
         break;

      case 1:
         for (element *e = e_list; e != nullptr; e = e->next)
         {
            printf( "     %c[%d] = %lg\n", name, e->i1, e->value);
         }
         break;

      case 2:
         for (element *e = e_list; e != nullptr; e = e->next)
         {
            printf( "     %c[%d,%d] = %lg\n", name, e->i1, e->i2,
                    e->value);
         }
         break;
      case 3:
         printf( "     %s\n", formula);
         break;
   }
}


/*
 *    OTHER FUNCTIONS.
 */


void
delete_exp( expr *e)
{
   if (e == nullptr) return;

   while (!e->hd()) e = e->back;

   e->back->next = nullptr;
   e->back = nullptr;

   expr *f;
   for (f = e; f != nullptr; f = f->next)
   {
      if (f->down != nullptr)
      {
         delete_exp( f->down);
         f->down = nullptr;
      }
   }

   while (e != nullptr)
   {
      f = e->next;
      e->next = nullptr;
      e->back = nullptr;
      if (e->is( "string") || e->is( "name"))
      {
         if (e->value.s != nullptr) delete e->value.s;
         e->value.s = nullptr;
      }
      delete e;
      e = f;
   }
}


int
demand( char *prompt, double *x)
{
   if (!read_line( prompt)) exit(0);

   char *t = input_line;
   while (*t == ' ') t++;
   if (strncmp( t, "Stop", 4) == 0 ||
       strncmp( t, "Cancel", 6) == 0) return 0;

   sscanf( t, "%lg", x);
   return 1;
}


vars *
find_var( int name)
{
   if ('a' <= name && name <= 'z')
      return( &V[name - 'a']);
   else if ('A' <= name && name <= 'Z')
      return( &V[26 + name - 'A']);
   else
   {
      printf( "Bad var name given: %d.\n", name);
      return( V);
   }
}


void
join( expr *e1, expr *e2)
{
   if (e2 == nullptr) return;

   if (e1 == nullptr || !e1->hd() || !e2->hd())
      printf( "join called, not at a header.\n" );

   if (e2->next == e2) return;

   expr *f1 = e1->back;
   expr *f2 = e2->back;
   f1->next = e2->next;
   e2->next->back = f1;
   f2->next = e1;
   e1->back = f2;

   e2->next = nullptr;
   e2->back = nullptr;
   e2->down = nullptr;
   delete e2;
}


void
join_down( expr *e1, expr *e2)
{
   if (e2 == nullptr) return;

   if (e1 == nullptr || e1->hd())
      printf( "join_down called at a null or header expression.\n");
   if (!e2->hd())
      printf( "join_down called, not at a header.\n" );

   if (e2->next == e2) return;

   if (e1->down == nullptr)
     e1->down = e2;
   else 
     join( e1->down, e2);
}


void
short_display( expr *e, char *s)
{
   if (e->is( "number"))
   {
      sprintf( s, "%g", e->value.f);
   }
   else if (e->is( "call") || e->is( "formula") || e->is( "ref")
         || e->is( "var"))
   {
      strncpy( s, e->symbol.c_str(), 3);
      s[3] = ':';
      s[4] = e->value.i;
      s[5] = '\0';
   }
   else if (e->is( "stack"))
   {
      strncpy( s, "stk:", 4);
      s[4] = '0' + e->value.i;
      s[5] = '\0';
   }
   else
   {
      strcpy( s, e->symbol.c_str());
   }
}


expr *
split( expr *q)
{
   expr *p = q->back;
   expr *head;
   for (head = q; !head->hd(); head = head->back) ;
   expr *r = head->back;

   expr *new_head = new expr( "head");

   if (q == head)
   {
      new_head->init_head();
   }
   else
   {
      new_head->next = q;
      q->back = new_head;
      new_head->back = r;
      r->next = new_head;
      head->back = p;
      p->next = head;
   }

   return( new_head);
}


expr *
split_at( expr *e)
{
   expr *q;

   if (e->next->hd())
     q = nullptr;
   else
     q = split( e->next);

   expr *p = split( e);
   delete_exp( p);
   return q;
}


expr *
split_down( expr *e)
{
   expr *f = e->down;
   e->down = nullptr;
   return f;
}


void
trim( char *s)
{
   char *t = s + strlen( s) - 1;
   do
   {
      if (*t == ' ' || *t == '\012' || *t == '\015') *t-- = '\0';
      else break;
   }
   while (t != s);

   return;
}
