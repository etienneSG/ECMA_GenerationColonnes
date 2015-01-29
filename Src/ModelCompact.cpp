#include "ModelCompact.h"

#include <stdio.h>   // Standard Input/Output Header
#include <stdlib.h>  // C Standard General Utilities Library
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>

using namespace std;


/**
 * Lit un fichier de donnees au format OPL et remplit les attributs de la classe
 * @param iFile: fichier de donnees
 * @param model: Modele
 * @param c:     Matrice des couts d'affectation
 * @param a:     Matrice des capacites consommees
 * @param b:     Vecteur des capacites des machines
 */
void ReadData (string iFile, IloModel model, NumMatrix c, NumMatrix a, IloIntArray b)
{
  IloEnv env = model.getEnv();

  IloInt n = 0; IloInt m = 0;

  char * buffer = 0;
  long length;
  FILE * f = fopen (iFile.c_str(), "rb");
  
  // Lecture du fichier de données
  if (f)
  {
    fseek (f, 0, SEEK_END);
    length = ftell (f);
    fseek (f, 0, SEEK_SET);
    buffer = new char[length];
    if (buffer)
    {
      fread (buffer, 1, length, f);
    }
    fclose (f);
  }
  else
  {
    std::cout << "Impossible d'ouvrir le fichier !" << std::endl;
    assert(false);
  }

  if (buffer)
  {
    // Recuperation des coefficients
    vector<int> data;
    int total_w = 0;
    int w;
    int i;
    while (buffer[total_w] != '=')
      total_w++;
    while (1 == sscanf(buffer + total_w, "%*[^0123456789]%d%n", &i, &w))
    {
      total_w += w;
      data.push_back(i);
    }

    // creation des parametres
    n = data[0];
    m = data[1];
    cout << m << " " << n << "\n";

    int idx = 2;
    for (int idx_l = 0; idx_l < m; idx_l++)
    {
      IloNumArray ligne(env);
      for (int idx_c = 0; idx_c < n; idx_c++)
      {
        ligne.add(data[idx]);
        idx++;
      }
      c.add(ligne);
    }
    for (int idx_l = 0; idx_l < m; idx_l++)
    {
      IloNumArray ligne(env);
      for (int idx_c = 0; idx_c < n; idx_c++)
      {
        ligne.add(data[idx]);
        idx++;
      }
      a.add(ligne);
    }
    for (int idx_l = 0; idx_l < m; idx_l++)
    {
      b.add(data[idx]);
      idx++;
    }
  }
  
}


ModelCompact::ModelCompact(std::string iFile, IloEnv iEnv):
  _Model(iEnv),
  _c(iEnv),
  _a(iEnv),
  _b(iEnv)
{
  ReadData(iFile, _Model, _c, _a, _b);
}


ModelCompact::~ModelCompact()
{
}

