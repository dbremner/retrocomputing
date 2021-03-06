#include <stdio.h>

#define bytesperword 4
#define c_esc 0x1B

#define true -1
#define false 0

typedef long word;
typedef char* string;

/* This is a library of I/O routines which conform in name
   and function to those in the BCPL runtime system. By adding
   this to the C version of a BCPL program no routine names or
   string processing code need be changed.
*/

word result2;

FILE *cis;
FILE *cos;

word fputc();
word fgetc();

word (*wrch_fn)();  /* replaceable functions */
word (*rdch_fn)();
char lastc;

wrch(c)
{
   if ( c == c_esc )
      fflush( cos );
   else
      (*wrch_fn)(c,cos);
}

rdch() { return lastc = (*rdch_fn)(cis); }

unrdch() { ungetc(lastc, cis); return -1; }

selectinput(fd)
FILE *fd;
{ cis = fd; }

selectoutput(fd)
FILE *fd;
{ cos = fd; }

FILE *input() { return cis; }
FILE *output() { return cos; }

FILE *findinput( name )
string name;
{
   FILE *fd;
   char uname[ 40 ];
   bcpl2cstr( name, uname );
   fd = fopen( uname , "r+" );
   return fd;
}

FILE *findoutput( name )
string name;
{
   FILE *fd;
   char uname[ 40 ];
   bcpl2cstr( name , uname );
   fd = fopen( uname , "w" );
   return fd;
}

endread( )
{
   fclose( cis );
}

endwrite( )
{
   fclose( cos );
}

endstream( fd )
FILE *fd;
{
   fclose( fd );
}

word readwords( buf , n )
word *buf, n;
{
   word res;
   res = fread( buf, sizeof(word), n, cis);
   if ( res < n ) return -res;
   return res;
}

word writewords( buf, n )
word *buf, n;
{
   return fwrite( buf, sizeof(word), n, cos);
}

pointword( n )
{
   fseek( cis, n*bytesperword, 0);
}

newline ( )
{
   wrch ( '\n' );
}

writed ( n, d )
word n, d;
{
   word j; /* FOR loop variables */
   word t [ 10 ];
   word i = 0;
   word k =  - n;
   if ( n < 0 )
   {
      d = d - 1;
      k = n;
   }
   do
   {
      t[ i ] =  -( k % 10 );
      k = k / 10;
      i = i + 1;
   } while ( ! ( k == 0 )  );
   for ( j = i + 1 ; j <= d ; j = j + 1 )
      wrch ( ' ' );
   if ( n < 0 )
      wrch ( '-' );
   for ( j = i - 1 ; j >= 0 ; j = j +  -( 1 ) )
      wrch ( t[ j ] + '0' );
}

writen ( n )
word n;
{
   writed ( n, 0 );
}

