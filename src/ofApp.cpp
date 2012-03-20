#include "ofApp.h"
#include "math.h"

#include <string>
#include <sstream>
#include <iostream> 
using namespace std; 


/*
 
 --rather than recording to a recordImg file, create a vector of img objects, and continually rewrite over the last one. This way, we can define time as the number of objects (or positions on our vector array) away from the present. t-30 would give us 30 frames into the past. If we record at 30 frames a second, and have 120 img objects in our array, we can always travel between 0 and 4 seconds into the past.
 
 --if we want to write to disk, we can write to an sxs card. shoud be efficient for all these small png writes
 
 --rather than storing recordImg files, perhaps it would be less memory intensive to actually store mesh objects. Down the line, anyway, 
 --add a time function for recordRead() and declare variables
 
 
 */
int  numberOfFramesToRecord = 320;
bool recordingOn = false;
bool kinectDisplayEnabled = false;
int frameNumber = 0;
ostringstream fileNameToSave;
int lastTime = 0;
int recordInterval = 30;
int currentTime = 0; 
int picX=0; //short for pixelIndexCounterX
int picY=0; //short for pixelIndexCounterY


void ofApp::setup() {
    kinect.init();
	kinect.setRegistration(true);
	kinect.open();
    recordImg.allocate(320, 240, OF_IMAGE_COLOR_ALPHA);
    
    
    //counter on time setup
    lastTime = ofGetElapsedTimeMillis();
    currentTime = ofGetElapsedTimeMillis();

    

    
}

void ofApp::update() {
    
    currentTime = ofGetElapsedTimeMillis();

//    printf("currentTime is: %d,  lastTime is: %d\n", currentTime, lastTime);
    
    
//-----------------------record and store information about the present-------
	
    kinect.update();
    
    if(ofGetKeyPressed(' ')) {
        recordingOn = true;
        printf("we are recording: %d\n", recordingOn);
    }
    
    if(ofGetKeyPressed('s')) {
        recordingOn = false;
        printf("we are NO LONGER recording \n");
    }
    
    
    
    if(ofGetKeyPressed('d')) {
        kinectDisplayEnabled =! kinectDisplayEnabled;
    }
    
	if(recordReady() == true) {
        ofPixels& depthPixels = kinect.getDepthPixelsRef();
        ofPixels& colorPixels = kinect.getPixelsRef();
		
        picX = 0;
        
        for(int x = 0; x < 640; x=x+2) {
            picX++;
            picY=0;
            for(int y = 0; y < 480; y=y+2) {
                picY++;                
                ofColor color = colorPixels.getColor(x, y);
                ofColor depth = depthPixels.getColor(x, y);
                recordImg.setColor(picX, picY, ofColor(color, depth.getBrightness()));
				
            }
        }
        
       
        
            ostringstream fileNameToSave;
            fileNameToSave << frameNumber << "lowrez.png";
            string result = fileNameToSave.str();
			recordImg.saveImage(result);

    }
}

//testing


void ofApp::draw() {
    
	ofBackground(0);
	ofSetColor(255, 255, 255);
    
    if(kinectDisplayEnabled == true){
    	kinect.drawDepth(0, 0, 640, 480);
        kinect.draw(0, 480, 640, 480);
    }

}

void ofApp::exit() {
	kinect.close();
}

bool ofApp::recordReady() {
    if (recordingOn == true){
        if (kinect.isFrameNew()) { 
            if(currentTime > lastTime + recordInterval) {
                lastTime = currentTime;
                if (frameNumber < numberOfFramesToRecord){
                    frameNumber = frameNumber + 1;
                    printf("time: %d frame number: %d\n", currentTime/1000, frameNumber);
                    return true;
            
                }
            }
        }
    }else{
        return false;
    }
}



