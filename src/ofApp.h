#pragma once

#include "ofMain.h"
#include "ofxOpenNI.h" // addonを読み込み
#include "ofxDelaunay.h"

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
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
	
private:
    ofxOpenNI kinect; // インスタンス
    ofxDelaunay delaunay;
    ofVec3f pastVec[4];
    ofVec3f mPastVec;
    float strength[3];
    //vector <ofVec3f> point;
    vector <ofVec3f> tip;//線の先端
    vector <ofVec3f> end;//線の後端
    vector <ofVec3f> direction;//発射する線の方向ベクトル
    
    ofImage myImage;
};
