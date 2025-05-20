# www makefile
SNAME = www
PROGRAM = www.mex
SOURCES = www_000.meg \
          www_015.meg \
          www_019.meg \
          www_01e.meg
OBJECTS = www_000.mec \
          www_015.mec \
          www_019.mec \
          www_01e.mec
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
www_015.mec:www_000.meg
www_019.mec:www_000.meg
www_01e.mec:www_000.meg
