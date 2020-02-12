/*
 * FastOutHeader.cpp
 *
 *  Created on: Jun 29, 2019
 *      Author: oleh
 */

#include <iostream>
#include <boost/token_functions.hpp>

#include <Utils.h>
#include <DataGroup.h>
#include <DataSet.h>
#include <IntegralData.h>

const static std::string SIGNATURE_ID("HPLogic_Fast_Binary_Format_Data_File");

DataGroup* DataGroup::spInst = nullptr;
DataGroup::DataGroup(std::ifstream& anInFile)
 : inFile(anInFile)
 , verMajor(0)
 , verMinor(0)
 , timeCrossCorrID(0)
 , stateCrossCorrID(0)
 , numDataSets(0)
{
	spInst = this;
}

DataGroup::~DataGroup() {
}

bool DataGroup::process() {
	std::string signature;

	getline(inFile, signature);

	std::cout << "+++ Signature: " << signature << std::endl;

	if(signature != SIGNATURE_ID)
		return false;

	std::string line;
	getline(inFile, line);

    ::sscanf(line.data(), "%d %d", &verMajor, &verMinor) ;
    std::cout << "+++ Version: Major: " << verMajor << ", Minor: " << verMinor << std::endl;

    /* the correlation bits */
	getline(inFile, line);
    std::cout << "+++ Correlation Bits: " << line << std::endl;

	int bit = 0;
    auto list = Utils::tokenizeInt(line);
	int num = list.front();
	list.pop_front();

    for (auto& bit: list) {
        switch ( bit ) {
          case 0:
        	  std::cout << "+++ Time ";
            break ;
          case 1:
        	  std::cout << "+++ State ";
            break ;
          default:
        	  std::cout << "+++ unknown " << bit;
            break ;
        }
    }
    std::cout << std::endl;

    /* the cross correlation id's */
	getline(inFile, line);
    ::sscanf(line.data(), "%d %d\n", &timeCrossCorrID, &stateCrossCorrID ) ;

    std::cout << "+++    Time cross correlation id = " << timeCrossCorrID << std::endl;
    std::cout << "+++    State cross correlation id = " << stateCrossCorrID << std::endl;

	getline(inFile, line);
    numDataSets = ::atoi(line.data());

    std::cout << "+++ DataSets: "  << numDataSets << std::endl;
    std::flush(std::cout);

    ///     std::cout << "+++ line |" << line << "|" << std::endl;

    for (int i = 0 ; i < numDataSets ; i++ ) {
        std::cout << "+++ DataSet #" << i+1 << std::endl;
        auto dataSet = new DataSet(inFile);
        dataSet->process();
        dataSets.push_back(dataSet);
    }

	return true;
}

SharedObject* DataGroup::isObject(long id) {
	auto itr = sharedObjMap.find(id);
	if(itr != sharedObjMap.end())
	{
		printf("+++ DataGroup::isObject id: %ld. obj: %p \n", id, itr->second);

		return itr->second;
	}

	printf("+++ DataGroup::isObject id: %ld. obj: nil \n", id);

	return nullptr;
}

bool DataGroup::addObject(SharedObject* anObj) {
	auto itr = sharedObjMap.find(anObj->getId());
	if(itr == sharedObjMap.end())
	{
		printf("+++ DataGroup::addObject id: %ld. obj: %p \n", anObj->getId(), anObj);
		sharedObjMap[anObj->getId()] = anObj;
		return true;
	}

	return false;
}
