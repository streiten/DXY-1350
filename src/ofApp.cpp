#include "ofApp.h"
#include "string.h"
#include "stdio.h"

// FULL PLOT RANGE
// #define PLOTHEIGTH 11040
// #define PLOTWIDTH 16158

// 210 x 210 (A4 Square)
#define PLOTHEIGTH 7340
#define PLOTWIDTH 8200

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(60);
    
    ofBackground(255);
    // ofSetLogLevel(OF_LOG_VERBOSE);
    
    plot.setup();
    
    // start sending only after 'b' keystroke (clearing the deviceBufferFlag)
    plot.deviceBufferFull = true;
    
    recalculate = true;
    emulate = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if(recalculate){
        
        recalculate = false;
        
        plot.clearHpglCmdBuffer();
        simulationPoints.clear();
        
        // testing the 42 diffrent plot speeds HPGL "VS"
        // Black pen pls
        pushToBuffer("SP1;VS5;\r",0);

        // now lets generate our drawing: 42 VS values = 42 lines
        int lineCount = 200;
        
        // Margin to paper edge
        int margin = 400;
        int lineheight = ( PLOTHEIGTH - 2 * margin )  / (lineCount-1);
        
        for (int i=0; i < lineCount/2; i++) {
            int y = lineheight * i * 2;
            int x = margin * 2;
            int x1 = PLOTWIDTH - margin * 2;
            
            // Setting up the HPGL Cmd for each line
            pushToBuffer("PA" + ofToString(x) + "," + ofToString(y) + ";PD;\r",0);
            pushToBuffer("PA" + ofToString(x1) + "," + ofToString(y) + ";PU;\r",0);

            // for simulation on Screen
            simulationPoints.push_back(translateToOFCords(x,y));
            simulationPoints.push_back(translateToOFCords(x1,y));
            
        }
        
        pushToBuffer("SP2;VS5;\r",0);

        int y = 0;
        for (int i=0; i < lineCount/2; i++) {
            int y = lineheight * i * 2 + lineheight;
            int x = margin * 2;
            int x1 = PLOTWIDTH - margin * 2;
            
            // Setting up the HPGL Cmd for each line
            pushToBuffer("PA" + ofToString(x) + "," + ofToString(y) + ";PD;\r",0);
            pushToBuffer("PA" + ofToString(x1) + "," + ofToString(y) + ";PU;\r",0);

            // for simulation on Screen
            simulationPoints.push_back(translateToOFCords(x,y));
            simulationPoints.push_back(translateToOFCords(x1,y));
            
        }
        
        pushToBuffer("SP2;",0);
        
    }
    
    plot.update();

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetColor(0,0,0);
    ofSetLineWidth(1);
    
    vector<int>::size_type i = 0;
    while( i < simulationPoints.size() - 1) {
        
        ofDrawLine(simulationPoints[i], simulationPoints[i+1]);
        ofDrawBitmapString(ofToString(i/2),simulationPoints[i]);
        
        i += 2;
    }
    
    // lets keep an eye on the buffer
    drawBufferInfo();
}


void ofApp::pushToBuffer(string cmd, int delay)
{
    if(!emulate){
        plot.hpglCmd.hpglCmd = cmd;
        plot.hpglCmd.hpglCmdDelay = delay;
        plot.hpglCmdBuffer.push(plot.hpglCmd);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key) {
    switch(key) {
            
        // send the HPGLString to Plotter
        case 's': {
            cout << "Sending: " << HPGLString << endl ;
            plot.sendStringToPlotter(&HPGLString);
            break;
        }
            
        case 'r': {
            cout << "Redraw!" << endl ;
            recalculate = true;
            break;
        }
            
        // Push dummies to cmdBuffer
        case 'd': {
            
            struct plotter::hpglCmd pushy;
            pushy.hpglCmd = "PA"+ ofToString(floor(ofRandom(8000))) + "," + ofToString(floor(ofRandom(8000))) + ";\r";
            pushy.hpglCmdDelay = ofRandom(100);
            plot.hpglCmdBuffer.push(pushy);
            break;
        }
            
        case 'x': {
            cout << "Reset" << endl ;
            plot.reset();
            break;
        }
            
        case 'e': {
            cout << "Output Err" << endl ;
            plot.outputError();
            break;
        }
        
        case 'c': {
            cout << "Request remaining buffer... " << endl ;
            plot.requestBuffer();
            break;
        }
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
    struct plotter::hpglCmd pushy;
    
    pX = mouseX * PLOTWIDTH / ofGetWidth();
    pY = ( ofGetHeight() - mouseY ) * PLOTHEIGTH / ofGetHeight();
    
    pushy.hpglCmd  = "PA" + ofToString(pX) + "," + ofToString(pY) + ";\r";
    pushy.hpglCmdDelay = 0;
    plot.hpglCmdBuffer.push(pushy);
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    struct plotter::hpglCmd pushy;
    
    pX = mouseX * PLOTWIDTH / ofGetWidth();
    pY = ( ofGetHeight() - mouseY ) * PLOTHEIGTH / ofGetHeight();
    
    pushy.hpglCmd = "PA" + ofToString(pX) + "," + ofToString(pY) + ";PD;\r";
    pushy.hpglCmdDelay = 0;

    plot.hpglCmdBuffer.push(pushy);
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    struct plotter::hpglCmd pushy;
    
    pX = mouseX * PLOTWIDTH / ofGetWidth();
    pY = ( ofGetHeight() - mouseY ) * PLOTHEIGTH / ofGetHeight();

    pushy.hpglCmd = "PU;\r";
    pushy.hpglCmdDelay = 0;

    plot.hpglCmdBuffer.push(pushy);

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

ofPoint ofApp::translateToOFCords(int x , int y) {
    
    float ofWX = ofMap(x,0,PLOTWIDTH,0,ofGetWidth());
    // inversing y here for oF zero zero cords in upper left
    float ofWY = ofMap(PLOTHEIGTH-y-400,0,PLOTHEIGTH,0,ofGetHeight());
    return ofPoint(ofWX,ofWY);
}

void ofApp::drawBufferInfo(){
    ofPushMatrix();
    ofFill();
    ofSetColor(192);
    
    int bufferSizeScaled = plot.hpglCmdBuffer.size() / 2;
    
    ofDrawRectangle(0, 0,bufferSizeScaled,15 );
    if(plot.deviceBufferFull){
        ofPushStyle();
        ofSetColor(255,0,0);
        statusText = "FULL";
    } else {
        ofSetColor(0,255,0);
        statusText = "OK";
    }
    
    ofDrawBitmapString(statusText, bufferSizeScaled + 3, 12 );
    ofDrawBitmapString(ofToString(plot.hpglCmdBuffer.size()), bufferSizeScaled - 36, 11 );
    
    ofPopStyle();
    ofPopMatrix();
};

