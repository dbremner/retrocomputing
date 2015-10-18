#include <stdio.h>
#include <string.h>
#include "joss.hpp"

char input_line[LINE_LEN];
char *cur_line;
char *subcommand_end;

expr *root;
int step_number;
int direct;
int DEBUG;

extern forms F;
extern steps S;

main()
{
   expr *e;
   double x_result;
   int i_result;

   DEBUG = 0;

   F.init();
   S.init();
   init_vars();

   while( read_line( ": "))
   {
      root = nullptr;
      if (!scan())
      {
         delete_exp( root);
         continue;
      }

      if (DEBUG)
      {
         printf( "Scanned the line:\n     ");
         for (e = root->next; !e->is("head"); e = e->next)
            printf( " %s", e->symbol.c_str());
         printf( "\n");
      }

      if (!parse( COMMAND, root))
      {
         delete_exp( root);
         continue;
      }

      if (DEBUG)
         tree_print( 0, root);

      if (root->next->is( "Quit"))
         break;
      if (!direct)
         S.insert( step_number, cur_line, root);
      else
      {
         init_eval();
         (void) eval( root, &x_result, &i_result);
         delete_exp( root);
      }
      root = nullptr;
   }
}


int
read_line( const char *prompt)
{
   int len;

   for(;;)
   {
      printf( prompt);
      if (!fgets( input_line, LINE_LEN, stdin)) return 0;

      trim( input_line);
      len = strlen( input_line);
      if (len == 0) continue;

      if (input_line[0] == '*' || input_line[len-1] == '*') continue;

      cur_line = &input_line[0];
      subcommand_end = &input_line[len-1];
      if (*subcommand_end == '.') subcommand_end--;
      return 1;
   }
}
