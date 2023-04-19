
# Compilation Command
COMPILE = cc -c

# Link Command
LINK  = cc

# Source directory
SRCDIR  = src

# Temporary directory
TEMPDIR = tmp

# Binary output name
BINARY = fameChecker.o

# Backend output name
BACKEND = backend.o

# Backend temporary files
BACKENDFILES =	$(TEMPDIR)

# Objective temporary files
FILES =	$(TEMPDIR)/Main.o

all: $(BINARY)

backend-test: $(BACKEND)

backend: $(BACKENDFILES)

clean:
	rm $(BINARY) $(TEMPDIR)/$(BACKEND) $(BACKEND) $(FILES);\
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

$(TEMPDIR)/Route.o: 	$(TEMPDIR) \
					$(SRCDIR)/backend/route.h \
					$(SRCDIR)/backend/Route.c
	$(COMPILE) $(SRCDIR)/backend/Route.c -o $(TEMPDIR)/Route.o

$(TEMPDIR)/data.o: 	$(TEMPDIR) \
					$(SRCDIR)/backend/data.c
	$(COMPILE) $(SRCDIR)/backend/data.c -o $(TEMPDIR)/data.o

# The main Function
$(TEMPDIR)/Main.o: 	$(TEMPDIR)\
					$(SRCDIR)/main.c
	$(COMPILE) $(SRCDIR)/main.c -o $(TEMPDIR)/Main.o

# The final binary depends of all temporary files
$(BINARY): $(FILES)
	$(LINK) $(FILES) -o $(BINARY)
