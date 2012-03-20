#include "ofApp.h"
#include "math.h"

#include <string>
#include <sstream>
#include <iostream> 
using namespace std; 


/*   --PROGRAMMING IDEAS--
 
 --rather than recording to a recordImg file, create a vector of img objects, and continually rewrite over the last one. This way, we can define time as the number of objects (or positions on our vector array) away from the present. t-30 would give us 30 frames into the past. If we record at 30 frames a second, and have 120 img objects in our array, we can always travel between 0 and 4 seconds into the past.
 
 --if we want to write to disk, we can write to an sxs card. shoud be efficient for all these small png writes
 
 --rather than storing recordImg files, perhaps it would be less memory intensive to actually store mesh objects. Down the line, anyway, 
 --add a time function for recordRead() and declare variables
 
 
 */

//---------recording the present-----------

int  numberOfFramesToRecord = 320;
bool recordingOn = false;
bool kinectDisplayEnabled = false;
int frameNumber = 2;  //will make the first frame recorded "2"
ostringstream fileNameToSave;
int lastTime = 0;
int recordInterval = 30;
int currentTime = 0; 
int picX=0; //short for pixelIndexCounterX
int picY=0; //short for pixelIndexCounterY

//---------showing the present or past-----------

int currentFrame = 1;
int previousFrame = 0;
string frameResult;
ostringstream fileNameToLoad;
int skip;	
int width;
int height;
int startY = 0;
int startX = 200;
int endBufferY = 10;
int endBufferX = 100;

//---------creating a mesh out of the present or past-----------

void addFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c) {
	mesh.addVertex(a);
	mesh.addVertex(b);
	mesh.addVertex(c);
}

void addFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c, ofVec3f d) {
	addFace(mesh, a, b, c);
	addFace(mesh, a, c, d);
}

void addTexCoords(ofMesh& mesh, ofVec2f a, ofVec2f b, ofVec2f c) {
	mesh.addTexCoord(a);
	mesh.addTexCoord(b);
	mesh.addTexCoord(c);
}

void addTexCoords(ofMesh& mesh, ofVec2f a, ofVec2f b, ofVec2f c, ofVec2f d){
	addTexCoords(mesh, a, b, c);
	addTexCoords(mesh, a, c, d);
}

ofVec3f getVertexFromImg(ofImage& pastImg, int x, int y) {   
	ofColor color = pastImg.getColor(x, y);
	if(color.a > 0) {
		float z = ofMap(color.a, 0, 255, -480, 480);
        // this maps this to -480, 480
		return ofVec3f(x - pastImg.getWidth() / 2, y - pastImg.getHeight() / 2, z);
	} else {
		return ofVec3f(0, 0, 0);
	}
}





void ofApp::setup() {
    kinect.init();
	kinect.setRegistration(true);
	kinect.open();
    presentImg.allocate(320, 240, OF_IMAGE_COLOR_ALPHA);
    
//---------mesh stuff-----------
    
    ofSetVerticalSync(true);
    pastImg.loadImage("1.png"); //this means we'll always have to have one image to start!
    mesh.setMode(OF_PRIMITIVE_TRIANGLES); //rather than points
    skip = 5;	
	width = pastImg.getWidth();
	height = pastImg.getHeight();
	ofVec3f zero(0, 0, 0);
    glEnable(GL_DEPTH_TEST);


    /*
     
     Below, we'll set up our time/millis check
     so that we don't record too many frames. This 
     will become more important once we're recording to
     ssd media and aren't rate limited by the hdd
     
     */
    
    lastTime = ofGetElapsedTimeMillis();
    currentTime = ofGetElapsedTimeMillis();
}



void ofApp::update() {
    
    currentTime = ofGetElapsedTimeMillis();

//printf("currentTime is: %d,  lastTime is: %d\n", currentTime, lastTime);
    
    
//---------RECORDING the present-----------
	
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
                presentImg.setColor(picX, picY, ofColor(color, depth.getBrightness()));
				
            }
        }
        
       
        
            ostringstream fileNameToSave;
            fileNameToSave << frameNumber << ".png";
            string result = fileNameToSave.str();
			presentImg.saveImage(result);

    }

//---------SHOWING the present or past-----------

if (ofGetKeyPressed('p') || ofGetKeyPressed('o')){
    printf("we are viewing frame number: %d\n", currentFrame);
    if(ofGetKeyPressed('p')){
        if(currentFrame > 2){
            currentFrame--;
        }else{
            currentFrame = 2;
        }
    }
    if(ofGetKeyPressed('o')){
        if (currentFrame < 320){
            currentFrame++;
        }else{
            currentFrame = 320;
        }
        }
    
    ostringstream fileNameToLoad;     
    fileNameToLoad << currentFrame << ".png";     
    frameResult = fileNameToLoad.str(); 
    pastImg.loadImage(ofToString(frameResult));
    mesh.clear();
    
    for(int y = startY; y < height - endBufferY - skip; y += skip) {        
        for(int x = startX; x < width - endBufferX - skip; x += skip) {
            
            /* 
             this is kind of like quadrants
             ofVec3f short for oF vector w/ 3 floats
             vector in c++ could be (1) coming from stl or (2)
             math -- a direction -- in either 2D or 3D space...
             */
            
            ofVec2f nwi (x,y);
            ofVec2f nei (x+skip, y);
            ofVec2f sei (x+skip, y+skip);
            ofVec2f swi (x, y+skip);
            
            ofVec3f nw = getVertexFromImg(pastImg, x, y);
            ofVec3f ne = getVertexFromImg(pastImg, x + skip, y);
            ofVec3f sw = getVertexFromImg(pastImg, x, y + skip);
            ofVec3f se = getVertexFromImg(pastImg, x + skip, y + skip);
           
            /*
             check for bad data i.e. making sure that nothing 
            is zero, otherwise vertices point to front of screen
             */

            if(nw != 0 && ne != 0 && sw != 0 && se != 0) {                 
                addTexCoords(mesh, nwi, nei, sei, swi);
                addFace(mesh, nw, ne, se, sw);                  
                }
            }
        }
    }    
}   



void ofApp::draw() {
    
	ofBackground(0);
    
    cam.begin();
    ofScale(1, -1, 1); // "make y point down" I still don't understand what this means
    pastImg.bind();
    mesh.draw();  //
    pastImg.unbind();
    cam.end();
    
    
    if(kinectDisplayEnabled == true){
        ofSetColor(255, 255, 255);
    	kinect.drawDepth(0, 0, 640, 480);
        kinect.draw(0, 480, 640, 480);
    }

}

void ofApp::exit() {
	kinect.close();
}

//---------recording the present-----------

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



