#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    volume = 1.0f;
    distortion = 1.0f;
    gWidth = 300; // Width of graph
    gHeight = 200;
    gHeightCenter = gHeight / 2;
    
    ofSetVerticalSync(true);
    ofSetCircleResolution(80);
    ofBackground(54, 54, 54);

    ofLog(OF_LOG_NOTICE, "-------------------------------------");
    soundStream.printDeviceList();
    ofLog(OF_LOG_NOTICE, "-------------------------------------");

    int bufferSize = 256;
    lMic.assign(bufferSize, 0.0);
    rMic.assign(bufferSize, 0.0);
    lMicSize = lMic.size();
    rMicSize = rMic.size();

    lAudio.assign(bufferSize, 0.0);
    rAudio.assign(bufferSize, 0.0);
    lAudioSize = lAudio.size();
    rAudioSize = rAudio.size();

    volHistory.assign(400, 0.0);
    
    bufferCounter	= 0;
    drawCounter		= 0;
    smoothedVol     = 0.0;
    scaledVol		= 0.0;

    //ofSoundStreamSetup(int nOutputs, int nInputs, int sampleRate, int bufferSize, int nBuffers)
    ofSoundStreamSettings settings;

    // if you want to set the device id to be different than the default
    // auto devices = soundStream.getDeviceList();
    // settings.device = devices[4];

    // you can also get devices for an specific api
    // auto devices = soundStream.getDevicesByApi(ofSoundDevice::Api::PULSE);
    // settings.device = devices[0];

    // or get the default device for an specific api:
    // settings.api = ofSoundDevice::Api::PULSE;

    // or by name
    auto devices = soundStream.getDeviceList();
    //auto devices = soundStream.getMatchingDevices("default in");
    if(!devices.empty())
    {
        ///*
        // get sound in PC(Mac) by "Black Hole"
        settings.setInDevice(devices[2]);
        ofLog(OF_LOG_NOTICE, "deviceID = %d", devices[2].deviceID);
        ofLog(OF_LOG_NOTICE,  devices[2].name);
        //*/
        /*
        // Mic input
        settings.setInDevice(devices[0]);
        ofLog(OF_LOG_NOTICE, "deviceID = %d", devices[0].deviceID);
        ofLog(OF_LOG_NOTICE,  devices[0].name);
        */
        // Audio output
        settings.setOutDevice(devices[1]);
        ofLog(OF_LOG_NOTICE, "deviceID = %d", devices[1].deviceID);
        ofLog(OF_LOG_NOTICE,  devices[1].name);
    }

    settings.setInListener(this);  // Mic
    settings.setOutListener(this); // Audio
    settings.sampleRate = 44100;
    settings.numOutputChannels = 2; // default: settings.numOutputChannels = 0;
    settings.numInputChannels = 2;
    settings.bufferSize = bufferSize;
    soundStream.setup(settings);

    //----------------------- gui(ofxGui)
    guiPanel.setup(); // most of the time you don't need a name
    guiPanel.setPosition(32+2*gWidth+10,150);
    // initial,min,max
    guiPanel.add(volume.setup("volume", 1, 0, 3));
    guiPanel.add(distortion.setup("clip input", 1, 0, 1));
    
    bHide = false;

    //----------------------- gui(ofxDatGui)
    /*
    // instantiate and position the gui //
    gui = new ofxDatGui( ofxDatGuiAnchor::TOP_RIGHT );
    //gui = new ofxDatGui(32+2*gWidth+10,150);
    gui->addHeader("Settings");
    gui->addBreak();
    gui->addSlider("clip", 0, 1, 1);
    gui->addSlider("volume", 0, 3, 1);

    // once the gui has been assembled, register callbacks to listen for component specific events //
    gui->onButtonEvent(this, &ofApp::onButtonEvent);
    gui->onToggleEvent(this, &ofApp::onToggleEvent);
    gui->onSliderEvent(this, &ofApp::onSliderEvent);
    gui->onTextInputEvent(this, &ofApp::onTextInputEvent);
    gui->on2dPadEvent(this, &ofApp::on2dPadEvent);
    gui->onDropdownEvent(this, &ofApp::onDropdownEvent);
    gui->onColorPickerEvent(this, &ofApp::onColorPickerEvent);
    gui->onMatrixEvent(this, &ofApp::onMatrixEvent);

    gui->setOpacity(gui->getSlider("datgui opacity")->getScale());
    gui->setLabelAlignment(ofxDatGuiAlignment::CENTER);

    // finally let's load up the stock themes, press spacebar to cycle through them //
    themes = {  new ofxDatGuiTheme(true),
               new ofxDatGuiThemeSmoke(),
               new ofxDatGuiThemeWireframe(),
               new ofxDatGuiThemeMidnight(),
               new ofxDatGuiThemeAqua(),
               new ofxDatGuiThemeCharcoal(),
               new ofxDatGuiThemeAutumn(),
               new ofxDatGuiThemeCandy()
    };
    tIndex = 0;

    // launch the app //
    mFullscreen = false;
    refreshWindow();
    */
}

