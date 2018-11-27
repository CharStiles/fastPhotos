#include "ofApp.h"
using namespace cv;
using namespace ofxCv;

string folder = "italyPhotos";
string path;// = "C:\\Users\\a\\Documents\\of_v0.10.0_vs2017_release\\apps\\myApps\\fastPhotos_threaded_2\\bin\\data\\" + folder;
//string path = "C:\\Users\\a\\Documents\\of_v0.10.0_vs2017_release\\apps\\myApps\\fastPhotos_threaded\\bin\\data\\" + folder;

int i;
int j;
ofColor colorOne(0);
ofColor colorTwo(50);
int pad = 100;
bool skip;
int pi;
int pj;
bool withPics;
bool record;
VideoWriter video;
//VideoWriter video("C:\\Users\\a\\Documents\\of_v0.10.0_vs2017_release\\apps\\myApps\\fastPhotos_threaded_2\\bin\\data\\outcpp.mp4", CV_FOURCC('m', 'p', '4', 'v'), 10, Size(1920,1080));
void ofApp::setup() {
	record = false;
	ofSetLogLevel(OF_LOG_VERBOSE);

	////Open the Open File Dialog
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a folder of photos",true);

	//Check if the user opened a file
	if (openFileResult.bSuccess) {

		ofLogVerbose("User selected a file");

	}
	else {
		ofLogVerbose("User hit cancel");
	}

	ofLogVerbose("getName(): " + openFileResult.getName());
	ofLogVerbose("getPath(): " + openFileResult.getPath());

	path = openFileResult.getPath();

	//cout << path;
	frameRateAvg = 0;
	withPics = true;
	threadPerImage = true;
	ofSetFrameRate(30);
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
	skip = false;
	i = 0;
	j = (TOTAL / 2);
	pi = 0;
	pj = 0;
	sliderX = pad;
	sliderY = ofGetHeight() / 2;
	dir.open(path);
	//populate the directory object
	dir.allowExt("png");
	dir.allowExt("jpg");
	dir.allowExt("JPG");
	dir.allowExt("jpeg");
	dir.allowExt("gif");
	dir.listDir();

	int dirNum = dir.size();

	if (total > dirNum) {

		total = dirNum;
	}

	loading = false;
	for (int i = 0; i < TOTAL; ++i) {
		images[i] = *(new ofImage());
		//images[i].setUseTexture(false);
		loaders[i].loadFromDisk(images[i], path + "\\" + dir.getName(i));
		images[i].setUseTexture(false);
	}
}

