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
    
    int bufferSize;
    bool bufferSizeRequested;
    bool deviceBufferFull;

    int requestSent;
    
    struct hpglCmd {
        string hpglCmd;
        int hpglCmdDelay;
    };
    
    queue <hpglCmd> hpglCmdBuffer;
    hpglCmd hpglCmd;
    void clearHpglCmdBuffer();

    atTimer timer;
    
    void setup();
    void update();
    
    void pushInitCmdsToBuffer();
    
    int reset();
    int outputError();
    int outputStatus();
    int requestBuffer();
    
    void sendStringToPlotter( string * buffer);
    
    int processRX(char * rx);
    
    void coutStringDebug(char * str);
    
};

