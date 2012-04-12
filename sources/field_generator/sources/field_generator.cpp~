/*!
  * \brief Program which generate à field from a PGM picture !
  * \author Alexandre DUPOUY
  * \author Michaël MORGAN
  * \author Arthur VANPOUCKE
  * \version 1.0
  * \date 02/2012
  */

/* Includes */
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <time.h>
#include "../includes/PgmReader.h"

/* Namespaces */
using namespace std;

  /**
  * @param[in] w Longueur du terrain
  * @param[in] h Largeur du terrain
  * @param[in] filename Chemin vers le fichier dans lequel écrire
  **/
void generateField(int w, int h, char *local_filename, char *project_filename, PgmReader pr){
  ofstream file, remote;
  stringstream content(stringstream::in | stringstream::out);
  file.open(local_filename, ios::out | ios::trunc);
  remote.open(project_filename, ios::out | ios::trunc);
  if(file){
    int numFaces = (w-1) * (h-1) * 2;
    int numVertices = w * h;        
    content << "OFF" << endl;
    content << numVertices << " " << numFaces << " " << 0 << endl;
    for(int i = 0; i < h; i++){
      for(int j = 0; j < w; j++){
	content << i << " " << j << " " << -(pr.getPixValue(i, j))/3 << endl;
      }
    }
    for(int i = 0; i < numVertices; i++){
      if(i%w != (w-1)){
	if(i < w){
	  content << "3" << " " << i << " " << i+1 << " " << i+w << endl;
	  numFaces++;
	}else if(i < numVertices-w){
	  content << "3" << " " << i << " " << i+1 << " " << i+w << endl;
	  content << "3" << " " << i << " " << i+1 << " " << i+1-w << endl;
	  numFaces += 2;
	}else{
	  content << "3" << " " << i << " " << i+1 << " " << i+1-w << endl;
	  numFaces++;
	}
      }
    }
    file << content.str();
    remote << content.str();
    file.close();
    remote.close();
  }else{
    cout << "An error has occured while openning file" << endl;
  }
}

int main(){
  PgmReader *pr = new PgmReader("../data/image.pgm");
  if(!pr->readPgmFile()){
    return EXIT_FAILURE;
  }
  clock_t start, end;
  double elapsed;
  start = clock();
  generateField(pr->getWidth(), pr->getHeight(),"../data/test.off", "../../projet/data/test.off", *pr);
  end = clock();
  elapsed = ((double)end - start) / CLOCKS_PER_SEC;
  cout << elapsed << endl;
  delete pr;
  return EXIT_SUCCESS;
}
