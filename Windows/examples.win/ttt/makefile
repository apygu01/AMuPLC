# ttt makefile
SNAME = ttt
PROGRAM = ttt.mex
SOURCES = ttt_000.meg \
          ttt_015.meg \
          ttt_018.meg
OBJECTS = ttt_000.mec \
          ttt_015.mec \
          ttt_018.mec
MEC = mec -m1
MLK = mlk -m1
.SILENT:
.SUFFIXES:
.SUFFIXES: .meg .mec
.meg.mec:
	 $(MEC) $<
all: $(PROGRAM)
$(PROGRAM): $(OBJECTS)
	    mlk $(SNAME)
ttt_015.mec:ttt_000.meg
ttt_018.mec:ttt_000.meg
