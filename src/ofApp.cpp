#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    volume = 0.1f;
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
		/*
        // Mac内の音声を取得する(Black Hole)
        settings.setInDevice(devices[2]);
        ofLog(OF_LOG_NOTICE, "deviceID = %d", devices[2].deviceID);
        ofLog(OF_LOG_NOTICE,  devices[2].name);
        */
        ///*
        // マイク入力
        settings.setInDevice(devices[0]);
        ofLog(OF_LOG_NOTICE, "deviceID = %d", devices[0].deviceID);
        ofLog(OF_LOG_NOTICE,  devices[0].name);
        //*/
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
    
	ofSetColor(225);
	ofDrawBitmapString("AUDIO INPUT EXAMPLE", 32, 42);
	string reportString = "volume: (" + ofToString(volume, 2) + ") modify with -/+";
    ofDrawBitmapString(reportString, 32, 82);
    ofDrawBitmapString("press 's' to unpause the audio\n'e' to pause the audio", 32, 92);
    
    
	ofNoFill();
	
	// draw the left Mic:
	ofPushStyle();
		ofPushMatrix();
		//ofTranslate(32, 170, 0);
        ofTranslate(32, 150, 0); // 座標の原点を変更する

		ofSetColor(225);
		ofDrawBitmapString("Left Mic", 4, 18);
		
		ofSetLineWidth(1);	
		ofDrawRectangle(0, 0, gWidth, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
					
			ofBeginShape();
			for (unsigned int i = 0; i < lMic.size(); i++){
                //ofVertex(i*2, 100 -lMic[i]*180.0f);
                ofVertex(i, 100 -lMic[i]*180.0f);
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
		ofDrawBitmapString("Right Mic", 4, 18);
		
		ofSetLineWidth(1);	
		//ofDrawRectangle(0, 0, 512, 200);
        ofDrawRectangle(0, 0, gWidth, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
					
			ofBeginShape();
			for (unsigned int i = 0; i < rMic.size(); i++){
                //ofVertex(i*2, 100 -rMic[i]*180.0f);
                ofVertex(i, 100 -rMic[i]*180.0f);
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
        ofDrawBitmapString("Left Audio", 4, 18);
        
        ofSetLineWidth(1);
        ofDrawRectangle(0, 0, gWidth, 200);

        ofSetColor(245, 58, 135);
        ofSetLineWidth(3);
                    
            ofBeginShape();
            for (unsigned int i = 0; i < lAudio.size(); i++){
                //ofVertex(i*2, 100 -lMic[i]*180.0f);
                ofVertex(i, 100 -lAudio[i]*180.0f);
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
        ofDrawBitmapString("Right Audio", 4, 18);
        
        ofSetLineWidth(1);
        //ofDrawRectangle(0, 0, 512, 200);
        ofDrawRectangle(0, 0, gWidth, 200);

        ofSetColor(245, 58, 135);
        ofSetLineWidth(3);
                    
            ofBeginShape();
            for (unsigned int i = 0; i < rAudio.size(); i++){
                //ofVertex(i*2, 100 -rMic[i]*180.0f);
                ofVertex(i, 100 -rAudio[i]*180.0f);
            }
            ofEndShape(false);
            
        ofPopMatrix();
    ofPopStyle();
    
    ///////////////////////////////////////////////////////////////////////////////////////////
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
                ofVertex(i, 100 -(lMic[i] + lAudio[i])*180.0f);
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
                ofVertex(i, 100 - (rMic[i] + rAudio[i])*180.0f);
            }
            ofEndShape(false);
            
        ofPopMatrix();
    ofPopStyle();

    
    
	// draw the average volume:
    /*
    ofPushStyle();
		ofPushMatrix();
		ofTranslate(565, 170, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("Scaled average vol (0-100): " + ofToString(scaledVol * 100.0, 0), 4, 18);
		ofDrawRectangle(0, 0, 400, 400);
		
		ofSetColor(245, 58, 135);
		ofFill();		
		ofDrawCircle(200, 200, scaledVol * 190.0f);
		
		//lets draw the volume history as a graph
		ofBeginShape();
		for (unsigned int i = 0; i < volHistory.size(); i++){
			if( i == 0 ) ofVertex(i, 400);

			ofVertex(i, 400 - volHistory[i] * 70);
			
			if( i == volHistory.size() -1 ) ofVertex(i, 400);
		}
		ofEndShape(false);		
			
		ofPopMatrix();
	ofPopStyle();
	*/
	drawCounter++;
	
	ofSetColor(225);
	reportString = "buffers received: "+ofToString(bufferCounter)+"\ndraw routines called: "+ofToString(drawCounter)+"\nticks: " + ofToString(soundStream.getTickCount());
	ofDrawBitmapString(reportString, 300, 82);
		
}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer & input){
    //ofLog(OF_LOG_NOTICE, "audioIn called");

	float curVol = 0.0;
	
	// samples are "interleaved"
	int numCounted = 0;	

	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume	
	for (size_t i = 0; i < input.getNumFrames(); i++){
		
        lMic[i]	= input[i*2]*0.5;   // 偶数番目に左の音
        if(lMic[i] > 1){
            lMic[i] = 1;
        }
        if(lMic[i] < -1){
            lMic[i] = -1;
        }
        rMic[i]	= input[i*2+1]*0.5; // 奇数番目に右の音
        if(rMic[i] > 1){
            rMic[i] = 1;
        }
        if(rMic[i] < -1){
            rMic[i] = -1;
        }
        lAudio[i] = -lMic[i]; //
        rAudio[i] = -rMic[i];
        
        //ofLog(OF_LOG_NOTICE, "l = %f",lMic[i]);
		//ofLog(OF_LOG_NOTICE, "r = %f",rMic[i]);
		curVol += lMic[i] * lMic[i];
		curVol += rMic[i] * rMic[i];
		numCounted+=2;
        //ofLog(OF_LOG_NOTICE, "lMic[i] = %f",lMic[i]);
        //ofLog(OF_LOG_NOTICE, "rMic[i] = %f",rMic[i]);

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
        //lAudio[i] = buffer[i * buffer.getNumChannels()] = - lMic[i];
        //rAudio[i] = buffer[i * buffer.getNumChannels() + 1] = - rMic[i];
        buffer[i * buffer.getNumChannels()] = lAudio[i] * volume;
        buffer[i * buffer.getNumChannels() + 1] = rAudio[i] * volume;
       // ofLog(OF_LOG_NOTICE, "lAudio[i] = %f",lAudio[i]);
       // ofLog(OF_LOG_NOTICE, "rAudio[i] = %f",rAudio[i]);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
    if (key == '-' || key == '_')
    {
        volume -= 0.05;
        volume = MAX(volume, 0);
    }
    else if (key == '+' || key == '=')
    {
        volume += 0.05;
        volume = MIN(volume, 1);
    }
    
    if( key == 's' ){
		soundStream.start();
	}
	
	if( key == 'e' ){
		soundStream.stop();
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

