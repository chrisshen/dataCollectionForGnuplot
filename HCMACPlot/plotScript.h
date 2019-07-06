/**************************
By Yiwen (Chris) Shen
Apr 26, 2016

Collect simulation data
Main tasks:
1. scan target directory to get file name
2. parse file name
3. read last line of each file
4. save data into vector
5. calculate statistic information, e.g. mean, variance, confidence intervals, CDF
6. save data to file for GNU Plot

Compile command:
g++ -DDEBUG -std=c++11 -o plotScript.o plotScript.cpp plotScript.h

Using C++ standard 11

Debug with arg: -DDEBUG

***************************/

#include <string.h>
#include <iostream>
#include <stdio.h>

#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>

#include <fstream>
#include <limits>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <map>
#include <iomanip>      // std::setw
#include <algorithm>    // std::sort
#include <tuple>        // std::tuple, std::get, std::tie, std::ignore

#define STMAC ;
// #define SCMAC ;

typedef std::map< double, std::vector<double> > dataMapVecType;
typedef std::map< double, double> dataMapType;
typedef std::multimap<float, double> dataMultiMapType;
typedef std::multimap<float, std::map< float, double> > dataMultiMapMapType;
typedef std::vector< std::tuple<float, float, double> > dataVecTupType;

std::istream& ignoreline(std::ifstream&, std::ifstream::pos_type&);

std::string getLastLine(std::ifstream&);

std::vector<double> lineDataReturn( std::string );
std::vector<double> SCMAC_LineDataToVec( std::string );
/** save data into dataMapVecType
*	input: index, line data, ,key(x or y axis)
*/
void saveDelay( int, std::vector<double>, dataMapVecType&, float );

// void saveDelay( int, vector<double>, map< int, vector<double> >&, float );

dataMapType computeAverage( dataMapVecType );

dataMapType computePercentage( dataMapType, int );

/** save data into dataMultiMapMapType, it is shared data
	it need to be called several times to save complete 3D data
*/
void combine3DData( dataMapType, float, dataVecTupType& );

dataMapType computeVariance( dataMapVecType, dataMapType );

dataMapType computeConfiInterval( dataMapVecType, dataMapType );

dataMultiMapType getCDF( dataMapVecType );

void saveDataToFile(std::string, dataMapType, dataMapType );

void saveDataToFile(std::string, dataMultiMapType );

void saveDataToFile(std::string, dataMapType );

void HCMAC(	struct dirent*, 
											DIR*, 
											dataMapVecType&, 
											dataMapVecType&, 
											dataMapVecType&, 
											dataMapVecType&, 
											dataMapVecType&, 
											dataMapVecType&, 
											dataMapVecType& );

void lineDataToVec(std::string line, std::vector<double>&);

void EDCA(	struct dirent*, 
											DIR*, 
											dataMapVecType&, 
											dataMapVecType&, 
											dataMapVecType&
											);

void OCB(		std::string,
						struct dirent*, 
											DIR*, 
											dataMapVecType&, 
											dataMapVecType&, 
											dataMapVecType&
											);

std::vector<double> CSVLineDataToVec(std::string line);