#include "ofApp.h"
using namespace cv;
using namespace ofxCv;

VideoWriter video;
// uncomment for faster debugging, stops the prompt to ask user where video should be stored
//VideoWriter video("C:\\Users\\a\\Documents\\of_v0.10.0_vs2017_release\\apps\\myApps\\fastPhotos_threaded_2\\bin\\data\\outcpp.mp4", CV_FOURCC('m', 'p', '4', 'v'), 10, Size(1920,1080));
ofColor colorOne(0.);
ofColor colorTwo(50.);

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

	frameRateAvg = 0;
	withPics = true;
	threadPerImage = true;
	ofSetFrameRate(25);
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
	skip = false;

	startBuffer = 0;
	endBuffer = (TOTAL / 2);

	pStartBuffer = 0;
	pEndBuffer = 0;

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

	for (int i = 0; i < TOTAL; i++) {
		images[i] = *(new ofImage());
		loaders[i].loadFromDisk(images[i], path + "\\" + dir.getName(i));
		images[i].setUseTexture(false);
	}
}

//--------------------------------------------------------------
void ofApp::update() {
	int h = ofGetHeight();

	float height = 1.0*h; // casting to float

	speed = (1 - (float(sliderY - pad) / (height - (2 * pad))) - 0.5) * 2.;

	if (speed < -1 || speed > 1) {
		ofLog(OF_LOG_ERROR, "fastPhoto: speed out of range");
	}
	if (sliderY == ofGetHeight() / 2) {
		speed = 0;
	}

	skip = false;

	int m = ofLerp(2, 30, abs(abs(speed) - 0.5) * 2); // mapping from [-1,1] to [2,30]

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
		startBuffer = (startBuffer + 1) % TOTAL;
		endBuffer = (endBuffer + 1) % dir.size();
	}
	else {
		startBuffer = (startBuffer - 1) % TOTAL;
		endBuffer = (endBuffer - 1) % dir.size();
	}
	if (withPics == false) {
		return;
	} 
	if (ofGetFrameNum() >= (TOTAL / 2)) {
		if (speed > 0) {
			pStartBuffer = (ofGetFrameNum() + (TOTAL / 2)) % TOTAL;
			pEndBuffer = endBuffer;
		}
		else {
			pStartBuffer = (ofGetFrameNum() - (TOTAL / 2)) % TOTAL;
			pEndBuffer = (endBuffer - TOTAL) % dir.size();
		}
		if (pEndBuffer < 0) { //for going backwards
			pEndBuffer += dir.size();
		}
		if (pStartBuffer < 0) { //for going backwards
			pStartBuffer += TOTAL;
		}

		loaders[pStartBuffer].loadFromDisk(images[pStartBuffer], path + "\\" + dir.getName(pEndBuffer));
		images[pStartBuffer].setUseTexture(false);

	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofSetColor(255);
	ofBackgroundGradient(colorOne, colorTwo, OF_GRADIENT_LINEAR);
	
	if (startBuffer < 0) {
		startBuffer += TOTAL;
	}

	//draw Image
	if (images[startBuffer].isAllocated()) {
		loaders[startBuffer].lock();
		images[startBuffer].setUseTexture(true);

		ofImage img = images[startBuffer];
		int w = (img.getWidth()) / (img.getHeight()) * ofGetHeight();
		img.draw((ofGetWidth() / 2) - w / 2, 0, w, ofGetHeight());

		if (record) {
			ofSetColor(255, 0, 0);
			ofEllipse(ofGetWidth() - 120, 120, 50, 50);
			frames.push_back(endBuffer);
			frameRateAvg += ofGetFrameRate();
		}
		loaders[startBuffer].unlock();
	}
	else {
		cout << "viewer too fast!";
	}

	// draw the FPS
	ofSetColor(100);
	ofDrawRectangle(0, ofGetHeight() - 50, 100, 50);

	ofSetColor(200);
	ofRect(pad / 2, (ofGetHeight() / 2) - (25), pad, 50);

	ofSetColor(100);
	ofEllipse(sliderX, sliderY, 40, 40);
	ofSetColor(255);
	ofEllipse(sliderX, sliderY, 30, 30);

	ofDrawBitmapString("Speed:" + ofToString(speed) + "\nFrameRate" + ofToString(ofGetFrameRate(), 0), 5, ofGetHeight() - 30);

}

void ofApp::writeMovie(string fileName) {
	int fr = frameRateAvg / frames.size();

	// save movie as a avi, the mjpg codec comes with most windows machines
	video.open(fileName + ".avi", CV_FOURCC('M', 'J', 'P', 'G'), fr, Size(ofGetWidth(), ofGetHeight()));
	
	//this is another codec:
	//video.open(fileName + ".avi", CV_FOURCC('M', 'R', 'L', 'E'), fr, Size(ofGetWidth(), ofGetHeight()), true);
	
	for (int i = 0; i < frames.size(); i++) {
		ofImage img;
		img.load(path + "\\" + dir.getName(frames[i]));
		int w = (img.getWidth()) / (img.getHeight()) * ofGetHeight();
		img.resize(w, ofGetHeight());

		int border = floor((ofGetWidth() - w) / 2.0);
		Mat imgMat = toCv(img);
		
		Mat resized(ofGetHeight(), ofGetWidth(), imgMat.type());

		if (ofGetHeight() - w % 2 == 1) {
			copyMakeBorder(imgMat, resized, 0, 0, border + 1, border, BORDER_CONSTANT);
		}
		else {
			copyMakeBorder(imgMat, resized, 0, 0, border, border, BORDER_CONSTANT);
		}
		Mat fin;
		cvtColor(resized,fin,CV_BGR2RGB);
		video.write(fin);
	}
	video.release();

}


//--------------------------------------------------------------
void ofApp::exit() {
	for (int i = 0; i < TOTAL; ++i) { // free image memory
		images[i].clear();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == ' ') { // hit space to record you flipping through photos
		record = !record; // hit space again to stop

			if (record == false) {

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

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	sliderX = pad;
	sliderY = ofClamp(y, pad, float(ofGetHeight()) - pad);

}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	if (abs(y - ofGetHeight() / 2) < pad/2) {
		sliderY = ofGetHeight() / 2;
	}

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
