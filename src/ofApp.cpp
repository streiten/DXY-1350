#include "ofApp.h"
#include "string"

#define PLOTHEIGTH 11040
#define PLOTWIDTH 16158

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
    ofSetFrameRate(25);
    
	ofBackground(255);
	ofSetLogLevel(OF_LOG_VERBOSE);
    	
    serial.listDevices();
	vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
	
	int baud = 9600;
	serial.setup("/dev/tty.usbserial-FT8ZMXZW", baud); //linux example
	
	memset(bytesReadString, 0, 255);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if(serial.available() > 0) {
        unsigned char bytesReturned[255];
        memset(bytesReadString, 0, 256);
        memset(bytesReturned, 0, 255);
        serial.readBytes(bytesReturned, serial.available());
        memcpy(bytesReadString, bytesReturned, 255);
        cout << "RX:" << bytesReadString << endl;
    }
    
}

void ofApp::sendStringToPlotter(string * buffer){

    cout << "TX:" << buffer->data() << endl;
    serial.writeBytes((unsigned char *) &buffer[0],buffer->size());

}

//--------------------------------------------------------------
void ofApp::draw(){
    
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){ 
    switch(key) {
    case 'x':
        cout << "Reset" << endl ;
            string outString = "OS;OE;IN;\r";
            sendStringToPlotter(&outString);
        break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){ 
	
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
	
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
    pX = mouseX * PLOTWIDTH / ofGetWidth();
    pY = ( ofGetHeight() - mouseY ) * PLOTHEIGTH / ofGetHeight();
    
    string outString = "PA" + ofToString(pX) + "," + ofToString(pY) + ";";
    sendStringToPlotter(&outString);
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    pX = mouseX * PLOTWIDTH / ofGetWidth();
    pY = ( ofGetHeight() - mouseY ) * PLOTHEIGTH / ofGetHeight();

    cout << "Mouse pressed" << endl;
    string outString = "PA" + ofToString(pX) + "," + ofToString(pY) + ";PD;\r" ;
    sendStringToPlotter(&outString);
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
    pX = mouseX * PLOTWIDTH / ofGetWidth();
    pY = ( ofGetHeight() - mouseY ) * PLOTHEIGTH / ofGetHeight();
    
    string outString = "PU;\r";
    cout << "Mouse released" << endl;
    sendStringToPlotter(&outString);
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
	
}

