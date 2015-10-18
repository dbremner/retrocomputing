#include <stdio.h>
#include <string.h>
#include "joss.hpp"

extern char *input_line;
extern char *cur_line;
extern char *subcommand_end;

extern expr *root;
extern int step_number;
extern int direct;
extern int DEBUG;

#define PERROR(A,B)        { parse_error( (A)->pos, (B));   return 0; }
#define PERROR1(A,B,C)     { parse_error( (A)->pos, (B));   delete_exp( C); \
                             return 0; }
#define PERROR2(A,B,C,D)   { parse_error( (A)->pos, (B));   delete_exp( C); \
                             delete_exp(D);   return 0; }
#define PERROR3(A,B,C,D,E) { parse_error( (A)->pos, (B));   delete_exp( C); \
                             delete_exp(D);   delete_exp(E);   return 0; }

#define PARSE(G,S)         if (!parse( G, S)) return 0;
#define PARSE1(G,S,A)      if (!parse( G, S)) { delete_exp(A); return 0; }
#define PARSE2(G,S,A,B)    if (!parse( G, S)) { delete_exp(A); delete_exp(B); \
                                                return 0; }
#define PARSE3(G,S,A,B,C)  if (!parse( G, S)) { delete_exp(A); delete_exp(B); \
                                                delete_exp(C); return 0; }

int
scan()
{
   char *s, *t;
   expr *e;
   int i, len, exp;
   double x;
   static char *reserved[] = {
      "formulas",
      "formula",
      "Delete", "Demand", "values",
      "Debug", "first", "forms", "parts", "steps", "while",
      "ceil", "conj", "disj", "Done", "Form", "form", "part", "prod",
              "sqrt", "step", "Stop", "Type", "Quit",
      "all", "and", "cos", "exp", "flr", "for", "Let", "log", "max",
             "min", "not", "Set", "sin", "sum", "tan",
      "<=", ">=", "~=", "as", "Do", "Go", "if", "in", "or", "To", "tv",
      "+", "-", "*", "^", "/", "(", ")", "[", "]", "|", ",", ":", ";",
           "=", "<", ">",
      nullptr
   };

   /*  Initialize the expression. */

   root = new expr( "head");
   root->init_head();

   direct = 1;
   step_number = (-1);

   while (*cur_line == ' ') cur_line++;
   s = cur_line;

   while (*s != '\0')
   {
      while (*s == ' ') s++;
      if (*s == '\0') return 1;

      /*  Look for a reserved word. */

      for (i = 0, t = reserved[0];
           t != nullptr;
           t = reserved[++i])
      {
         len = strlen( t);
         if (strncmp( s, t, len) == 0) break;
      }

      if (t != nullptr)
      {
         e = new expr( t);
         e->pos = s - cur_line;
         s += len;
      }

      else if (*s == '.')
      {
         subcommand_end = s-1;
         return 1;
      }

      else if (*s == '"')
      {
         e = new expr( "string");
         e->value.s = new fstring();
         e->pos = s - cur_line;

         t = ++s;
         s =  e->value.s->c;
         while (*t != '\0' && *t != '"')
            *s++ = *t++;
         *s = '\0';

         if (*t == '\0')
         {
            parse_error( t-cur_line, "Scan error: unclosed string.");
            delete e->value.s;
            delete e;
            return 0;
         }
         s = ++t;
      }
      else if (*s >= '0' && *s <= '9')
      {
         e = new expr( "number");
         e->pos = s - cur_line;

         x = 0.0;
         exp = 0;
         while (*s >= '0' && *s <= '9')
         {
            x = 10.0 * x + (double) (*s - '0');
            s++;
         }
         if (*s == '.')
         {
            s++;
            while( *s >= '0' && *s <= '9')
            {
               x = 10.0 * x + (double) (*s - '0');
               s++;
               exp++;
            }
         }
         e->value.f = x;
         while (exp-- > 0) e->value.f /= 10.0;
      }
      else if ((*s >= 'a' && *s <= 'z') ||
               (*s >= 'A' && *s <= 'Z'))
      {

         if ((s[1] >= 'a' && s[1] <= 'z')
          || (s[1] >= 'A' && s[1] <= 'Z'))
         {
            parse_error( s-cur_line,
               "Scan error -- don't recognize this word.");
            return 0;
         }
         e = new expr( "var");
         e->value.i = *s;
         e->pos = s - cur_line;
         s++;
      }
      else
      {
         parse_error( s-cur_line, "Illegal character.");
         return 0;
      }

      /*  Check for a step number. */

      if (direct && e->is( "number") && root->next == root)
      {
         if (e->value.f < 1.0 || e->value.f > 999.999)
         {
            parse_error( 0, "Step number must be between 1 and 999.999.");
            return 0;
         }

         direct = 0;
         step_number = (int) (e->value.f * 1000.0 + 0.5);
         while (*s == ' ') s++;
         cur_line = s;

         delete e;
      }
      else
      {
         root->back->append( e);
      }
   }
   return 1;
}


