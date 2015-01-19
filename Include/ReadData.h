#ifndef READDATA_H
#define READDATA_H

#include <iostream>
#include <fstream>

#include "ConstantsAndTypes.h"
#include <ilcplex/ilocplex.h>

void ReadData(std::string iFile, IloModel model, NumMatrix c, NumMatrix a, IloIntArray b);

#endif


