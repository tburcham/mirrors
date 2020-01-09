#include "ofApp.h"


void ofApp::setup()
{
    
    
    servos.resize(numMirrors * 2);
    
    cout << numMirrors << endl;
    
    mirrors.resize(numMirrors);
    
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
    textFbo.allocate(w, h);
    
    warper.setSourceRect(ofRectangle(0, 0, w, h));              // this is the source rectangle which is the size of the image and located at ( 0, 0 )
    warper.setTopLeftCornerPosition(ofPoint(x, y));             // this is position of the quad warp corners, centering the image on the screen.
    warper.setTopRightCornerPosition(ofPoint(x + w, y));        // this is position of the quad warp corners, centering the image on the screen.
    warper.setBottomLeftCornerPosition(ofPoint(x, y + h));      // this is position of the quad warp corners, centering the image on the screen.
    warper.setBottomRightCornerPosition(ofPoint(x + w, y + h)); // this is position of the quad warp corners, centering the image on the screen.
    warper.setup();
    warper.load(); // reload last saved changes.
    warper.toggleShow();
    
    
    panel.setup("", "settings.xml", 100, 500);
    panel.add(useSchedule.setup("Use Schedule?", false));
    panel.add(gridSize.setup("Grid Size", 4, 1, 16));
    panel.add(lightPattern.setup("Light Pattern", 0, 0, 9));
    panel.add(servoPattern.setup("Servo Pattern", 0, 0, 7));
    panel.add(pace.setup("Pace", 10, 10, 180));
    panel.add(specificMirror.setup("Specific Mirror", 0, 0, 15));
    
    panel.add(videoBrightness.setup("Video Brightness", 0.5, 0.0, 1.0));
    panel.add(videoContrast.setup("Video Contrast", 0.5, 0.0, 1.0));
    panel.loadFromFile("settings.xml");
    
    lightPattern = 0;
    servoPattern = 0;
    
    player.load("geese3.mp4");
    player.setVolume(0);
    player.setLoopState(OF_LOOP_PALINDROME);
    //player.setSpeed(0.3);
    //player.stop();
    //player.play();
    
    font.load("Apercu.ttf", 200, true, true, true);
}

void ofApp::exit()
{
    device.unregisterAllEvents(this);
    warper.save();
}