int
parse( int goal, expr *s)
{
   expr *p, *pa, *pb, *q, *r;

   static expr *e;
   static int ineq_type;
   static char message[LINE_LEN];

   if (DEBUG)
   {
      printf( "Parse ");
      switch (goal)
      {
         case 1: printf( "COMMAND");
                 break;
         case 2: printf( "SUBCOMMAND");
                 break;
         case 3: printf( "ITERATION");
                 break;
         case 4: printf( "RANGE");
                 break;
         case 5: printf( "PREDLIST");
                 break;
         case 6: printf( "PREDICATE");
                 break;
         case 7: printf( "EXPLIST");
                 break;
         case 8: printf( "EXPRESSION");
                 break;
         case 9: printf( "SUBEXPRESSION");
                 break;
         default:
                 printf( "%d", goal);
      }
      printf( ":\n");
      for (p = s->next;   !p->hd();   p = p->next)
      {
         printf( "   %s", p->symbol.c_str());
      }
      printf( "\n");
   }

   /*
    *  COMMAND
    */

   switch (goal)
   {

      case COMMAND:
      {
         /*  Remove parentheses. */

         for (;;)
         {

            /*  Look for the innermost parentheses pair. */

            p = nullptr;
            for (q = s->next;  !q->hd();   q = q->next)
            {
               if (q->is( "(" /* ) */ ) || q->is( "[" /* ] */ ))
               {
                  p = q;
                  continue;
               }

               if (q->is( /* ( */ ")") || q->is( /* [ */ "]")) break;
            }

            if (q->hd() && p == nullptr) break;

            if (q->hd())
               PERROR( p, "Unbalanced parentheses.")
            if (p == nullptr)
               PERROR( q, "Unbalanced parentheses.")

            if ((p->is( "(" /* ) */) && q->is(/* [ */ "]"))
             || (p->is( "[" /* ] */) && q->is(/* [ */ ")")))
               PERROR( p, "Mixed parentheses.")

            q = split_at( q);

            if (p->next->hd()) pb = nullptr;
            else pb = split( p->next);

            if (p->is( "(" /* ) */ ))
               p->symbol = "()";
            else
               p->symbol = "[]";

            if (pb != nullptr) join_down( p, pb);
            if (q != nullptr) join( s, q);
         }

         PARSE( SUBCOMMAND, s)
         return 1;
      }

      /*
       *  SUBCOMMAND
       */

      case  SUBCOMMAND:
      {

         /*
          *  ... if ...
          *  ... for ...
          *  ... while ...
          */

         for (;;)
         {
            for (p = s->back;   !p->hd(); p = p->back)
            {
               if (p->is( "if") || p->is( "while") || p->is( "for"))
                  break;
            }
            if (p->hd()) break;

            subcommand_end = &cur_line[p->pos - 1];

            if (s->next == p)
            {
               sprintf( message, "Statement cannot start with %s.\n",
                        p->symbol.c_str());
               PERROR( p, message)
            }

            if (p->next == s)
            {
               sprintf( message, "Incomplete %s statement.", p->symbol.c_str());
               PERROR( p, "Incomplete if, for, or while statement.")
            }

            pb = split( p->next);
            p = split( p);
            pa = split( s->next);
            join( s, p);

            PARSE2( SUBCOMMAND, pa, pa, pb)

            if (s->next->is( "for"))
            {
               PARSE2( ITERATION, pb, pa, pb)
            }
            else
            {
               PARSE2( PREDICATE, pb, pa, pb)
            }

            join( pb, pa);
            join_down( s->next, pb);
            return 1;
         }

         /*
          *  Form 3: "dist = %f10.2   accel = %f10.2"
          */

         if (s->next->is( "Form"))
         {
            p = s->next;

            if (direct == 0)
               PERROR( p, "Form command must be direct.")

            p = p->next;
            if (!p->is( "number"))
               PERROR( p, "Form must have a form number.")

            p = p->next;
            if (!p->is(":"))
               PERROR( p, "Colon (:) expected.")

            p = p->next;
            if (!p->is("string"))
               PERROR( p, "No form string found.")

            if (!p->next->hd())
               PERROR( p->next, "Error in Form statement.")

            q = split( s->back);
            p = split( s->back);
            delete_exp( p);
            p = split( s->back);
            join( p, q);
            join_down( s->next, p);
            return 1;
         }

         /*
          *  Do part 1.
          *
          *  Do step 3.15.
          */

         if (s->next->is( "Do"))
         {
            p = s->next->next;
            if (!(p->is( "step") || p->is( "part")))
               PERROR( p, "Part or step omitted.")

            q = split( p->next);
            if (p->is( "part"))
               p->back->symbol = "Dopart";
            else
               p->back->symbol = "Dostep";
            p = split( p);
            delete_exp(p);

            PARSE1( EXPRESSION, q, q)
            join_down( s->next, q);
            return 1;
         }

         /*
          *  To step 3.15.
          *  To part 3.
          */

         if (s->next->is( "To"))
         {
            p = s->next->next;
            if (p->is( "step"))
               p->back->symbol = "Tostep";
            else if (p->is( "part"))
               p->back->symbol = "Topart";
            else
               PERROR( p, "Form: To step n  or  To part n.")

            p = split_at( p);
            PARSE1( EXPRESSION, p, p)
            join_down( s->next, p);
            return 1;
         }

         /*
          *  Demand x, y, ...
          *  Demand x as "..."
          */

         if (s->next->is( "Demand"))
         {
            q = s->next->find( "as");
            if (q != nullptr)
            {
               q = split_at( q);
               if (!q->next->is2( "string", "head"))
                  PERROR1( q, "String expected.", q)

               s->next->symbol = "Demand_as";
               p = split( s->next->next);
               PARSE2( EXPRESSION, p, p, q)
               if (!p->next->is( "var") && !p->next->is( "ref"))
               {
                  strcpy( message,
                     "You can only demand variables or array references.");
                  PERROR2( p->next, message, p, q)
               }

               join_down( s->next, p);
               join_down( s->next, q);
               return 1;
            }

            p = split( s->next->next);
            while (p != nullptr)
            {
               q = p->next->find( ",");
               if (q != nullptr) q = split_at( q);

               if (!p->next->is2( "var", "head") &&
                   !p->next->is3( "var", "[]", "head"))
              {
                  strcpy( message,
                     "You can only demand variables or array references.");
                  PERROR2( p, message, p, q)
               }

               PARSE2( SUBEXPRESSION, p, p, q)
               join_down( s->next, p);
               p = q;
            }
            return 1;
         }

         /*
          *  Delete x, all forms, part2.
          *
          *  Type x, all forms, part 2.
          *     (parse 1)
          *
          *  Type <exp>,...,<exp> in form <exp>.
          *     (parse 2)
          */

         if (s->next->is( "Type") || s->next->is( "Delete"))
         {
            /* Look for parse 2. */

            q = nullptr;
            if (s->next->is( "Type"))
              for (p = s->next;   !p->hd(); p = p->next)
                if (p->is2( "in", "form"))
                {
                    q = p;
                    break;
                }

            /* Parse 2 found. */

            if (q != nullptr)
            {
               pb = split( q->next->next);
               delete_exp( split( q));
               q = pb;

               s->next->symbol = "Typeinform";
               p = split( s->next->next);
               if (!p->next->hd())
                  PARSE2( EXPLIST, p, p, q)

               PARSE2( EXPRESSION, q, p, q)

               join( q, p);
               join_down( s->next, q);
               return 1;
            }

            /* Here for Delete and for Type (parse 1). */

            p = split( s->next->next);
            while (p != nullptr)
            {
               char *name;
               int len;

               name = &cur_line[p->next->pos];

               /*
                *  Make p = first iterm in the list,
                *       q = rest of the list.
                */

               q = p->next->find( ",");

               if (q != nullptr)
               {
                  len = &cur_line[q->pos] - name;
                  q = split_at( q);
               }
               else
                  len = subcommand_end - name + 1;

               /*  Parse the item p. */

               if (p->next->is2( "all", "head"))
               {
               }
               else if (p->next->is3( "all", "forms", "head"))
               {
                  p->next->symbol = "allforms";
                  delete_exp( split( p->next->next));
               }
               else if (p->next->is3( "all", "formulas", "head"))
               {
                  p->next->symbol = "allformulas";
                  delete_exp( split( p->next->next));
               }
               else if (p->next->is3( "all", "parts", "head"))
               {
                  p->next->symbol = "allparts";
                  delete_exp( split( p->next->next));
               }
               else if (p->next->is3( "all", "steps", "head"))
               {
                  p->next->symbol = "allsteps";
                  delete_exp( split( p->next->next));
               }
               else if (p->next->is3( "all", "values", "head"))
               {
                  p->next->symbol = "allvalues";
                  delete_exp( split( p->next->next));
               }
               else if (p->next->is( "form") || p->next->is( "part")
                     || p->next->is( "step"))
               {
                  pb = split( p->next->next);
                  PARSE3( EXPRESSION, pb, p, pb, q)
                  join_down( p->next, pb);
               }
               else if (p->next->is3( "formula", "var", "head"))
               {
                  p->next->value.i = p->next->next->value.i;
                  delete_exp( split( p->next->next));
               }
               else if (p->next->is2( "string", "head"))
               {
                  if (s->next->is( "Delete"))
                     PERROR2( p->next, "Can't delete a string.", p, q)
               }
               else if (p->next->is2( "var", "head"))
               {
                   /*
                    *  You can Type any expression, but may Delete
                    *  only a variable name.
                    */
               }
               else if (s->next->is( "Delete"))
                  PERROR2( p->next, "I don't know what to delete.", p, q)
               else
               {
                  PARSE2( EXPRESSION, p, p, q)

                  /*
                   *  If the expression is not a variable, array
                   *  reference, or a number, then remember the expression
                   *  string (in a "name" expr record), so that the Type
                   *  function can write it out along with its value.
                   */

                  if (!p->next->is( "ref") && !p->next->is( "number"))
                  {
                     e = new expr( "name");

                     e->value.s = new fstring();
                     strncpy( e->value.s->c, name, len);
                     e->value.s->c[len] = '\0';
                     trim( e->value.s->c);

                     p->append( e);
                  }
               }

               /*  Append p to the new expression. */

               join_down( s->next, p);

               p = q;
            }

            return 1;
         }

         /*
          *  Set x = <exp>, y[1] = <exp>, a = b = 0,  ...
          */

         if (s->next->is( "Set"))
         {
            p = s->next->next;
            while( !p->hd())
            {
               p = split( p);
               q = p->next->find( ",");
               if (q != nullptr)
               {
                  q = split_at( q);
                  join( s, q);
               }

               pa = p->next->find( "=");
               if (pa == nullptr)
                  PERROR1( p, "No assignment symbol.", p)

               q = split(pa->next);
               pa = split( pa);

               for(;;)
               {
                  if (q != nullptr && !p->next->is2( "var", "head") &&
                      !p->next->is3( "var", "[]", "head"))
       PERROR3( p, "Must assign to a variable or array element.", p, q, pa)

                  PARSE2( EXPRESSION, p, q, pa)
                  join_down( pa->next, p);
                  p = q;
                  if (p == nullptr) break;
                  q = p->next->find( "=");
                  if (q != nullptr) q = split_at( q);
               }
               join_down( s->next, pa);
               p = s->next->next;
            }
            return 1;
         }

         /*
          *  Let x = <exp>, y(a,b,c) = <exp>, ...
          */

         if (s->next->is( "Let"))
         {
            int num_args;
            static char var_list[MAX_NUM_VARS];

            if (!direct)
               PERROR( s->next, "Let command must be direct.")

            p = split( s->next->next);
            while( p != nullptr)
            {
               char *name;
               int len;

               name = &cur_line[p->next->pos];

               q = p->next->find( ",");
               if (q != nullptr)
               {
                  len = q->pos - p->pos;
                  q = split_at( q);
               }
               else len = subcommand_end - name + 1;

               r = p->next->find( "=");
               if (r == nullptr)
                  PERROR2( p, "No assignment symbol.", p, q)
               r = split_at( r);

               if (!p->next->is2( "var", "head") &&
                   !p->next->is3( "var", "()", "head"))
                  PERROR3( p, "Incorrect form for Let command.", p, q, r)

               PARSE3( EXPRESSION, p, p, q, r)
               PARSE3( EXPRESSION, r, p, q, r)

               if (p->next->is( "call"))
               {
                  num_args = 0;
                  for (pa = p->next->down->next; !pa->hd(); pa = pa->next)
                  {
                     if (!pa->is( "var"))
         PERROR3( pa, "Function arguments here must be variables.", p, q, r)
                     var_list[num_args++] = pa->value.i;
                  }

                  alter_function( r, num_args, var_list);
               }

               /*  Append the function definition as a name. */

               e = new expr( "name");
               e->value.s = new fstring();
               strncpy( e->value.s->c, name, len);
               e->value.s->c[len] = '\0';
               trim( e->value.s->c);
               p->append( e);

               join( p, r);
               join_down( s->next, p);
               p = q;
            }
            return 1;
         }

         /*
          *   Debug.
          *   Done.
          *   Go.
          *   Stop.
          *   Quit.
          */

         if (s->next->is( "Debug") || s->next->is( "Done")
          || s->next->is ("Go") ||  s->next->is( "Stop")
          || s->next->is( "Quit"))
         {
            p = s->next;
            if (!p->next->hd())
               PERROR( p->next, "Syntax error")
            if (!direct)
            {
               if (p->is( "Debug"))
                  PERROR( p, "Debug command in direct mode only.")
               if (p->is( "Go"))
                  PERROR( p, "Go command in direct mode only.")
               if (p->is( "Quit"))
                  PERROR( p, "Quit command in direct mode only.")
            }
            return 1;
         }

         if (direct && s->next->find( "=") != nullptr)
         {
            /* Treat this as short Set command. */

            e = new expr( "Set");
            s->append( e);

            PARSE( SUBCOMMAND, s)
            return 1;
         }

         PERROR( s->next, "Don't recognize this command.")
      }

      /*
       *  PREDICATE LIST.
       */

      case PREDLIST:
      {
         p = split( s->next);

         while (p != nullptr)
         {
            q = p->next->find( ",");
            if (q != nullptr) q = split_at( q);
            PARSE2( PREDICATE, p, p, q)
            join( s, p);
            p = q;
         }
         return 1;
      }

      /*
       *  PREDICATE.
       */

      case PREDICATE:
      {
         /* Process or, and, not. */

         p = s->next->find( "or");
         if (p == nullptr) p = s->next->find( "and");
         if (p != nullptr)
         {
            pb = split( p->next);
            p = split( p);
            pa = split( s->next);
            join( s, p);

            PARSE2( PREDICATE, pa, pa, pb)
            PARSE2( PREDICATE, pb, pa, pb)

            join( pa, pb);
            join_down( s->next, pa);
            return 1;
         }

         if (s->next->is( "not"))
         {
            p = split( s->next->next);
            PARSE1( PREDICATE, p, p)
            join_down( s->next, p);
            return 1;
         }

         /*  Look for predicates in parentheses */

         if (s->next->is2( "()", "head"))
         {
            p = split_down( s->next);
            if (p == nullptr)
               PERROR( s->next, "Empty predicate.")
            delete_exp( split( s->next));
            join( s, p);
            PARSE( PREDICATE, s)
            return 1;
         }

         /*  conjunction or disjunction */

         if (s->next->is3( "conj", "()", "head")
          || s->next->is3( "disj", "()", "head"))
         {
            p = split_down( s->next->next);
            delete_exp( split( s->next->next));
            if (p == nullptr) return 1;   /* Empty conjunction or disjunction */

            q = p->next->find( ":");
            if (q == nullptr)
            {
               PARSE1( PREDLIST, p, p)
               join_down( s->next, p);
               return 1;
            }
            q = split_at( q);

            PARSE2( ITERATION, p, p, q)
            PARSE2( PREDICATE, q, p, q)

            join_down( s->next, p);
            join_down( s->next, q);
            return 1;
         }

         /*
          *  Look for equalities:
          *              exp1 = exp2
          *           extended equalities:
          *              exp1 = exp2 = exp3
          *           inequalities
          *              exp1 < exp2   exp1 >= exp2   exp1 ~= exp2
          *           or extended inequalities:
          *              exp1 < exp2 <= exp3
          *              exp1 >= exp2 > exp3
          *              etc.
          */

         ineq_type = 0;

         /*
          *   Type:
          *
          *       0   no inequalities.
          *       1   ascending, descending, or equal
          *       2   ascending
          *       3   descending
          *       4   not equal
          */

         for (p = s->next; !p->hd(); p = p->next)
         {
            if (p->is( "<") || p->is( "<="))
            {
               if (ineq_type == 0 || ineq_type == 1 || ineq_type == 2)
                  ineq_type = 2;
               else
                  PERROR( p, "Incorrect multiple comparison.")
            }
            else if (p->is( ">") || p->is( ">="))
            {
               if (ineq_type == 0 || ineq_type == 1 || ineq_type == 3)
                  ineq_type = 3;
               else
                  PERROR( p, "Incorrect multiple comparison.")
            }
            else if (p->is( "="))
            {
               if (ineq_type == 0) ineq_type = 1;
               else if (ineq_type == 4)
                  PERROR( p, "Incorrect multiple comparison.")
            }
            else if (p->is( "~="))
            {
               if (ineq_type == 0) ineq_type = 4;
               else PERROR( p, "Incorrect multiple comparison.")
            }
         }

         if (ineq_type == 0)
            PERROR( s->next, "Cannot parse this predicate.")

         p = split( s->next);
         e = new expr( "ineq");
         e->pos = p->next->pos;
         s->append( e);

         for(;;)
         {
            for (q = p->next; !q->hd(); q = q->next)
            {
               if (q->is( "<") || q->is( "<=")) break;
               if (q->is( ">") || q->is( ">=")) break;
               if (q->is( "=") || q->is( "~=")) break;
            }

            if (p->next->hd())
               PERROR1( q, "Incomplete comparison statement.", p)

            if (q->hd()) q = nullptr;
            else q = split( q);

            PARSE2( EXPRESSION, p, p, q)
            join_down( s->next, p);
            if (q == nullptr) break;

            if (q->next->next->hd())
               PERROR1( q->next, "Incomplete comparison statement.", q)
            p = split( q->next->next);
            join_down( s->next, q);
         }
         return 1;
      }

      /*
       *  ITERATION
       */

      case ITERATION:
      {
         p = s->next;
         if (!p->is2( "var", "="))
            PERROR( p, "Syntax error in this iteration.")

         q = split( p->next->next);
         delete_exp( split( p->next));
         p->symbol = "iter";

         /*  Get each range. */

         p = q;
         while (p != nullptr)
         {
            q = p->next->find( ",");
            if (q != nullptr) q = split_at( q);

            PARSE2( RANGE, p, p, q)

            join_down( s->next, p);
            p = q;
         }
         return 1;
      }

      /*
       *  RANGE
       */

      case RANGE:
      {
         p = s->next->find( "<");
         if (p == nullptr || (q = p->next->find( ">")) == nullptr)
         {
            PARSE( EXPRESSION, s)
            return 1;
         }

         if (s->next == p || p->next == q)
           PERROR( p, "Incomplete range specification.")
         if (q->next->hd())
           PERROR( q, "Incomplete range specification.")

         q = split_at( q);

         p->symbol = "<>";
         pb = split( p->next);
         p = split( p);
         pa = split( s->next);
         join( s, p);

         PARSE3( EXPRESSION, pa, pa, pb, q)
         join_down( s->next, pa);

         PARSE2( EXPRESSION, pb, pb, q)
         join( s->next->down, pb);

         PARSE1( EXPRESSION, q, q)
         join( s->next->down, q);
         return 1;
      }

      /*
       *  EXPLIST
       */

      case EXPLIST:
      {
         p = split( s->next);

         while (p != nullptr)
         {
            q = p->next->find( ",");
            if (q != nullptr) q = split_at(q);

            PARSE2( EXPRESSION, p, p, q)
            join( s, p);
            p = q;
         }
         return 1;
      }

      /*
       *  EXPRESSION
       */

      case EXPRESSION:
      {
         /*
          *  Distinguish a "-" as a "b-"(subtraction) or "u-"
          *     (unary opposite)
          *
          *  Distinguish a "|" as a "(|" and "|)".
          */

         for (q = s->next;   !q->hd();  q = q->next)
         {
            if (q->is( "|") || q->is( "-"))
            {
               p = q->back;
               if (p->is( "var") || p->is( "number") || p->is( "string")
                || p->is( "()")  || p->is( "[]")  || p->is( /* ( */ "|)" ))
                  q->symbol = (q->is( "|") ? /* ( */ "|)" : "b-");
               else
                  q->symbol = (q->is( "|") ? "(|" /* ) */ : "u-");
            }
         }

         /*
          *  Put absolute value expressions into subtrees.
          */

         for(;;)
         {
            p = nullptr;
            for (q = s->next;   !q->hd();   q = q->next)
            {
               if (q->is( "(|" /* ) */ ))
               {
                  p = q;
                  continue;
               }
               if (q->is( /* ( */ "|)" )) break;
            }

            if (p == nullptr && q->hd()) break;
            if (p == nullptr)
               PERROR( q, "Unbalanced absolute value bars.")
            if (q->hd())
               PERROR( p, "Unbalanced absolute value bars.")

            q = split_at( q);

            pb = split( p->next);
            p->symbol = "abs";
            join_down( p, pb);
            join( s, q);
         }

         PARSE( SUBEXPRESSION, s)
         return 1;
      }

      /*
       *  SUBEXPRESSION
       */

      case SUBEXPRESSION:
      {
         /*  Variable */

         if (s->next->is2( "var", "head"))
            return 1;

         /*  Number */

         if (s->next->is2( "number", "head"))
            return 1;

         /*  Conditional or parenthetical expression. */

         if (s->next->is2( "()", "head"))
         {
            p = split_down( s->next);
            if (p == nullptr)
               PERROR( s->next, "Empty expression.")

            q = p->next->find( ":");
            if (q == nullptr)
            {
               delete_exp( split( s->next));
               join( s, p);
               PARSE( EXPRESSION, s)
               return 1;
            }

            s->next->symbol = "cond";
            while (p != nullptr)
            {
               q = p->next->find( ":");

               if (q == nullptr)
               {
                  PARSE1( EXPRESSION, p, p)
               }
               else
               {
                  q = split_at(q);
                  PARSE2( PREDICATE, p, p, q)
               }

               join_down( s->next, p);
               p = q;
               if (p == nullptr) break;

               q = p->next->find( ";");
               if (q != nullptr) q = split_at( q);

               PARSE2( EXPRESSION, p, p, q)
               join_down( s->next, p);
               p = q;
            }
            return 1;
         }

         /*  Addition / Subtraction */

         for (p = s->back;  !p->hd();   p = p->back)
         {
            if (p->is( "+") || p->is( "b-")) break;
         }
         if (!p->hd())
         {
            pb = split( p->next);
            p = split( p);
            pa = split( s->next);
            join( s, p);

            if (pa->next->hd() || pb->next->hd())
               PERROR2( s->next, "Syntax error in expression.", pa, pb)

            PARSE2( SUBEXPRESSION, pa, pa, pb)
            PARSE2( SUBEXPRESSION, pb, pa, pb)
            join( pa, pb);
            join_down( s->next, pa);
            return 1;
         }

         /* Unary - */

         if (s->next->is( "u-"))
         {
            p = split( s->next->next);
            if (p->next->hd())
               PERROR1( s->next, "Syntax error in expression.", p)

            PARSE1( SUBEXPRESSION, p, p)
            join_down( s->next, p);
            return 1;
         }

         /* Multiplication / Division */

         for (p = s->back;  !p->hd();   p = p->back)
         {
            if (p->is( "*") || p->is( "/")) break;
         }
         if (!p->hd())
         {
            pb = split( p->next);
            p = split( p);
            pa = split( s->next);
            join( s, p);

            if (pa->next->hd() || pb->next->hd())
               PERROR2( s->next, "Syntax error in expression.", pa, pb)

            PARSE2( SUBEXPRESSION, pa, pa, pb)
            PARSE2( SUBEXPRESSION, pb, pa, pb)
            join( pa, pb);
            join_down( s->next, pa);
            return 1;
         }

         /* Exponentiation */

         p = s->next->find( "^");
         if (p != nullptr)
         {
            pb = split( p->next);
            p = split( p);
            pa = split( s->next);
            join( s, p);

            if (pa->next->hd() || pb->next->hd())
               PERROR2( s->next, "Syntax error in expression.", pa, pb)

            PARSE2( SUBEXPRESSION, pa, pa, pb)
            PARSE2( SUBEXPRESSION, pb, pa, pb)
            join( pa, pb);
            join_down( s->next, pa);
            return 1;
         }

         /* Absolute Value */

         p = s->next;
         if (p->is2( "abs", "head"))
         {
            if (p->down == nullptr)
               PERROR( p, "Syntax error in expression.")
            PARSE( SUBEXPRESSION, p->down)
            return 1;
         }

         /*
          *   Array reference and user function call.
          */

         if (p->is3( "var", "[]", "head") ||
             p->is3( "var", "()", "head"))
         {
            if (p->next->is( "()"))
               p->symbol = "call";
            else
               p->symbol = "ref";

            q = split_down( p->next);
            if (q == nullptr && p->is( "ref"))
               PERROR( p->next, "Array ref must have one or two indices.")

            delete_exp( split( p->next));
            if (q != nullptr)
            {
               PARSE1( EXPLIST, q, q)
               join_down( p, q);
            }

            return 1;
         }

         /*
          *  From here on, only function calls.
          */

         if (!p->next->is2( "()", "head"))
            PERROR( p, "Syntax error in expression.")

         if (p->is( "ceil") || p->is( "cos") || p->is( "flr")
          || p->is( "exp")  || p->is( "log") || p->is( "sin") || p->is( "sqrt")
          || p->is( "tan"))
         {
            q = split_down( p->next);
            if (q == nullptr)
               PERROR( p->next, "No argument for this function.")

            delete_exp( split( p->next));

            PARSE1( EXPRESSION, q, q)
            join_down( s->next, q);
            return 1;
         }

         if (p->is( "max") || p->is( "min") || p->is( "sum")
          || p->is( "prod") || p->is( "first"))
         {
            p = split_down( p->next);
            if (p == nullptr)
            {
               if (s->next->is( "max") || s->next->is( "min")
                || s->next->is( "first"))
                  PERROR1( s->next->next, "Empty iteration.", p)
               delete_exp( split(s->next->next));
               return 1;
            }

            q = p->next->find( ":");
            if (q == nullptr)
            {
               if (s->next->is( "first"))
    PERROR1( s->next->next, "The 'first' function must have an iteration.", p)
               
               delete_exp( split( s->next->next));
               PARSE1( EXPLIST, p, p)
               join_down( s->next, p);
               return 1;
            }

            delete_exp( split( s->next->next));
            q = split_at( q);

            PARSE2( ITERATION, p, p, q)
            if (s->next->is( "first"))
            {
               PARSE2( PREDICATE, q, p, q)
            }
            else
            {
               PARSE2( EXPRESSION, q, p, q)
            }
            join_down( s->next, p);
            join_down( s->next, q);
            return 1;
         }

         if (p->is3( "tv", "()", "head"))
         {
            q = split_down( p->next);
            if (q == nullptr)
               PERROR( p->next, "Predicate required for the tv function.")

            delete_exp( split( p->next));

            PARSE1( PREDICATE, q, q)
            join_down( s->next, q);
            return 1;
         }

         PERROR( p, "Unknown function.")
      }

      default:
      {
         PERROR( s->next, "Parse error: unknown goal.")
      }
   }
}


void
alter_function (expr *f, int num_args, char *var_list)
{
   int i;

   for (f = f->next; !f->hd(); f = f->next)
   {
      if (f->down != nullptr)
         alter_function( f->down, num_args, var_list);
      if (!f->is( "var")) continue;

      for (i=0; i<num_args; i++)
      {
         if (var_list[i] != f->value.i) continue;
         f->value.i = num_args - 1 - i;
         f->symbol = "stack";
         break;
      }
   }
}


void
parse_error( int pos, const char *message)
{
   static char line[LINE_LEN];
   int i, n;

   if (direct)
   {
      printf( "     %s\n", cur_line);
      n = 5 + pos;
   }
   else
   {
      sprintf( line, "     %-.3f  ", ((float) step_number) / 1000.0);

      n = strlen( line) + pos;
      printf( "%s%s\n", line, cur_line);
   }

   for (i=0; i<n; i++) line[i] = ' ';
   strcpy( line + n, "|\n");
   printf( line);
   printf( "     %s\n", message);
   return;
}
