#include <iostream>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <math.h>
#include <string.h>
#include <map>
#include <algorithm>
#include <vector>
#include <assert.h>
using namespace std;
typedef pair<int,int> Coordinate;


#include "LiftOver.h"


int main(int argc, char* argv[]) {
	if (argc < 4) {
		cout << "Usage: samLiftover file.sam coordinates.bed out.bed [options]" << endl;
		cout << "  --dir 0|1 (0)  Map from coordinates on the query to the target." << endl;
		cout << "                 A value of 1 maps from the target to the query." << endl;
		exit(1);
	}
	int dir = Q;
	ifstream samIn(argv[1]);
	ifstream bedIn(argv[2]);
	ofstream bedOut(argv[3]);
	string outName = argv[3];
	
	outName += ".bad";
	ofstream badOut(outName.c_str());
	int argi = 4;
	bool printBedLine = false;
	if (argc >= 5) {
		while (argi < argc) {
			if (strcmp(argv[argi], "--dir") == 0) {
				dir = atoi(argv[++argi]);
			}
			if (strcmp(argv[argi], "--bedline") == 0) {
			  printBedLine = true;
			}
			++argi;
		}
	}
	Block::dir = dir;
		
	if (bedIn.good() == false) {
		cerr << "Could not open " << argv[2] << endl;
		exit(1);
	}
	if (samIn.good() == false) {
		cerr << "Could not open " << argv[1] << endl;
		exit(1);
	}		

	int contigIndex = 0;
	map<string, MultiBlocks > posMap;
	map<string, Strands> strands;
	LengthMap lengths;
	map<string, vector<string> > chromMap;
	map<string, vector<string> > seqMap;
	ClipMap clipMap;
	cerr << "Building map database." << endl;
	int nContigs = 0;
  nContigs = BuildMapDB(samIn, dir, posMap, strands, lengths, chromMap, seqMap, clipMap);
	string bedLine;

	while (getline(bedIn, bedLine)) {
		if (bedLine.size() == 0) {
			break;
		}
		stringstream bedStrm(bedLine);
		string chrom, mapChrom, mapEndChrom;
		int start, mapStart, end, mapEnd, mapFrontStrand, mapEndStrand;
		bedStrm >> chrom >> start >> end;
		vector<string> remainder;
		while (bedStrm) {
		  string val;
		  bedStrm >> val;
		  if (val != "") {
			remainder.push_back(val);
		  }
		}
		bool foundStart, foundEnd;
				string startContig="", endContig="";
		int startContigIndex=0, endContigIndex=0;

		foundStart = SearchContig(posMap, chromMap, strands, lengths,
								  chrom, start+1, mapChrom, mapStart, mapFrontStrand, startContig, startContigIndex);


        // end-1 is the 0-based coordinate for the end of the alignment
        // and SearchContig uses 0-based coordinates (DG, 161205)
		foundEnd = SearchContig(posMap, chromMap, strands, lengths,
								chrom, end, mapEndChrom, mapEnd, mapEndStrand, endContig, endContigIndex);


		if (mapFrontStrand == mapEndStrand and mapFrontStrand != 0) {
		  int temp = mapStart;
		  mapStart = mapEnd; 
		  mapEnd   = temp;
		}
        
        --mapStart; // convert to 0-based for bed format

		if (foundStart == false or foundEnd == false or mapChrom != mapEndChrom) {

		  badOut << bedLine << " " << (int) foundStart << " " << (int) foundEnd << " " << (int) (mapStart == mapEnd) << endl;
		}
		else {
			bedOut << mapChrom << "\t" << mapStart << "\t" << mapEnd;
			int s;
			for (s = 0; s < remainder.size(); s++) {
				bedOut << "\t" << remainder[s];
			}
			//
			// Add the originals just in case
			//
			bedOut << "\t" << chrom << "\t" << start << "\t"<< end;
			
			bedOut << endl;
		}
	}
	
}
	
