
# Compilation Command
COMPILE = cc -c

# Link Command
LINK  = cc

# Source directory
SRCDIR  = src

# Temporary directory
TEMPDIR = tmp

# Binary output name
BINARY = fameChecker

# Objective temporary files
FILES =	$(TEMPDIR)/Main.o

all: $(BINARY)

backend: $(BACKENDFILES)

clean:
	rm $(BINARY) $(FILES);\
	rmdir $(TEMPDIR)

# Rules of make...

# Create Temporary folder if not exists
$(TEMPDIR):
	mkdir $(TEMPDIR)

# Backend files
$(TEMPDIR)/List.o: 	$(TEMPDIR) \
					$(SRCDIR)/backend/list.h \
					$(SRCDIR)/backend/List.c
	$(COMPILE) $(SRCDIR)/backend/List.c -o $(TEMPDIR)/List.o

# The main Function
$(TEMPDIR)/Main.o: 	$(TEMPDIR)\
					$(SRCDIR)/main.c
	$(COMPILE) $(SRCDIR)/main.c -o $(TEMPDIR)/Main.o

# The final binary depends of all temporary files
$(BINARY): $(FILES)
	$(LINK) $(FILES) -o $(BINARY)
