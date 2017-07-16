# compile
# g++ -DDEBUG -std=c++11 -o plotScript.o plotScript.cpp plotScript.h

all: plotScript_EDCA

# plot: plotScript.o
# 	g++ plotScript.o -o plot

plotScript_EDCA: plotScript_EDCA.cpp plotScript_EDCA.h
	g++ -std=c++11 plotScript_EDCA.cpp plotScript_EDCA.h -o plotScript_EDCA.o

# Debug
all.db: plotScript_EDCA.db

plotScript_EDCA.db: plotScript_EDCA.cpp plotScript_EDCA.h
	g++ -g -DDEBUG -std=c++11 plotScript_EDCA.cpp plotScript_EDCA.h -o plotScript_EDCA.o

clean: 
	rm -f *.o