//--------------------------------------------------------------
void ofApp::update() {
	int h = ofGetHeight();

	float height = 1.0*h;

	speed = (1 - (float(sliderY - pad) / (height - (2 * pad))) - 0.5) * 2.;


	if (speed < -1 || speed > 1) {
		ofLog(OF_LOG_ERROR, "fastPhoto: speed out of range");
	}

	skip = false;

	int m = ofLerp(2, 30, abs(abs(speed) - 0.5) * 2);

	if (abs(speed) < 0.5) { // most of the time advance
		m = m / 2;
		skip = (ofGetFrameNum() % m != 0); // when speed is hhalf m should b 2 

	}
	else { // less than half the time advance
		skip = (ofGetFrameNum() % m == 0); // when speed is small m should be 30 when speed is half then speed should b 2
	}

	if (skip || speed == 0) {
		return;
	}
	if (speed >= 0) {
		i = (i + 1) % TOTAL;
		j = (j + 1) % dir.size();
	}
	else {
		i = (i - 1) % TOTAL;
		j = (j - 1) % dir.size();
	}
	if (withPics == false) {
		return;
	}
	if (ofGetFrameNum() >= (TOTAL / 2)) {
		if (speed > 0) {
			pi = (ofGetFrameNum() + (TOTAL / 2)) % TOTAL;
			pj = j;
		}
		else {
			pi = (ofGetFrameNum() - (TOTAL / 2)) % TOTAL;
			pj = (j - TOTAL) % dir.size();
		}
		if (pj < 0) { //for going backwards
			pj += dir.size();
		}
		if (pi < 0) { //for going backwards
			pi += TOTAL;
		}
		//images[pi] = *( new ofImage());
		//images[pi].setUseTexture(false);
		loaders[pi].loadFromDisk(images[pi], path + "\\" + dir.getName(pj));
		images[pi].setUseTexture(false);

	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofSetColor(255);
	ofBackgroundGradient(colorOne, colorTwo, OF_GRADIENT_LINEAR);
	if (i < 0) {
		i += TOTAL;
	}
	if (loaders[i].isThreadRunning()) {
		//ofLog(OF_LOG_NOTICE, "\nis it going to crash? \n");
	}

	////draw Image
	if (images[i].isAllocated()) {
		loaders[i].lock();
		images[i].setUseTexture(true);

		ofImage img = images[i];
		int w = (img.getWidth()) / (img.getHeight()) * ofGetHeight();

		//img.update();
		//img.resize(w, ofGetHeight());
		img.draw((ofGetWidth() / 2) - w / 2, 0, w, ofGetHeight());
		if (record) {
			ofSetColor(255, 0, 0);
			ofEllipse(ofGetWidth() - 120, 120, 50, 50);
			frames.push_back(j);
			frameRateAvg += ofGetFrameRate();
		}
		loaders[i].unlock();

	}
	else {
		cout << "viewer too fast!2";
	}
	ofSetColor(255);
	// draw the FPS
	ofDrawRectangle(0, ofGetHeight() - 50, 100, 50);

	ofEllipse(sliderX, sliderY, 30, 30);

	ofSetColor(0);
	ofDrawBitmapString("Speed:" + ofToString(speed) + "\nFrameRate" + ofToString(ofGetFrameRate(), 0), 5, ofGetHeight() - 30);

}

void ofApp::writeMovie(string fileName) {
	int fr = frameRateAvg / frames.size();

	//video.open(fileName + ".avi", CV_FOURCC('M', 'J', 'P', 'G'), fr, Size(ofGetWidth(), ofGetHeight()));
	video.open(fileName + ".avi", CV_FOURCC('M', 'J', 'P', 'G'), fr, Size(ofGetWidth(), ofGetHeight()));
	//video.open(fileName + ".avi", CV_FOURCC('M', 'R', 'L', 'E'), fr, Size(ofGetWidth(), ofGetHeight()), true);
	for (int i = 0; i < frames.size(); i++) {
		ofImage img;
		img.load(path + "\\" + dir.getName(frames[i]));
		int w = (img.getWidth()) / (img.getHeight()) * ofGetHeight();
		img.resize(w, ofGetHeight());

		int border = floor((ofGetWidth() - w) / 2.0);
		Mat imgMat = toCv(img);
		
		Mat resized(ofGetHeight(), ofGetWidth(), imgMat.type());//imgMat.depth());

		if (ofGetHeight() - w % 2 == 1) {
			//cout << w + border + border + 1;
			copyMakeBorder(imgMat, resized, 0, 0, border + 1, border, BORDER_CONSTANT);
		}
		else {
			//cout << w + border + border;
			copyMakeBorder(imgMat, resized, 0, 0, border, border, BORDER_CONSTANT);
		}
		//resized *= 1. / 255;
		Mat fin;
		cvtColor(resized,fin,CV_BGR2RGB);
		video.write(fin);
	}
	video.release();

}


//--------------------------------------------------------------
void ofApp::exit() {
	/*for (int i = 0; i < total; ++i) {
		loaders[i].waitForThread();
		loaders[i].stopThread();
	}*/
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'm') {
		record = true;
	}
	if (key == 'n') {
		record = false;

		ofFileDialogResult result = ofSystemSaveDialog("fastPhotoMovie_" + ofGetTimestampString("%Y-%m-%d-%H-%M"), "Save Fast Photo Movie");
		if (result.bSuccess) {

			writeMovie(result.getPath());

			// save your file to `path`
		}
		else {
			frames.clear();
			frameRateAvg = 0;
		}
	}

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	sliderX = pad;
	sliderY = ofClamp(y, pad, float(ofGetHeight()) - pad);

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

	sliderY = ofGetHeight() / 2;

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
