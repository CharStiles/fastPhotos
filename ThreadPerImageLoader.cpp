#include "ThreadPerImageLoader.h"

ThreadPerImageLoader::ThreadPerImageLoader()
{
	loaded = false;
	seen = false;
	image.setUseTexture(false);
};

ThreadPerImageLoader::~ThreadPerImageLoader() // this gets called whenebver there is an instance of this class destroyed
{
}

void ThreadPerImageLoader::setup(string inPath) {
	path = inPath;
	//image.loadImage(path);
	loaded = false;
	
	//startThread();

}
void ThreadPerImageLoader::update(string inPath) {
	path = inPath;
	loaded = false;
	//startThread();
}

void ThreadPerImageLoader::threadedFunction() {
	cout << "is thread running?\n";
	cout << isThreadRunning();
	cout << "\n";
	image.setUseTexture(false);
	image.loadImage(path);
	loaded = true;
}




