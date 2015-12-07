/////////////////////////////////////////////
// Danny Dutton
// 12/03/15
// ECE2500
// 
// Project 3: Cache Simulator
//
/////////////////////////////////////////////

#include <string>
#include <fstream>
#include <iostream>
#include <list>
#include <iomanip>
#include <cstdio> 
#include <vector>
#include <cmath>

using namespace std;

typedef struct
{
	bool rw;	// Read=0, write=1
	unsigned int loc;	// 4byte memory location
} memop;

typedef struct
{
	int tag;
	int set;
	bool valid;
	bool dirty;
} Block;

typedef struct
{
	int hit;
	int miss;
	int m2c;
	int c2m;
	int hitcom;
	int block_num;
	int ways;
	int sets;
	vector<Block> blocks;
} Cache;

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

	// Prefix of filename is the first argument
	string filename = argv[1];
	

	// Read all the memory operations and save in list
	readFile(trace, filename);

	// Clear the old file since we are normally appending
	ofstream outFile(filename+".result");
	outFile.close();

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			for(int k = 0; k < 4; k++)
			{
				for(int l = 0; l < 2; l++)
				{
					// Create a new cache with all the correct attributes
					Cache *cache = new Cache;
					Block *block = new Block;

					cache->hit = 0;
					cache->miss = 0;
					cache->m2c = 0;
					cache->c2m = 0;
					cache->block_num = cs[i]/bs[j];
					cache->ways = (mt[k] == "DM") ? 1 : (mt[k] == "2W") ? 2 : (mt[k] == "4W") ? 4 : (mt[k] == "FA") ? cache->block_num : 0;
					cache->hitcom = cache->ways;
					cache->sets = (cache->block_num)/(cache->ways);

					// Clear out the vector of blocks in the cache
					for(int m = 0; m < cache->block_num; m++)
					{
						block->valid = 0;
						block->dirty = 0;
						block->tag = -1;
						cache->blocks.push_back(*block);
					}

					// Determine indices
					int offset_index = log2(bs[j]);
					int set_index = offset_index + log2(cache->sets);
					int tag_index = 32;
					

					// Go through each item in trace
					for(list<memop>::iterator it = trace.begin(); it != trace.end(); it++)
					{
						int newtag = (it->loc)>>(set_index);
						int newset = ((it->loc)>>(offset_index))%(set_index);
						bool eviction = false;

						Block tempblock;

						// Write to cache
						if(it->rw)
						{
							bool found = false;
							int m;
							for(m = 0; m < cache->block_num; m++)
							{
								tempblock = cache->blocks[m];
								
								// Hit
								if(tempblock.set == newset && tempblock.tag == newtag)
								{
									cache->hit += 1;
									
									block->dirty = true;

									cache->blocks.erase(cache->blocks.begin()+m);
									cache->blocks.push_back(tempblock);
									found = true;
									m = cache->block_num;
								}
								// No hit
								else
								{
									found = false;
								}
							}

							// Miss
							if(found == false)
							{
								block->set = newset;
								block->tag = newtag;
								block->valid = true;
								
								
								// Cache is full so erase the oldest
								if(m == (cache->block_num - 1));
								{
									cache->blocks.erase(cache->blocks.begin());
									eviction = true;
								}
								cache->blocks.push_back(*block);
							
								cache->miss += 1;
								cache->m2c += 1;
							}

							// Write policy
							if (l == 0 && eviction)
							{
								cache->c2m += 1;
							}
							if(l == 1)
							{
								cache->c2m += 1;
							}

							eviction  = false;
						}
						// Read from cache
						else
						{
							bool found = false;
							int m;
							for(m = 0; m < cache->block_num; m++)
							{
								tempblock = cache->blocks[m];
								
								// hit
								if(tempblock.set == newset && tempblock.tag == newtag)
								{
									cache->hit += 1;

									cache->blocks.erase(cache->blocks.begin()+m);
									cache->blocks.push_back(tempblock);
									m = cache->block_num;
									found = true;
								}
								
								// No hit
								else
								{
									found =  false;
								}
							}

							// Miss
							if (found == false)
							{
								block->set = newset;
								block->tag = newtag;
								block->valid = true;
									
								// Cache is full so erase the oldest
								if(m == (cache->block_num - 1));
								{
									cache->blocks.erase(cache->blocks.begin());
								}

								cache->blocks.push_back(*block);

								cache->miss += 1;
								cache->m2c += 1;

								m = cache->block_num;
							}
						}
					}

					// result output
					printResult(filename, i, j, k, l, cache->hit, cache->miss, cache->m2c * bs[j], cache->c2m * bs[j], cache->hitcom);
				}
			}
		}	
	}
	return 0;
}