void ofApp::update()
{
    
    if (useSchedule) {
        // Change ptterns based on time
        
        float time = ofGetElapsedTimef();
        //cout << "time:" << time << " pace:" << pace << endl;
        
        float p = pace * (lightPattern.getMax() + 1);
        float timemod = fmodf(time, p);
        float pattern = floor(timemod / pace);
        lightPattern = pattern;
        
        float diff = (timemod / pace) - pattern;
        
        //cout << "Schedule: p:" << p << ", timemod:" << timemod << ", pattern:" << pattern << endl;
        if (showDebug) {
            std::stringstream ss;
            ss << "Schedule: p:" << p;
            ss << ", timemod: " << timemod;
            ss << ", pattern: " << pattern;
            
            ss << ", diff:" << diff;
            ss << std::endl;
            
            scheduleDebug = ss.str();
            //ofSetColor(255, 255, 255);
            //ofDrawBitmapString(ss.str(), 20, 200);
            
            //cout << ss.str() << endl;
        }
        
        //
        
        float s = pace * (servoPattern.getMax() + 1);
        float stimemod = fmodf(time, s);
        float spattern = floor(stimemod / pace);
        servoPattern = spattern;
        
    }
    
    // Create a byte buffer.
    //ofx::IO::ByteBuffer buffer("Frame Number: " + ofToString(ofGetFrameNum()));
    
    
    
    //ofx::IO::ByteBuffer buffer("0;" + ofToString(pitch) + ":" + ofToString(pitch));
    //ofx::IO::ByteBuffer buffer("0:" + ofToString(yaw) + "&1:" + ofToString(pitch));
    
    
    
    //for (int i = 0; i < mirrors.size(); i++) {
    
    cout << "Mirrors:" << mirrors.size() << endl;
    
    // Map mirror positions and servo numbers
    mirrors[0].setup( 10, 20, 21, 0, 0);
    mirrors[1].setup( 8,  16, 17, 1, 0);
    mirrors[2].setup( 0,  0,  1,  2, 0);
    mirrors[3].setup( 2,  4,  5,  3, 0);
    
    mirrors[4].setup( 11, 22, 23, 0, 1);
    mirrors[5].setup( 9,  18, 19, 1, 1);
    mirrors[6].setup( 1,  2,  3,  2, 1);
    mirrors[7].setup( 3,  6,  7,  3, 1);
    
    mirrors[8].setup( 14, 28, 29, 0, 2);
    mirrors[9].setup( 12, 24, 25, 1, 2);
    mirrors[10].setup(4,  8,  9,  2, 2);
    mirrors[11].setup(6,  12, 13, 3, 2);
    
    mirrors[12].setup(15, 30, 31, 0, 3);
    mirrors[13].setup(13, 26, 27, 1, 3);
    mirrors[14].setup(5,  10, 11, 2, 3);
    mirrors[15].setup(7,  14, 15,  3, 3);
    
    //}
    
    
    // Calibration
    if (servoPattern == 0) {
        
        int yaw, pitch, pitch2, yaw2;
        if (resetServoToZero) {
            pitch = 15;
            yaw = 0;
            pitch2 = 15;
            yaw2 = 0;
        } else {
            pitch = 90;
            yaw = 180;
            pitch2 = 90;
            yaw2 = 180;
            
        }
        
        for (int i = 0; i < numMirrors; i++) {
            
            int mirrorNum = mirrors[i].mirrorNum;
            
            if (mirrorNum == specificMirror) {
                mirrors[i].yawPosition = yaw;
                mirrors[i].pitchPosition = pitch;
            }
            
            //cout << "Debugging 0, Mirror " << i << ":" << yaw << "," << pitch << "," << yaw2 << "," << pitch2 << endl;
            
        }
        
    // Slow & Halfway
    } else if (servoPattern == 1) {
        
        int pitch = sin(ofGetElapsedTimef()/2) * 90;
        pitch = ofMap(pitch, -90, 90, 30, 60);
        
        int yaw = cos(ofGetElapsedTimef()/2) * 180;
        yaw = ofMap(yaw, -180, 180, 120, 180);
        
        int pitch2 = sin(ofGetElapsedTimef()/2) * 90;
        pitch2 = ofMap(pitch2, -90, 90, 60, 30);
        
        int yaw2 = cos(ofGetElapsedTimef()/2) * 180;
        yaw2 = ofMap(yaw2, -180, 180, 60, 30);
        
        /*int pitch = 45;  //45   30 - 60
         int pitch2 = 45; //45   30 - 60
         
         int yaw = 180;   //150  120 - 180
         int yaw2 = 0;   //30    0 - 60*/
        
        
        // Debugging!!!
        if (resetServoToZero) {
            yaw = 0;
            pitch = 0;
            pitch2 = 0;
            yaw2 = 0;
        }
        
        for (int i = 0; i < numMirrors; i++) {
            
            int mirrorNum = mirrors[i].mirrorNum;
            
            if (mirrorNum < 8) {
                mirrors[i].yawPosition = yaw;
                mirrors[i].pitchPosition = pitch;
            } else {
                mirrors[i].yawPosition = yaw2;
                mirrors[i].pitchPosition = pitch2;
            }
            
            cout << "Pattern 0, Mirror " << i << ":" << yaw << "," << pitch << "," << yaw2 << "," << pitch2 << endl;
            
        }
        
        
        //for (int i = 0; i < numMirrors; i++) {
        
    // Stay in place
    } else if (servoPattern == 22) {
        
        int pitch, yaw, pitch2, yaw2;
        
        yaw = 180;
        pitch = 45;
        pitch2 = 45;
        yaw2 = 0;
        
        for (int i = 0; i < numMirrors; i++) {
            
            int mirrorNum = mirrors[i].mirrorNum;
            
            if (mirrorNum < 8) {
                mirrors[i].yawPosition = yaw;
                mirrors[i].pitchPosition = pitch;
            } else {
                mirrors[i].yawPosition = yaw2;
                mirrors[i].pitchPosition = pitch2;
            }
            
            //cout << "Mirror " << i << ":" << yaw << "," << pitch << "," << endl;
            
        }
        
    // Slower & Halfway
    } else if (servoPattern == 2) {
        
        int pitch = sin(ofGetElapsedTimef()/8) * 90;
        pitch = ofMap(pitch, -90, 90, 15, 90);
        
        int yaw = cos(ofGetElapsedTimef()/8) * 180;
        yaw = ofMap(yaw, -180, 180, 100, 180);
        
        int pitch2 = sin(ofGetElapsedTimef()/16) * 90;
        pitch2 = ofMap(pitch2, -90, 90, 15, 90);
        
        int yaw2 = cos(ofGetElapsedTimef()/16) * 180;
        yaw2 = ofMap(yaw2, -180, 180, 100, 180);
        
        for (int i = 0; i < numMirrors; i++) {
            
            int mirrorNum = mirrors[i].mirrorNum;
            
            if (mirrorNum < 8) {
                mirrors[i].yawPosition = yaw;
                mirrors[i].pitchPosition = pitch;
            } else {
                mirrors[i].yawPosition = yaw2;
                mirrors[i].pitchPosition = pitch2;
            }
            
        }
        
    
    // Fast & Halfway
    } else if (servoPattern == 3) {
        
        int pitch = sin(ofGetElapsedTimef()/2) * 90;
        pitch = ofMap(pitch, -90, 90, 15, 90);
        
        int yaw = cos(ofGetElapsedTimef()/2) * 180;
        yaw = ofMap(yaw, -180, 180, 100, 180);
        
        int pitch2 = sin(ofGetElapsedTimef()/2) * 90;
        pitch2 = ofMap(pitch2, -90, 90, 15, 90);
        
        int yaw2 = cos(ofGetElapsedTimef()/2) * 180;
        yaw2 = ofMap(yaw2, -180, 180, 180, 100);
        
        
        // Debugging!!!
        if (resetServoToZero) {
            yaw = 0;
            pitch = 0;
            pitch2 = 0;
            yaw2 = 0;
        }
        
        for (int i = 0; i < numMirrors; i++) {
            
            int mirrorNum = mirrors[i].mirrorNum;
            
            if (mirrorNum < 8) {
                mirrors[i].yawPosition = yaw;
                mirrors[i].pitchPosition = pitch;
            } else {
                mirrors[i].yawPosition = yaw2;
                mirrors[i].pitchPosition = pitch2;
            }
            
        }
        
    // Up & Down 2 45
    } else if (servoPattern == 4) {
        
        int pitch = sin(ofGetElapsedTimef()) * 90;
        pitch = ofMap(pitch, -90, 90, 15, 75);
        
        /*int yaw = cos(ofGetElapsedTimef()/2) * 180;
         yaw = ofMap(yaw, -180, 180, 0, 180);*/
        
        int yaw = 135;
        
        int pitch2 = sin(ofGetElapsedTimef()) * 90;
        pitch2 = ofMap(pitch2, -90, 90, 15, 75);
        
        /*int yaw2 = cos(ofGetElapsedTimef()/2) * 180;
         yaw2 = ofMap(yaw2, -180, 180, 180, 0);*/
        
        int yaw2 = 45;
        
        
        // Debugging!!!
        if (resetServoToZero) {
            yaw = 0;
            pitch = 0;
            pitch2 = 0;
            yaw2 = 0;
        }
        
        for (int i = 0; i < numMirrors; i++) {
            
            int mirrorNum = mirrors[i].mirrorNum;
            
            if (mirrorNum < 8) {
                mirrors[i].yawPosition = yaw;
                mirrors[i].pitchPosition = pitch;
            } else {
                mirrors[i].yawPosition = yaw2;
                mirrors[i].pitchPosition = pitch2;
            }
            
        }
        
    // Twitchy & Tight
    } else if (servoPattern == 5) {
        
        int pitch = sin(ofGetElapsedTimef()/4) * 90;
        pitch = ofMap(pitch, -90, 90, 15, 75);
        
        int yaw = cos(ofGetElapsedTimef()/4) * 180;
        yaw = ofMap(yaw, -180, 180, 0, 180);
        
        //int yaw = 135;
        
        int pitch2 = sin(ofGetElapsedTimef()/4) * 90;
        pitch2 = ofMap(pitch2, -90, 90, 15, 75);
        
        int yaw2 = cos(ofGetElapsedTimef()/4) * 180;
        yaw2 = ofMap(yaw2, -180, 180, 180, 0);
        
        //int yaw2 = 45;
        
        
        // Debugging!!!
        if (resetServoToZero) {
            yaw = 0;
            pitch = 0;
            pitch2 = 0;
            yaw2 = 0;
        }
        
        for (int i = 0; i < numMirrors; i++) {
            
            int mirrorNum = mirrors[i].mirrorNum;
            
            if (mirrorNum < 8) {
                mirrors[i].yawPosition = yaw;
                mirrors[i].pitchPosition = pitch;
            } else {
                mirrors[i].yawPosition = yaw2;
                mirrors[i].pitchPosition = pitch2;
            }
            
        }
        
    // Solo Fast
    } else if (servoPattern == 6) {
        
        /*int pitch3 = sin(ofGetElapsedTimef()*4) * 90;
         pitch3 = ofMap(pitch3, -90, 90, 0, 90);
         
         int yaw3 = cos(ofGetElapsedTimef()*4) * 180;
         yaw3 = ofMap(yaw3, -180, 180, 0, 180);*/
        
        
        int pitch3 = ofRandom(90);
        int yaw3 = ofRandom(180);
        
        //int yaw = 135;
        
        /*int pitch2 = sin(ofGetElapsedTimef()/4) * 90;
         pitch2 = ofMap(pitch2, -90, 90, 15, 75);
         
         int yaw2 = cos(ofGetElapsedTimef()/4) * 180;
         yaw2 = ofMap(yaw2, -180, 180, 145, 125);*/
        
        //int yaw2 = 45;
        
        int yaw, pitch, yaw2, pitch2;
        
        yaw = 180;
        pitch = 45;
        pitch2 = 45;
        yaw2 = 0;
        
        
        // Debugging!!!
        if (resetServoToZero) {
            yaw = 0;
            pitch = 0;
            pitch2 = 0;
            yaw2 = 0;
        }
        
        for (int i = 0; i < numMirrors; i++) {
            
            int mirrorNum = mirrors[i].mirrorNum;
            
            if (mirrorNum == 8) {
                mirrors[i].yawPosition = yaw3;
                mirrors[i].pitchPosition = pitch3;
            } else if (mirrorNum < 8) {
                mirrors[i].yawPosition = yaw;
                mirrors[i].pitchPosition = pitch;
            } else {
                mirrors[i].yawPosition = yaw2;
                mirrors[i].pitchPosition = pitch2;
            }
            
        }
        
    // 180, Sync'd & smooth
    } else if (servoPattern == 7) {
        
        int pitch = sin(ofGetElapsedTimef()/4) * 90;
        pitch = ofMap(pitch, -90, 90, 0, 90);
        
        int yaw = cos(ofGetElapsedTimef()/4) * 180;
        yaw = ofMap(yaw, -180, 180, 0, 180);
        
        //int yaw = 135;
        
        int pitch2 = sin(ofGetElapsedTimef()/4) * 90;
        pitch2 = ofMap(pitch2, -90, 90, 0, 90);
        
        int yaw2 = cos(ofGetElapsedTimef()/4) * 180;
        yaw2 = ofMap(yaw2, -180, 180, 0, 180);
        
        //int yaw2 = 45;
        
        
        // Debugging!!!
        if (resetServoToZero) {
            yaw = 90;
            pitch = 0;
            pitch2 = 0;
            yaw2 = 90;
        }
        
        for (int i = 0; i < numMirrors; i++) {
            
            int mirrorNum = mirrors[i].mirrorNum;
            
            if (mirrorNum < 8) {
                mirrors[i].yawPosition = yaw;
                mirrors[i].pitchPosition = pitch;
            } else {
                mirrors[i].yawPosition = yaw2;
                mirrors[i].pitchPosition = pitch2;
            }
            
        }
        
        
    }
    
    
    
    /* Send to Arduino over Serial! */
    
    string bufferStr;
    
    //cout << "Num Mirrors " << numMirrors << endl;
    
    for (int i = 0; i < numMirrors; i++) {
        
        if (i > 0) {
            bufferStr += "&";
        }
        bufferStr += ofToString(mirrors[i].yawServo) + ":" + ofToString(mirrors[i].yawPosition) + "&" + ofToString(mirrors[i].pitchServo) + ":" + ofToString(mirrors[i].pitchPosition);
        
    }
    
    //cout << bufferStr << endl;
    
    ofx::IO::ByteBuffer buffer(bufferStr);
    
    /*ofx::IO::ByteBuffer buffer("0:" + ofToString(yaw) + "&1:" + ofToString(pitch) + "&2:" + ofToString(yaw2) + "&3:" + ofToString(pitch2) + "&4:" + ofToString(yaw) + "&5:" + ofToString(pitch) + "&6:" + ofToString(yaw2) + "&7:" + ofToString(pitch2) + "&8:" + ofToString(yaw) + "&9:" + ofToString(pitch) + "&10:" + ofToString(yaw2) + "&11:" + ofToString(pitch2) + "&12:" + ofToString(yaw) + "&13:" + ofToString(pitch) + "&14:" + ofToString(yaw2) + "&15:" + ofToString(pitch2) + "&16:" + ofToString(yaw) + "&17:" + ofToString(pitch) + "&18:" + ofToString(yaw2) + "&19:" + ofToString(pitch2) + "&20:" + ofToString(yaw) + "&21:" + ofToString(pitch) + "&22:" + ofToString(yaw2) + "&23:" + ofToString(pitch2) + "&24:" + ofToString(yaw) + "&25:" + ofToString(pitch) + "&26:" + ofToString(yaw2) + "&27:" + ofToString(pitch2) + "&28:" + ofToString(yaw) + "&29:" + ofToString(pitch) + "&30:" + ofToString(yaw2) + "&31:" + ofToString(pitch2));*/
    
    // Send the byte buffer.
    // ofx::IO::PacketSerialDevice will encode the buffer, send it to the
    // receiver, and send a packet marker.
    
    cout << "Sending " << buffer << endl;
    device.send(buffer);
    
    
    
    for(int i=0; i<10; i++) {
        // randomise points over the image area.
        points[i].x = ofRandom(fbo.getWidth());
        points[i].y = ofRandom(fbo.getHeight());
    }
    
    
    if (lightPattern == 8) {
        if (player.isPaused()) {
            player.play();
        }
        //if (player.isFrameNew()) {
            player.update();
        //}
    } else {
        player.stop();
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
    
    // Follow the mirrors
    if (lightPattern == 0) {
        
        for (int i = 0; i < numMirrors; i++) {
            
            int rate = ofMap(mirrors[i].yawPosition, 0, 180, 0, 255);
            /*if (i % 2 == 0) {
             rate = ofMap(rate, 0, 180, 0, 255);
             } else {
             rate = ofMap(rate, 0, 90, 0, 255);
             }*/
            
            ofSetColor(255, 255, 255, rate);
            //ofDrawRectangle(
            //ofDrawRectangle(i * (width / gridSize), i * (height / gridSize), width / gridSize, height / gridSize);
            
            int x, y, w, h;
            
            w = width / gridSize;
            h = height / gridSize;
            x = mirrors[i].x * w;
            y = mirrors[i].y * h;
            
            //cout << mirrors[i].mirrorNum << ":" << i << ":" << x << "," << y << "yp:" << mirrors[i].yawPosition << endl;
            
            ofDrawRectangle(x, y, w, h);
            
            
        }
        
    } else if (lightPattern == 1) {
        
        for (int i = 0; i < numMirrors; i++) {
            
            ofSetColor(255, 255, 255);
            
            int x, y, w, h;
            
            w = width / gridSize;
            h = height / gridSize;
            x = mirrors[i].x * w;
            y = mirrors[i].y * h;
            
            
            int radius = ofMap(sin(ofGetElapsedTimef() * 2 + i), -1, 1, 0.1, w/2);
            
            //cout << mirrors[i].mirrorNum << ":" << i << ":" << x << "," << y << "yp:" << mirrors[i].yawPosition << endl;
            
            //ofSetCircleResolution(100);
            //ofDrawCircle(x + w/2, y + h/2, radius);
            
            circleStroke(x + w/2, y + h/2, radius, radius);
            
            
        }
        
    } else if (lightPattern == 22) {
        
        for (int i = 0; i < numMirrors; i++) {
            
            ofSetColor(255, 255, 255);
            
            int x, y, w, h;
            
            w = width / gridSize;
            h = height / gridSize;
            x = mirrors[i].x * w;
            y = mirrors[i].y * h;
            
            
            int radius = ofMap(sin(ofGetElapsedTimef() * 2 + i), -1, 1, 0.1, w/2);
            
            //cout << mirrors[i].mirrorNum << ":" << i << ":" << x << "," << y << "yp:" << mirrors[i].yawPosition << endl;
            
            /*ofSetCircleResolution(100);
             ofNoFill();
             ofSetLineWidth(10);
             ofDrawCircle(x + w/2, y + h/2, radius);*/
            
            circleStroke(x + w/2, y + h/2, radius, 10);
            
        }
        
    // Text
    } else if (lightPattern == 2) {
        
        
        
        
        int w, h;
        w = width;
        h = height;
        
        //string text = "who said gas will get me the miles";
        string text = "Are image and video inputs processed by it stored, and how are they used by them? It may store and use image and video inputs processed by the service solely to provide and maintain the service and, unless you opt out as provided below, to improve and develop the quality of it and other  technologies. Use of your content is important for continuous improvement of its customer experience, including the development and training of related technologies. We do not use any personally identifiable information that may be contained in your content to target products, services or marketing to you or your end users. Your trust, privacy, and the security of your content are our highest priority and we implement appropriate and sophisticated technical and physical controls, including encryption at rest and in transit, designed to prevent unauthorized access to, or disclosure of, your content and ensure that our use complies with our commitments to you. Please see faq for more information. You may opt out of having your image and video inputs used to improve or develop the quality of it and other technologies by contacting Support. Can I delete image and video inputs stored by it? Yes. You can request deletion of image and video inputs associated with your account by contacting Support. Deleting image and video inputs may degrade its experience. Who has access to my content that is processed and stored by it? Only authorized employees will have access to your content that is processed by it. Your trust, privacy, and the security of your content are our highest priority and we implement appropriate and sophisticated technical and physical controls, including encryption at rest and in transit, designed to prevent unauthorized access to, or disclosure of, your content and ensure that our use complies with our commitments to you. Please see faq for more information.";
        vector<string> textTokens = ofSplitString(text, " ");
        
        
        float time = ofGetElapsedTimef();
        //cout << "time:" << time << " pace:" << pace << endl;
        
        float textPace = 2.0;
        
        float p = textPace * (textTokens.size() + 1);
        float timemod = fmodf(time, p);
        float textPos = floor(timemod / textPace);
        
        float diff = (timemod / textPace) - textPos;
        
        cout << "textPos:" << textPos << " textTokens.size():" << textTokens.size() << endl;
        cout << "diff:" << diff << endl;
        
        if (textPos < textTokens.size()) {
        
            if (diff < 0.5) {
                ofSetColor(255, 255, 255, ofMap(diff, 0, 0.5, 0, 255));
            } else {
                ofSetColor(255, 255, 255, ofMap(diff, 0.5, 1, 255, 0));
            }
            
            if (currTextPos != textPos) { // it just changed
                currTextCoords.x = ofRandom(w/2);
                currTextCoords.y = ofRandom(h);
            }
            
            cout << textPos << "," << ofRandom(textPos);
            
            //int x = ofRandom(textPos) * (w/2);
            //int y = ofRandom(textPos*2) * h;
            
            /*textFbo.clear();
            textFbo.begin();
            
                ofSetBackgroundColor(0,0,0);*/
                font.drawStringAsShapes(textTokens[textPos], currTextCoords.x, currTextCoords.y);
            
            /*textFbo.end();
            
            textFbo.draw(0, 0, w, h);
            
            //fbo.draw(w, h, -w, h);
            ofTexture flipped = textFbo.getTexture();*/
            
            
            
            
            //float pos = ofGetWidth()/2.;
            //flipped.draw(w, 0, -w, h);
            //flipped.draw(0,0);
            //fbo.draw();
            
            currTextPos = textPos;
            
        }
        
        
        /*for (int i = 0; i < textTokens.size(); i++) {
            
            font.drawStringAsShapes(textTokens[i], ofRandom(w), ofRandom(h));
            
        }*/
        
        
        
    // Fast circles
    } else if (lightPattern == 3) {
        
        ofSetColor(255, 255, 255, 200);
        
        int w, h;
        
        w = width;
        h = height;
        //x = w / 2;
        //y = h / 2;
        
        for (int i = 0; i < 8; i++) {
            
            int radius = ofMap(sin(ofGetElapsedTimef() / 3 + i), -1, 1, 0.1, w/1.5);
            
            int x = ofMap(sin(ofGetElapsedTimef() / 3 + i), -1, 1, 0, width);
            int y = ofMap(cos(ofGetElapsedTimef() / 3 + i), -1, 1, 0, height);
            
            circleStroke(x, y, radius, 50);
            
        }
        
    } else if (lightPattern == 4) {
        
        for (int i = 0; i < numMirrors; i++) {
            
            if (i % 3 == 0) {
                ofSetColor(255, 0, 0);
            } else if (i % 3 == 1) {
                ofSetColor(0, 255, 0);
            } else if (i % 3 == 2) {
                ofSetColor(0, 0, 255);
            }
            
            int x, y, w, h;
            
            w = width / gridSize;
            h = height / gridSize;
            x = mirrors[i].x * w;
            y = mirrors[i].y * h;
            
            
            int _w = ofMap(sin(ofGetElapsedTimef() / 2 + i), -1, 1, 0.1, w);
            int _h = ofMap(cos(ofGetElapsedTimef() / 2 + i), -1, 1, 0.1, h);
            
            //circleStroke(x + w/2, y + h/2, radius, radius);
            
            ofDrawRectangle(x, y, _w, _h);
            
            
        }
    
    } else if (lightPattern == 5) {
        
        int R_rate = ofMap(sin(ofGetElapsedTimef() / 3), -1, 1, 0, 255);
        int G_rate = ofMap(sin(ofGetElapsedTimef() / 4), -1, 1, 0, 255);
        int B_rate = ofMap(sin(ofGetElapsedTimef() / 5), -1, 1, 0, 255);
        
        ofSetColor(255, 0, 0, R_rate);
        ofDrawRectangle(0, 0, width / 3, height);
        
        ofSetColor(0, 255, 0, G_rate);
        ofDrawRectangle(width / 3, 0, width / 3, height);
        
        ofSetColor(0, 0, 255, B_rate);
        ofDrawRectangle(2 * (width / 3), 0, width / 3, height);
        
    } else if (lightPattern == 55) {
        
        for (int i = 0; i < numMirrors; i++) {
            
            if (i % 3 == 0) {
                ofSetColor(255, 0, 0);
            } else if (i % 3 == 1) {
                ofSetColor(0, 255, 0);
            } else if (i % 3 == 2) {
                ofSetColor(0, 0, 255);
            }
            
            int x, y, w, h;
            
            w = width / gridSize;
            h = height / gridSize;
            x = mirrors[i].x * w;
            y = mirrors[i].y * h;
            
            
            int radius = ofMap(sin((ofGetElapsedTimef() / 4) + ofRandom(i)), -1, 1, 0.1, w/2);
            
            if (radius == w/2 - 1) {
                circleStroke(x + w/2, y + h/2, radius, radius);
            } else {
                //circleStroke(x + w/2, y + h/2, 5, 1);
            }
            
            
        }
    } else if (lightPattern == 6) {
        
        
        
        for (int i = 0; i < numMirrors; i++) {
        
            int x, y, w, h;
            
            w = width / gridSize;
            h = height / gridSize;
            x = mirrors[i].x * w;
            y = mirrors[i].y * h;
            
            
            int R_rate = ofMap(sin((ofGetElapsedTimef() + i) / 3), -1, 1, 0, 255);
            int G_rate = ofMap(sin((ofGetElapsedTimef() + i) / 4), -1, 1, 0, 255);
            int B_rate = ofMap(sin((ofGetElapsedTimef() + i) / 5), -1, 1, 0, 255);
            
            ofSetColor(255, 0, 0, R_rate);
            ofDrawRectangle(x, y, w / 3, h);
            
            ofSetColor(0, 255, 0, G_rate);
            ofDrawRectangle(x + w / 3, y, w / 3, h);
            
            ofSetColor(0, 0, 255, B_rate);
            ofDrawRectangle(x + (2 * (w / 3)), y, w / 3, h);
            
        }
    } else if (lightPattern == 9) {
        
        int units = ofMap(sin(ofGetElapsedTimef() / 10), -1, 1, 1, 50);
        
        for (int i = 0; i < units; i++) {
            
            for (int j = 0; j < units; j++) {
                int x, y, w, h;
                
                w = width / units;
                h = height / units;
                x = i * w;
                y = j * h;
                
                
                int R_rate = ofMap(sin((ofGetElapsedTimef() + i + j)), -1, 1, 0, 255);
                int G_rate = ofMap(sin((ofGetElapsedTimef() + i + j) / 2), -1, 1, 0, 255);
                int B_rate = ofMap(sin((ofGetElapsedTimef() + i + j) / 1.5), -1, 1, 0, 255);
                
                ofSetColor(255, 0, 0, R_rate);
                ofDrawRectangle(x, y, w / 3, h);
                
                ofSetColor(0, 255, 0, G_rate);
                ofDrawRectangle(x + w / 3, y, w / 3, h);
                
                ofSetColor(0, 0, 255, B_rate);
                ofDrawRectangle(x + (2 * (w / 3)), y, w / 3, h);
            }
        }
        
    } else if (lightPattern == 66) {
        
        for (int i = 0; i < numMirrors; i++) {
            
            int rate = ofMap(sin(ofGetElapsedTimef() / 10), -1, 1, 127, 255);
            
            ofSetColor(255, 255, 255, rate);
            //ofDrawRectangle(
            //ofDrawRectangle(i * (width / gridSize), i * (height / gridSize), width / gridSize, height / gridSize);
            
            int x, y, w, h;
            
            w = width / gridSize;
            h = height / gridSize;
            x = mirrors[i].x * w;
            y = mirrors[i].y * h;
            
            ofDrawRectangle(x, y, w, h);
            
            
        }
        
    }  else if (lightPattern == 7) {
        
        for (int i = 0; i < numMirrors; i++) {
            
            int rate = ofMap(sin(ofGetElapsedTimef() / 20), -1, 1, 127, 255);
            
            ofSetColor(255, 255, 255, rate);
            //ofDrawRectangle(
            //ofDrawRectangle(i * (width / gridSize), i * (height / gridSize), width / gridSize, height / gridSize);
            
            int x, y, w, h;
            
            w = width / gridSize;
            h = height / gridSize;
            x = mirrors[i].x * w;
            y = mirrors[i].y * h;
            
            //ofDrawRectangle(x, y, w / 2, h / 2);
            
            int radius = ofMap(sin(ofGetElapsedTimef() / 3 + i), -1, 1, 0.1, w/1.5);
            int res = ofMap(cos(ofGetElapsedTimef() / 4 + i), -1, 1, 2, 25);
            
            //ofSetSphereResolution(res);
            
            //ofDrawSphere(x, y, radius);
            
            ofSpherePrimitive sphere;
            sphere.set(radius,res);
            sphere.setPosition(x,y,0);
            sphere.drawWireframe();
            
        }
        
    } else if (lightPattern == 8) {
        
        ofPixels p;
        //p.allocate(player.getWidth(), player.getHeight(), OF_IMAGE_GRAYSCALE);
        p = player.getPixels();
        
        ofxCvColorImage i;
        i.setFromPixels(p);
        //i.resize(i.getWidth() * 5, i.getHeight() * 5);
        
        ofxCvGrayscaleImage grayImage;
        grayImage = i;
        grayImage.invert();
        
        grayImage.resize(grayImage.getWidth() * 7, grayImage.getHeight() * 7);
        
        //grayImage.brightnessContrast(videoBrightness, videoContrast);
        grayImage.brightnessContrast(0.0, 1.0);
        
        //grayImage.draw(-grayImage.getWidth() / 2,-(grayImage.getHeight()/5));
        //grayImage.draw((-grayImage.getWidth() / 2) + 800, -2800);
        grayImage.draw(-5140, -2800);
        //grayImage.draw(0,0);
        
        //cout << "height:" << grayImage.getHeight() << endl;
        //cout << "mouseY:" << ofGetMouseX() << endl;
    }
    
    /*for (int i = 0; i < gridSize; i++) {
     
     for (int j = 0; j < gridSize; j++) {
     
     float rate = ofGetElapsedTimef() / (((i * j / 2)) + 1) * 2;
     cout << rate << endl;
     
     ofSetColor(255, 255, 255, ofMap(sin(rate), -1, 1, 0, 255));
     
     //float rate =
     
     ofDrawRectangle(i * (width / gridSize), j * (height / gridSize), width / gridSize, height / gridSize);
     
     }
     }*/
    
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
        ofShowCursor();
    } else {
        ofHideCursor();
    }
    
    if (showDebug) {
        panel.draw();
    }
    
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
        ss << "Connected to: " << device.getPortName() << std::endl;
        ss << "Servo Pattern:" << servoPattern << std::endl;
        
        ofDrawBitmapString(ss.str(), ofVec2f(20, 120));
        
        ofDrawBitmapString(scheduleDebug, 20, 160);
        
        
        int x = 20;
        int y = 170;
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

void ofApp::circleStroke( int x, int y, int rad, int stroke ){
    ofBeginShape();
    int resolution = 100;
    float angle = 0;
    
    for( int i=0; i<resolution; i++ ){
        angle = i*2*PI/resolution;
        ofVertex(x + rad*sin(angle), y + rad*cos(angle));
    }
    
    ofNextContour(true);
    int rad2 = rad - stroke;
    
    for( int i=0; i<resolution; i++ ){
        angle = i*2*PI/resolution;
        ofVertex(x + rad2*sin(angle), y + rad2*cos(angle));
    }
    
    ofEndShape(true);
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
