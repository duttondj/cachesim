#include <string>
#include <fstream>
#include <iostream>
#include <list>
#include <iomanip>
#include <cstdio> 

using namespace std;

typedef struct
{
	bool rw;	// Read=0, write=1
	unsigned int loc;	// 4byte memory location
} memop;

int cs [4] = {1024, 4096, 65536, 131072};
int bs [4] = {8, 16, 32, 128};
string mt [4] = {"DM", "2W", "4W", "FA"};
string wp [4] = {"WB", "WT"};

// Read the file and save the contents in a list passed by reference
void readFile(list<memop>& trace, string filename)
{
	string one,two;
	ifstream inFile(filename+".trace");

	while (inFile >> one >> two)
	{
		memop temp;

		if (one == "read")
			temp.rw = false;
		else if (one == "write")
			temp.rw = true;
		
		temp.loc = stoul(two, nullptr, 16);

		trace.push_back(temp);
	}

	inFile.close();
}

void printResult(string filename, int csit, int bsit, int mtit, int wpit, int hit, int miss, int m2c, int c2m, int hitcom)
{
	char cstr [100];
	string str;
	double ratio = ((double) hit)/((double) miss);

	sprintf(cstr, "%d\t%d\t%s\t%s\t%.2f\t%d\t%d\t%d", cs[csit], bs[bsit], mt[mtit].c_str(), wp[wpit].c_str(), ratio, m2c, c2m, hitcom);

	str = cstr;

	ofstream outFile(filename+".result", ios::app);
	outFile << str << endl;
	outFile.close();
}

int main(int argc, char **argv)
{
	// List containing memory operation structs
	list<memop> trace;
	list<memop>::iterator it = trace.begin();

	// Prefix of filename is the first argument
	string filename = argv[1];
	

	// Read all the memory operations and save in list
	readFile(trace, filename);

	// Clear the old file since we are normally appending
	ofstream outFile(filename+".result");
	outFile.close();

	// while (it != trace.end())
	// {
	// 	cout << it->rw << "\t" << hex << it->loc << endl;
	// 	it++;
	// }

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			for(int k = 0; k < 4; k++)
			{
				for(int l = 0; l < 2; l++)
				{
					// Test file output
					//printResult(filename, i, j, k, l, i, j, k, l, 7);
				}
			}
		}	
	}

	return 0;
}