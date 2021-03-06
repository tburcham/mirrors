#pragma once


#include "ofMain.h"
#include "ofxSerial.h"
#include "ofxQuadWarp.h"
#include "ofxGui.h"
#include "mirror.h"


class SerialMessage
{
public:
    SerialMessage(): fade(0)
    {
    }
    
    SerialMessage(const std::string& _message,
                  const std::string& _exception,
                  int _fade):
        message(_message),
        exception(_exception),
        fade(_fade)
    {
    }

    std::string message;
    std::string exception;
    int fade;
};


class ofApp: public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed(int key);
    /*void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);*/

    void onSerialBuffer(const ofx::IO::SerialBufferEventArgs& args);
    void onSerialError(const ofx::IO::SerialBufferErrorEventArgs& args);

    ofx::IO::PacketSerialDevice device;

    std::vector<SerialMessage> serialMessages;
    
    bool resetServoToZero = false;
    
    bool showDebug = false;
    
    ofxQuadWarp warper;
    ofFbo fbo;
    ofPoint points[10];
    
    ofxPanel panel;
    ofxIntSlider gridSize;
    ofxIntSlider lightPattern;
    ofxIntSlider servoPattern;
    ofxIntSlider pace;
    ofxIntSlider specificMirror;
    ofxToggle useSchedule;
    
    ofxFloatSlider videoContrast;
    ofxFloatSlider videoBrightness;
    
    int numMirrors = 16;
    //int gridSize = 4;
    
    vector<int> servos;
    
    vector<Mirror> mirrors;
    //Mirror mirrors[2];
    
    void circleStroke( int x, int y, int rad, int stroke );
    
    //int lightPattern = 0;
    //int servoPattern = 0;
    
    ofVideoPlayer player;
    
    ofTrueTypeFont font;
    int currTextPos;
    ofVec2f currTextCoords;
    ofFbo textFbo;
    
    string scheduleDebug;

};
