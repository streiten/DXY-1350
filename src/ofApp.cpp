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
    
    
    // testing string stuffs
    //cout << ofToHex('A');
    //cout << "String: " << ofHexToString("1B2e4d303b303b303b31333b303b303a1B2e493135303b303b31373a1B2e4e3b31393a1B2e403b303a");
    //cout << "Char: " << ofHexToChar("1B");
    //cout << "Int:  " << ofHexToInt("1B");
    
    //char string[] = { 27 ,'*','+' , 0x1B };
    //char string1[] = { "\x1B.M0" };
    
    //for (int i = 0; i < strlen(string1); i++) {
        //  cout << i << ":" << int(string1[i]);
    //}
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if(recalculate){
        
        recalculate = false;
        
        plot.clearHpglCmdBuffer();
        simulationPoints.clear();
        
        // testing the 42 diffrent plot speeds HPGL "VS"

        // Black pen pls
        plot.hpglCmd.hpglCmd = "SP1;";
        plot.hpglCmd.hpglCmdDelay = 0;
        // plot.hpglCmdBuffer.push(plot.hpglCmd);
        
        //plot.hpglCmd.hpglCmd = "\r";
        //plot.hpglCmd.hpglCmdDelay = 0;
        //plot.hpglCmdBuffer.push(plot.hpglCmd);
        
        // now lets generate our drawing: 42 VS values = 42 lines
        int lineCount = 42;
        
        // Margin to paper edge
        int margin = 400;
        int lineheight = ( PLOTHEIGTH - 2 * margin )  / (lineCount-1);
        
        for (int i=0; i < lineCount; i++) {
            int y = lineheight * i;
            int x = margin;
            int x1 = PLOTWIDTH - margin;
            
            // Setting up the HPGL Cmd for each line
            plot.hpglCmd.hpglCmd =  "PA" + ofToString(x) + "," + ofToString(y) + "," + ofToString(x1) + "," + ofToString(y) + ";\r";
            plot.hpglCmd.hpglCmdDelay = 0;
            
            plot.hpglCmdBuffer.push(plot.hpglCmd);
            
            //plot.outputBuffer();
            
            // for simulation on Screen
            simulationPoints.push_back(translateToOFCords(x,y));
            simulationPoints.push_back( translateToOFCords(x1,y));
            
            // cout << margin << "," << y << " -> " << margin <<  "," << y << endl ;
        }
        
        // Is this still needed actually ? final CR for end of command ?
        //plot.hpglCmd.hpglCmd ="\r";
        //plot.hpglCmdBuffer.push(plot.hpglCmd);
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
        i = i + 2;
    }
    
    // lets keep an eye on the buffer
    drawBufferInfo();
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
            
            // Toggl the deviceBufferFull Flag (stop/start TX in plotter update)
        case 'b': {
            if(plot.deviceBufferFull){
                plot.deviceBufferFull = false;
                cout << "deviceBufferFull FALSE now" << endl;
            } else {
                plot.deviceBufferFull = true;
                cout << "deviceBufferFull TURE now" << endl;
            }
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
            cout << "Reset" << endl ;
            plot.outputBuffer();
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
        statusText = "XOFF";
    } else {
        ofSetColor(0,255,0);
        statusText = "XON";
    }
    
    ofDrawBitmapString(statusText, bufferSizeScaled + 3, 12 );
    ofDrawBitmapString(ofToString(plot.hpglCmdBuffer.size()), bufferSizeScaled - 36, 11 );
    
    ofPopStyle();
    ofPopMatrix();
};

