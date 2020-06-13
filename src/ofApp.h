#pragma once

#include "ofMain.h"
#include "ofxGui.h"  // ofxPanel
#include "ofxDatGui.h"

class ofApp : public ofBaseApp
{

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

    vector <float> lMic;  // Left Mic
    vector <float> rMic;  // Right Mic
    vector <float> lMicCliped;  // Left Mic
    vector <float> rMicCliped;  // Right Mic

    vector <float> volHistory;
    int lMicSize;
    int rMicSize;
    
    int bufferCounter;
    int drawCounter;

    float smoothedVol;
    float scaledVol;

    ofSoundStream soundStream;

    /* Speaker */
    vector <float> lAudio;
    vector <float> rAudio;
    int lAudioSize;
    int rAudioSize;
    //--------- grpah
    int gWidth;  // Width of graph
    int gHeight;
    int gHeightCenter;
    //--------- ofxPanel
    ofxPanel guiPanel;
    ofxFloatSlider inputGrpahScale;
    ofxFloatSlider outputGrpahScale;
    ofxFloatSlider volume;
    ofxFloatSlider gain;
    ofxFloatSlider distortion;
    
    // float volume;
    // float distortion;
    bool bHide;
    //--------- ofxDataGui
    ofxDatGui* gui;
    bool mFullscreen;
    void refreshWindow();
    void toggleFullscreen();

    void onButtonEvent(ofxDatGuiButtonEvent e);
    void onToggleEvent(ofxDatGuiToggleEvent e);
    void onSliderEvent(ofxDatGuiSliderEvent e);
    void onTextInputEvent(ofxDatGuiTextInputEvent e);
    void on2dPadEvent(ofxDatGui2dPadEvent e);
    void onDropdownEvent(ofxDatGuiDropdownEvent e);
    void onColorPickerEvent(ofxDatGuiColorPickerEvent e);
    void onMatrixEvent(ofxDatGuiMatrixEvent e);

    uint tIndex;
    vector<ofxDatGuiTheme*> themes;
};
