#include "mirror.h"
Mirror::Mirror(){
}

void Mirror::setup(int _mirrorNum, int _yawServo, int _pitchServo, float _x, float _y){
    //x = ofRandom(0, ofGetWidth());      // give some random positioning
    //y = ofRandom(0, ofGetHeight());
    
    //speedX = ofRandom(-1, 1);           // and random speed and direction
    //speedY = ofRandom(-1, 1);
    
    //dim = 20;
    
    mirrorNum = _mirrorNum;
    yawServo = _yawServo;
    pitchServo = _pitchServo;
    
    x = _x;
    y = _y;
    
    color.set(ofRandom(255),ofRandom(255),ofRandom(255)); // one way of defining digital color is by addressing its 3 components individually (Red, Green, Blue) in a value from 0-255, in this example we're setting each to a random value
}

void Mirror::update(){
    /*if(x < 0 ){
        x = 0;
        speedX *= -1;
    } else if(x > ofGetWidth()){
        x = ofGetWidth();
        speedX *= -1;
    }
    
    if(y < 0 ){
        y = 0;
        speedY *= -1;
    } else if(y > ofGetHeight()){
        y = ofGetHeight();
        speedY *= -1;
    }
    
    x+=speedX;
    y+=speedY;*/
}

void Mirror::draw(){
    ofSetColor(color);
    ofDrawCircle(x, y, 0);
}
