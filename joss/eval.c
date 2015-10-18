#include <stdio.h>
#include <string.h>
#include <math.h>
#include "joss.h"

double cos( double x);
double exp( double x);
double log( double x);
double pow( double x, double y);
double sqrt( double x);
double sin( double x);
double tan( double x);

extern int DEBUG;
extern expr *root;

forms F;
vars V[MAX_NUM_VARS];
steps S;

#define MAX_NUM_STACK 100
double stack[MAX_NUM_STACK];
int num_stack;

#define EVAL( A)  if (!eval( (A), &x, &i)) return 0;

#define EVALERROR( S) { eval_error( S);   return 0; }

step *pc, *pc_next;

/*
 *   TREE PRINT
 */


void
tree_print( int level, expr *e)
{
   static char lines[20 * LINE_LEN];
   static char buf[LINE_LEN];
   static int cursor;
   int i, j, len;

   if (level == 0)
   {
      printf( "Tree form:\n");
      cursor = 3;
      for (i=0; i<2000; i++) lines[i] = ' ';
   }

   while (!e->hd()) e = e->back;

   for (e = e->next;   !e->hd();   e = e->next)
   {

      if (level > 0) lines[(level-1)*LINE_LEN + cursor] = '|';

      short_display( e, buf);

      len = strlen( buf);
      strncpy( &lines[level*LINE_LEN + cursor], buf, len);
      cursor += len;

      if (e->down != ENULL)
         tree_print( level+1, e->down);
      else if (!e->next->hd())
         cursor++;
   }

   if (level == 0)
   {
      for (i=0; i<2000; i+=LINE_LEN)
      {
         lines[i+99] = '\0';
         for (j = 99; j >= 0; j--)
         {
            if (lines[i+j] == ' ')
               lines[i+j] = '\0';
            else if (lines[i+j] != '\0')
               break;
         }
         if (lines[i] != '\0')
            printf( "%s\n", &lines[i]);
      }
   }
}


void
init_vars()
{
   int i;

   for (i=0; i<26; i++)
   {
      V[i].init( 'a' + i);
      V[i+26].init( 'A' + i);
   }
}


void
init_eval()
{
   pc = pc_next = SNULL;
   num_stack = 0;
}


