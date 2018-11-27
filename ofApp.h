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
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);	

		string appPath = ofFilePath::getAbsolutePath(ofFilePath::getCurrentExeDir());
		//string appPathBase = 
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

};
