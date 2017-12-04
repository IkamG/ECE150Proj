#include <iostream>
//#include <ugpio/ugpio.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <unistd.h>
#include <string>
#include <fcntl.h>
#include <fstream>

using namespace std;

struct Mode{
	int numModes;
	int* modes;
};

struct PassStatistics{
	int minimum;
	int average;
	int maximum;
	int popStdDev;
	int smplStdDev;
	Mode  mode;
	int histogram[24];
	int robbed;
};

//struct AppStatistics {
	
//};

void computeAppStats(int approachTimes[],int passerbyTimes[],int passerbyCount,int approachCount);

float averageFunc(int movement[], int size, int passerby, int close);

void approachTimes(float approachTimes[], int size);

void selection(int dataset[], int size);

void extractStats();

enum states { HOURP, MINP, SECP, SPACE4, HOUR, MIN, SEC };

int mode(int dataset[], int size, PassStatistics& stats);

int minimum( int dataset[],  int size);

int average( int dataset[],  int size);

int maximum( int dataset[],  int size);

int popStdDev( int dataset[],  int size);

int smplStdDev( int dataset[], int size);

int median( int dataset[],  int size);

int robbed(int dataset[] , int size);

string converter(int value);

void writeFunc(int approachTimes[],int passerbyTimes[],int passerbyCount,int approachCount, PassStatistics stats);

int main( const int argc, const char* const argv[]) {
	
	extractStats();
	
	//computeAppStats(movement, sizeM);
	//approachTimes(approachTimes, sizeA);
	
}

void extractStats() {
	states currentState = HOUR;
	ifstream infile;
	infile.open("rawStats.csv");
	if (infile.fail())
		return;
	
	bool done = false; 
	int fileLineNumber = -1;
	int approachLine = -1;
	int passerbyLine = -1;
	
	int secArray[1000] = {0};
	string date[2];
	
	const int maxLineLength = 100;
	char line[maxLineLength];
	
	int approachTempTimes[1000];
	int passerbyTempTimes[1000];
	
	int hour, min, sec;
	int hourP, minP, secP;
	int passerbyCount;
	int approachCount;
	
	while(!done) {
		hour = 0;
		min = 0;
		sec = 0;
		hourP = 0;
		minP = 0;
		secP = 0;
		++fileLineNumber;
		
		if (!infile.getline(line, maxLineLength)) {
			if (infile.eof()) {
				done = true;
			}
		}
		if(fileLineNumber == 0) {
			date[0] == line;
		}
			
		
		for(int i = 0; i < maxLineLength-1; i++) {
			if (i == 0) {
				if (line[0] == '^'  ) {
					infile.getline(line, maxLineLength);
					approachCount = atoi(line);
					infile.getline(line, maxLineLength);
					passerbyCount = atoi(line);
					infile.getline(line, maxLineLength);
					currentState = HOURP;
					
				}else if(line[0] == '~') {
					date[1] = line;
					fileLineNumber++;
					infile.getline(line, maxLineLength);
				}
			}
			switch(currentState) {
				case HOUR:
					cout << "hour" << endl;
					
					if(line[i] == ':'){
						approachLine++;
						currentState = MIN;
						cout << "wtf" << endl;
						break;
					}
					hour = (hour * 10) + (line[i]-48);
					break;
					
				case MIN:
					if(line[i] == ':'){
						currentState = SEC;
						break;
					}
					min = (min * 10) + (line[i]-48);
					break;
					
				case SEC:
					if (line[i] == '\0') {
						cout << (hour * 60 * 60) + (min * 60) + sec << endl;
						approachTempTimes[approachLine] = (hour * 60 * 60) + (min * 60) + sec;
						currentState = HOUR;
						i = maxLineLength;
						break;
					}
					sec = (sec * 10) + (line[i]-48);
					break;
					
				case HOURP:
				cout << "hour" << endl;
					
					if(line[i] == ':'){
						passerbyLine++;
						currentState = MINP;
						break;
					}
					hourP = (hourP * 10) + (line[i]-48);
					break;
					
				case MINP:
					if(line[i] == ':'){
						currentState = SECP;
						break;
					}
					minP = (minP * 10) + (line[i]-48);
					break;
					
				case SECP:
					if (line[i] == '\0') {
						cout << (hourP * 60 * 60) + (minP * 60) + secP << endl;
						passerbyTempTimes[passerbyLine] = (hourP * 60 * 60) + (minP * 60) + secP;
						i = maxLineLength;
						currentState = HOURP;
						break;
					}
					secP = (secP * 10) + (line[i]-48);
					break;
			}
		}
	}
	int approachTimes[approachCount];
	int passerbyTimes[passerbyCount];
	for(int i = 0; i < approachCount; i++) {
		approachTimes[i] = approachTempTimes[i];
	}
	
	for(int i = 0; i < passerbyCount; i++) {
		passerbyTimes[i] = passerbyTempTimes[i];
	}
	
	computeAppStats(approachTimes, passerbyTimes, passerbyCount, approachCount);
}