//--------------------------------------------------------------
void ofApp::update()
{
    //lets scale the vol up to a 0-1 range
    scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);

    //lets record the volume into an array
    volHistory.push_back( scaledVol );

    //if we are bigger the the size we want to record - lets drop the oldest value
    if( volHistory.size() >= 400 )
    {
        volHistory.erase(volHistory.begin(), volHistory.begin()+1);
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
    // auto draw?
    // should the gui control hiding?
    if(!bHide){
        guiPanel.draw();
    }

    float scale = 100.0f;
    float tmp_x = 0.0f;
    int tmp_vec_size = 0;
    ofSetColor(225);
    ofDrawBitmapString("AUDIO INPUT EXAMPLE", 32, 42);
    //string reportString = "";
    //string reportString = "volume: (" + ofToString((float)volume,2) + ") modify with -/+";
    string reportString = "volume: (" + ofToString((float)volume,2) + ")";
    ofDrawBitmapString(reportString, 32, 82);
    ofDrawBitmapString("clip: (" + ofToString((float)distortion,2) + ")",32,94);
    ofDrawBitmapString("press 's' to unpause the audio\n'e' to pause the audio", 32, 106);

    ofNoFill();

    //------------------- draw the left Mic:
    ofPushStyle();
    ofPushMatrix();
    //ofTranslate(x,y,z); // Move the coordinate system to position of point(x,y,z) and make that zero.
    ofTranslate(32, 150, 0); // Change origin of the coordinates.
    ofSetColor(225);
    ofDrawBitmapString("Left input", 4, 18);

    ofSetLineWidth(1);
    ofDrawRectangle(0, 0, gWidth, gHeight);

    ofSetColor(245, 58, 135);
    ofSetLineWidth(3);

    ofBeginShape();
    for (unsigned int i = 0; i < lMicSize; i++)
    {
        //ofMap: Given a value and an input range, map the value to an output range.
        tmp_x = ofMap(i, 0, lMicSize, 0, gWidth, true);
        ofVertex(tmp_x, gHeightCenter - lMic[i]*scale);
    }
    ofEndShape(false);

    ofPopMatrix();
    ofPopStyle();

    //------------------- draw the right Mic:
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(32+gWidth, 150, 0);
    ofSetColor(225);
    ofDrawBitmapString("Right input", 4, 18);
    ofSetLineWidth(1);
    ofDrawRectangle(0, 0, gWidth, gHeight);

    ofSetColor(245, 58, 135);
    ofSetLineWidth(3);
    ofBeginShape();
    for (unsigned int i = 0; i < rMicSize; i++)
    {
        //iの範囲(i1_min,i1_max)から別の範囲(i2_min,i2_max)へ変換する
        tmp_x = ofMap(i, 0, rMicSize, 0, gWidth, true);
        ofVertex(tmp_x, gHeightCenter - rMic[i]*scale);
    }
    ofEndShape(false);
    ofPopMatrix();
    ofPopStyle();
    ///////////////////////////////////////////////////////////////////////////////////////////
    //------------------- draw the left Audio:
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(32, 350, 0);
    ofSetColor(225);
    ofDrawBitmapString("Left output", 4, 18);
    ofSetLineWidth(1);
    ofDrawRectangle(0, 0, gWidth, gHeight);

    ofSetColor(245, 58, 135);
    ofSetLineWidth(3);
    ofBeginShape();
    for (unsigned int i = 0; i < lAudioSize; i++)
    {
        tmp_x = ofMap(i, 0, lAudioSize, 0, gWidth, true);
        ofVertex(tmp_x, gHeightCenter - lAudio[i]*scale);
    }
    ofEndShape(false);
    ofPopMatrix();
    ofPopStyle();

    //------------------- draw the right Audio:
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(32+gWidth, 350, 0);
    ofSetColor(225);
    ofDrawBitmapString("Right output", 4, 18);
    ofSetLineWidth(1);
    ofDrawRectangle(0, 0, gWidth, gHeight);

    ofSetColor(245, 58, 135);
    ofSetLineWidth(3);
    ofBeginShape();
    for (unsigned int i = 0; i < rAudioSize; i++)
    {
        tmp_x = ofMap(i, 0, rAudioSize, 0, gWidth, true);
        ofVertex(tmp_x, gHeightCenter - rAudio[i]*scale);
    }
    ofEndShape(false);
    ofPopMatrix();
    ofPopStyle();
    //---------------------------------
    
    drawCounter++;

    ofSetColor(225);
    reportString = "buffers received: "+ofToString(bufferCounter)+"\ndraw routines called: "+ofToString(drawCounter)+"\nticks: " + ofToString(soundStream.getTickCount());
    ofDrawBitmapString(reportString, 300, 82);

}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer & input)
{
    //ofLog(OF_LOG_NOTICE, "audioIn called");
    float amp_in = 30;
    float clip_in = 1.0;
    float amp = 1.0;

    float curVol = 0.0;

    // samples are "interleaved"
    int numCounted = 0;

    //lets go through each sample and calculate the root mean square which is a rough way to calculate volume
    for (size_t i = 0; i < input.getNumFrames(); i++)
    {
        //--------------------- input
        lMic[i]	= input[i*2]*amp_in;   // even number: left audio
        if(lMic[i] > clip_in)
        {
            lMic[i] = clip_in;
        }
        if(lMic[i] < -clip_in)
        {
            lMic[i] = -clip_in;
        }
        rMic[i]	= input[i*2+1]*amp_in; // odd number: right audio
        if(rMic[i] > clip_in)
        {
            rMic[i] = clip_in;
        }
        if(rMic[i] < -clip_in)
        {
            rMic[i] = -clip_in;
        }
        //--------------------- set distortion sound for output
        lAudio[i] = lMic[i]; //
        if(lAudio[i] > distortion)
        {
            lAudio[i] = distortion;
        }
        if(lAudio[i] < -distortion)
        {
            lAudio[i] = -distortion;
        }

        rAudio[i] = rMic[i];
        if(rAudio[i] > distortion)
        {
            rAudio[i] = distortion;
        }
        if(rAudio[i] < -distortion)
        {
            rAudio[i] = -distortion;
        }

        lAudio[i] *= volume;
        rAudio[i] *= volume;

        /*
        lAudio[i] *= volume*amp;
        rAudio[i] *= volume*amp;
        */

        //ofLog(OF_LOG_NOTICE, "l = %f",lMic[i]);
        //ofLog(OF_LOG_NOTICE, "r = %f",rMic[i]);
        curVol += lMic[i] * lMic[i];
        curVol += rMic[i] * rMic[i];
        numCounted+=2;

    }

    //this is how we get the mean of rms :)
    curVol /= (float)numCounted;

    // this is how we get the root of rms :)
    curVol = sqrt( curVol );

    smoothedVol *= 0.93;
    smoothedVol += 0.07 * curVol;

    bufferCounter++;
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer &buffer)
{
    //ofLog(OF_LOG_NOTICE, "audioOut called");
    for (size_t i = 0; i < buffer.getNumFrames(); i++)
    {
        float lSample = 0.0;
        float rSample = 0.0;
        // buffer.getNumChannels() : 2
        buffer[i * buffer.getNumChannels()] = lAudio[i];
        buffer[i * buffer.getNumChannels() + 1] = rAudio[i];
        // ofLog(OF_LOG_NOTICE, "lAudio[i] = %f",lAudio[i]);
        // ofLog(OF_LOG_NOTICE, "rAudio[i] = %f",rAudio[i]);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key)
{
    float tmp = 0.0;
    /*
    if (key == '-' || key == '_')
    {
        volume = volume - 0.05;
        volume = MAX(volume, 0);
    }
    else if (key == '+' || key == '=')
    {
        volume = volume + 0.05;
        volume = MIN(volume, 3);
    }
    */
    if( key == 's' )
    {
        soundStream.start();
    }

    if( key == 'e' )
    {
        soundStream.stop();
    }
    if (key == 'f')
    {
        toggleFullscreen();
    }
    else if (key == 32)
    {
        tIndex = tIndex < themes.size()-1 ? tIndex+1 : 0;
        gui->setTheme(themes[tIndex]);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{

}

//--------------------------------------------------------------
void ofApp::onButtonEvent(ofxDatGuiButtonEvent e)
{
    cout << "onButtonEvent: " << e.target->getLabel() << endl;
}

void ofApp::onToggleEvent(ofxDatGuiToggleEvent e)
{
    if (e.target->is("toggle fullscreen")) toggleFullscreen();
    cout << "onToggleEvent: " << e.target->getLabel() << " " << e.checked << endl;
}

void ofApp::onSliderEvent(ofxDatGuiSliderEvent e)
{
    cout << "onSliderEvent: " << e.target->getLabel() << " " << e.target->getValue() << endl;
    if (e.target->is("datgui opacity")) gui->setOpacity(e.scale);
    if (e.target->is("volume")) volume = e.target->getValue();
    if (e.target->is("distortion")) distortion = e.target->getValue();
}

void ofApp::onTextInputEvent(ofxDatGuiTextInputEvent e)
{
    cout << "onTextInputEvent: " << e.target->getLabel() << " " << e.target->getText() << endl;
}

void ofApp::on2dPadEvent(ofxDatGui2dPadEvent e)
{
    cout << "on2dPadEvent: " << e.target->getLabel() << " " << e.x << ":" << e.y << endl;
}

void ofApp::onDropdownEvent(ofxDatGuiDropdownEvent e)
{
    cout << "onDropdownEvent: " << e.target->getLabel() << " Selected" << endl;
}

void ofApp::onColorPickerEvent(ofxDatGuiColorPickerEvent e)
{
    cout << "onColorPickerEvent: " << e.target->getLabel() << " " << e.target->getColor() << endl;
    ofSetBackgroundColor(e.color);
}

void ofApp::onMatrixEvent(ofxDatGuiMatrixEvent e)
{
    cout << "onMatrixEvent " << e.child << " : " << e.enabled << endl;
    cout << "onMatrixEvent " << e.target->getLabel() << " : " << e.target->getSelected().size() << endl;
}


void ofApp::toggleFullscreen()
{
    mFullscreen = !mFullscreen;
    gui->getToggle("toggle fullscreen")->setChecked(mFullscreen);
    refreshWindow();
}

void ofApp::refreshWindow()
{
    ofSetFullscreen(mFullscreen);
    if (!mFullscreen)
    {
        int width = ofGetScreenWidth() * .8;
        int height = ofGetScreenHeight() * .8;
        ofSetWindowShape(width, height);
        ofSetWindowPosition((ofGetScreenWidth()/2)-(width/2), 0);
    }
}


