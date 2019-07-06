# compile
# g++ -DDEBUG -std=c++11 -o plotScript.o plotScript.cpp plotScript.h

all: plotScript

# plot: plotScript.o
# 	g++ plotScript.o -o plot

plotScript: plotScript.cpp plotScript.h
	g++ -std=c++11 plotScript.cpp plotScript.h -o plotScript.o

# Debug
all.db: plotScript.db

plotScript.db: plotScript.cpp plotScript.h
	g++ -g -DDEBUG -std=c++11 plotScript.cpp plotScript.h -o plotScript.o

clean: 
	rm -f *.o