#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{
	
	public:
		
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
				
		void audioIn(ofSoundBuffer & input);
        void audioOut(ofSoundBuffer & buffer);
    
        /*
         vector <float> left;
         vector <float> right;
        */
		vector <float> lMic;  // Left Mic
		vector <float> rMic; // Right Mic
		vector <float> volHistory;
		
		int 	bufferCounter;
		int 	drawCounter;
		
		float smoothedVol;
		float scaledVol;
		
		ofSoundStream soundStream;
    
        /* Speaker */
        vector <float> lAudio;
        vector <float> rAudio;
        float     volume;

};