int
eval( expr *s, double *x_result, int *i_result)
{
   expr *p, *q;
   double x, y;
   int i;
   vars *v;
   static char message[80];

   if (s == ENULL)
      EVALERROR( "Evaluator called with a null expression.")

   if (s->hd()) s = s->next;

   if (DEBUG)
   {
      printf( "Evaluating:");

      for (p=s; ; p=p->next)
      {
         short_display( p, message);
         printf( " %s", message);
         if (p->hd()) break;
      }
      printf( "\n");
   }


   if (s->is( "abs"))
   {
      EVAL( s->down->next)
      if (x < 0) *x_result = (-x);
         else *x_result = x;
      return 1;
   }

   else if (s->is( "and"))
   {
      p = s->down->next;
      EVAL( p)
      if (i == 0)
      {
        *i_result = 0;   return 1;
      }
      p = p->next;
      EVAL( p)
      *i_result = i;
      return 1;
   }

   else if (s->is( "b-"))
   {
      p = s->down->next;
      EVAL( p)
      y = x;   EVAL(p->next)
      *x_result = y - x;   return 1;
   }

   else if (s->is( "call"))
   {
      int num_args, num_x_stack;
      auto double x_stack[10];

      v = find_var( s->value.i);
      if (v->type == (-1))
      {
         sprintf( message, "Formula %c is not defined.", v->name);
         EVALERROR( message)
      }
      if (v->type != 3)
      {
         sprintf( message, "%c is not a formula.", v->name);
         EVALERROR( message)
      }

      /*  Check that the number of arguments are the same. */

      num_args = 0;
      if (s->down != ENULL)
      {
         for (p = s->down->next; !p->hd(); p = p->next) num_args++;
      }
      if (num_args != v->num_args)
         EVALERROR( "Mismatched number of arguments.")

      if (num_args == 0)
      {
         EVAL( v->f)
         *x_result = x;
         return 1;
      }

      num_x_stack = 0;
      for (p = s->down->next; !p->hd(); p = p->next)
      {
         EVAL( p)
         x_stack[num_x_stack++] = x;
      }

      if (num_stack + num_x_stack >= MAX_NUM_STACK)
         EVALERROR( "Stack overflow.")
      for (i=0; i<num_x_stack; i++)
         stack[num_stack++] = x_stack[i];

      if (DEBUG)
      {
         printf( "     Evalulating %c(" /* ) */ , v->name);
         for (i=0; i<num_args; i++)
         {
            printf( "%g%c", stack[num_stack-1-i],
                            (i == num_args-1 ? /* ( */ ')' : ','));
         }
         printf( "\n");
      }
      EVAL( v->f)
      *x_result = x;
      num_stack -= num_args;
      return 1;
   }

   else if (s->is( "cond"))
   {
      for( p = s->down->next;   !p->hd();  p = p->next->next)
      {
         if (p->next->hd())
         {
            EVAL( p)
            *x_result = x;
            return 1;
         }

         EVAL(p)
         if (i == 0) continue;
         EVAL( p->next)
         *x_result = x;
         return 1;
      }
      EVALERROR( "Badly formed conditional.")
   }

   else if (s->is( "conj") || s->is( "disj"))
   {
      *i_result = (s->is( "conj") ? 1 : 0);
      if (s->down == ENULL) return 1;
      p = s->down->next;

      if (p->is( "iter"))
         return eval_iter( p, p->next, &x, i_result, s->symbol);

      for ( ; !p->hd(); p = p->next)
      {
         EVAL( p)
         *i_result = i;
         if ((s->is( "conj" ) && i == 0)
          || (s->is( "disj" ) && i == 1)) break;
      }
      return 1;
   }

   else if (s->is( "cos"))
   {
      EVAL(s->down->next)
      *x_result = cos( x);
      return 1;
   }

   else if (s->is( "Debug"))
   {
      DEBUG = 1 - DEBUG;
      return 1;
   }

   else if (s->is( "Delete"))
   {
      for (p = s->down->next;  !p->hd();   p = p->next)
      {
         if (p->is( "all"))
         {
            S.remove_all();

            for (i=1; i<=MAX_NUM_FORMS; i++)
               if (F.defined(i)) F.remove(i);

            for (i=0; i<MAX_NUM_VARS; i++)
               if (V[i].type != (-1)) V[i].clear();
         }
         else if (p->is( "allforms"))
         {
            for (i=1; i<=MAX_NUM_FORMS; i++)
               if (F.defined(i)) F.remove(i);
         }
         else if (p->is( "allformulas"))
         {
            for (i=0; i<MAX_NUM_VARS; i++)
               if (V[i].type == 3) V[i].clear();
         }
         else if (p->is( "allparts") || p->is( "allsteps"))
         {
            S.remove_all();
         }
         else if (p->is( "allvalues"))
         {
            for (i=0; i<MAX_NUM_VARS; i++)
               if (V[i].type >= 0 && V[i].type <= 2) V[i].clear();
         }
         else if (p->is( "form"))
         {
            EVAL( p->down->next)
            i = (int) (x + 0.5);
            if (i < 1 || i > MAX_NUM_FORMS)
               printf( "     Bad form number -- %d.\n", i);
            else
               F.remove(i);
         }
         else if (p->is( "formula"))
         {
            v = find_var( p->value.i);
            if (v->type != 3)
            {
               sprintf( message, "%c is not a formula.", v->name);
               EVALERROR( message)
            }
            v->clear();
         }
         else if (p->is( "part"))
         {
            int part;
            step *st;

            EVAL( p->down->next)
            part = 1000 * (int) (x + 0.5);
            st = S.get_next( part-1);
            while (st != SNULL)
            {
               if (st->step_number < part || st->step_number >= part+1000)
                  break;
               st = S.remove( st);
            }
         }
         else if (p->is( "step"))
         {
            step *st;

            EVAL( p->down->next)
            i = (int) (x * 1000.0 + 0.5);
            st = S.get( i);
            if (st == SNULL)
               printf( "     Step %-.3f is undefined.\n", (float) x);
            else
               st = S.remove( st);
         }
         else if (p->is( "var"))
         {
            v = find_var( p->value.i);
            v->clear();
         }
         else
         {
            sprintf( message, "Cannot delete %s.", p->symbol);
            EVALERROR( message)
         }
      }
      return 1;
   }

   else if (s->is( "Demand"))
   {
      element *el;

      for (p = s->down->next; !p->hd();
           p = p->next)
      {
         if (p->is( "var"))
         {
            v = find_var( p->value.i);
            v->clear();
            v->type = -1;
            v->get_value( &x, NULL);
         }
         else if (p->is( "ref"))
         {
            el = eval_ref( p, 2, NULL);
            if (el == ELNULL) return 0;
         }
         else
            EVALERROR( "You can demand only variables or array elements.")
      }
      return 1;
   }

   else if (s->is( "Demand_as"))
   {
      p = s->down->next;
      if (p->is( "var"))
      {
         v = find_var( p->value.i);
         v->clear();
         v->type = -1;
         v->get_value( &x, p->next->value.s->c);
         return 1;
      }

      if (p->is( "ref"))
      {
         element *el;

         el = eval_ref( p, 2, p->next->value.s->c);
         if (el == ELNULL) return 0;
         return 1;
      }

      EVALERROR( "You can demand only variables or array elements.")
   }

   else if (s->is( "Done"))
   {
      pc_next = SNULL;
      return 1;
   }

   else if (s->is( "Dopart"))
   {
      int part;
      step *save_pc, *save_pc_next;

      EVAL( s->down->next)
      part = 1000 * int (x + 0.5);
      save_pc = pc;   save_pc_next = pc_next;
      pc = S.get_next( part-1);

      while (pc != SNULL)
      {
         if (pc->step_number < part || pc->step_number >= part + 1000)
            break;
         p = pc->e;
         pc_next = S.next( pc);

         EVAL( p)
         pc = pc_next;
      }
      pc = save_pc;
      pc_next = save_pc_next;
      return 1;
   }

   else if (s->is( "Dostep"))
   {
      step *save_pc, *save_pc_next, *pc1;

      EVAL( s->down->next)
      i = (int) (x * 1000.0 + 0.5);
      save_pc = pc;   save_pc_next = pc_next;
      pc1 = S.get( i);
      if (pc1 == SNULL)
      {
         sprintf( message, "Cannot do step %-.3f -- undefined.", (float) x);
         EVALERROR( message)
      }
      pc = pc1;

      EVAL( pc->e)
      pc = save_pc;   pc_next = save_pc_next;
      return 1;
   }

   else if (s->is( "exp") || s->is( "flr") || s->is( "ceil"))
   {
      EVAL( s->down)
      if (s->is( "exp"))
      {
         *x_result = exp( x);   return 1;
      }
      *x_result = (float) (int) x;
      if (s->is( "flr") && *x_result > x) *x_result -= 1.0;
      else if (s->is( "ceil") && *x_result < x) *x_result += 1.0;
      return 1;
   }

   else if (s->is( "Go"))
   {
      step *save_pc, *save_pc_next;

      save_pc = pc;   save_pc_next = pc_next;
      pc = S.get_next( -1);

      while (pc != SNULL)
      {
         p = pc->e;
         pc_next = S.next( pc);

         EVAL( p)
         pc = pc_next;
      }
      pc = save_pc;   pc_next = save_pc_next;
      return 1;
   }

   else if (s->is( "first"))
   {
      p = s->down->next;

      if (!p->is( "iter"))
         EVALERROR( "Iteration expected.")

      return eval_iter( p, p->next, x_result, &i, s->symbol);
   }

   else if (s->is( "for"))
   {
      p = s->down->next;
      if (!p->is( "iter"))
         EVALERROR( "Badly formed iteration.")

      return eval_iter( p, p->next, &x, &i, s->symbol);
   }

   else if (s->is( "Form"))
   {
      p = s->down->next;
      EVAL( p)
      i = (int) (x + 0.5);
      if (i < 1 || i > 10)
         EVALERROR( "Form number must be between 1 and 10.")
      if (!p->next->is( "string"))
         EVALERROR( "Bad form command.")
      F.set( i, p->next->value.s->c);
      return 1;
   }

   else if (s->is( "if"))
   {
      p = s->down->next;
      EVAL( p)
      if (i > 0)
         EVAL (p->next)
      return 1;
   }

   else if (s->is( "ineq"))
   {
      *i_result = 1;
      p = s->down->next;
      EVAL( p)
      for ( p = p->next; !p->hd();  p = p->next->next)
      {
         y = x;
         EVAL( p->next)

         if (p->is( "<") && !(y < x))
         {
            *i_result = 0;   return 1;
         }
         else if (p->is( "<=") && !(y <= x))
         {
            *i_result = 0;   return 1;
         }
         else if (p->is( ">") && !(y > x))
         {
            *i_result = 0;   return 1;
         }
         else if (p->is( ">=") && !(y >= x))
         {
            *i_result = 0;   return 1;
         }
         else if (p->is( "=") && !(y == x))
         {
            *i_result = 0;   return 1;
         }
         else if (p->is( "~=") && !(y != x))
         {
            *i_result = 0;   return 1;
         }
      }
      return 1;
   }

   else if (s->is( "Let"))
   {
      p = s->down->next;
      while (!p->hd() && !p->next->hd() && !p->next->next->hd())
      {
         expr *qa, *r;

         q = p->next;   r = q->next;

         if (!p->is( "name"))
            EVALERROR( "Bad function definition.")
         if (!q->is( "var") && !q->is( "call"))
            EVALERROR( "Bad function definition.")

         v = find_var( q->value.i);
         if (v->type != (-1)) v->clear();

         /* Count number of arguments. */

         v->num_args = 0;
         qa = q->down;
         if (q->is( "call") && qa != ENULL)
            for (qa = qa->next; !qa->hd(); qa = qa->next) v->num_args++;

         r = split( r);
         v->set_formula( r->copy(), p->value.s->c);
         join( s->down, r);
         r = q->next;

         p = r->next;
      }
      return 1;
   }

   else if (s->is( "log"))
   {
      EVAL(s->down->next)
      *x_result = log( x);
      return 1;
   }

   else if (s->is( "max") || s->is( "min"))
   {
      p = s->down->next;

      if (p->is( "iter"))
         return eval_iter( p, p->next, x_result, &i, s->symbol);

      *x_result = 0;
      p = s->down->next;
      if (p->hd())
         EVALERROR ("Max or min requires at least one argument.")

      EVAL( p)
      *x_result = x;
      for (p = p->next; !p->hd(); p = p->next)
      {
         EVAL( p)
         if (s->is( "max") && x > *x_result
         ||  s->is( "min") && x < *x_result)  *x_result = x;
      }
      return 1;
   }

   else if (s->is( "not"))
   {
      p = s->down->next;
      EVAL( p)
      *i_result = 1 - i;
      return 1;
   }

   else if (s->is( "or"))
   {
      p = s->down->next;
      EVAL( p)
      if (i == 1)
      {
        *i_result = 1;   return 1;
      }
      p = p->next;
      EVAL( p)
      *i_result = i;
      return 1;
   }

   else if (s->is( "number"))
   {
      *x_result = s->value.f;
      return 1;
   }

   else if (s->is( "prod") || s->is( "sum"))
   {
      *x_result = (s->is( "prod") ? 1.0 : 0.0);
      if (s->down == ENULL) return 1;
      p = s->down->next;

      if (p->is( "iter"))
         return eval_iter( p, p->next, x_result, &i, s->symbol);

      for ( ; !p->hd(); p = p->next)
      {
         EVAL( p)

         if (s->is( "prod" )) *x_result *= x;
         else *x_result += x;
      }
      return 1;
   }

   else if (s->is( "ref"))
   {
      element *el;

      v = find_var( s->value.i);
      el = eval_ref( s, 1, NULL);
      if (el == ELNULL) return 0;

      *x_result = el->value;
      return 1;
   }

   else if (s->is( "Set"))
   {
      element *el;

      for (p = s->down->next; !p->hd(); p = p->next)
      {
         if (!p->is( "="))
            EVALERROR( "Bad structure for asignment command.")

         q = p->down->back;
         EVAL( q)
         for ( q=q->back; !q->hd(); q=q->back)
         {
            if (q->is( "var"))
            {
               v = find_var( q->value.i);
               if (v->type != (-1)) v->clear();
               v->set_value( x);
               continue;
            }

            if (!q->is( "ref"))
   EVALERROR( "Assignment allowed only to variables and array references.")

            el = eval_ref( q, 0, NULL);
            if (el == ELNULL) return 0;
            el->value = x;
         }
      }
      return 1;
   }

   else if (s->is( "sin"))
   {
      EVAL(s->down->next)
      *x_result = sin( x);
      return 1;
   }

   else if (s->is( "sqrt"))
   {
      EVAL( s->down->next)
      if (x < 0.0)
         EVALERROR( "Square root of a negative number.")
      *x_result = sqrt( x);
      return 1;
   }

   else if (s->is( "stack"))
   {
      i = num_stack-1 - s->value.i;
      if (i<0 || i>=num_stack)
         EVALERROR( "Stack underflow.")
      *x_result = stack[i];
      return 1;
   }

   else if (s->is( "Stop"))
   {
      pc_next = SNULL;
      return 0;
   }

   else if (s->is( "tan"))
   {
      EVAL(s->down->next)
      *x_result = tan( x);
      return 1;
   }

   else if (s->is( "Topart"))
   {
      EVAL( s->down->next)
      i = 1000 * (int) (x + 0.5);
      pc_next = S.get_next( i-1);
      if (pc_next != SNULL && pc_next->step_number >= i + 1000)
         pc_next = SNULL;
      return 1;
   }

   else if (s->is( "Tostep"))
   {
      EVAL( s->down->next)
      i = (int) (x * 1000.0 + 0.5);
      pc_next = S.get( i);
      return 1;
   }

   else if (s->is( "tv"))
   {
      EVAL(s->down->next)
      *x_result = (double) i;
      return 1;
   }

   else if (s->is( "Type"))
   {
      for (p = s->down->next;  !p->hd();   p = p->next)
      {
         if (p->is( "all"))
         {
            step *st;

            st = S.get_next(0);
            while (st != SNULL)
            {
               st->display();
               st = S.next( st);
            }

            for (i=1; i<=MAX_NUM_FORMS; i++)
               if (F.defined(i)) F.display(i);

            for (i=0; i<MAX_NUM_VARS; i++)
               if (V[i].type >= 0 && V[i].type <= 2) V[i].display();

            for (i=0; i<MAX_NUM_VARS; i++)
               if (V[i].type == 3) V[i].display();
         }
         else if (p->is( "allforms"))
         {
            for (i=1; i<=MAX_NUM_FORMS; i++)
               if (F.defined(i)) F.display(i);
         }
         else if (p->is( "allformulas"))
         {
            for (i=0; i<MAX_NUM_VARS; i++)
               if (V[i].type == 3) V[i].display();
         }
         else if (p->is( "allparts") || p->is( "allsteps"))
         {
            step *st;

            st = S.get_next(0);
            while (st != SNULL)
            {
               st->display();
               st = S.next( st);
            }
         }
         else if (p->is( "allvalues"))
         {
            for (i=0; i<MAX_NUM_VARS; i++)
               if (V[i].type >= 0 && V[i].type <= 2) V[i].display();
         }
         else if (p->is( "form"))
         {
            EVAL( p->down->next)
            i = (int) (x + 0.5);
            if (i < 1 || i > MAX_NUM_FORMS)
               printf( "     Bad form number -- %d.\n", i);
            else if (!F.defined(i))
               printf( "     Form %d is undefined.\n", i);
            else
               F.display(i);
         }
         else if (p->is( "formula"))
         {
            v = find_var( p->value.i);
            if (v->type != 3)
            {
               sprintf( message, "%c is not a formula.", v->name);
               EVALERROR( message)
            }
            v->display();
         }
         else if (p->is( "name"))
         {
            EVAL( p->next)
            printf( "     %s = %lg\n", p->value.s->c, x);
            p = p->next;
         }
         else if (p->is( "part"))
         {
            int part;
            step *st;

            EVAL( p->down->next)
            part = 1000 * (int) (x + 0.5);
            st = S.get_next( part-1);
            while (st != SNULL)
            {
               if (st->step_number < part || st->step_number >= part+1000)
                  break;
               st->display();
               st = S.next( st);
            }
         }
         else if (p->is( "ref"))
         {
            element *el;

            v = find_var( p->value.i);
            el = eval_ref( p, 1, NULL);
            if (el == ELNULL) continue;

            if (v->type == 1)
               printf( "     %c[%d] = %lg\n", v->name, el->i1, el->value);
            else
               printf( "     %c[%d,%d] = %lg\n", v->name, el->i1, el->i2,
                       el->value);
         }
         else if (p->is( "step"))
         {
            step *st;

            EVAL( p->down->next)
            i = (int) (x * 1000.0 + 0.5);
            st = S.get( i);
            if (st == SNULL)
               printf( "     Step %-.3f is undefined.\n", (float) x);
            else
               st->display();
         }
         else if (p->is( "string"))
         {
            printf( "     %s\n", p->value.s->c);
         }
         else if (p->is( "var"))
         {
            v = find_var( p->value.i);
            if (v->type == (-1))
               v->get_value( &x, NULL);

            if (v->type == 3 && v->num_args == 0)
            {
               EVAL( p)
               printf( "     %c = %lg\n", v->name, x);
            }
            else v->display();
         }
         else
         {
            EVAL( p)
            printf( "     %lg\n", x);
         }
      }
      return 1;
   }

   else if (s->is( "Typeinform"))
   {
      int form_number;

      p = s->down->next;
      EVAL( p)
      form_number = int (x + 0.5);
      if (form_number < 1 || form_number > MAX_NUM_FORMS)
      {
         sprintf( message,
           "Bad form number %d -- must be between 1 and %d.", form_number,
           MAX_NUM_FORMS);
         EVALERROR( message)
      }

      if (!F.defined( form_number))
      {
         sprintf( message, "Form %d is not defined.", form_number);
         EVALERROR( message)
      }

      F.clear_vars();
      for (p = p->next;   !p->hd();   p = p->next)
      {
         EVAL(p)
         F.add_var( x);
      }
      F.type_vars( form_number);
      return 1;
   }

   else if (s->is( "u-"))
   {
      EVAL( s->down->next)
      *x_result = (-x);
      return 1;
   }

   else if (s->is( "var"))
   {
      v = find_var( s->value.i);
      if (v->type == 3)
      {
         if (v->num_args != 0)
            EVALERROR( "Argument(s) expected for this function.")
         EVAL( v->f)
         *x_result = x;
         return 1;
      }
      if (!v->get_value( &x, NULL)) return 0;
      *x_result = x;
      return 1;
   }

   else if (s->is( "while"))
   {
      p = s->down->next;
      q = p->next;
      for(;;)
      {
         EVAL(p)
         if (i == 0) break;
         EVAL(q)
      }
      return 1;
   }

   else if (s->is( "+"))
   {
      p = s->down->next;
      EVAL( p)
      y = x;   EVAL(p->next)
      *x_result = x + y;   return 1;
   }

   else if (s->is( "*"))
   {
      p = s->down->next;
      EVAL( p)
      y = x;   EVAL(p->next)
      *x_result = y * x;   return 1;
   }

   else if (s->is( "/"))
   {
      p = s->down->next;
      EVAL( p)
      y = x;   EVAL(p->next)
      if (x == 0.0)
         EVALERROR( "Divide by zero error.")
      *x_result = y / x;   return 1;
   }

   else if (s->is( "^"))
   {
      p = s->down->next;
      EVAL( p)
      y = x;   EVAL(p->next)
      *x_result = pow( y, x);   return 1;
   }

   else
   {
      sprintf( message, "Cannot recognize token: %s.\n", s->symbol);
      EVALERROR( message)
   }
}


