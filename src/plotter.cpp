//
//  plotter.cpp
//  plotter
//
//  Created by Alex on 24.01.16.
//
//

// #define EMULATE

#include "plotter.hpp"

void plotter::setup() {
    
    int baud = 9600;
    
    deviceBufferFull = false;
    bufferSizeRequested = false;
    requestSent = 0;
    
    timer.set(1,true);
    
    serial.listDevices();
    vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
    serial.setup("/dev/tty.usbserial-FT8ZMXZW", baud);
    
    hpglCmd.hpglCmd = "OE;OS;IN;\r" ;
    sendStringToPlotter(&hpglCmd.hpglCmd);
    requestSent = 2;
    
};

void plotter::update(){
    
    // RX
    if(serial.isInitialized() ){
        
        // for how many responses do we wait
        while (requestSent > 0 ) {

            unsigned char currentByte = 0;
            char bytesReturned[255];
            memset(bytesReturned, 0, 255);
            
            int i = 0;
            while ( currentByte != 13 ) {
                if(serial.available() > 0) {
                    bytesReturned[i] = serial.readByte();
                    currentByte = bytesReturned[i];
                    i++;
                }
            }
            
            currentByte = 0;
            requestSent--;
            
            processRX(&bytesReturned[0]);
        }
    }
    
    if(!bufferSizeRequested) {
    
        // TX
        if(!hpglCmdBuffer.empty()) {
            // wait for the delay of last cmd
            if(timer.check()) {
                
            if(!deviceBufferFull) {

                    // get the first in cmd from que
                    struct hpglCmd poppy = hpglCmdBuffer.front();
                    
                    // if there is a delay set, reinit the timer for that
                    if(poppy.hpglCmdDelay > 0) {
                        timer.set(poppy.hpglCmdDelay,true);
                    } else {
                        timer.set(0,true);
                    }
                    
                    sendStringToPlotter(&poppy.hpglCmd);
                    // remove the command from queue
                    hpglCmdBuffer.pop();
            } else {
                ofSleepMillis(3000);
            }
            
            requestBuffer();

            }
        }
    }
    
}

void plotter::clearHpglCmdBuffer () {
    // flush the queue
    queue <struct hpglCmd> empty;
    swap( hpglCmdBuffer, empty );
}

void plotter::sendStringToPlotter(string * buffer){
    
    cout << "TX: " << buffer->data() << endl;
#ifdef EMULATE
#else
    serial.writeBytes((unsigned char *) &buffer[0],buffer->size());
#endif
    
}

int plotter::processRX(char * rx) {
    
    // coutStringDebug(rx);
    
    if(bufferSizeRequested) {
        
        bufferSize = ofToInt(rx);
        bufferSizeRequested = false;
        
        if( bufferSize > 128 )  {
            deviceBufferFull = false;
            cout << endl << "Buffer ok @ " << rx << endl ;
        } else {
            deviceBufferFull = true;
            cout << endl << "Buffer full @" << rx << endl ;
        }
    } else {
        cout << "Something else:" << ofToInt(rx);
    }
    
    cout << endl;
    
}

void plotter::pushInitCmdsToBuffer(){
    
    // Initialize
    hpglCmd.hpglCmd = "IN;OE;\r";
    hpglCmdBuffer.push(hpglCmd);
    
    // Sets terminator for data output from the DXY-1000 series to 13 (D) -> CR
    hpglCmd.hpglCmd = "\x1B.M50;0;0;13;0;0:\r"; // 1B2e4d303b303b303b31333b303b303a
    // hpglCmdBuffer.push(hpglCmd);
    
    // XON char 17 and buffer threshold for XOFF to 150 bytes
    // XON is sent if theres more than 512 bytes in free buffer
    hpglCmd.hpglCmd = "\x1B.I256;0;17:\r"; // 1B2e493135303b303b31373a
    // hpglCmdBuffer.push(hpglCmd);
    
    // XOFF char 19 (less than 150 bytes free -> xoff is sent)
    hpglCmd.hpglCmd = "\x1B.N;19:\r"; // 1B2e4e3b34333a
    // hpglCmdBuffer.push(hpglCmd);
    
    // Enable software XON/XOFF handshaking
    hpglCmd.hpglCmd = "\x1B.@;0:\r"; // 1B2e403b303a
    // hpglCmdBuffer.push(hpglCmd);
    
};

int plotter::reset(){
    
    // flush the queue
    clearHpglCmdBuffer();
    
    // Reset Device Control
    // hpglCmd.hpglCmd = "\x1B.2E52" ;
    
    hpglCmd.hpglCmd = "OS;OE;IN;\r";
    sendStringToPlotter(&hpglCmd.hpglCmd);
    requestSent += 2;
    
};

int plotter::outputError(){
    // Output Error Code
    hpglCmd.hpglCmd = "\x1B.E\r" ;
    sendStringToPlotter(&hpglCmd.hpglCmd);
    requestSent++;

};

int plotter::outputStatus(){
    // Output plotter status
    hpglCmd.hpglCmd = "\x1B.O\r" ;
    sendStringToPlotter(&hpglCmd.hpglCmd);
    requestSent++;

};

int plotter::requestBuffer(){
    // Remaining buffer cap
    hpglCmd.hpglCmd = "\x1B.B\r" ;
    sendStringToPlotter(&hpglCmd.hpglCmd);
    requestSent++;
    bufferSizeRequested = true;
    
};


void plotter::coutStringDebug(char * str) {
    
    cout << "ASCII: " << str << endl;;
    cout << "  HEX: " ;
    
    for(int i = 0 ; i < strlen(str) ; i++) {
        printf("%X ", str[i] ) ;
    }
    cout << endl;
    
    cout << "  DEC: " ;
    for(int i = 0 ; i < strlen(str) ; i++) {
        printf("%u ", str[i] ) ;
    }
    
    cout << endl;
}

