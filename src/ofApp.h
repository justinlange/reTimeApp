#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
//#include "time.h"



class ofApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
    bool recordReady();
    
    int lastTime;
    int recordInterval;
    int currentTime;
    
	
	ofxKinect kinect;
    ofImage recordImg;

    ofImage displayImg;
    ofMesh mesh;
    ofEasyCam cam;



};
