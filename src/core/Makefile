MTL       = ../mtl
VPL 	  = ../../VeriPB_Prooflogger/core
CHDRS     = $(wildcard *.h) $(wildcard prooflogging/*.h) $(wildcard $(MTL)/*.h) $(wildcard $(VPL)/*.h) 
CSRCS     = $(wildcard *.C) $(wildcard prooflogging/*.cc)
EXEC      = qmaxsat
CFLAGS    = -I$(MTL) -I.  -I$(VPL) -I$(prooflogging) -Wall -ffloat-store
LFLAGS    = -lz

include ../mtl/template.mk