void
eval_error( char *s)
{
   if (pc == SNULL)
      printf( "  Evaluation error: %s\n", s);
   else
   {
      printf( "  Evaluation error:\n");
      pc->display();
      printf( "  %s\n", s);
   }
}


int
eval_iter( expr *s, expr *t, double *x_result, int *i_result, char *symbol)
{
   expr *p, *q;
   double x_start, x_inc, x_end, x_cur;
   double x;
   int i, action, num_iter;
   vars *v;

   static char *symbols[] = {
      "conj", "disj", "first", "for", "max", "min", "prod", "sum", NULL
   };

   if (!s->is( "iter"))
      EVALERROR( "Badly formed iteration.")

   /*  Get the iteration variable. */

   v = find_var( s->value.i);
   if (v->type > 0) v->clear();

   /* Determine which action is to be done. */

   for (action = 0; symbols[action] != NULL; action++)
      if (strcmp( symbol, symbols[action]) == 0) break;

   /*
    *  It is assumed that i_result is initialized for conj() and disj(),
    *  and that x_result is initialized for sum() and prod().
    */

   num_iter = 0;
   for (p = s->down->next;   !p->hd();   p=p->next)
   {
      if (!p->is( "<>"))
      {
         EVAL( p)
         x_start = x_end = x;
         x_inc = 1.0;
      }
      else
      {
         q = p->down->next;
         EVAL( q)
         x_start = x;
         EVAL( q->next)
         x_inc = x;
         if (x_inc == 0)
            EVALERROR( "Iteration increment is zero.")
         EVAL( q->next->next)
         x_end = x;
      }

      x_cur = x_start;
      while (x_inc > 0.0 && x_cur <= x_end
        ||   x_inc < 0.0 && x_cur >= x_end)
      {
         v->set_value( x_cur);
         EVAL( t)

         switch( action)
         {
            case 0: /* conj */
               if (i == 0)
               {
                  *i_result = 0;   return 1;
               }
               break;

            case 1: /* disj */
               if (i == 1)
               {
                  *i_result = 1;   return 1;
               }
               break;

            case 2: /* first */
               if (i == 1)
               {
                  *x_result = x_cur;   return 1;
               }
               break;

            case 3:  /* for */
               break;

            case 4: /* max */
               if (num_iter == 0 || x > *x_result) *x_result = x;
               break;

            case 5: /* min */
               if (num_iter == 0 || x < *x_result) *x_result = x;
               break;

            case 6: /* prod */
               *x_result *= x;
               break;

            case 7: /* sum */
               *x_result += x;
               break;
         }

         num_iter++;
         x_cur += x_inc;
      }
   }

   if (action == 2)
      EVALERROR( "The first() function failed.")

   if (num_iter == 0 && (action == 4 || action == 5))
      EVALERROR( "The max() or min() function failed.")

   return 1;
}


element *
eval_ref( expr *s, int demand_switch, char *prompt)
{
   int i, i1, i2, num_args;
   double x;
   vars *v;

   v = find_var( s->value.i);
   s = s->down->next;

   if (s->hd())
   {
      printf( " Null array reference.\n");
      return ELNULL;
   }

   if (s->next->hd())
      num_args = 1;
   else if (s->next->next->hd())
      num_args = 2;
   else
   {
      printf( "Array reference with more than two indices.\n");
      return ELNULL;
   }

   if (!eval( s, &x, &i)) return ELNULL;
   i1 = (int) (x + 0.5);   i2 = 0;
   if (num_args == 2)
   {
      if (!eval( s->next, &x, &i)) return ELNULL;
      i2 = (int) (x + 0.5);
   }

   if (v->type != num_args)
   {
      v->clear();
      v->type = num_args;
   }
   return v->find_element( i1, i2, demand_switch, prompt);
}
