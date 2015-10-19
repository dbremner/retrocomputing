# First define the standard options for bcpl
# All of these can be overridden explicitly when make is invoked.
BCPL = bcpl
BCPF = opt '"-w30000,$$UNIX"'
BCPLLIB = /usr/sol/jim/bcpl
~   = $(HOME)
BCPT = cpt
BCPLTF = opt L30000/W40000

# Insert the following line IF the bcpl program uses static variables
# (it makes the TEXT segement non shareable and read write.)
LINKOPT = -Qoption ld -N

# Default rules for making bcpl files
.SUFFIXES: .bcp .hx1

# How to make a .o file from a .bcp file
.bcp.o:
	$(BCPL) from $< to $@ $(BCPF) 
	-chmod 0666 $@
	
.bcp.hx1:
	$(BCPT) $< to $@ define "unix,t4c,transputer" $(BCPLTF) 
	-chmod 0666 $@

.bcp.hx8:
	cpt $< to $@ define "unix,T8,transputer"
	-chmod 0666 $@

#List of object files which make up the front end of the compiler
OBJECTS = $(BCPLLIB)/first.o \
	  $(BCPLLIB)/blib.o 
	           
#List of source file names
SOURCES = btoc.bcp \
	   toc.bcp \
	   lex.bcp \
	   syn.bcp   

# List of header files
HEADERS =  btochdr \
	   synhdr  \
	   string-to-number

btoc:    $(OBJECTS) \
             btoc.o \
              toc.o \
              lex.o \
              syn.o        
	cc $(LINKOPT) \
           $(OBJECTS) \
	   btoc.o  \
	    toc.o  \
	    lex.o  \
	    syn.o  \
	   $(BCPLLIB)/last.o \
	-o btoc
	mv btoc /usr/sol/jim/bin