writehex ( n, d )
word n, d;
{
   static word digits[] = {
      '0', '1', '2', '3', '4', '5', '6', '7', 
      '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
   if ( d > 1 )
      writehex ( n >> 4, d - 1 );
   wrch ( digits[ n & 15 ] );
}

writeoct ( n, d )
word n, d;
{
   if ( d > 1 )
      writeoct ( n >> 3, d - 1 );
   wrch ( ( n & 7 ) + '0' );
}

writes ( s )
string s;
{
   word i; /* FOR loop variables */
   for ( i = 1 ; i <= s[ 0 ] ; i = i + 1 )
      wrch ( s[ i ] );
}

writet ( s, n )
string s;
{
   word i; /* FOR loop variables */
   writes ( s );
   for ( i = 1 ; i <= ( n - s[ 0 ] ) ; i = i + 1 )
      wrch ( ' ' );
}

writeu ( n, d )
word n, d;
{
   word m = ( n >> 1 ) / 5;
   if ( ! ( m == 0 ) )
   {
      writed ( m, d - 1 );
      d = 1;
   }
   writed ( n - ( m * 10 ), d );
}

writef ( format, a, b, c, d, e, f, g, h, i, j, k )
string format;
{
   word p; /* FOR loop variables */
   word *t =  &( a );
   for ( p = 1 ; p <= format[ 0 ] ; p = p + 1 )
   {
      word k = format[ p ];
      if ( k == '%' )
      {
         word (*f)();
         word arg = t[ 0 ];
         word n = 0;
         p = p + 1;
         {
            word type = capitalch(format[ p ]);
            switch ( type )
            {
            default:
               wrch ( type );
               break;
            case 'S':
               f = writes;
               goto l;
            case 'T':
               f = writet;
               goto m;
            case 'C':
               f = wrch;
               goto l;
            case 'O':
               f = writeoct;
               goto m;
            case 'X':
               f = writehex;
               goto m;
            case 'I':
               f = writed;
               goto m;
            case 'N':
               f = writen;
               goto l;
            case 'U':
               f = writeu;
               goto m;
            m:
               p = p + 1;
               n = format[ p ];
               n = ( '0' <= n ) && ( n <= '9' ) ? n - '0' : ( 10 + n ) - 
                  'A';
            l:
               (*f) ( arg, n );
            case '$':
               t = t + 1;
               break;
            }
         }
      }
      else
         wrch ( k );
   }
}

word capitalch ( ch )
word ch;
{
    return ( 'a' <= ch ) && ( ch <= 'z' ) ? ( ch + 'A' ) - 'a' : ch;
}

word compch( ch1, ch2 )
word ch1, ch2;
{
   return capitalch ( ch1 ) - capitalch ( ch2 );
}

word compstring( s1, s2 )
string s1, s2;
{
   word i;
   word lens1 = s1[ 0 ];
   word lens2 = s2[ 0 ];
   string smaller = lens1 < lens2 ? s1 : s2;

   for ( i = 1 ; i <= smaller[ 0 ] ; i++ )
   {
      word res = compch( s1[ i ], s2[ i ] );
      if ( ! res == 0 ) return res;
   }
   if ( lens1 == lens2 ) return 0;
   return smaller == s1 ? -1 : 1;
}

word readn ( )
{
   word sum = 0;
   word ch = 0;
   word neg = false;
l:
   ch = rdch();
   if ( ! ( ( '0' <= ch ) && ( ch <= '9' ) ) )
      switch ( ch )
      {
      default:
         unrdch (  );
         result2 =  - 1;
         return 0;
      case ' ':
      case '\t':
      case '\n':
         goto l;
      case '-':
         neg = true;
      case '+':
         ch = rdch();
      }
   while ( ( '0' <= ch ) && ( ch <= '9' ) )
   {
      sum = ( ( 10 * sum ) + ch ) - '0';
      ch = rdch();
   }
   if ( neg )
      sum =  - sum;
   unrdch (  );
   result2 = 0;
   return sum;
}

word *getvec( upb )
word upb;
{
   return (word*) malloc( (upb+1)*bytesperword );
}

freevec( ptr )
word *ptr;
{
   free( ptr );
}

static word argcount;
static string *argvec;

main(argc, argv, envp)
word argc;
string *argv, *envp;
{
   argcount = argc;
   argvec = argv;
   wrch_fn = fputc;
   rdch_fn = fgetc;
   cis = stdin;
   cos = stdout;
   start( );
}

word rdargs( args, argv, size )
string args;
word *argv, size;
{
   word p, i;
   word *w = argv;
   word numbargs;

   *w = 0;
   for ( p = 1 ; p <= args[0] ; p++ )
   {
      word kch = args[p];
      if ( kch == '/' )
      {
         word c = capitalch( args[ p + 1 ] );
         if ( c == 'A' ) w[0] = w[0] | 1;
         if ( c == 'K' ) w[0] = w[0] | 2;
         if ( c == 'S' ) w[0] = w[0] | 4;
         continue;
      }
      if ( kch == ',' )
      {
         w++;
         *w = 0;
      }
   }
   w = w + 1;
   numbargs = w-argv;

   for ( p = 1 ; p < argcount; p++ )
   {
      word argno = -1;
      word wsize = size + (word)argv - (word)w;
      char arg[40];

      c2bcplstr( argvec[p] , arg );

      argno = findarg( args, arg );
      if ( argno >= 0 )
         if ( ( 4 <= argv[argno] ) && (argv[argno] <= 7) )
         {
            argv[argno] = true;
            continue;
         }
         else
         {
            p++;
            if ( argvec[p][0] == '=' ) p++;
            if ( p >= argcount ) goto error;
            c2bcplstr( argvec[p], arg );
         }

      if ( ( wsize * bytesperword ) < arg[0] ) goto error;

      if ( argno < 0 )
         for ( i = 0 ; i < numbargs ; i = i + 1 )
         {
            switch ( argv[ i ] )
            {
            case 0:
            case 1:
               argno = i; goto done;
            case 2:
            case 3:
               goto error;
            }
         }
      if ( argno < 0 ) goto error;
   done:
      argv[argno] = (word)w;
      for ( i = 0 ; i <= arg[0] ; i++ ) ((char*)w)[i] = arg[i];
      w = w + arg[0]/bytesperword + 1;
   }

   for ( i = 0 ; i < numbargs ; i++ )
   {
      word a = argv[i];
      if ( ( 0 <= a ) && ( a <= 7 ) )
         if ( (a & 1) == 0 ) argv[i] = 0;
         else goto error;
   }
   return (w - argv);

error:
   return 0;
}

word findarg ( keys, w )
string keys, w;
{
   word i; /* FOR loop variables */
   /* result =argno if found */
   /*        =-1 otherwise */
   word matching = 0;
   word skipping = 1;
   
   word state = matching;
   word wp = 0;
   word argno = 0;
   for ( i = 1 ; i <= keys[ 0 ] ; i = i + 1 )
   {
      word kch = keys[ i ];
      if ( state == matching )
      {
         if ( ( ( ( kch == '=' ) || ( kch == '/' ) ) || ( kch == ',' ) ) && 
            ( wp == w[ 0 ] ) )
            return argno;
         wp = wp + 1;
         if ( ! ( compch(kch, w[ wp ]) == 0 ) )
            state = skipping;
      }
      if ( ( kch == ',' ) || ( kch == '=' ) )
      {
         state = matching;
         wp = 0;
      }
      if ( kch == ',' )
         argno = argno + 1;
   }
   if ( ( state == matching ) && ( wp == w[ 0 ] ) )
      return argno;
   return  -1;
}

c2bcplstr( s1, s2 )
string s1, s2;
{
   word i;
   for ( i = 0 ; s1[i] != 0 ; i++ ) s2[i+1] = s1[i];
   s2[0] = i;
}

bcpl2cstr( s1, s2 )
string s1, s2;
{
   word i;
   for ( i = 1 ; i <= s1[0] ; i++ ) s2[i-1] = s1[i];
   s2[i] = 0;
}
