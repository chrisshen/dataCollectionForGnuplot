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

or using Makefile
Normal: make all
Debug: make all.db

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
saveDelay( int index, vector<double> lineData, dataMapVecType& data, double xAxis ){
	if ( data.find(xAxis) != data.end() ){
		vector<double> t;
		data.insert( pair< double, vector<double> >(xAxis, t) );

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
		dataAverage.insert( pair<double, double>(it->first, 0.0) );
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
		dataPercentage.insert( pair<double, double>(it->first, 0.0) );
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
		dataVariance.insert( pair<double, double>(it->first, 0.0) );
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
		dataConfiInterval.insert( pair<double, double>(it->first, 0.0) );
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
		cdfData.insert( pair<double, double>( allData[ii], actFreq[ii] ) );
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
	/**************************************
		HCMAC data
	***************************************/
	char dir_name[] = "/home/chris/SimulationData/HCMAC-project/ICDCS2018/S-1Persistence-PktInte-01-Dist-2km-Density-171212";
	// 1. HCMAC-RandSent-TSAlgo-170728			HCMAC randonm dest with time-slto algo density
	// 2. HCMAC-RandSent-NoTSAlgo-170728		HCMAC randonm dest with no time-slto algo density
	// 3. HCMAC-Unicast-NoTSAlgo-170729			HCMAC emergency dest with no stime-slto algo density
	// 4. HCMAC-Unicast-TSAlgo-170729				HCMAC emergency dest with time-slto algo density
	// 5. HCMAC-PktInterval-TSAlgo-170729		HCMAC pktInterval
	// 6. EDCA-PktInte-01-Dist-2km-Density  EDCA emergency dest density
	// 7. EDCA-Density-03-Dist-2km-PktInte	EDCA emergency dest pktInterval
	// 8. DMMAC-PktInte-01-Dist-2km-Density DMMAC emergency dest density
	// 9. DMMAC-Density-03-Dist-2km-PktInte	DMMMAC emergency dest pktInterval
	// ICDCS-2018
	// 10. 11OCB-PktInte-01-Dist-2km-Density-171212							OCB emergency dest density
	// 11. DMMAC-PktInte-01-Dist-2km-Density-171212							DMMAC emergency dest density
	// 12. P-Persistence-PktInte-01-Dist-2km-Density-171212			P-Persistence emergency dest density
	// 13. S-1Persistence-PktInte-01-Dist-2km-Density-171212		S-1Persistence emergency dest density
	// 14. 11OCB-Density-03-Dist-2km-PktInte-171212							OCB emergency dest pktInterval
	// 15. DMMAC-Density-03-Dist-2km-PktInte-171212							DMMAC ............ pktInterval
	// 16. P-Persistence-Density-03-Dist-2km-PktInte-171212			P-Persistence .............. pktInterval
	// 17. S-1Persistence-Density-03-Dist-2km-PktInte-171212		S-1Persistence ............. pktInterval

	string dir_name_string = dir_name;
	string dataType;
	// cout << dir_name_string << endl;
	if ( dir_name_string.find("HCMAC-RandSent-TSAlgo-170728") != string::npos ) {
		dataType = "HCMAC_TS";
	}
	else if ( dir_name_string.find("HCMAC-RandSent-NoTSAlgo-170728") != string::npos){
		dataType = "HCMAC_NoTS";
	}
	else if ( dir_name_string.find("HCMAC-Unicast-NoTSAlgo-170729") != string::npos){
		dataType = "HCMAC_Emg_NoTSAlgo";
	}
	else if ( dir_name_string.find("HCMAC-Unicast-TSAlgo-170729") != string::npos){
		dataType = "HCMAC_Emg_TSAlgo";
	}
	else if ( dir_name_string.find("HCMAC-PktInterval-TSAlgo-170729") != string::npos){
		dataType = "HCMAC_PktInte_TSAlgo";
	}
	else if ( dir_name_string.find("EDCA-PktInte-01-Dist-2km-Density") != string::npos){
		dataType = "EDCA_Emg";
	}
	else if ( dir_name_string.find("EDCA-Density-03-Dist-2km-PktInte") != string::npos){
		dataType = "EDCA_Emg_PktInte";
	}
	else if ( dir_name_string.find("DMMAC-PktInte-01-Dist-2km-Density") != string::npos){
		dataType = "DMMAC_Emg";
	}
	else if ( dir_name_string.find("DMMAC-Density-03-Dist-2km-PktInte") != string::npos){
		dataType = "DMMAC_Emg_PktInte";
	}
	//
	// ICDCS 2018
	// 
	else if ( dir_name_string.find("11OCB-PktInte-01-Dist-2km-Density-171212") != string::npos){
		dataType = "EDCA_Emg";
	}
	else if ( dir_name_string.find("11OCB-Density-03-Dist-2km-PktInte-171212") != string::npos){
		dataType = "EDCA_Emg_PktInte";
	}
	else if ( dir_name_string.find("DMMAC-PktInte-01-Dist-2km-Density-171212") != string::npos){
		dataType = "DMMAC_Emg";
	}
	else if ( dir_name_string.find("DMMAC-Density-03-Dist-2km-PktInte-171212") != string::npos){
		dataType = "DMMAC_Emg_PktInte";
	}
	else if ( dir_name_string.find("P-Persistence-PktInte-01-Dist-2km-Density-171212") != string::npos ){
		dataType = "P-Persistence_Emg";
	}
	else if ( dir_name_string.find("P-Persistence-Density-03-Dist-2km-PktInte-171212") != string::npos ){
		dataType = "P-Persistence_Emg_PktInte";
	}
	else if ( dir_name_string.find("S-1Persistence-PktInte-01-Dist-2km-Density-171212") != string::npos ){
		dataType = "S-1Persistence_Emg";
	}
	else if ( dir_name_string.find("S-1Persistence-Density-03-Dist-2km-PktInte-171212") != string::npos ){
		dataType = "S-1Persistence_Emg_PktInte";
	}
	else {
		dataType = "error";
		throw runtime_error("L:513, Can't identify dataType!");
	}
	
#ifdef DEBUG
	cout << "dir_name: " << dir_name << endl;
#endif

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

	/* HCMAC */
	dataMapVecType dataAvgE2EDelay_SCMAC;
	dataMapVecType dataMaxE2EDelay_SCMAC;
	dataMapVecType dataPktRatio_SCMAC;
	dataMapVecType dataClusterSize_HCMAC;
	dataMapVecType dataClusterNum_HCMAC;
	dataMapVecType dataMeanSlotCountByTSAlgo_HCMAC;
	dataMapVecType dataMeanSlotCountOptimal_HCMAC;

	// dataMapVecType dataAvgE2EDelay_EDCA;
	// dataMapVecType dataMaxE2EDelay_EDCA;
	// dataMapVecType dataPktRatio_EDCA;
	/* end define data saving place*/

#ifdef DEBUG
	cout << "dir_name: " << dir_name << endl;
#endif

	if ( dataType.find("HCMAC") != std::string::npos){
	/* HCMAC */
		HCMAC(	entry, 
						dp, 
						dataPktRatio_SCMAC, 
						dataMaxE2EDelay_SCMAC, 
						dataAvgE2EDelay_SCMAC,
						dataClusterSize_HCMAC,
						dataClusterNum_HCMAC,
						dataMeanSlotCountByTSAlgo_HCMAC,
						dataMeanSlotCountOptimal_HCMAC
						);
	}
	else if ( dataType.find("EDCA") != std::string::npos ){
		/* EDCA */
		// EDCA(		entry, 
		// 				dp, 
		// 				dataPktRatio_SCMAC,
		// 				dataAvgE2EDelay_SCMAC, 
		// 				dataMaxE2EDelay_SCMAC
		// 				);
#ifdef DEBUG
	cout << "else if (EDCA) " << endl;
#endif
		/* OCB, ICDCS-2018 */
		OCB(		"EDCA",
						entry, 
						dp, 
						dataPktRatio_SCMAC,
						dataAvgE2EDelay_SCMAC, 
						dataMaxE2EDelay_SCMAC
						);

	}
	else if ( dataType.find("DMMAC") != std::string::npos ){
		/* DMMAC */
		// EDCA(		entry, 
		// 				dp, 
		// 				dataPktRatio_SCMAC,
		// 				dataAvgE2EDelay_SCMAC, 
		// 				dataMaxE2EDelay_SCMAC
		// 				);
#ifdef DEBUG
	cout << "else if (DMMAC) " << endl;
#endif
		/* ICDCS-2018 */
		OCB(		"DMMAC",
						entry, 
						dp, 
						dataPktRatio_SCMAC,
						dataAvgE2EDelay_SCMAC, 
						dataMaxE2EDelay_SCMAC
						);
	}
	else if ( dataType.find("P-Persistence") != std::string::npos ){
		/* DMMAC */
		// EDCA(		entry, 
		// 				dp, 
		// 				dataPktRatio_SCMAC,
		// 				dataAvgE2EDelay_SCMAC, 
		// 				dataMaxE2EDelay_SCMAC
		// 				);
#ifdef DEBUG
	cout << "else if (P-Persis) " << endl;
#endif
		/* ICDCS-2018 */
		OCB(		"pPersis",
						entry, 
						dp, 
						dataPktRatio_SCMAC,
						dataAvgE2EDelay_SCMAC, 
						dataMaxE2EDelay_SCMAC
						);
	}	
	else if ( dataType.find("S-1Persistence") != std::string::npos ){
		/* DMMAC */
		// EDCA(		entry, 
		// 				dp, 
		// 				dataPktRatio_SCMAC,
		// 				dataAvgE2EDelay_SCMAC, 
		// 				dataMaxE2EDelay_SCMAC
		// 				);
#ifdef DEBUG
	cout << "else if (S-1Persistence) " << endl;
#endif
		/* ICDCS-2018 */ // s1Persis
		OCB(		"s1Persis",
						entry, 
						dp, 
						dataPktRatio_SCMAC,
						dataAvgE2EDelay_SCMAC, 
						dataMaxE2EDelay_SCMAC
						);
	}	
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

	dataMapType meanClusterSize;
	if ( dataClusterSize_HCMAC.size() != 0)
		meanClusterSize = computeAverage(dataClusterSize_HCMAC);

	dataMapType meanClusterNum;
	if ( dataClusterNum_HCMAC.size() != 0)
		meanClusterNum = computeAverage(dataClusterNum_HCMAC);

	dataMapType meanSlotCountByTSAlgo;
	if ( dataMeanSlotCountByTSAlgo_HCMAC.size() != 0)
		meanSlotCountByTSAlgo = computeAverage(dataMeanSlotCountByTSAlgo_HCMAC);

	dataMapType meanSlotCountOptimal;
	if ( dataMeanSlotCountOptimal_HCMAC.size() != 0)
		meanSlotCountOptimal = computeAverage(dataMeanSlotCountOptimal_HCMAC);

	// calculate variance
	dataMapType variMeanPktRatio;
	variMeanPktRatio = computeVariance(dataPktRatio_SCMAC, meanPktRatio);

	dataMapType variMeanMaxE2EDelay;
	variMeanMaxE2EDelay = computeVariance(dataMaxE2EDelay_SCMAC, meanMaxE2EDelay);

	dataMapType variMeanAvgE2EDelay;
	variMeanAvgE2EDelay = computeVariance(dataAvgE2EDelay_SCMAC, meanAvgE2EDelay);

	dataMapType variMeanClusterSize;
	variMeanClusterSize = computeVariance(dataClusterSize_HCMAC, meanClusterSize);

	dataMapType variMeanClusterNum;
	variMeanClusterNum = computeVariance(dataClusterNum_HCMAC, meanClusterNum);

	dataMapType variMeanMeanSlotCountByTSAlgo;
	variMeanMeanSlotCountByTSAlgo = computeVariance(dataMeanSlotCountByTSAlgo_HCMAC, meanSlotCountByTSAlgo);

	dataMapType variMeanSlotCountOptimal;
	variMeanSlotCountOptimal = computeVariance(dataMeanSlotCountOptimal_HCMAC, meanSlotCountOptimal);


	// calculate confidence interval
	dataMapType confiIntervalMeanPktRatio;
	confiIntervalMeanPktRatio = computeConfiInterval(dataPktRatio_SCMAC, variMeanPktRatio);

	dataMapType confiIntervalMeanMaxE2EDelay;
	confiIntervalMeanMaxE2EDelay = computeConfiInterval(dataMaxE2EDelay_SCMAC, variMeanMaxE2EDelay);

	dataMapType confiIntervalMeanAvgE2EDelay;
	confiIntervalMeanAvgE2EDelay = computeConfiInterval(dataAvgE2EDelay_SCMAC, variMeanAvgE2EDelay);

	dataMapType confiIntervalClusterSize;
	confiIntervalClusterSize = computeConfiInterval(dataClusterSize_HCMAC, variMeanClusterSize);

	dataMapType confiIntervalClusterNum;
	confiIntervalClusterNum = computeConfiInterval(dataClusterNum_HCMAC, variMeanClusterNum);

	dataMapType confiIntervalMeanSlotCountByTSAlgo;
	confiIntervalMeanSlotCountByTSAlgo = computeConfiInterval(dataMeanSlotCountByTSAlgo_HCMAC, variMeanMeanSlotCountByTSAlgo);

	dataMapType confiIntervalSlotCountOptimal;
	confiIntervalSlotCountOptimal = computeConfiInterval(dataMeanSlotCountOptimal_HCMAC, variMeanSlotCountOptimal);

	// get CDF
	dataMultiMapType cdfDataPktRatio_SCMAC;
	cdfDataPktRatio_SCMAC = getCDF(dataPktRatio_SCMAC);

	dataMultiMapType cdfDataMaxE2EDelay_SCMAC;
	cdfDataMaxE2EDelay_SCMAC = getCDF(dataMaxE2EDelay_SCMAC);

	dataMultiMapType cdfDataAvgE2EDelay_SCMAC;
	cdfDataAvgE2EDelay_SCMAC = getCDF(dataAvgE2EDelay_SCMAC);

	dataMultiMapType cdfDataClusterSize_HCMAC;
	cdfDataClusterSize_HCMAC = getCDF(dataClusterSize_HCMAC);

	dataMultiMapType cdfDataClusterNum_HCMAC;
	cdfDataClusterNum_HCMAC = getCDF(dataClusterNum_HCMAC);

	dataMultiMapType cdfDataMeanSlotCountByTSAlgo_HCMAC;
	cdfDataMeanSlotCountByTSAlgo_HCMAC = getCDF(dataMeanSlotCountByTSAlgo_HCMAC);

	dataMultiMapType cdfDataMeanSlotCountOptimal_HCMAC;
	cdfDataMeanSlotCountOptimal_HCMAC = getCDF(dataMeanSlotCountOptimal_HCMAC);

	/****************************
	 write data for GNU Plot 
	 ****************************/
	chdir( "/home/chris/SimulationData/HCMAC-project/gnuplot/plotData" );

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
	fileName.clear();
	fileName = "plotData_ClusterSize_";
	fileName.append(dataType);

	saveDataToFile(fileName, meanClusterSize, confiIntervalClusterSize);	

	// save EV count
	fileName.clear();
	fileName = "plotData_ClusterNum_";
	fileName.append(dataType);

	saveDataToFile(fileName, meanClusterNum, confiIntervalClusterNum);	

	// save EV count
	fileName.clear();
	fileName = "plotData_MeanSlotCountByTSAlgo_";
	fileName.append(dataType);

	saveDataToFile(fileName, meanSlotCountByTSAlgo, confiIntervalMeanSlotCountByTSAlgo);

	// save EV count
	fileName.clear();
	fileName = "plotData_MeanSlotCountOptimal_";
	fileName.append(dataType);

	saveDataToFile(fileName, meanSlotCountOptimal, confiIntervalSlotCountOptimal);


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

	// cdf: max link delay
	fileName.clear();
	fileName = "plotData_cdf_ClusterSize_";
	fileName.append(dataType);

	saveDataToFile(fileName, cdfDataClusterSize_HCMAC);

	// cdf: EV count
	fileName.clear();
	fileName = "plotData_cdf_ClusterNum_";
	fileName.append(dataType);

	saveDataToFile(fileName, cdfDataClusterNum_HCMAC);

	// cdf: E2E GV
	fileName.clear();
	fileName = "plotData_cdf_MeanSlotCountByTSAlgo_";
	fileName.append(dataType);

	saveDataToFile(fileName, cdfDataMeanSlotCountByTSAlgo_HCMAC);

	// cdf: link delay
	fileName.clear();
	fileName = "plotData_cdf_MeanSlotCountOptimal_";
	fileName.append(dataType);

	saveDataToFile(fileName, cdfDataMeanSlotCountOptimal_HCMAC);
// #endif

	return 0;
}

void
HCMAC(	struct dirent* 	entry, 
				DIR* 						dp, 
				dataMapVecType&	dataPktRatio_SCMAC,
				dataMapVecType&	dataMaxE2EDelay_SCMAC, 
				dataMapVecType&	dataAvgE2EDelay_SCMAC,
				dataMapVecType&	dataClusterSize_HCMAC,
				dataMapVecType&	dataClusterNum_HCMAC,
				dataMapVecType&	dataMeanSlotCountByTSAlgo_HCMAC,
				dataMapVecType&	dataMeanSlotCountOptimal_HCMAC
			){

	struct stat statbuf;
	// scan directory
	while( (entry = readdir(dp)) != NULL ){
		lstat( entry->d_name, &statbuf );
		if ( !S_ISDIR(statbuf.st_mode) ){
			if ( strcmp(".", entry->d_name) == 0 ||
				 strcmp("..", entry->d_name) == 0 )
				continue;

			// parse file name to get xAxis
			double xAxis = 0.0;
			// float xAxis = 0.0;

			string dirName = entry->d_name;
			std::string::size_type found;

			// for x axis
			// found = dirName.find("E");
			// cout << dirName << endl;
			if ( dirName.find("HCMAC") != std::string::npos ){
				if ( ( found = dirName.find("d") ) != std::string::npos ){

#ifdef DEBUG
	cout << "dirName: " << dirName << endl;
#endif

					// get x axis
					int i = 0;
					int j = 2;
					// cout << dirName[found] << endl;
					while( dirName[found+j+i] != '-' ){
						// cout << (int)dirName[found - i] << endl;
						i++;
					}

					string tmp = dirName.substr( found+j, i );
					xAxis = stof(tmp);
					xAxis = xAxis * 0.01;
					cout << xAxis << endl;

					/*
					open file, read last line
					*/
					string line;
					vector<std::string> fileData;
					std::ifstream testfile ( entry->d_name ); // std::ios::ate: start from the end of a file
					// std::streampos size = testfile.tellg();
					if (testfile.is_open()){
						// cout << xAxis << ": " << "\t" << line << endl;
						vector<double> lineData;
/*
						char c;
						for ( int i=1; i<=size; i++ ){
							testfile.seekg(-i, std::ios::end);
							testfile.get(c);
							cout << "c: " << c << endl;
						}
						break;
*/
#ifdef DEBUG
	cout << "1 xAxis:" << xAxis << endl;
#endif						
						while ( getline(testfile, line) ) {
							fileData.push_back(line);
						}
#ifdef DEBUG
	cout << "2 xAxis:" << xAxis << ", fileData.size(): " << fileData.size() << endl;
#endif		
						vector<std::string>::reverse_iterator it;
						for (it = fileData.rbegin(); it != fileData.rend(); it++){
#ifdef DEBUG
	cout << "read fileData:" << *it << endl;
#endif	
							lineDataToVec(*it, lineData);
#ifdef DEBUG
	cout << "3 xAxis:" << xAxis << "\t" << *it << endl;
#endif							
							line.clear();

							if ( lineData.size() == 7)
								break;
						}

						fileData.clear();
						// break;
						if ( !lineData.empty() ){
							saveDelay( 6, lineData, dataPktRatio_SCMAC, xAxis );
							saveDelay( 5, lineData, dataMaxE2EDelay_SCMAC, xAxis );
							saveDelay( 4, lineData, dataAvgE2EDelay_SCMAC, xAxis);
							saveDelay( 3, lineData, dataClusterSize_HCMAC, xAxis);
							saveDelay( 2, lineData, dataClusterNum_HCMAC, xAxis);
							saveDelay( 1, lineData, dataMeanSlotCountByTSAlgo_HCMAC, xAxis);
							saveDelay( 0, lineData, dataMeanSlotCountOptimal_HCMAC, xAxis);							
						}

						lineData.clear();

						testfile.close();
					}
					else std::cout <<"Unable to open file! \n";
				}
/*
	Pkt sending interval data:
*/

				else if (( found = dirName.find("PktInterval") ) != std::string::npos ){
#ifdef DEBUG
	cout << "dirName: " << dirName << endl;
#endif

					// get x axis
					int i = 0;
					int j = 12;
					// cout << dirName[found] << endl;
					while( dirName[found+j+i] != '-' ){
						// cout << (int)dirName[found - i] << endl;
						i++;
					}

					string tmp = dirName.substr( found+j, i );
					xAxis = stof(tmp);
					xAxis = xAxis * 0.01;
					cout << xAxis << endl;

					/*
					open file, read last line
					*/
					string line;
					vector<std::string> fileData;
					std::ifstream testfile ( entry->d_name ); // std::ios::ate: start from the end of a file
					// std::streampos size = testfile.tellg();
					if (testfile.is_open()){
						// cout << xAxis << ": " << "\t" << line << endl;
						vector<double> lineData;

#ifdef DEBUG
	cout << "1 xAxis:" << xAxis << endl;
#endif						
					while ( getline(testfile, line) ) {
						fileData.push_back(line);
					}
#ifdef DEBUG
	cout << "2 xAxis:" << xAxis << ", fileData.size(): " << fileData.size() << endl;
#endif		
					vector<std::string>::reverse_iterator it;
					for (it = fileData.rbegin(); it != fileData.rend(); it++){
#ifdef DEBUG
	cout << "read fileData:" << *it << endl;
#endif	
						lineDataToVec(*it, lineData);
#ifdef DEBUG
	cout << "3 xAxis:" << xAxis << "\t" << *it << endl;
#endif							
						line.clear();

						if ( lineData.size() == 7)
							break;
					}

					fileData.clear();
					// break;
					if ( !lineData.empty() ){
						saveDelay( 6, lineData, dataPktRatio_SCMAC, xAxis );
						saveDelay( 5, lineData, dataMaxE2EDelay_SCMAC, xAxis );
						saveDelay( 4, lineData, dataAvgE2EDelay_SCMAC, xAxis);
						saveDelay( 3, lineData, dataClusterSize_HCMAC, xAxis);
						saveDelay( 2, lineData, dataClusterNum_HCMAC, xAxis);
						saveDelay( 1, lineData, dataMeanSlotCountByTSAlgo_HCMAC, xAxis);
						saveDelay( 0, lineData, dataMeanSlotCountOptimal_HCMAC, xAxis);							
					}

					lineData.clear();

					testfile.close();
				}
				else std::cout <<"Unable to open file! \n";
				}
			}
		}
	}
}

void 
lineDataToVec(std::string line, vector<double>& lineData){
	// vector<double> lineData;

	string::size_type posDelimiterStart = 0;
	string::size_type posDelimiterEnd = 0;
	// ICDCS 2018
	string::size_type found = 0;

	if ( line.find("Successful Ratio:") != std::string::npos ){
		if ( (posDelimiterEnd = line.find(":")) != std::string::npos){
			posDelimiterStart = posDelimiterEnd + 1;

			double d = stod(line.substr(posDelimiterStart, line.length()+1-posDelimiterStart));
			lineData.push_back(d);
		}
	}
	else if ( line.find("MaxE2Edelay:") != std::string::npos ){
		if ( (posDelimiterEnd = line.find(":")) != std::string::npos){
			posDelimiterStart = posDelimiterEnd + 1;

			double d = stod(line.substr(posDelimiterStart, line.length()+1-posDelimiterStart));
			lineData.push_back(d);
		}
	}
	else if ( line.find("MeanE2Edelay:") != std::string::npos ){
		if ( (posDelimiterEnd = line.find(":")) != std::string::npos){
			posDelimiterStart = posDelimiterEnd + 1;

			double d = stod(line.substr(posDelimiterStart, line.length()+1-posDelimiterStart));
			lineData.push_back(d);
		}
	}
	else if ( line.find("ClusterSize: ") != std::string::npos ){
		if ( (posDelimiterEnd = line.find(" ")) != std::string::npos){
			posDelimiterStart = posDelimiterEnd + 1;

			double d = stod(line.substr(posDelimiterStart, line.length()+1-posDelimiterStart));
			lineData.push_back(d);
		}
	}
	else if ( line.find("ClusterNum: ") != std::string::npos ){
		if ( (posDelimiterEnd = line.find(" ")) != std::string::npos){
			posDelimiterStart = posDelimiterEnd + 1;

			double d = stod(line.substr(posDelimiterStart, line.length()+1-posDelimiterStart));
			lineData.push_back(d);
		}
	}
	else if ( line.find("meanSlotCountByTSAlgo: ") != std::string::npos){
		if ( (posDelimiterEnd = line.find(" ")) != std::string::npos){
			posDelimiterStart = posDelimiterEnd + 1;

			double d = stod(line.substr(posDelimiterStart, line.length()+1-posDelimiterStart));
			lineData.push_back(d);
		}
	}
	else if ( line.find("meanSlotCountOptimal: ") != std::string::npos){
		if ( (posDelimiterEnd = line.find(" ")) != std::string::npos){
			posDelimiterStart = posDelimiterEnd + 1;

			double d = stod(line.substr(posDelimiterStart, line.length()+1-posDelimiterStart));
			lineData.push_back(d);
		}
	}
	// ICDCS 2018
	else if ( line.find("PDR: ") != std::string::npos ){
		if ( (posDelimiterEnd = line.find(" ")) != std::string::npos ){
			posDelimiterStart = posDelimiterEnd + 1;

			double d = stod(line.substr(posDelimiterStart, line.length()+1-posDelimiterStart));
			lineData.push_back(d);
		}
	}
	else if ( line.find("maxE2EDelay:") != std::string::npos ){
		if ( (posDelimiterEnd = line.find(":")) != std::string::npos){
			posDelimiterStart = posDelimiterEnd + 1;

			string::size_type posTemp = line.find(", ");
			double d = stod(line.substr(posDelimiterStart, posTemp - 1));
			lineData.push_back(d);

			posDelimiterEnd = line.find(":", posTemp);
			posDelimiterStart = posDelimiterEnd + 2;
			posTemp = line.find(", ", posDelimiterStart);
			d = stod(line.substr(posDelimiterStart, posTemp - 1));
			lineData.push_back(d);
		}
	}
	// special: start from found pos to search
	// else if ( (found = line.find("meanE2EDelay:")) != std::string::npos ){
	// 	if ( (posDelimiterEnd = line.find(":", found)) != std::string::npos){
	// 		posDelimiterStart = posDelimiterEnd + 1;

	// 		string::size_type posTemp = line.find(", ", found);
	// 		double d = stod(line.substr(posDelimiterStart, posTemp - 1));
	// 		lineData.push_back(d);
	// 	}
	// }
	else {

	}

#ifdef DEBUG
	cout << "@FUN: lineDataToVec(string line): " << endl;
	if ( !lineData.empty() ){
		for ( auto i:lineData)
			cout << setw(12) << i;
		cout << endl;
	}
	else 
		cout << "lineData is empty! " << endl;
#endif

	// return lineData;
}

void EDCA(	struct dirent* entry, 
											DIR* dp, 
						dataMapVecType& dataPktRatio_EDCA, 
						dataMapVecType& minE2EDelay_EDCA, 
						dataMapVecType& maxE2EDelay_EDCA
											){
	struct stat statbuf;
	// scan directory
	while( (entry = readdir(dp)) != NULL ){
		lstat( entry->d_name, &statbuf );
		if ( !S_ISDIR(statbuf.st_mode) ){
			if ( strcmp(".", entry->d_name) == 0 ||
				 strcmp("..", entry->d_name) == 0 )
				continue;

			// parse file name to get xAxis
			double xAxis = 0.0;
			// float xAxis = 0.0;

			string dirName = entry->d_name;
			std::string::size_type foundS;
			std::string::size_type foundE;

			// for x axis
			// found = dirName.find("E");
			// cout << dirName << endl;
			// file name example: EDCA_Result_PktInte_5s_Dist_2km_Density_9.csv
			// if ( dirName.find("EDCA") != std::string::npos ){
				if ( ( foundS = dirName.find_last_of("_") ) != std::string::npos ){
					if ( (foundE = dirName.find_last_of(".")) != std::string::npos ){
						string tmp = dirName.substr( foundS+1, foundE-foundS+1 );
						xAxis = stod(tmp);
						xAxis = xAxis * 0.01;
					}

					/*
					open file, read line by line
					*/
					string line;
					
					std::ifstream testfile ( entry->d_name );
					if (testfile.is_open()){

		    			while ( getline(testfile, line) ) {
		    				cout << xAxis << ": " << "\t" << line << endl;
		    				
		    				vector<double> lineData;

		    				lineData = CSVLineDataToVec(line);

		    				saveDelay( 0, lineData, dataPktRatio_EDCA, xAxis );
		    				saveDelay( 1, lineData, minE2EDelay_EDCA, xAxis );
		    				saveDelay( 2, lineData, maxE2EDelay_EDCA, xAxis );
		    			}

						testfile.close();
					}
					else std::cout <<"Unable to open file! \n";
					// cout << entry->d_name << endl;	
				}
			// }
		}
	}
}

void
OCB(		string 					findDirName,
				struct dirent* 	entry, 
				DIR* 						dp, 
				dataMapVecType&	dataPktRatio_SCMAC,
				dataMapVecType&	dataMaxE2EDelay_SCMAC, 
				dataMapVecType&	dataAvgE2EDelay_SCMAC
			){

	struct stat statbuf;
	// scan directory
	while( (entry = readdir(dp)) != NULL ){
		lstat( entry->d_name, &statbuf );
		if ( !S_ISDIR(statbuf.st_mode) ){
			if ( strcmp(".", entry->d_name) == 0 ||
				 strcmp("..", entry->d_name) == 0 )
				continue;

			// parse file name to get xAxis
			double xAxis = 0.0;
			// float xAxis = 0.0;

			string dirName = entry->d_name;
			std::string::size_type found;

			// for x axis
			// found = dirName.find("E");
			// cout << dirName << endl;
			// 
			if ( dirName.find( findDirName ) != std::string::npos ){
				if ( ( found = dirName.find("Density") ) != std::string::npos ){

#ifdef DEBUG
	cout << "dirName: " << dirName << endl;
#endif

					// get x axis
					int i = 0;
					int j = 8;
					// cout << dirName[found] << endl;
					while( dirName[found+j+i] != '-' ){
						// cout << (int)dirName[found - i] << endl;
						i++;
					}

					string tmp = dirName.substr( found+j, i );
					xAxis = stof(tmp);
					xAxis = xAxis * 0.01;
					cout << xAxis << endl;

					/*
					open file, read last line
					*/
					string line;
					vector<std::string> fileData;
					std::ifstream testfile ( entry->d_name ); // std::ios::ate: start from the end of a file
					// std::streampos size = testfile.tellg();
					if (testfile.is_open()){
						// cout << xAxis << ": " << "\t" << line << endl;
						vector<double> lineData;
/*
						char c;
						for ( int i=1; i<=size; i++ ){
							testfile.seekg(-i, std::ios::end);
							testfile.get(c);
							cout << "c: " << c << endl;
						}
						break;
*/
#ifdef DEBUG
	cout << "1 xAxis:" << xAxis << endl;
#endif						
						while ( getline(testfile, line) ) {
							fileData.push_back(line);
						}
#ifdef DEBUG
	cout << "2 xAxis:" << xAxis << ", fileData.size(): " << fileData.size() << endl;
#endif		
						vector<std::string>::reverse_iterator it;
						for (it = fileData.rbegin(); it != fileData.rend(); it++){
#ifdef DEBUG
	cout << "read fileData:" << *it << endl;
#endif	
							lineDataToVec(*it, lineData);
#ifdef DEBUG
	cout << "3 xAxis:" << xAxis << "\t" << *it << endl;
#endif							
							line.clear();

							if ( lineData.size() == 7)
								break;
						}

						fileData.clear();
						// break;
						if ( !lineData.empty() ){
							saveDelay( 2, lineData, dataPktRatio_SCMAC, xAxis );
							saveDelay( 1, lineData, dataMaxE2EDelay_SCMAC, xAxis );
							saveDelay( 0, lineData, dataAvgE2EDelay_SCMAC, xAxis);
							// saveDelay( 3, lineData, dataClusterSize_HCMAC, xAxis);
							// saveDelay( 2, lineData, dataClusterNum_HCMAC, xAxis);
							// saveDelay( 1, lineData, dataMeanSlotCountByTSAlgo_HCMAC, xAxis);
							// saveDelay( 0, lineData, dataMeanSlotCountOptimal_HCMAC, xAxis);							
						}

						lineData.clear();

						testfile.close();
					}
					else std::cout <<"Unable to open file! \n";
				}
/*
	Pkt sending interval data:
*/

				else if (( found = dirName.find("PktInterval") ) != std::string::npos ){
#ifdef DEBUG
	cout << "dirName: " << dirName << endl;
#endif

					// get x axis
					int i = 0;
					int j = 12;
					// cout << dirName[found] << endl;
					while( dirName[found+j+i] != '-' ){
						// cout << (int)dirName[found - i] << endl;
						i++;
					}

					string tmp = dirName.substr( found+j, i );
					xAxis = stof(tmp);
					xAxis = xAxis * 0.01;
					cout << xAxis << endl;

					/*
					open file, read last line
					*/
					string line;
					vector<std::string> fileData;
					std::ifstream testfile ( entry->d_name ); // std::ios::ate: start from the end of a file
					// std::streampos size = testfile.tellg();
					if (testfile.is_open()){
						// cout << xAxis << ": " << "\t" << line << endl;
						vector<double> lineData;

#ifdef DEBUG
	cout << "1 xAxis:" << xAxis << endl;
#endif						
					while ( getline(testfile, line) ) {
						fileData.push_back(line);
					}
#ifdef DEBUG
	cout << "2 xAxis:" << xAxis << ", fileData.size(): " << fileData.size() << endl;
#endif		
					vector<std::string>::reverse_iterator it;
					for (it = fileData.rbegin(); it != fileData.rend(); it++){
#ifdef DEBUG
	cout << "read fileData:" << *it << endl;
#endif	
						lineDataToVec(*it, lineData);
#ifdef DEBUG
	cout << "3 xAxis:" << xAxis << "\t" << *it << endl;
#endif							
						line.clear();

						if ( lineData.size() == 7)
							break;
					}

					fileData.clear();
					// break;
					if ( !lineData.empty() ){
						saveDelay( 2, lineData, dataPktRatio_SCMAC, xAxis );
						saveDelay( 1, lineData, dataMaxE2EDelay_SCMAC, xAxis );
						saveDelay( 0, lineData, dataAvgE2EDelay_SCMAC, xAxis);
						// saveDelay( 3, lineData, dataClusterSize_HCMAC, xAxis);
						// saveDelay( 2, lineData, dataClusterNum_HCMAC, xAxis);
						// saveDelay( 1, lineData, dataMeanSlotCountByTSAlgo_HCMAC, xAxis);
						// saveDelay( 0, lineData, dataMeanSlotCountOptimal_HCMAC, xAxis);							
					}

					lineData.clear();

					testfile.close();
				}
				else std::cout <<"Unable to open file! \n";
				}
			}
		}
	}
}


vector<double>
CSVLineDataToVec(string line) {

	vector<double> lineData;

	lineData.clear();
	// int index = 0;
#ifdef DEBUG
	// cout << "1. "<< foundInData << endl;
#endif
	
	// string::size_type posDelimiter = 0;
	string::size_type posDelimiterStart = 0;
	string::size_type posDelimiterEnd = 0;

	while ( (posDelimiterEnd = line.find(",", posDelimiterStart)) != string::npos ){
		// cout << posDelimiterStart << " " << posDelimiterEnd << endl;
		// cout << line.substr(posDelimiterStart, posDelimiterEnd-posDelimiterStart) << endl;

		string temp = line.substr(posDelimiterStart, posDelimiterEnd-posDelimiterStart);
		posDelimiterStart = posDelimiterEnd + 1;
#ifdef DEBUG
	cout << "1. " << ", temp: " << temp << endl;
#endif
		// check if the current item can be transfered to double
		// if temp size is 1, it is probaly 0, then skip
		if ( temp.size() != 1 ){
			char* pEnd;
			if ( !strtod(temp.c_str(), &pEnd) )
				continue;
		}

		double data = stod( temp );
		lineData.push_back(data);
#ifdef DEBUG
	cout << "2. "<< data << ", posDelimiterStart: " << posDelimiterStart << ", posDelimiterEnd: " << posDelimiterEnd << endl;
#endif
		// check the last item after ","
		if ( line.find(",", posDelimiterStart) == string::npos ){
			lineData.push_back(stod(line.substr(posDelimiterStart, line.length()+1-posDelimiterStart)));
		}

		// check the last "," without next item
		if ( posDelimiterEnd == line.length()){
			break;
		}
	}

#ifdef DEBUG
	cout << "@FUN: lineDataToVec(string line): " << endl;
	if ( lineData.empty() ){
		cout << "lineData is empty!!" << endl;
	}
	else {
		for ( auto i:lineData)
			cout << setw(12) << i;
		cout << endl;
	}
#endif

	return lineData;
}