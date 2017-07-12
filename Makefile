### Makefile --- 
## 
## Description: 
## Author: Hongyi Wu(吴鸿毅)
## Email: wuhongyi@qq.com 
## Created: 四 12月  8 19:23:25 2016 (+0800)
## Last-Updated: 五 7月  7 19:42:47 2017 (+0800)
##           By: Hongyi Wu(吴鸿毅)
##     Update #: 5
## URL: http://wuhongyi.cn 

OBJ = offline
OBJ1 = mainpsd
OBJ2 = mainenergy
OBJ3 = mainshortlong


sourcefile = main.cc offline.hh offline.cc pkuFFTW.hh pkuFFTW.cc
sourcefile1 = mainpsd.cc offline.hh offline.cc pkuFFTW.hh pkuFFTW.cc
sourcefile2 = mainenergy.cc offline.hh offline.cc pkuFFTW.hh pkuFFTW.cc
sourcefile3 = mainshortlong.cc offline.hh offline.cc pkuFFTW.hh pkuFFTW.cc

ROOTCFLAGS  = $(shell root-config --cflags)
ROOTLIBS    = $(shell root-config --libs)

CFLAGS = -g -Wall -O2 -I$(TARTSYS)/include -L$(TARTSYS)/lib -lfftw3 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

GXX = g++ 

all:$(OBJ) $(OBJ1) $(OBJ2) $(OBJ3)

$(OBJ): $(sourcefile)
	$(GXX) $(CFLAGS) $(ROOTCFLAGS) $(ROOTLIBS) -o $@ $(filter %.cc ,$(sourcefile))

$(OBJ1): $(sourcefile1)
	$(GXX) $(CFLAGS) $(ROOTCFLAGS) $(ROOTLIBS) -o $@ $(filter %.cc ,$(sourcefile1))

$(OBJ2): $(sourcefile2)
	$(GXX) $(CFLAGS) $(ROOTCFLAGS) $(ROOTLIBS) -o $@ $(filter %.cc ,$(sourcefile2))

$(OBJ3): $(sourcefile3)
	$(GXX) $(CFLAGS) $(ROOTCFLAGS) $(ROOTLIBS) -o $@ $(filter %.cc ,$(sourcefile3))


clean:
	rm -f *~ *.o $(OBJ) $(OBJ1) $(OBJ2) $(OBJ3)


######################################################################
### Makefile ends here
