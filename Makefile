# Compilateur
CPP=g++

# Les différents répertoires contenant respectivement les fichiers :
# Sources *.c, Headers *.h, Objets *.o, l'exécutable
SRCDIR=Src
HEADDIR=Include
LIBDIR=Objet
BINDIR=Bin

# Pour lier cplex
SYSTEM = x86-64_sles10_4.1
LIBFORMAT = static_pic
CPLEXDIR = /opt/ibm/ILOG/CPLEX_Studio125/cplex
CONCERTDIR = /opt/ibm/ILOG/CPLEX_Studio125/concert
CPLEXBINDIR = $(CPLEXDIR)/bin/$(SYSTEM)
CPLEXLIBDIR = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CPLEXFLAGS = -I $(CPLEXDIR)/include -I $(CONCERTDIR)/include -L $(CPLEXLIBDIR) -lilocplex -lcplex -L $(CONCERTLIBDIR) -lconcert -lm -m64 -lpthread

# Les différentes options de compilations
# -fopenmp : pour le multithreading
# -g : mode debug  /  -O6 : optimisé
CFLAGS=-I Include -g -Wall -ansi -pedantic -DIL_STD
# Les différents FrameWorks et bibliothèques pour le linkage
GLLIBS= -lstdc++

# Où trouver les différents sources *.cpp qu'il faudra compiler
# pour créer les objets correspondants
SRCPP= $(wildcard $(SRCDIR)/*.cpp)
OBJCPP= $(SRCPP:$(SRCDIR)/%.cpp=$(LIBDIR)/%.o)

# L'éxécutable
all: MonExec

#Création de l'exécutable
MonExec: $(LIBDIR)/main.o $(OBJCPP)
	$(CPP) -o $(BINDIR)/$@ $^ $(CFLAGS) $(CPLEXFLAGS) $(GLLIBS)

# Création de main.o
$(LIBDIR)/main.o: main.cpp
	mkdir -p $(BINDIR) $(LIBDIR)
	$(CPP) -o $@ -c $< $(CFLAGS) $(CPLEXFLAGS)

# Création des différents *.o à partir des *.cpp
$(LIBDIR)/%.o: $(SRCDIR)/%.cpp $(HEADDIR)/%.h
	$(CPP) -o $@ -c $< $(CFLAGS) $(CPLEXFLAGS)

# Nettoyage des objets => Tout sera recompilé !
clean:
	rm $(LIBDIR)/*.o
	rmdir $(LIBDIR)

# Nettoyage complet => clean + effacement du l'exécutable
Clean: clean
	rm $(BINDIR)/*
	rmdir $(BINDIR)
