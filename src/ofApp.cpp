#include "ofApp.h"


void ofApp::setup()
{

    ofEnableAlphaBlending();
    ofSeedRandom(1000);

    std::vector<ofx::IO::SerialDeviceInfo> devicesInfo = ofx::IO::SerialDeviceUtils::listDevices();

    ofLogNotice("ofApp::setup") << "Connected Devices: ";

    for (std::size_t i = 0; i < devicesInfo.size(); ++i)
    {
        ofLogNotice("ofApp::setup") << "\t" << devicesInfo[i];
    }

    if (!devicesInfo.empty())
    {
        // Connect to the first matching device.
        bool success = device.setup(devicesInfo[0], 115200);

        if(success)
        {
            device.registerAllEvents(this);
            ofLogNotice("ofApp::setup") << "Successfully setup " << devicesInfo[0];
        }
        else
        {
            ofLogNotice("ofApp::setup") << "Unable to setup " << devicesInfo[0];
        }
    }
    else
    {
        ofLogNotice("ofApp::setup") << "No devices connected.";
    }
    
    
    int width = ofGetWidth();
    int height = ofGetHeight();
    
    fbo.allocate(width, height);
    
    int x = (width - fbo.getWidth()) * 0.5;       // center on screen.
    int y = (height - fbo.getHeight()) * 0.5;     // center on screen.
    int w = fbo.getWidth();
    int h = fbo.getHeight();
    
    fbo.allocate(w, h);
    
    warper.setSourceRect(ofRectangle(0, 0, w, h));              // this is the source rectangle which is the size of the image and located at ( 0, 0 )
    warper.setTopLeftCornerPosition(ofPoint(x, y));             // this is position of the quad warp corners, centering the image on the screen.
    warper.setTopRightCornerPosition(ofPoint(x + w, y));        // this is position of the quad warp corners, centering the image on the screen.
    warper.setBottomLeftCornerPosition(ofPoint(x, y + h));      // this is position of the quad warp corners, centering the image on the screen.
    warper.setBottomRightCornerPosition(ofPoint(x + w, y + h)); // this is position of the quad warp corners, centering the image on the screen.
    warper.setup();
    warper.load(); // reload last saved changes.
}

void ofApp::exit()
{
    device.unregisterAllEvents(this);
    warper.save();
}


void ofApp::update()
{
    // Create a byte buffer.
    //ofx::IO::ByteBuffer buffer("Frame Number: " + ofToString(ofGetFrameNum()));
    
    int pitch = sin(ofGetElapsedTimef()/2) * 90;
    pitch = ofMap(pitch, -90, 90, 0, 90);
    
    int yaw = cos(ofGetElapsedTimef()/2) * 180;
    yaw = ofMap(yaw, -180, 180, 0, 180);
    
    int pitch2 = sin(ofGetElapsedTimef()/4) * 90;
    pitch2 = ofMap(pitch2, -90, 90, 0, 90);
    
    int yaw2 = cos(ofGetElapsedTimef()/4) * 180;
    yaw2 = ofMap(yaw2, -180, 180, 0, 180);
    
    
    // Debugging!!!
    if (resetServoToZero) {
        yaw = 0;
        pitch = 0;
        pitch2 = 0;
        yaw2 = 0;
    }

    //ofx::IO::ByteBuffer buffer("0;" + ofToString(pitch) + ":" + ofToString(pitch));
    
    //ofx::IO::ByteBuffer buffer("0:" + ofToString(yaw) + "&1:" + ofToString(pitch));
    
    ofx::IO::ByteBuffer buffer("0:" + ofToString(yaw) + "&1:" + ofToString(pitch) + "&2:" + ofToString(yaw2) + "&3:" + ofToString(pitch2) + "&4:" + ofToString(yaw) + "&5:" + ofToString(pitch) + "&6:" + ofToString(yaw2) + "&7:" + ofToString(pitch2) + "&8:" + ofToString(yaw) + "&9:" + ofToString(pitch) + "&10:" + ofToString(yaw2) + "&11:" + ofToString(pitch2) + "&12:" + ofToString(yaw) + "&13:" + ofToString(pitch) + "&14:" + ofToString(yaw2) + "&15:" + ofToString(pitch2) + "&16:" + ofToString(yaw) + "&17:" + ofToString(pitch) + "&18:" + ofToString(yaw2) + "&19:" + ofToString(pitch2) + "&20:" + ofToString(yaw) + "&21:" + ofToString(pitch) + "&22:" + ofToString(yaw2) + "&23:" + ofToString(pitch2) + "&24:" + ofToString(yaw) + "&25:" + ofToString(pitch) + "&26:" + ofToString(yaw2) + "&27:" + ofToString(pitch2) + "&28:" + ofToString(yaw) + "&29:" + ofToString(pitch) + "&30:" + ofToString(yaw2) + "&31:" + ofToString(pitch2));

    // Send the byte buffer.
    // ofx::IO::PacketSerialDevice will encode the buffer, send it to the
    // receiver, and send a packet marker.
    device.send(buffer);
    
    for(int i=0; i<10; i++) {
        // randomise points over the image area.
        points[i].x = ofRandom(fbo.getWidth());
        points[i].y = ofRandom(fbo.getHeight());
    }
}


