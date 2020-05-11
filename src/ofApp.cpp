#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    volume = 1.0f;
	ofSetVerticalSync(true);
	ofSetCircleResolution(80);
	ofBackground(54, 54, 54);	

	ofLog(OF_LOG_NOTICE, "-------------------------------------");
	soundStream.printDeviceList();
	ofLog(OF_LOG_NOTICE, "-------------------------------------");

	int bufferSize = 256;

    lMic.assign(bufferSize, 0.0);
    rMic.assign(bufferSize, 0.0);
	volHistory.assign(400, 0.0);

    lAudio.assign(bufferSize, 0.0);
    rAudio.assign(bufferSize, 0.0);

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
	if(!devices.empty()){
		///*
        // Mac内の音声を取得する(Black Hole)
        settings.setInDevice(devices[2]);
        ofLog(OF_LOG_NOTICE, "deviceID = %d", devices[2].deviceID);
        ofLog(OF_LOG_NOTICE,  devices[2].name);
        //*/
        /*
        // マイク入力
        settings.setInDevice(devices[0]);
        ofLog(OF_LOG_NOTICE, "deviceID = %d", devices[0].deviceID);
        ofLog(OF_LOG_NOTICE,  devices[0].name);
        */
        // Audio
        settings.setOutDevice(devices[1]);
        ofLog(OF_LOG_NOTICE, "deviceID = %d", devices[1].deviceID);
        ofLog(OF_LOG_NOTICE,  devices[1].name);
    }

    settings.setInListener(this);  // Mic
    settings.setOutListener(this); // Audio
	settings.sampleRate = 44100;
//	settings.numOutputChannels = 0;
    settings.numOutputChannels = 2;
	settings.numInputChannels = 2;
	settings.bufferSize = bufferSize;
	soundStream.setup(settings);

    /*
    gui.setup(); // most of the time you don't need a name
    // initial,,max
    
    gui.add(volume.setup("volume", 1, 0.1, 3));
*/
     /*
    gui.add(center.setup("center", {ofGetWidth()*.5, ofGetHeight()*.5}, {0, 0}, {ofGetWidth(), ofGetHeight()}));
    gui.add(color.setup("color", ofColor(100, 100, 140), ofColor(0, 0), ofColor(255, 255)));
    gui.add(circleResolution.setup("circle res", 5, 3, 90));
    gui.add(twoCircles.setup("two circles"));
    gui.add(ringButton.setup("ring"));
    gui.add(screenSize.setup("screen size", ofToString(ofGetWidth())+"x"+ofToString(ofGetHeight())));
*/

  //  bHide = false;
    // instantiate and position the gui //
    gui = new ofxDatGui( ofxDatGuiAnchor::TOP_RIGHT );
    
// add some components //
    gui->addTextInput("message", "# open frameworks #");
    
    gui->addFRM();
    gui->addBreak();
    
// add a folder to group a few components together //
    ofxDatGuiFolder* folder = gui->addFolder("white folder", ofColor::white);
    folder->addTextInput("** input", "nested input field");
    folder->addSlider("** slider", 0, 100);
    folder->addToggle("** toggle");
    folder->addColorPicker("** picker", ofColor::fromHex(0xFFD00B));
// let's have it open by default. note: call this only after you're done adding items //
    folder->expand();

    gui->addBreak();
        
    // add a couple range sliders //
        
        gui->addSlider("position X", 0, 120, 75);
        gui->addSlider("position Y", -40, 240, 200);
        gui->addSlider("position Z", -80, 120, -40);
        
    // and a slider to adjust the gui opacity //
        gui->addSlider("datgui opacity", 0, 100, 100);
        
    // and a colorpicker //
        gui->addColorPicker("color picker", ofColor::fromHex(0xeeeeee));
        
    // add a wave monitor //
    // take a look inside example-TimeGraph for more examples of this component and the value plotter //
        gui->addWaveMonitor("wave\nmonitor", 3, .2);
        
        gui->addBreak();
        
    // add a dropdown menu //
        vector<string> opts = {"option - 1", "option - 2", "option - 3", "option - 4"};
        gui->addDropdown("select option", opts);
        gui->addBreak();

    // add a 2d pad //
        ofxDatGui2dPad* pad = gui->add2dPad("2d pad");

    // a button matrix //
        gui->addMatrix("matrix", 21, true);

    // and a couple of simple buttons //
        gui->addButton("click");
        gui->addToggle("toggle fullscreen", true);

    // adding the optional header allows you to drag the gui around //
        gui->addHeader(":: drag me to reposition ::");

    // adding the optional footer allows you to collapse/expand the gui //
        gui->addFooter();
        
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
    //  gui->setLabelAlignment(ofxDatGuiAlignment::CENTER);

    // finally let's load up the stock themes, press spacebar to cycle through them //
        themes = {  new ofxDatGuiTheme(true),
            new ofxDatGuiThemeSmoke(),
            new ofxDatGuiThemeWireframe(),
            new ofxDatGuiThemeMidnight(),
            new ofxDatGuiThemeAqua(),
            new ofxDatGuiThemeCharcoal(),
            new ofxDatGuiThemeAutumn(),
            new ofxDatGuiThemeCandy()};
        tIndex = 0;
        
    // launch the app //
        mFullscreen = false;
        refreshWindow();
}

