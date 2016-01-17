#pragma once

#include "ofMain.h"
#include "string"


class ofApp : public ofBaseApp{
	
	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
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
    
        void sendStringToPlotter( string * buffer);
        string HPGLString;
		
		char		bytesRead[255];				// data from serial, we will be trying to read 3
		char		bytesReadString[256];			// a string needs a null terminator, so we need 3 + 1 bytes
    
        int pX, pY;

		ofSerial	serial;
};

