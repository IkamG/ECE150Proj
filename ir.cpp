#include <iostream>
//#include <ugpio/ugpio.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <time.h>
#include <fstream>
#include <fstream>
#include <fcntl.h>

using namespace std;

string getDate();
string getTime();
bool check(const int array[], int n);
void writeApproach( string time);
void writePass(string passerbyTimes[], int passerby, int num);
void writeDate(string date);

struct Approacher {
	string date;
	string time;
};

int main(int argc, char *argv[]){

	int rv = -4;

	int rq;

	int value=0;

	unsigned int gpio = atoi(argv[1]);
	
	int runtime = atoi(argv[2]);
	
	int irInputArray 

	gpio_free(gpio);
	
	
	
	//cout<<"pin"<< gpio<<" ??? Hello World\n";

	rq = gpio_is_requested(gpio);

	rv = gpio_request(gpio, NULL);

	//cout<< "rv"<< rv<< "\n";

	//cout<< "rq"<< rq<< "\n";

	rv = gpio_direction_input(gpio);

	//cout<< "rv"<< rv<< "\n";
	int maxTraffic = runtime/10;
	if (maxTraffic < 10) {
		maxTraffic == 10;
	}//set min value of maxTraffic to 10
	string startDate = getDate();
	writeDate(startDate);
	
	int data[20];
	int p = 0;
	int numApp = 0;
	int passerby = 0;
	string passerbyTimes[maxTraffic];
	

	for(int i = 0; (i/4) < runtime; i++) {
		
		data[p] = gpio_get_value(gpio);
		//cout<<" Read GPIO%d: value "<< value <<" 89889\n"<< value;
		if(p == 19) {
			p = 0;
			bool checking = check(data, 20);
			
			if(!checking) {
				Approacher current;
				current.date = getDate();
				current.time = getTime();
				numApp++;
				while(gpio_direction_input(gpio) == 1) {
					i++;
					sleep(0.25);
				}
				if(current.date == startDate) {
					writeApproach(current.time);
				}else {
					startDate = getDate();
					writeDate(startDate);
					writeApproach(current.time);
				}
			}else{
				bool passCheck = false;
				for(int k = 0; k < 20; k++) {
					if(data[k] == 1) {
						passCheck == true;
						break;
					}
				}
				if(passCheck) {
					passerbyTimes[passerby] = getTime();
					passerby++;
				}
			}
		}
		p++;
		// pause between each read
		
		sleep(0.25);
	}
	passerbyTimes[passerby+1] = "0";
	writePass(passerbyTimes, passerby, numApp);
	
	
}

bool check(const int array[], int n)
{   
    for (int i = 0; i < n - 1; i++)      
    {         
        if (array[i] != 1)
            return true;
    }
    return false;
}

string getDate() {
	time_t t = time(0);   
    struct tm * now = localtime( & t );
	
	string year = to_string(now->tm_year + 1900);
	string month = to_string(now->tm_mon + 1);
	string day = to_string(now->tm_mday);
	
	string date = year + '-' + month + '-' + day;
	return date;
}


string getTime() {
	time_t t = time(0);   
    struct tm * now = localtime( & t );
	
	string hour = to_string(now->tm_hour);
	string min = to_string(now->tm_min);
	string sec = to_string(now->tm_sec);
	
	string time = hour + ':' + min + ':' + sec;
	return time;
}

void writeApproach( string time) {
	
	ofstream outfile;
	outfile.open("rawData.csv");
	if (!outfile.is_open()) {
		cerr << "Error: Cannot Open" << endl;
		return;
	}
	
	outfile << time << endl;
	
	outfile.close;
}

void writePass(string passerbyTimes[], int passerby, int numApp) {
	ofstream outfile;
	outfile.open("rawData.csv");
	if (!outfile.is_open()) {
		cerr << "Error: Cannot Open" << endl;
		return;
	}	
	
	outfile << "^PASSERBY" << endl;
	outfile << numApp << endl;
	outfile << passerby << endl;
	int i = 0;
	while(passerbyTimes[i] != "0") {
		outfile << passerbyTimes[i] << endl;
	}
	outfile.close;
	
}

void writeDate(string date) {
	ofstream outfile;
	outfile.open("rawData.csv");
	if (!outfile.is_open()) {
		cerr << "Error: Cannot Open" << endl;
		return;
	}
	
	outfile << "~" << date << endl;
	
	outfile.close;
	
}