//--------------------------------------------------------------
void ofApp::update(){
	//lets scale the vol up to a 0-1 range 
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);

	//lets record the volume into an array
	volHistory.push_back( scaledVol );
	
	//if we are bigger the the size we want to record - lets drop the oldest value
	if( volHistory.size() >= 400 ){
		volHistory.erase(volHistory.begin(), volHistory.begin()+1);
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	int gWidth = 300; // Width of graph
    float scale = 100.0f;
	ofSetColor(225);
	ofDrawBitmapString("AUDIO INPUT EXAMPLE", 32, 42);
    //string reportString = "";
    string reportString = "volume: (" + ofToString((float)volume,2) + ") modify with -/+";
    ofDrawBitmapString(reportString, 32, 82);
    ofDrawBitmapString("press 's' to unpause the audio\n'e' to pause the audio", 32, 94);
    
    
	ofNoFill();
	
	// draw the left Mic:
	ofPushStyle();
		ofPushMatrix();
		//ofTranslate(32, 170, 0);
        ofTranslate(32, 150, 0); // 座標の原点を変更する

		ofSetColor(225);
		ofDrawBitmapString("Left Input", 4, 18);
		
		ofSetLineWidth(1);	
		ofDrawRectangle(0, 0, gWidth, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
					
			ofBeginShape();
			for (unsigned int i = 0; i < lMic.size(); i++){
                //ofVertex(i*2, 100 -lMic[i]*180.0f);
                ofVertex(i, 100 -lMic[i]*scale);
            }
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();

	// draw the right Mic:
	ofPushStyle();
		ofPushMatrix();
		//ofTranslate(32, 370, 0);
        ofTranslate(32+gWidth, 150, 0); // 座標の原点を変更する

		ofSetColor(225);
		ofDrawBitmapString("Right Input", 4, 18);
		
		ofSetLineWidth(1);	
		//ofDrawRectangle(0, 0, 512, 200);
        ofDrawRectangle(0, 0, gWidth, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
					
			ofBeginShape();
			for (unsigned int i = 0; i < rMic.size(); i++){
                //ofVertex(i*2, 100 -rMic[i]*180.0f);
                ofVertex(i, 100 -rMic[i]*scale);
            }
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();
    ///////////////////////////////////////////////////////////////////////////////////////////
    // draw the left Audio:
    ofPushStyle();
        ofPushMatrix();
        //ofTranslate(32, 170, 0);
        ofTranslate(32, 350, 0); // 座標の原点を変更する

        ofSetColor(225);
        ofDrawBitmapString("Left Output", 4, 18);
        
        ofSetLineWidth(1);
        ofDrawRectangle(0, 0, gWidth, 200);

        ofSetColor(245, 58, 135);
        ofSetLineWidth(3);
                    
            ofBeginShape();
            for (unsigned int i = 0; i < lAudio.size(); i++){
                //ofVertex(i*2, 100 -lMic[i]*180.0f);
                ofVertex(i, 100 -lAudio[i]*scale);
            }
            ofEndShape(false);
            
        ofPopMatrix();
    ofPopStyle();

    // draw the right Mic:
    ofPushStyle();
        ofPushMatrix();
        //ofTranslate(32, 370, 0);
        ofTranslate(32+gWidth, 350, 0); // 座標の原点を変更する

        ofSetColor(225);
        ofDrawBitmapString("Right Output", 4, 18);
        
        ofSetLineWidth(1);
        //ofDrawRectangle(0, 0, 512, 200);
        ofDrawRectangle(0, 0, gWidth, 200);

        ofSetColor(245, 58, 135);
        ofSetLineWidth(3);
                    
            ofBeginShape();
            for (unsigned int i = 0; i < rAudio.size(); i++){
                //ofVertex(i*2, 100 -rMic[i]*180.0f);
                ofVertex(i, 100 -rAudio[i]*scale);
            }
            ofEndShape(false);
            
        ofPopMatrix();
    ofPopStyle();
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    /*
    // draw the Left canceled Audio:
    ofPushStyle();
        ofPushMatrix();
        //ofTranslate(32, 170, 0);
        ofTranslate(32, 550, 0); // 座標の原点を変更する

        ofSetColor(225);
        ofDrawBitmapString("Left Canceled Audio", 4, 18);
        
        ofSetLineWidth(1);
        ofDrawRectangle(0, 0, gWidth, 200);

        ofSetColor(245, 58, 135);
        ofSetLineWidth(3);
                    
            ofBeginShape();
            for (unsigned int i = 0; i < lAudio.size(); i++){
                //ofVertex(i*2, 100 -lMic[i]*180.0f);
                ofVertex(i, 100 -(lMic[i] + lAudio[i])*scale);
            }
            ofEndShape(false);
            
        ofPopMatrix();
    ofPopStyle();

    // draw the right Mic:
    ofPushStyle();
        ofPushMatrix();
        //ofTranslate(32, 370, 0);
        ofTranslate(32+gWidth, 550, 0); // 座標の原点を変更する

        ofSetColor(225);
        ofDrawBitmapString("Right Canceled Audio", 4, 18);
        
        ofSetLineWidth(1);
        //ofDrawRectangle(0, 0, 512, 200);
        ofDrawRectangle(0, 0, gWidth, 200);

        ofSetColor(245, 58, 135);
        ofSetLineWidth(3);
                    
            ofBeginShape();
            for (unsigned int i = 0; i < rAudio.size(); i++){
                //ofVertex(i*2, 100 -rMic[i]*180.0f);
                ofVertex(i, 100 - (rMic[i] + rAudio[i])*scale);
            }
            ofEndShape(false);
            
        ofPopMatrix();
    ofPopStyle();
    */
    
    
	drawCounter++;
	
	ofSetColor(225);
	reportString = "buffers received: "+ofToString(bufferCounter)+"\ndraw routines called: "+ofToString(drawCounter)+"\nticks: " + ofToString(soundStream.getTickCount());
	ofDrawBitmapString(reportString, 300, 82);
		
    // auto draw?
    // should the gui control hiding?
    /*
    if(!bHide){
        gui.draw();
    }
     */
}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer & input){
    //ofLog(OF_LOG_NOTICE, "audioIn called");
    float amp_in = 30;
    float clip_in = 1.0;
    float clip_out = 0.2;
    float amp = 1.0;
    
	float curVol = 0.0;
	
	// samples are "interleaved"
	int numCounted = 0;	

	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume	
	for (size_t i = 0; i < input.getNumFrames(); i++){
        
        lMic[i]	= input[i*2]*amp_in;   // 偶数番目に左の音
        if(lMic[i] > clip_in){
            lMic[i] = clip_in;
        }
        if(lMic[i] < -clip_in){
            lMic[i] = -clip_in;
        }
        rMic[i]	= input[i*2+1]*amp_in; // 奇数番目に右の音
        if(rMic[i] > clip_in){
            rMic[i] = clip_in;
        }
        if(rMic[i] < -clip_in){
            rMic[i] = -clip_in;
        }
        /*
        if(lMic[i]>0.3){
            ofLog(OF_LOG_NOTICE, "lMic[i] = %f",lMic[i]);
            ofLog(OF_LOG_NOTICE, "rMic[i] = %f",rMic[i]);
        }
        */
        //-------------- output
        lAudio[i] = -lMic[i]; //
        if(lAudio[i] > clip_out){
            lAudio[i] = clip_out;
        }
        if(lAudio[i] < -clip_out){
            lAudio[i] = -clip_out;
        }
        
        rAudio[i] = -rMic[i];
        if(rAudio[i] > clip_out){
            rAudio[i] = clip_out;
        }
        if(rAudio[i] < -clip_out){
            rAudio[i] = -clip_out;
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
        buffer[i * buffer.getNumChannels()] = lAudio[i];
        buffer[i * buffer.getNumChannels() + 1] = rAudio[i];

        //buffer[i * buffer.getNumChannels()] = lAudio[i] * volume;
        //buffer[i * buffer.getNumChannels() + 1] = rAudio[i] * volume;

        // buffer.getNumChannels() : 2
        //lAudio[i] = buffer[i * buffer.getNumChannels()] = - lMic[i];
        //rAudio[i] = buffer[i * buffer.getNumChannels() + 1] = - rMic[i];
       // ofLog(OF_LOG_NOTICE, "lAudio[i] = %f",lAudio[i]);
       // ofLog(OF_LOG_NOTICE, "rAudio[i] = %f",rAudio[i]);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
    float tmp = 0.0;
    if (key == '-' || key == '_')
    {
        volume = volume - 0.05;
        //tmp = volume;
        volume = MAX(volume, 0);
        //volume = MAX(tmp, 0.0);
    }
    else if (key == '+' || key == '=')
    {
        volume = volume + 0.05;
        //tmp = volume;
        volume = MIN(volume, 3);
        //volume = MIN(tmp, 3.0);
    }
    
    if( key == 's' ){
		soundStream.start();
	}
	
	if( key == 'e' ){
		soundStream.stop();
	}
    if (key == 'f') {
        toggleFullscreen();
    }   else if (key == 32){
        tIndex = tIndex < themes.size()-1 ? tIndex+1 : 0;
        gui->setTheme(themes[tIndex]);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){ 
	
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

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
    if (!mFullscreen) {
        int width = ofGetScreenWidth() * .8;
        int height = ofGetScreenHeight() * .8;
        ofSetWindowShape(width, height);
        ofSetWindowPosition((ofGetScreenWidth()/2)-(width/2), 0);
    }
}