void computeAppStats(int approachTimes[],int passerbyTimes[],int passerbyCount,int approachCount) {
	PassStatistics stats = {0};
	int temp;
	for(int i = 0; i < passerbyCount; i++) {
		temp = (int)(passerbyTimes[i])/3600;
		stats.histogram[temp] = stats.histogram[temp] + 1;
	}
	stats.minimum = minimum(stats.histogram, 24);
	stats.maximum = maximum(stats.histogram, 24);
	stats.average = average(passerbyTimes, passerbyCount);
	stats.popStdDev = popStdDev(passerbyTimes, passerbyCount);
	stats.smplStdDev = smplStdDev(passerbyTimes, passerbyCount);
	stats.mode.numModes = mode(stats.histogram, 24, stats);	
	stats.robbed = robbed(passerbyTimes, passerbyCount);
	

	writeFunc(approachTimes, passerbyTimes, passerbyCount, approachCount, stats);
	
}

void writeFunc(int approachTimes[],int passerbyTimes[],int passerbyCount,int approachCount, PassStatistics stats){
	string average = converter(stats.average);
	string popStdDev = converter(stats.popStdDev);
	string smplStdDev = converter(stats.smplStdDev);
	string robbed = converter(stats.robbed);
	
	
	ofstream outfile;
	outfile.open("stats.csv");
	if (!outfile.is_open()) {
		cerr << "Error: Cannot Open" << endl;
		return;
	}	
	
	outfile << "-------------------FINAL STATS-------------------" << endl;
	outfile << "# of people that approached/# of shots fired: " << approachCount << endl;
	outfile << "Shots Fired Times: " << endl;
	for(int i = 1; i < approachCount+1; i++) {
		outfile << i << "." << converter(approachTimes[i-1]) << endl;
	}
	outfile << "Hour with least number of people passing by: " << stats.minimum << endl;
	outfile << "Hour with most number of people passing by: " << stats.maximum << endl;
	outfile << "Average time of all passerbys: " << average << endl;
	outfile << "Population Standard Deviation: " << popStdDev << endl;
	outfile << "Sample Standatd Deviation: " << smplStdDev << endl;
	outfile << "Modes: ";
	for(int i = 0; i < stats.mode.numModes; i++) {
		outfile << stats.mode.modes[i] << "      ";
	}
	outfile << endl;
	
	outfile << "Histogram: " << endl;
	for(int i = 0; i < 24; i++) {
		outfile << i << ": " << stats.histogram[i] << endl;
	}
	
	
	
}



string converter(int value){
	string hour, min, sec;
	
	hour = to_string((int)(value/3600));
	value = value % 3600;
	min = to_string((int)(value/60));
	value = value % 60;
	sec = to_string(value);
	string fin = hour + ":" + min + ":" + sec;
	return fin;
	
}

int robbed(int dataset[] , int size) {
	selection(dataset, size);
	int dist = 0;
	int top;
	int bot;
	for(int i = 0; i < size-1; i++) {
	int temp = dataset[i+1] - dataset[i];
	
		if(temp > dist) {
			dist = temp;
			top = dataset[i+1];
			bot = dataset[i];
		}
	}
	dist = (top + bot)/2;
	return dist;
	
}

int minimum(int dataset[], const int size) {
	int i = 0;
	float low = FLT_MAX;
	while(i < size) {
		float x = dataset[i];
		if(x < low) {
			low = i;
		}
		i++;
	}
	return low;
}


int average( int dataset[],  int size) {
	float total;
	int i = 0;
	while(i < size) {
		float x = dataset[i];
		total =+ total + x;	
		i++;
	}
	return (total/(size));
}


int maximum( int dataset[],  int size) {
	int i = 0;
	float high = FLT_MIN;
	float low = FLT_MAX;
	while(i < size) {
		float x = dataset[i];		
		if(x > high) {
			high = i;
		}
		if(x < low) {
			low = x;
		}
		i++;
	}
	return high;
}


int stdDev( int dataset[], int size, int q, float avg, float dev) {
	if (size == q){
		return dev;
	}else {
		return stdDev(dataset, size, q+1, avg , dev + pow((dataset[q] - avg),2));
	}
}


int popStdDev(int dataset[], int size) {
	return sqrt(stdDev(dataset, size,0, average(dataset,size), 0)/(size));
}


int smplStdDev(int dataset[], int size) {
	return sqrt(stdDev(dataset, size,0, average(dataset,size), 0)/(size-1));
}


void selection(int dataset[], int size) {
	if  (size == 0) {
		return;
	}
	int max = 0;
	int temp = 0;
	int i = 0;
    for (i = max; i < size; i++) {
        if (dataset[i] > dataset[max]) {
            max = i;
        }
    }
    temp = dataset[size-1];
    dataset[size-1] = dataset[max];
    dataset[max] = temp;
 
    if (size > 1) {
        selection(dataset, size - 1);
    }
}


int mode(int dataset[],int size, PassStatistics& stats){
	int array[size];
	for(int i = 0; i < size; i++){
		array[i] = dataset[i];
	}
	selection(array, size);
	int i = 1;
	int c = 1;
	int j = 0;
	int high = 0;
	int temp = 0;
	stats.mode.modes = new int [size];
	
	for(int p = 0; p < size; p++){
		temp = array[p];
		for(int l =  p+1; l < size; l++){ 
			if(array[p] == array[l]) {
				i++;
			}
			else{
				p = l-1;
				break;
			}
		}
		//cout << i << endl;
		if(i > high) {
			//cout << c << endl;
			high = i;
			c = 1;
			stats.mode.modes[0] = temp;	
		}else if(i == high) {
			//cout << c << endl;
			c++;
			stats.mode.modes[c-1] = temp;
		}
		i = 1;
	}
	return c;
}
