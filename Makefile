SHELL    = /bin/bash

ifeq ($(CXX),)
CXX=g++
endif
CFLAGS   = -g -Wall -W -ansi -pedantic -O2
CXXFLAGS = $(CFLAGS) -std=c++11
LIBS     = -L/usr/X11R6/lib -lX11 -lXext -lpthread
RM       = rm -f

EXECSPP = SERV

all :: $(EXECSPP)

.cpp.o :
	$(CXX) -c $(CXXFLAGS) $*.cpp

$(EXECSPP): sockinte sockcli sockserv

sockinte : cli_req.o serv_req.o sockinte.o
	$(CXX) -o $@ $^ $(LIBS)

sockserv : serv_req.o sockserv.o
	$(CXX) -o $@ $^ $(LIBS)

sockcli : cli_req.o sockcli.o
	$(CXX) -o $@ $^ $(LIBS)

depends :
	$(CXX) -MM $(CXXFLAGS) *.cpp >depends

clean ::
	$(RM) *.o core

distclean :: clean
	$(RM) *% *~ .*.swp $(EXECSPP)

include depends
