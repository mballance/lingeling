#****************************************************************************
#* makefile.vs
#*
#* Makefile for building lgl library using Visual Studio compilers
#****************************************************************************
# MAKEFLAGS=-j $(if $(CORES),$(CORES),1)

CC=cl
#CFLAGS=-W -Wall -O3 -DNLGLOG -DNDEBUG -DNCHKSOL -DNLGLDRUPLIG -DNLGLYALSAT -DNLGLFILES -DNLGLDEMA
CFLAGS=-DNLGLOG -DNDEBUG -DNCHKSOL -DNLGLDRUPLIG -DNLGLYALSAT -DNLGLFILES -DNLGLDEMA

all: lgl.dll

lgl.dll: lglib.obj lglbnr.obj lgldimacs.obj lglopts.obj 
	link -dll -out:lgl.dll -implib:lgl.lib \
		lglib.obj lglbnr.obj lgldimacs.obj lglopts.obj \
		kernel32.lib

%.obj : %.c
	$(CC) $(CFLAGS) -c

#lglcfg.h: VERSION mkconfig.sh lglbnr.c lglib.c lglmain.c lglcflags.h makefile
#	rm -f $@
#	./mkconfig.sh > $@
#lglcflags.h: makefile
#	rm -f $@
#	echo '#define LGL_CC "$(shell $(CC) --version|head -1)"' >> $@
#	echo '#define LGL_CFLAGS "$(CFLAGS)"' >> $@

clean: 
	del /q /f *.obj


