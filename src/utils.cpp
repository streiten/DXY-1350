//
//  utils.cpp
//
//  Created by Alex on 17.11.15.
//
//

#include "utils.h"
#include "ofMain.h"

void atTimer::set(int millis,bool autoReset){
    this->delay = millis;
    this->autoReset = autoReset;
}

bool atTimer::check() {
    
    if( (ofGetElapsedTimeMillis() - this->last_time) > this->delay){
        
        if(autoReset){
            reset();
        }
        return true;
    } else {
        return false;
    }
    
};

int atTimer::get() {
    return ofGetElapsedTimeMillis() - this->last_time;
}

void atTimer::reset() {
    this->last_time = ofGetElapsedTimeMillis();
};