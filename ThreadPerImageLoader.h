#pragma once
#include "ofMain.h"

class ThreadPerImageLoader : public ofThread { // extending ofThread like in ofBook
											   // I only got 4 cores so we can really only have 4 threads? but imma put 8 because i have 8 logical processors
public:
	ThreadPerImageLoader();
	~ThreadPerImageLoader();
	void setup(string inPath);
	void update(string inPath);
	void threadedFunction();
	ofImage image;
	string path;
	bool loaded;
	bool seen;

};

