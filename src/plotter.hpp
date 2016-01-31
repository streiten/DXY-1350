//
//  plotter.hpp
//  plotter
//
//  Created by Alex on 24.01.16.
//
//

#include "ofMain.h"
#include "string"
#include "utils.h"

class plotter {

public:
    
    ofSerial	serial;
    char		bytesRead[255];
    char		bytesReadString[256];
    
    struct hpglCmd {
        string hpglCmd;
        int hpglCmdDelay;
    };
    
    queue <hpglCmd> hpglCmdBuffer;
    hpglCmd hpglCmd;
    void clearHpglCmdBuffer();

    
    bool deviceBufferFull;
    atTimer timer;
    
    void setup();
    void update();
    
    void pushInitCmdsToBuffer();
    
    int reset();
    int outputError();
    int outputStatus();
    int outputBuffer();
    
    void sendStringToPlotter( string * buffer);
    
    int processRX(char * rx);
    
    void coutStringDebug(char * str);
    
};

