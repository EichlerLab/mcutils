#include <iostream>
#include <string>
#include <stdlib.h>
using namespace std;
int main(int argc, char* argv[]) {
	int subsample = atoi(argv[1]);
	int idx=0;
	while (true) {
		string chrom;
		int start;
		int cov;
		if ((cin >> chrom >> start >> cov) == 0) {
			break;
		}
		if (start % subsample == 0) {
			cout << chrom << "\t" << start << "\t" << start+subsample << "\t" << cov << endl;
			idx+=1;
																																									if (idx % 10000 == 0) {
																																										cerr << chrom << "\t" << start << endl;
																																									}
																																																											
																																									
																																									  
		}
	}
}
			
	
