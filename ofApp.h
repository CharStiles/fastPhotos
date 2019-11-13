#pragma once

#include "ofMain.h"
#include "ofxThreadedImageLoader.h"
#include "ThreadPerImageLoader.h"
#include "ofxCv.h"

#define TOTAL 16

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();
		void writeMovie(string fileName);
		
		void keyPressed(int key);
		void mouseDragged(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);	

		string appPath = ofFilePath::getAbsolutePath(ofFilePath::getCurrentExeDir());
		
		bool loading;
		bool threadPerImage;
		
		ofxThreadedImageLoader loaders[TOTAL];
		ofImage images[TOTAL];
		int total = TOTAL;
		ofDirectory dir;
		int frameRateAvg;
		vector<int> frames;
		float sliderX, sliderY;
		float speed = 0.0; // range from -1 to 1
		int startBuffer;
		int endBuffer;

		int pad = 100;
		bool skip;
		int pStartBuffer; // previous start buffer
		int pEndBuffer; // previous end buffer

		string path;
		// uncomment these two lines for debugging, bypasses the part where you have to choose the folder manually
		// string folder = "italyPhotos";
		// string path = "C:\\Users\\a\\Documents\\of_v0.10.0_vs2017_release\\apps\\myApps\\fastPhotos_threaded_2\\bin\\data\\" + folder;

		bool withPics;
		bool record;

};
