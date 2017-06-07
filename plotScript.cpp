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

#include "plotScript.h"

using namespace std;

std::istream& 
ignoreline(std::ifstream& in, std::ifstream::pos_type& pos) {
    pos = in.tellg();
    return in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string 
getLastLine(std::ifstream& in) {
    std::ifstream::pos_type pos = in.tellg();

    std::ifstream::pos_type lastPos;
    while (in >> std::ws && ignoreline(in, lastPos))
        pos = lastPos;

    in.clear();
    in.seekg(pos);

    std::string line;
    std::getline(in, line);
    return line;
}

vector<double>
lineDataReturn(string line) {

	std::string::size_type foundInData = 0;
	std::string::size_type foundInDataLast = 0;
	vector<double> lineData;

	lineData.clear();
	// int index = 0;
#ifdef DEBUG
	// cout << "1. "<< foundInData << endl;
#endif
	while ( foundInData <= line.length() ){
		// read 1st data
		if ( foundInData == 0 ){
			foundInData = line.find( ',', foundInData);
			if ( foundInData != std::string::npos ){
#ifdef DEBUG
				// cout << "2. " << foundInData << ",#" << foundInDataLast << ",#" 
				// << line.substr( foundInDataLast, foundInData - foundInDataLast) << endl;
#endif
				double data = stod( line.substr( foundInDataLast , foundInData - foundInDataLast) );
				lineData.push_back(data);

				foundInDataLast = foundInData;
				// cout << data << endl;
			}
		}
		// read 2nd and following data
		else {
			foundInData = line.find( ',', foundInData + 1);
			if ( foundInData != std::string::npos ){
#ifdef DEBUG						
				// cout << "3. " << foundInData << ",#" << foundInDataLast << ",#" 
				// << line.substr( foundInDataLast + 1, foundInData - foundInDataLast) << endl;
#endif
				double data = stod( line.substr( foundInDataLast + 1, foundInData - foundInDataLast) );
				lineData.push_back(data);

				foundInDataLast = foundInData;

				// cout << data << endl;
			}
			// read last data
			else if ( foundInDataLast <= line.length() ){
#ifdef DEBUG
				// cout << "4. " << foundInData << ",#" << foundInDataLast << ",#" 
				// << line.substr( foundInDataLast + 1, foundInData - foundInDataLast) << endl;
#endif
				double data = stod( line.substr( foundInDataLast + 1, line.length() - foundInDataLast) );
				lineData.push_back(data);

				foundInDataLast = foundInData;

				// cout << data << endl;
			}
		}
		// index++;
	}

	return lineData;
}

vector<double>
SCMAC_LineDataToVec(string line) {

	vector<double> lineData;

	lineData.clear();
	// int index = 0;
#ifdef DEBUG
	// cout << "1. "<< foundInData << endl;
#endif
	
	string::size_type posStr;
	string::size_type posS = 0;
	string::size_type posE = 0;

	while ( posE < line.length() ){
		string s1 = "SuccessfulRatio";
		if ( (posStr = line.find(s1, posS)) != string::npos ){
			int i = 0;
			posS += posStr + s1.size() + 1;
			while( line[posS + i] != ',' && posS + i < line.length() )
				i++;
			posE = posS + i;
			double data = stod( line.substr(posS, posE-posS) );
			lineData.push_back(data);

			posS = posE + 1;
		}

		string s2 = "MaxE2Edelay";
		if ( (posStr = line.find(s2, posS)) != string::npos ){
			int i = 0;
			posS += s2.size() + 1;
			while( line[posS + i] != ',' && posS + i < line.length() )
				i++;
			posE = posS + i;
			double data = stod( line.substr(posS, posE-posS) );
			lineData.push_back(data);

			posS = posE + 1;
		}

		string s3 = "MeanE2Edelay";
		if ( (posStr = line.find(s3, posS)) != string::npos ){
			int i = 0;
			posS += s3.size() + 1;
			while( line[posS + i] != ',' && posS + i < line.length())
				i++;
			posE = posS + i;
			double data = stod( line.substr(posS, posE-posS) );
			lineData.push_back(data);

			posS = posE + 1;
		}
	}

#ifdef DEBUG
	cout << "@FUN: lineDataToVec(string line): " << endl;
	for ( auto i:lineData)
		cout << setw(12) << i;
	cout << endl;
#endif

	return lineData;
}

void
saveDelay( int index, vector<double> lineData, dataMapVecType& data, float xAxis ){
	if ( data.find(xAxis) != data.end() ){
		vector<double> t;
		data.insert( pair< float, vector<double> >(xAxis, t) );

		data[xAxis].push_back(lineData[index]);
	}
	else {
		data[xAxis].push_back(lineData[index]);
	}
}

// void
// saveDelay( int index, vector<double> lineData, map< float, vector<double> > &data, float xAxis ){
// 	if ( data.find(xAxis) != data.end() ){
// 		vector<double> t;
// 		data.insert( pair< float, vector<double> >(xAxis, t) );

// 		data[xAxis].push_back(lineData[index]);
// 	}
// 	else {
// 		data[xAxis].push_back(lineData[index]);
// 	}
// }

dataMapType
computeAverage( dataMapVecType data ){
	
	dataMapType dataAverage;
	dataMapVecType::iterator it;
	
	for ( it = data.begin(); it != data.end(); it++ ){
		dataAverage.insert( pair<float, double>(it->first, 0.0) );
	}

	for ( it = data.begin(); it != data.end(); it++ ){
		vector<double>::iterator it_d;
		double sum = 0.0;

		for ( it_d = it->second.begin(); it_d != it->second.end(); it_d++ ){
			sum = sum + *it_d;
		}

		double mean = sum / (double)( it->second.size() );

		dataAverage[it->first] = mean;
	}

#ifdef DEBUG 
	// dataMapType::iterator it_map;
	// cout << endl << "Mean: " << endl;
	// for ( it_map = dataAverage.begin(); it_map != dataAverage.end(); it_map++ ){
	// 	cout << it_map->first << ": " << it_map->second << endl;
	// }
	// cout << endl;
#endif

	return dataAverage;
}

dataMapType
computePercentage( dataMapType data, int sum ){
	
	dataMapType dataPercentage;
	dataMapType::iterator it;
	
	for ( it = data.begin(); it != data.end(); it++ ){
		dataPercentage.insert( pair<float, double>(it->first, 0.0) );
	}

	for ( it = data.begin(); it != data.end(); it++ ){
		
		double ratio = it->second / sum * 100;

		dataPercentage[it->first] = ratio;
	}

#ifdef DEBUG 
	dataMapType::iterator it_map;
	cout << endl << "Percentage: " << endl;
	for ( it_map = dataPercentage.begin(); it_map != dataPercentage.end(); it_map++ ){
		cout << it_map->first << ": " << it_map->second << endl;
	}
	cout << endl;
#endif

	return dataPercentage;
}

void 
combine3DData( dataMapType data, float xAxis, dataVecTupType& dataReturn){
	// dataMultiMapMapType dataReturn;
	dataMapType::iterator it_map;
	// dataMultiMapMapType::iterator it_multi;
	std::tuple<float, float, double> tmpDataEntry;

	for ( it_map = data.begin(); it_map != data.end(); it_map++ ){
		tmpDataEntry = make_tuple(xAxis, it_map->first, it_map->second);
		dataReturn.push_back( tmpDataEntry );
	}

#ifdef DEBUG 

#endif
	cout << "size: " << dataReturn.size() << endl;
}

dataMapType
computeVariance( dataMapVecType data, dataMapType mean ){
	
	dataMapType dataVariance;
	dataMapVecType::iterator it;
	
	for ( it = data.begin(); it != data.end(); it++ ){
		dataVariance.insert( pair<float, double>(it->first, 0.0) );
	}

	for ( it = data.begin(); it != data.end(); it++ ){
		vector<double>::iterator it_d;
		double sum = 0.0;

		for ( it_d = it->second.begin(); it_d != it->second.end(); it_d++ ){
			// (xk- mean)^2
			sum = sum + pow(*it_d - mean[it->first], 2);
		}

		double var = sum / (double)( it->second.size() - 1 );

		dataVariance[it->first] = var;
	}

#ifdef DEBUG 
	// dataMapType::iterator it_map;
	// cout << endl << "Variance: " << endl;
	// for ( it_map = dataVariance.begin(); it_map != dataVariance.end(); it_map++ ){
	// 	cout << it_map->first << ": " << it_map->second << endl;
	// }
	// cout << endl;
#endif

	return dataVariance;
}

dataMapType
computeConfiInterval( dataMapVecType data, dataMapType var ){
	
	dataMapType dataConfiInterval;
	dataMapType::iterator it;
	
	for ( it = var.begin(); it != var.end(); it++ ){
		dataConfiInterval.insert( pair<float, double>(it->first, 0.0) );
	}

	for ( it = var.begin(); it != var.end(); it++ ){

		// student t dis., T(10-1, 90%) = 1.833, T(6-1, 90%) = 2.015
		double stuT_9 = 1.833;
		double stuT_5 = 2.015;
		
		dataConfiInterval[it->first] = stuT_9 * sqrt( var[it->first] / (double)(data[it->first].size()) );
	}

#ifdef DEBUG 
	// dataMapType::iterator it_map;
	// cout << endl << "Confidence Interval: " << endl;
	// for ( it_map = dataConfiInterval.begin(); it_map != dataConfiInterval.end(); it_map++ ){
	// 	cout << it_map->first << ": " << it_map->second << endl;
	// }
	// cout << endl;
#endif

	return dataConfiInterval;
}

/***************************** 
calculate CDF 
Procedure:
1. (x axil) sort all data nondecreacingly 
2. (y axil) data ratio ( Actual Frequency(%) ): ratio(k) = 1/dataCount + ratio(k-1), if k = 0, ratio(0) = 1/dataCount
// 3. median, stdev
******************************/
dataMultiMapType
getCDF( dataMapVecType data ){
	vector<double> allData;
	dataMapVecType::iterator it_map;

	for ( it_map = data.begin(); it_map != data.end(); it_map++ ){
		allData.insert(allData.end(), it_map->second.begin(), it_map->second.end() );
	}

	// sort
	sort( allData.begin(), allData.end() );

#ifdef DEBUG 
	// vector<double>::iterator it;
	// cout << endl << "allData(Sorted), size = " << allData.size() << endl;
	// for ( it = allData.begin(); it != allData.end(); it++ ){
	// 	cout << *it << " ";
	// }
	// cout << endl;
#endif

	// actFreq
	vector<double> actFreq;
	for ( unsigned int i = 0; i < allData.size(); i++ ){
		if (actFreq.size() == 0 ){
			actFreq.insert( actFreq.end(), 1.0/(double)(allData.size()) );
		}
		else {
			// cout << "Test: " << 1.0/(double)(allData.size()) + actFreq[actFreq.size()-1] << endl;
			actFreq.insert( actFreq.end(), 1.0/(double)(allData.size()) + actFreq[actFreq.size()-1] );
		}		
	}

#ifdef DEBUG 
	// cout << endl << "actFreq, size: " << actFreq.size() << endl;
	// for ( it = actFreq.begin(); it != actFreq.end(); it++ ){
	// 	cout << *it << " ";
	// }
	// cout << endl;
#endif

	dataMultiMapType cdfData;

	for ( size_t ii = 0; ii < allData.size(); ii++ ){
		cdfData.insert( pair<float, double>( allData[ii], actFreq[ii] ) );
	}

#ifdef DEBUG 
	// dataMultiMapType::iterator it_map_d;
	// cout << endl << "cdfData, size: " << cdfData.size() << endl;
	// for ( it_map_d = cdfData.begin(); it_map_d != cdfData.end(); it_map_d++ ){
	// 	cout << it_map_d->first << ": " << "\t" << it_map_d->second << endl;
	// }
	// cout << endl;
#endif

	return cdfData;
}

void
saveDataToFile(string fileName, dataMapType dataMean, dataMapType dataConfiInterval){

	std::fstream plotData;
	plotData.open( fileName ,  ios::out /*| ios::in| ios::app*/ | ios::binary );
	if ( plotData.is_open() ) {
		dataMapType::iterator it_map;
		for ( it_map = dataMean.begin(); it_map != dataMean.end(); it_map++ ){
			plotData << it_map->first << "\t" << it_map->second << "\t" 
			<< dataConfiInterval[it_map->first] << "\n";
		}

		plotData.close();
	}
	else std::cout <<"Unable to write file! \n";
}

void
saveDataToFile(string fileName, dataMultiMapType data ){

	std::fstream plotData;
	plotData.open( fileName ,  ios::out /*| ios::in| ios::app*/ | ios::binary );
	if ( plotData.is_open() ) {
		dataMultiMapType::iterator it_map;
		for ( it_map = data.begin(); it_map != data.end(); it_map++ ){
			plotData << it_map->first << "\t" << it_map->second << "\n";
		}

		plotData.close();
	}
	else std::cout <<"Unable to write file! \n";
}

void
saveDataToFile(string fileName, dataVecTupType data ){

	std::fstream plotData;
	plotData.open( fileName ,  ios::out /*| ios::in| ios::app*/ | ios::binary );
	if ( plotData.is_open() ) {
		dataVecTupType::iterator it;
		for ( it = data.begin(); it != data.end(); it++ ){
			plotData << get<0>(*it) << "\t" << get<1>(*it) << "\t" << get<2>(*it) << "\n";
		}

		plotData.close();
	}
	else std::cout <<"Unable to write file! \n";
}

void
saveDataToFile(string fileName, dataMapType data ){

	std::fstream plotData;
	plotData.open( fileName ,  ios::out /*| ios::in| ios::app*/ | ios::binary );
	if ( plotData.is_open() ) {
		dataMapType::iterator it_map;
		for ( it_map = data.begin(); it_map != data.end(); it_map++ ){
			plotData << it_map->first << "\t" << it_map->second << "\n";
		}

		plotData.close();
	}
	else std::cout <<"Unable to write file! \n";
}

int 
main(){

	// char dir_name[] = "/home/chris/usr/sim/data/20160604_mEVs_SP";
	// char dir_name[] = "/home/chris/usr/RTP/SAINTP_Chris/sim/data/20160530_RTP";
	// 1. 20160425_SP				SAINT+ with a 0.5
	// 2. 20160427_Dijkstra			Dijkstra
	// 3. 20160425_S 				SAINT
	// 4. 20160429_A_02 			SAINT+ with a 0.2
	// 5. 20160429_A_08 			SAINT+ with a 0.8
	// 6. 20160430_speed_SP			SAINT+ speed
	// 7. 20160501_accel_S			SAINT acceleration
	// 8. 20160502_accel_dijkstra	Dijkstra acceleration
	// 9. 20160501_speed_S			SAINT speed
	// 10. 20160502_speed_dijkstra	Dijkstra speed
	// 11. 20160503_accel_SP		SAINT+ accel
	// 12. 20160508_A				SAINT+ alpha, a folder including sub-folders

	// (delete)13. 20160516_RTP		RTP vehicle number
	// 14. 20160530_RTP				RTP vehicle number

	/**************************************
		Multiple EVs Injection Scenario
	***************************************/
	// 15. 20160603_mEVs_dijkstra	mEVs Dijkstra
	// 16. 20160603_mEVs_RTP		mEVs RTP
	// 17. 20160603_mEVs_S 			mEVs SAINT
	// 18. 20160604_mEVs_SP 		mEVs SAINT+, there is a bad SP abondaned

	/**************************************
		SCMAC data
	***************************************/
	char dir_name[] = "/home/chrisomnet/usr/veins_SCMAC/examples/veins/result/20161129_SCMAC";
	// 1. 20161124_SCMAC			SCMAC with density
	// 2. 20161125_SCMAC			...
	// 3. 20161127_SCMAC			...
	// 4. 20161129_SCMAC			...
	// 5. 20161130_WAVE				WAVE with density

	/**************************************
		STMAC data
	***************************************/
	// char dir_name[] = "/home/chrisomnet/usrtmac/veins_stableTMAC/examples/veins/result/STMAC";
	// 1. STMAC			STMAC with density
	// 2. WPCF			...
	// 3. LMA			...
	// 4. 			...
	// 5. 				

	string dir_name_string = dir_name;
	string dataType;
	// cout << dir_name_string << endl;
	if ( dir_name_string.find("20160425_SP") != string::npos ) {
		dataType = "SP";
	}
	/*
	SCMAC
	*/
	else if ( dir_name_string.find("20161129_SCMAC") != string::npos){
		dataType = "SCMAC";
	}
	else if ( dir_name_string.find("20161130_WAVE") != string::npos){
		dataType = "WAVE";
	}
	else {
		dataType = "error";
		throw runtime_error("L:535, Can't identify dataType!");
	}
	
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;

	char *dir;
	
	// dir_name = "/home/chris/usr/sim"
	dir = dir_name;

	if ( (dp = opendir(dir)) == NULL ){
		cout << "cannot open directory!" << endl;
		return 1;
	}

	chdir(dir);

	// int count = 0 ;
	/* SAINT+ */
	/* define data saving place*/
	dataMapVecType dataE2EDelayEV;
	dataMapVecType dataE2EDelayGV;
	dataMapVecType dataLinkDelay;
	dataMapVecType dataMaxLinkDelay;
	dataMapVecType dataEVCount;

	dataVecTupType dataE2EDelayEV3D;
	dataVecTupType dataE2EDelayGV3D;
	dataVecTupType dataLinkDelay3D;
	dataVecTupType dataMaxLinkDelay3D;

	/* SCMAC */
	dataMapVecType dataAvgE2EDelay_SCMAC;
	dataMapVecType dataMaxE2EDelay_SCMAC;
	dataMapVecType dataPktRatio_SCMAC;
	/* end define data saving place*/
	
	/* SAINTP */
	// SAINTP();
	
	/* SCMAC */
	SCMAC(entry, dp, dataPktRatio_SCMAC, dataMaxE2EDelay_SCMAC, dataAvgE2EDelay_SCMAC);

#ifdef DEBUG
	// dataMapVecType::iterator it;
	// vector<double>::iterator it_d;

	// cout << endl <<"E2E delay of Emergency Vehicles (EV): " << endl;
	// for ( it = dataE2EDelayEV.begin(); it != dataE2EDelayEV.end(); it++ ){
	// 	cout << it->first << ": "; 
	// 	for ( it_d = it->second.begin(); it_d != it->second.end(); it_d++ ){
	// 		cout << *it_d << ", ";
	// 	}
	// 	cout << endl;
	// }

	// cout << endl << "E2E delay of General Vehicles (GV): " << endl;
	// for ( it = dataE2EDelayGV.begin(); it != dataE2EDelayGV.end(); it++ ){
	// 	cout << it->first << ": ";
	// 	for ( it_d = it->second.begin(); it_d != it->second.end(); it_d++ ){
	// 		cout << *it_d << ", ";
	// 	}
	// 	cout << endl;
	// }

	// cout << endl << "3D Data:" << endl;
	// for ( size_t i = 0; i < dataE2EDelayGV3D.size(); i++ ){
	// 	cout << get<0>(dataE2EDelayGV3D[i]) << "\t" << get<1>(dataE2EDelayGV3D[i]) << "\t" << get<2>(dataE2EDelayGV3D[i]) << endl;
	// }
#endif

	// get mean
	dataMapType meanPktRatio;
	if ( dataPktRatio_SCMAC.size() != 0)
		meanPktRatio = computeAverage(dataPktRatio_SCMAC);

	dataMapType meanMaxE2EDelay;
	if ( dataMaxE2EDelay_SCMAC.size() != 0)
		meanMaxE2EDelay = computeAverage(dataMaxE2EDelay_SCMAC);

	dataMapType meanAvgE2EDelay;
	if ( dataAvgE2EDelay_SCMAC.size() != 0)
		meanAvgE2EDelay = computeAverage(dataAvgE2EDelay_SCMAC);

	// dataMapType meanMaxLinkDelay;
	// if ( dataMaxLinkDelay.size() != 0)
	// 	meanMaxLinkDelay = computeAverage(dataMaxLinkDelay);

	// dataMapType meanEVCount;
	// if ( dataEVCount.size() != 0 )
	// 	meanEVCount = computeAverage(dataEVCount);

	// dataMapType meanEVCountArrivalRatio;
	// if ( dataEVCount.size() != 0 && dataType.find("mEVs") != std::string::npos )
	// 	meanEVCountArrivalRatio = computePercentage(meanEVCount, 10);

	// calculate variance
	dataMapType variMeanPktRatio;
	variMeanPktRatio = computeVariance(dataPktRatio_SCMAC, meanPktRatio);

	dataMapType variMeanMaxE2EDelay;
	variMeanMaxE2EDelay = computeVariance(dataMaxE2EDelay_SCMAC, meanMaxE2EDelay);

	dataMapType variMeanAvgE2EDelay;
	variMeanAvgE2EDelay = computeVariance(dataAvgE2EDelay_SCMAC, meanAvgE2EDelay);

	// dataMapType varianceMaxLinkDelay;
	// varianceMaxLinkDelay = computeVariance(dataMaxLinkDelay, meanMaxLinkDelay);

	// dataMapType varianceEVCount;
	// varianceEVCount = computeVariance(dataEVCount, meanEVCount);

	// calculate confidence interval
	dataMapType confiIntervalMeanPktRatio;
	confiIntervalMeanPktRatio = computeConfiInterval(dataPktRatio_SCMAC, variMeanPktRatio);

	dataMapType confiIntervalMeanMaxE2EDelay;
	confiIntervalMeanMaxE2EDelay = computeConfiInterval(dataMaxE2EDelay_SCMAC, variMeanMaxE2EDelay);

	dataMapType confiIntervalMeanAvgE2EDelay;
	confiIntervalMeanAvgE2EDelay = computeConfiInterval(dataAvgE2EDelay_SCMAC, variMeanAvgE2EDelay);

	// dataMapType confiIntervalMaxLinkDelay;
	// confiIntervalMaxLinkDelay = computeConfiInterval(dataMaxLinkDelay, varianceMaxLinkDelay);

	// dataMapType confiIntervalEVCount;
	// confiIntervalEVCount = computeConfiInterval(dataEVCount, varianceEVCount);

	// get CDF
	dataMultiMapType cdfDataPktRatio_SCMAC;
	cdfDataPktRatio_SCMAC = getCDF(dataPktRatio_SCMAC);

	dataMultiMapType cdfDataMaxE2EDelay_SCMAC;
	cdfDataMaxE2EDelay_SCMAC = getCDF(dataMaxE2EDelay_SCMAC);

	dataMultiMapType cdfDataAvgE2EDelay_SCMAC;
	cdfDataAvgE2EDelay_SCMAC = getCDF(dataAvgE2EDelay_SCMAC);

	// dataMultiMapType cdfDataMaxLinkDelay;
	// cdfDataMaxLinkDelay = getCDF(dataMaxLinkDelay);

	// dataMultiMapType cdfDataEVCount;
	// cdfDataEVCount = getCDF(dataEVCount);

	/****************************
	 write data for GNU Plot 
	 ****************************/
	// chdir( "/home/chris/usr/sim/data/plot/plotData" );
	chdir( "/home/chrisomnet/usr/SCMAC_gnuplot" );
	// chdir( "/home/chris/usr/RTP/saint/sim/data/plot/plotData" );

	// save EV E2E delay
	string fileName = "plotData_PktRatio_";
	fileName.append(dataType);

	saveDataToFile(fileName, meanPktRatio, confiIntervalMeanPktRatio);

	// save GV E2E delay
	fileName.clear();
	fileName = "plotData_MeanMaxE2EDelay_";
	fileName.append(dataType);

	saveDataToFile(fileName, meanMaxE2EDelay, confiIntervalMeanMaxE2EDelay);

	// save link delay
	fileName.clear();
	fileName = "plotData_MeanAvgE2EDelay_";
	fileName.append(dataType);

	saveDataToFile(fileName, meanAvgE2EDelay, confiIntervalMeanAvgE2EDelay);

	// save max link delay
	// fileName.clear();
	// fileName = "plotData_MaxLinkDelay_";
	// fileName.append(dataType);

	// saveDataToFile(fileName, meanMaxLinkDelay, confiIntervalMaxLinkDelay);	

	// save EV count
	// fileName.clear();
	// fileName = "plotData_EVCount_";
	// fileName.append(dataType);

	// saveDataToFile(fileName, meanEVCount, confiIntervalEVCount);	

	// save EV count
	// fileName.clear();
	// fileName = "plotData_EVCount_";
	// fileName.append(dataType);

	// saveDataToFile(fileName, meanEVCount);

	// save EV arrival ratio
	// if ( meanEVCountArrivalRatio.size() != 0 ){
	// 	fileName.clear();
	// 	fileName = "plotData_EVArrivalRatio_";
	// 	fileName.append(dataType);

	// 	saveDataToFile(fileName, meanEVCountArrivalRatio);		
	// }

	/*****************
			CDF 
	******************/

	// cdf: E2E EV
	fileName.clear();
	fileName = "plotData_cdf_PktRatio_";
	fileName.append(dataType);

	saveDataToFile(fileName, cdfDataPktRatio_SCMAC);	

	// cdf: E2E GV
	fileName.clear();
	fileName = "plotData_cdf_MeanMaxE2EDelay_";
	fileName.append(dataType);

	saveDataToFile(fileName, cdfDataMaxE2EDelay_SCMAC);

	// cdf: link delay
	fileName.clear();
	fileName = "plotData_cdf_MeanAvgE2EDelay_";
	fileName.append(dataType);

	saveDataToFile(fileName, cdfDataAvgE2EDelay_SCMAC);

	// // cdf: max link delay
	// fileName.clear();
	// fileName = "plotData_cdf_MaxLinkDelay_";
	// fileName.append(dataType);

	// saveDataToFile(fileName, cdfDataMaxLinkDelay);

	// // cdf: EV count
	// fileName.clear();
	// fileName = "plotData_cdf_EVCount_";
	// fileName.append(dataType);

	// saveDataToFile(fileName, cdfDataEVCount);

	// // alpha 3d: E2E GV
	// if (dataE2EDelayGV3D.size() != 0 ){
	// 	fileName.clear();
	// 	fileName = "plotData_E2E_GV_3d_";
	// 	fileName.append(dataType);

	// 	saveDataToFile(fileName, dataE2EDelayGV3D);		
	// }


// #endif

	return 0;
}

void
SCMAC(	struct dirent 	*entry, 
		DIR 			*dp, 
		dataMapVecType& dataPktRatio_SCMAC,
		dataMapVecType& dataMaxE2EDelay_SCMAC, 
		dataMapVecType& dataAvgE2EDelay_SCMAC){

	struct stat statbuf;
	// scan directory
	while( (entry = readdir(dp)) != NULL ){
		lstat( entry->d_name, &statbuf );
		if ( !S_ISDIR(statbuf.st_mode) ){
			if ( strcmp(".", entry->d_name) == 0 ||
				 strcmp("..", entry->d_name) == 0 )
				continue;

			// parse file name to get xAxis
			float xAxis = 0.0;
			// float xAxis = 0.0;

			string dirName = entry->d_name;
			std::string::size_type found;

			// for x axis
			// found = dirName.find("E");
			// cout << dirName << endl;
			if ( dirName.find("App") != std::string::npos ){
				if ( ( found = dirName.find("d") ) != std::string::npos ){
					// get x axis
					int i = 0;
					int j = 2;
					// cout << dirName[found] << endl;
					while( dirName[found+j+i] != '_' ){
						// cout << (int)dirName[found - i] << endl;
						i++;
					}

					string tmp = dirName.substr( found+j, i );
					xAxis = stof(tmp);
					xAxis = xAxis * 0.01;
					// cout << xAxis << endl;

					/*
					open file, read last line
					*/
					string line;
					
					std::ifstream testfile ( entry->d_name );
					if (testfile.is_open()){

		    			getline(testfile, line);

						cout << xAxis << ": " << "\t" << line << endl;
						
						vector<double> lineData;

						/***********************
							SCMAC project
						***********************/
						// parse line string into line vector
						// example: 
						// string: Stat,gPeTxPackets,4162,gPeRxPackets,2504,gPeRxDropPkts,345311,SuccessfulRatio,0.601634,MaxE2Edelay,0.0298161,MeanE2Edelay,0.0168287
						// vector: 0.601634		0.0298161	0.0168287
						// index:  0			1      		2      		3       4       5       6
						lineData = SCMAC_LineDataToVec(line);
						
						// random pkt delivery ratio
						saveDelay( 0, lineData, dataPktRatio_SCMAC, xAxis );

						// random pkt max E2E delay
						saveDelay( 1, lineData, dataMaxE2EDelay_SCMAC, xAxis);

						// random pkt avg E2E delay
						saveDelay( 2, lineData, dataAvgE2EDelay_SCMAC, xAxis);

						// link delay
						// saveDelay( 4, lineData, dataLinkDelay, xAxis);

						// max link delay
						// saveDelay( 5, lineData, dataMaxLinkDelay, xAxis);

						/***********************
							End SAINT+ project
						************************/

						testfile.close();
					}
					else std::cout <<"Unable to open file! \n";
					// cout << entry->d_name << endl;	
				}
			}
			else if ( dirName.find("Mac") != std::string::npos ){

			}
			// if ( ( found = dirName.find("speed") ) != std::string::npos ){
			// 	int i = 0;
			// 	int j = 6;
			// 	while( dirName[found + j + i] != '_' ){
			// 		i++;
			// 	}
			// 	string tmp = dirName.substr( found+j, i );
			// 	xAxis = stof(tmp);
			// 	// cout << "TMP: " << tmp << endl;
			// 	// cout << xAxis_f << endl;
			// }

		}
	}
}

int
SAINTP(	string 			dir_name,
		struct dirent 	*entry, 
		DIR 			*dp, 
		dataMapVecType& dataEVCount,
		dataMapVecType& dataE2EDelayEV, 
		dataMapVecType& dataE2EDelayGV,
		dataMapVecType& dataLinkDelay,
		dataMapVecType& dataMaxLinkDelay ){

	struct stat statbuf;
	while( (entry = readdir(dp)) != NULL ){
		lstat( entry->d_name, &statbuf );
		if ( !S_ISDIR(statbuf.st_mode) ){
			if ( strcmp(".", entry->d_name) == 0 ||
				 strcmp("..", entry->d_name) == 0 )
				continue;

			// parse file name to get xAxis
			float xAxis = 0.0;
			// float xAxis = 0.0;

			string dirName = entry->d_name;
			std::string::size_type found;

			// for x axis
			// found = dirName.find("E");
			// cout << dirName << endl;
			if ( ( found = dirName.find("E") ) != std::string::npos ){
				// get x axis
				int i = 2;
				// cout << dirName[found] << endl;
				while( dirName[found - i] != '_' ){
					// cout << (int)dirName[found - i] << endl;
					if ( (int)dirName[found - i] >= 48 && (int)dirName[found - i] <= 57 ){
						xAxis = xAxis + ( (int)dirName[found - i] - 48 ) * pow( 10, i-2 );
					}
					i++;
				}
				// cout << xAxis << endl;			
			}

			if ( ( found = dirName.find("speed") ) != std::string::npos ){
				int i = 0;
				int j = 6;
				while( dirName[found + j + i] != '_' ){
					i++;
				}
				string tmp = dirName.substr( found+j, i );
				xAxis = stof(tmp);
				// cout << "TMP: " << tmp << endl;
				// cout << xAxis_f << endl;
			}

			if ( ( found = dirName.find("accel") ) != std::string::npos ){

				int i = 0;
				int j = 6;
				while( dirName[found + j + i] != '_' ){
					i++;
				}
				string tmp = dirName.substr( found+j, i );
				xAxis = stof(tmp);

			}
			// else if ( ( found = dirName.find("speed") ) != std::string::npos ){

			// }
			// else { throw runtime_error("Can't locate key word in file name!"); }

			// open file, read last line
			string line;
			
			std::ifstream testfile ( entry->d_name );
			if (testfile.is_open()){
				line = getLastLine(testfile);

				cout << xAxis << ": " << "\t" << line << endl;
				
				vector<double> lineData;

				/***********************
					SAINT+ project
				***********************/
				// parse line string into line vector
				// example: 
				// string: 6,154,680,209.294,9.49762,9.52315,4.96902
				// vector: 6.00_154.00_680.00_209.294_9.49762_9.52315_4.96902
				// index:  0	1      2      3       4       5       6
				lineData = lineDataReturn(line);
				
				// EV
				saveDelay( 0, lineData, dataEVCount, xAxis );

				// EV E2E delay data
				saveDelay( 1, lineData, dataE2EDelayEV, xAxis);

				// GV E2E delay data
				saveDelay( 3, lineData, dataE2EDelayGV, xAxis);				

				// link delay
				saveDelay( 4, lineData, dataLinkDelay, xAxis);

				// max link delay
				saveDelay( 5, lineData, dataMaxLinkDelay, xAxis);

				/***********************
					End SAINT+ project
				************************/

				testfile.close();
			}
			else std::cout <<"Unable to open file! \n";
			// cout << entry->d_name << endl;
		}

		/****************************
			is folder, for 3D plot
			each entry of data saved should be like: 
			x_axis y_axis z_axisc
		*****************************/
		else {
			if ( strcmp(".", entry->d_name) == 0 ||
				 strcmp("..", entry->d_name) == 0 )
				continue;

			DIR *dp_sub;
			struct dirent *entry_sub;
			struct stat statbuf_sub;

			const char *dir_sub;
			
			string tem_dir_name = dir_name;
			tem_dir_name.append("/");
			tem_dir_name.append(entry->d_name);
			// cout << tem_dir_name << endl;

			dir_sub = tem_dir_name.c_str();

			if ( (dp_sub = opendir(dir_sub)) == NULL ){
				cout << "cannot open directory!" << endl;
				return 1;
			}

			chdir(dir_sub);	

			string dirName_up = entry->d_name;
			std::string::size_type found_x;
			float xAxis = 0.0;
			// get x Axis from folder name
			if ( ( found_x = dirName_up.find('A') ) != std::string::npos ) {
				xAxis = stof( dirName_up.substr(found_x+2, 2) ) * 0.1;
				// cout << xAxis << " ";
			}

			while( (entry_sub = readdir(dp_sub)) != NULL ){
				lstat( entry_sub->d_name, &statbuf_sub );
				if ( !S_ISDIR(statbuf_sub.st_mode) ){
					if ( strcmp(".", entry_sub->d_name) == 0 ||
						 strcmp("..", entry_sub->d_name) == 0 )
						continue;

					// parse file name to get Axis
					float yAxis = 0.0;
					float zAxis = 0.0;

					string dirName_sub = entry_sub->d_name;
					std::string::size_type found;

					// get y Axis from file name
					if ( ( found = dirName_sub.find("E") ) != std::string::npos ){
						int i = 2;
						while( dirName_sub[found - i] != '_' ){
							// cout << (int)dirName[found + i] << endl;
							if ( (int)dirName_sub[found - i] >= 48 && (int)dirName_sub[found - i] <= 57 ){
								yAxis += ( (int)dirName_sub[found - i] - 48 ) * pow( 10, i-2 );
							}
							i++;
						}
						// cout << yAxis << endl;			
					}

					// open file, read last line
					string line;
					
					std::ifstream testfile ( entry_sub->d_name );
					if (testfile.is_open()){
						line = getLastLine(testfile);

						// cout << xAxis << ": " << "\t" << line << endl;
						
						vector<double> lineData;

						/***********************
							SAINT+ project
						***********************/
						// parse line string into line vector
						// example: 
						// string: 6,154,680,209.294,9.49762,9.52315,4.96902
						// vector: 6.00_154.00_680.00_209.294_9.49762_9.52315_4.96902
						// index:  0	1      2      3       4       5       6
						lineData = lineDataReturn(line);
						
						// EV E2E delay data
						saveDelay( 1, lineData, dataE2EDelayEV, yAxis);

						// GV E2E delay data
						saveDelay( 3, lineData, dataE2EDelayGV, yAxis);				

						// link delay
						saveDelay( 4, lineData, dataLinkDelay, yAxis);

						// max link delay
						saveDelay( 5, lineData, dataMaxLinkDelay, yAxis);

						/***********************
							End SAINT+ project
						************************/

						testfile.close();
					}
					else std::cout <<"Unable to open file in subfloder! \n";
					// cout << entry->d_name << endl;
				}
			}
			
			// get mean
			dataMapType meanE2EDelayEV;
			if ( dataE2EDelayEV.size() != 0)
				meanE2EDelayEV = computeAverage(dataE2EDelayEV);

			dataMapType meanE2EDelayGV;
			if ( dataE2EDelayGV.size() != 0)
				meanE2EDelayGV = computeAverage(dataE2EDelayGV);

			dataMapType meanLinkDelay;
			if ( dataLinkDelay.size() != 0)
				meanLinkDelay = computeAverage(dataLinkDelay);

			dataMapType meanMaxLinkDelay;
			if ( dataMaxLinkDelay.size() != 0)
				meanMaxLinkDelay = computeAverage(dataMaxLinkDelay);

			// dataMultiMapMapType meanE2EDelayEV3D;
			// if ( meanE2EDelayEV.size() != 0)
			// 	combine3DData(meanE2EDelayEV, xAxis, dataE2EDelayEV3D);
			
			// if ( meanE2EDelayGV.size() != 0)
			// 	combine3DData(meanE2EDelayGV, xAxis, dataE2EDelayGV3D);

			// if ( meanLinkDelay.size() != 0)
			// 	combine3DData(meanLinkDelay, xAxis, dataLinkDelay3D);

			// if ( meanMaxLinkDelay.size() != 0)
			// 	combine3DData(meanMaxLinkDelay, xAxis, dataMaxLinkDelay3D);

		}
	}
}