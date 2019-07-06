# compile
# g++ -DDEBUG -std=c++11 -o plotScript.o plotScript.cpp plotScript.h

all: plotScript

# plot: plotScript.o
# 	g++ plotScript.o -o plot

plotScript: plotScript_STMAC.cpp plotScript_STMAC.h
	g++ -std=c++11 plotScript_STMAC.cpp plotScript_STMAC.h -o plotScript_STMAC.o

# Debug
all.db: plotScript.db

plotScript.db: plotScript_STMAC.cpp plotScript_STMAC.h
	g++ -g -DDEBUG -std=c++11 plotScript_STMAC.cpp plotScript_STMAC.h -o plotScript_STMAC.o

clean: 
	rm -f *.o