void ofApp::draw()
{
    

    ofBackground(0);
    ofSetColor(255);

    int width = ofGetWidth();
    int height = ofGetHeight();
    
    fbo.begin();
    
    ofBackground(0);
    
    for (int i = 0; i < gridSize; i++) {
        
        for (int j = 0; j < gridSize; j++) {
            
            float rate = ofGetElapsedTimef() / (((i * j / 2)) + 1) * 2;
            cout << rate << endl;
            
            ofSetColor(255, 255, 255, ofMap(sin(rate), -1, 1, 0, 255));
            
            ofDrawRectangle(i * (width / 4), j * (height / 4), width / 4, height / 4);
            
            
        }
        
    }
    
    fbo.end();
    
    //fbo.draw(0, 0);
    
    
    //======================== get our quad warp matrix.
    
    ofMatrix4x4 mat = warper.getMatrix();
    
    //======================== use the matrix to transform our fbo.
    
    ofPushMatrix();
    ofMultMatrix(mat);
    fbo.draw(0, 0);
    ofPopMatrix();

    //======================== use the matrix to transform points.
    
    ofSetLineWidth(2);
    ofSetColor(ofColor::cyan);
    
    for(int i=0; i<9; i++) {
        int j = i + 1;
        
        ofVec3f p1 = mat.preMult(ofVec3f(points[i].x, points[i].y, 0));
        ofVec3f p2 = mat.preMult(ofVec3f(points[j].x, points[j].y, 0));
        
        //ofDrawLine(p1.x, p1.y, p2.x, p2.y);
    }
    
    //======================== draw quad warp ui.
    
    ofSetColor(ofColor::magenta);
    warper.drawQuadOutline();
    
    ofSetColor(ofColor::yellow);
    warper.drawCorners();
    
    ofSetColor(ofColor::magenta);
    warper.drawHighlightedCorner();
    
    ofSetColor(ofColor::red);
    warper.drawSelectedCorner();
    
    //======================== info.
    
    
    if (showDebug) {
        ofSetColor(ofColor::white);
        ofDrawBitmapString("to warp the image, drag the corners of the image.", 20, 30);
        ofDrawBitmapString("press 's' to toggle quad warp UI. this will also disable quad warp interaction.", 20, 50);
        ofDrawBitmapString("press & hold 1, 2, 3, 4 to snap that point to the mouse", 20, 70);
        ofDrawBitmapString("when a corner is selected (red), use keyboard arrow keys to nudge the corner position.", 20, 90);
    }
    
    

    if (showDebug) {
        std::stringstream ss;

        ss << "         FPS: " << ofGetFrameRate() << std::endl;
        ss << "Connected to: " << device.getPortName();

        ofDrawBitmapString(ss.str(), ofVec2f(20, 120));
        
        int x = 20;
        int y = 150;
        int height = 20;


        auto iter = serialMessages.begin();

        while (iter != serialMessages.end())
        {
            iter->fade -= 20;
            
            if (iter->fade < 0)
            {
                iter = serialMessages.erase(iter);
            }
            else
            {
                ofSetColor(255, ofClamp(iter->fade, 0, 255));
                ofDrawBitmapString(iter->message, ofVec2f(x, y));

                y += height;

                if (!iter->exception.empty())
                {
                    ofSetColor(255, 0, 0, ofClamp(iter->fade, 0, 255));
                    ofDrawBitmapString(iter->exception, ofVec2f(x + height, y));
                    y += height;
                }

                ++iter;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == '0') {
        
        resetServoToZero = true;
        
    } else if (key == '9') {
        
        resetServoToZero = false;
        
    } else if (key == ' ') {
        
        showDebug = !showDebug;
        
    }
    
    if(key == 's' || key == 'S') {
        warper.toggleShow();
    }
    
    if(key == 'l' || key == 'L') {
        warper.load();
    }
    
    if(key == 'h' || key == 'H') {
        warper.save();
    }
}


void ofApp::onSerialBuffer(const ofx::IO::SerialBufferEventArgs& args)
{
    // Decoded serial packets will show up here.
    SerialMessage message(args.getBuffer().toString(), "", 255);
    serialMessages.push_back(message);
}


void ofApp::onSerialError(const ofx::IO::SerialBufferErrorEventArgs& args)
{
    // Errors and their corresponding buffer (if any) will show up here.
    SerialMessage message(args.getBuffer().toString(),
                          args.getException().displayText(),
                          500);

    serialMessages.push_back(message);
}
