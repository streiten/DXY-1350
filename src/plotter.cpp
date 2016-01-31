//
//  plotter.cpp
//  plotter
//
//  Created by Alex on 24.01.16.
//
//

#define EMULATE

#include "plotter.hpp"

void plotter::setup() {
    
    int baud = 9600;
    memset(bytesReadString, 0, 255);
    
    // ToDo: assume its full, check with outputBuffer() and clear the flag afterwards
    deviceBufferFull = false;

    timer.set(0,true);
    
    serial.listDevices();
    vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
    serial.setup("/dev/tty.usbserial-FT8ZMXZW", baud);
    
    // ToDo: make the whole init independent of the buffer processing in update
    pushInitCmdsToBuffer();

};

void plotter::update(){
    
    // RX
    if(serial.isInitialized() ){
        if(serial.available() > 0) {
            unsigned char bytesReturned[255];
            memset(bytesReadString, 0, 256);
            memset(bytesReturned, 0, 255);
            serial.readBytes(bytesReturned, serial.available());
            memcpy(bytesReadString, bytesReturned, 255);
            
            cout << "RX: " << bytesReadString << endl;
            
            processRX(bytesReadString);
            
        }
    }
    
    // TX
    if(!hpglCmdBuffer.empty()) {
        if(!deviceBufferFull) {
            // wait for the delay of last cmd
            if(timer.check()) {
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
    
    coutStringDebug(rx);
    
    for(int i = 0 ; i < strlen(rx) ; i++) {
        
        if( rx[i] == 17 )  {
            deviceBufferFull = false;
            cout << endl << "XON recieved!" << endl ;
        }
        
        if ( rx[i] == 19){
            deviceBufferFull = true;
            cout << endl << "XOFF recieved!" << endl ;
        }
    }
    
    cout << endl;
    
}

void plotter::pushInitCmdsToBuffer(){
    // Initialize
    hpglCmd.hpglCmd = "IN;";
    hpglCmdBuffer.push(hpglCmd);
    
    // Sets terminator for data output from the DXY-1000 series to 13 -> CR
    hpglCmd.hpglCmd = "\x1B.M0;0;0;13;0;0:"; // 1B2e4d303b303b303b31333b303b303a
    hpglCmdBuffer.push(hpglCmd);
    
    // XON char 17 and buffer threshold for XOFF to 150 bytes
    // XON is sent if theres more than 512 bytes in free buffer
    hpglCmd.hpglCmd = "\x1B.I150;0;17:"; // 1B2e493135303b303b31373a
    hpglCmdBuffer.push(hpglCmd);
    
    // XOFF char 19 (less than 150 bytes free -> xoff is sent)
    hpglCmd.hpglCmd = "\x1B.N;19:"; // 1B2e4e3b34333a
    hpglCmdBuffer.push(hpglCmd);
    
    // Enable software XON/XOFF handshaking
    hpglCmd.hpglCmd = "\x1B.@;0:"; // 1B2e403b303a
    hpglCmdBuffer.push(hpglCmd);

};

int plotter::reset(){
    
    // flush the queue
    clearHpglCmdBuffer();
    
    // Reset Device Control
    // hpglCmd.hpglCmd = "\x1B.2E52" ;
    
    hpglCmd.hpglCmd = "OS;OE;IN;\r";
    hpglCmdBuffer.push(hpglCmd);
    
};

int plotter::outputError(){
    // Output Error Code
    hpglCmd.hpglCmd = "\x1B.2E45" ;
    hpglCmdBuffer.push(hpglCmd);
};

int plotter::outputStatus(){
    // Output plotter status
    hpglCmd.hpglCmd = "\x1B.2E4F" ;
    hpglCmdBuffer.push(hpglCmd);
};

int plotter::outputBuffer(){
    // Remaining buffer cap
    hpglCmd.hpglCmd = "\x1B.2E42" ;
    hpglCmdBuffer.push(hpglCmd);
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
