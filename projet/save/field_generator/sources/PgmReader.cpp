/*!
 * \brief Small PgmReader program
 * \author Alexandre DUPOUY
 * \author Michaël MORGAN
 * \author Arthur VANPOUCKE
 * \version 1.0
 * \date 02/2012
 */

#include "../includes/PgmReader.h"

PgmReader::PgmReader(char *filename){
    this->filename = filename;
}

PgmReader::~PgmReader(){

}

bool PgmReader::readPgmFile(){
    ifstream file;
    file.open(this->filename, ios::in);
    if(file){
        string in;
        file >> in;
        if(in != "P2"){
            cerr << "Incorrect file format" << endl;
            return EXIT_FAILURE;
        }
        file >> this->width >> this->height;
        int numPix = this->width * this->height;
        unsigned short value;
        for(int i = 0; i < numPix; i++){
	  file >> value;
	  this->pixValues.push_back(value);
        }
        return true;
    }else{
        cerr << "An error has occured while openning file " << this->filename << endl;
        return false;
    }
}

unsigned short PgmReader::getPixValue(int i){
    return this->pixValues[i];
}

unsigned short PgmReader::getPixValue(int i, int j){
    return this->pixValues[i*this->width+j];
}

int PgmReader::getHeight(){
    return this->height;
}

int PgmReader::getWidth(){
    return this->width;
}

vector<unsigned short> PgmReader::getPixValues(){
  return this->pixValues;
}
