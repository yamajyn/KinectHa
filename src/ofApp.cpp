#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // 画面サイズの設定
    ofSetWindowShape(640, 480);
    // Kinectの初期化
    kinect.setup();
    // RGB画像の取得ON
    kinect.addImageGenerator();
    // 距離画像の取得ON
    kinect.addDepthGenerator();
    // 取得した画像の反転
    kinect.setMirror(true);
    // RGB画像と距離画像の位置合わせ
    kinect.setRegister(true);
    
    // 手の検出ON
    kinect.addHandsGenerator();
    kinect.addAllHandFocusGestures();
    
    // 検出する手の数の設定
    kinect.setMaxNumHands(3);
    
    // Kinect開始
    kinect.start();
    
}

//--------------------------------------------------------------
void ofApp::update(){
    // 取得データの更新
    kinect.update();
    
    //線の先端と後端を移動させるアニメーション
    for(int i=0; i<tip.size(); i++){
        //線の先端を移動させる
        tip[i] += direction[i];
        //ベクトル同士の距離がある程度離れたら後端も移動させる
        if(tip[i].distance(end[i]) > 500){
            end[i] += direction[i];
        }
        //画面外に出たら削除
        if(end[i].x < 0 || end[i].x >640 || end[i].y < 0 || end[i].y > 480){
            tip.erase(tip.begin() + i);
            end.erase(end.begin() + i);
            direction.erase(direction.begin() + i);
        }
        
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    delaunay.reset();
    // 描画色の設定
    ofBackground(0, 0, 0);
    ofSetColor(255, 255, 255);
    // 距離画像の描画
    //kinect.drawDepth(0, 0, 640, 480);
    // RGB画像の描画
    //kinect.drawImage(0, 0, 640, 480);
    
    // 検出した手の数を取得
    int numHands = kinect.getNumTrackedHands();
    
    // 手を検出したら
    if(numHands > 0){
        //その数だけ処理を繰り返す
        for(int i = 0; i < numHands; i++){
            
            // 手の番号を取得
            ofxOpenNIHand & hand = kinect.getTrackedHand(i);
            
            // 手の位置を取得
            ofPoint & handPosition = hand.getPosition();
            
            
            ofVec3f d;
            //手の位置を前フレームと比較して、動きの強さを測る
            d = handPosition - pastVec[i];
            strength[i] = d.x * d.x + d.y * d.y + d.z * d.z;
            
            //手の速さがある程度速くなったら、線を作る
            if(strength[i] > 500){
                //線の先端の位置ベクトル保存
                tip.push_back(pastVec[i]);
                //線の後端の位置ベクトル保存
                end.push_back(pastVec[i]);
                //方向ベクトルの保存
                direction.push_back(d);
            }
            
            //次のフレームで使用する手の位置をpastVecに保存しておく
            pastVec[i] = handPosition;
            
            
            
            // 描画色の設定
            ofSetColor(0);
            // 手の位置に円を描画(いらない気はする)
            ofDrawCircle(handPosition.x, handPosition.y, 15);
            
            //printf("%f\n",strength[i]);
        }
    }
    
    // 線の描画色の設定
    //ofSetColor(ofRandom(150, 255), ofRandom(150, 255), ofRandom(150, 255));
    //ofSetLineWidth(ofRandom(1,100));
    
    
    
    /*光る線 中林さんの改変*/
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    
    
    //手を振った数だけ線を書く
    for(int i=0; i<tip.size(); i++){
        ofSetLineWidth(1);
        ofSetColor(255, 255, 180);
        ofDrawLine(tip[i].x, tip[i].y, end[i].x, end[i].y);
        
        int n = ofRandom(1,8);
        ofSetLineWidth(n);
        ofSetColor(ofRandom(50,100), ofRandom(50,180), ofRandom(200,255));
        ofDrawLine(tip[i].x, tip[i].y, end[i].x, end[i].y);
    }
    /*ここまで*/
    /*
    kinect.drawDepth();
    
    int step = 20;
    
    for (int j = 0; j < 640; j += step) {
        for (int i = 0; i < 480; i += step) {
            
            float distance = kinect.getDepthThreshold(<#int index#>)
            
            if (distance > 50 && distance < 1000) {
                ofSetColor(0);
                
                //ofPushMatrix();
                
                //ofTranslate(i - kinect.width, j - kinect.height, distance * -2.0);
                
                //ofDrawCircle(0, 0, 1);
                
                delaunay.addPoint(4*i - kinectD.width, 4*j - kinectD.height, distance* -2.0);
                
                //ofPopMatrix();
            }
        }
    }*/
    
    int step = 15;
    //ofPixels RGBPixels = kinect.getImagePixels();
    ofShortPixels depthRawPixels = kinect.getDepthRawPixels();
    for(int y = 0; y < 480; y += step){
        for(int x = 0; x < 640; x += step){
            int distance = depthRawPixels.getData()[640 * y + x];
            if(distance>1000 &&distance <2000){
                delaunay.addPoint(x, y, -distance);
            }
        }
    }
    ofNoFill();
    ofSetColor(255);
    ofSetLineWidth(0.5);
    delaunay.triangulate();
    ofPushMatrix();
    
    ofScale(4, 4, 1); // 上下と前後を反転
    ofTranslate(-200,-220,0);
    delaunay.draw();
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == ' '){
        myImage.grabScreen(0, 0, 640, 480);
        myImage.save("jaket.png");
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    //マウスの位置を前フレームと比較して、動きの強さを測る
    ofVec3f d;
    d.x = x - mPastVec.x;
    d.y = y - mPastVec.y;
    strength[2] = d.x * d.x + d.y * d.y;
    printf("%f\n",strength[2]);
    //手の速さがある程度速くなったら、線を作る
    if(strength[2] > 500){
        //線の先端の位置ベクトル保存
        tip.push_back(mPastVec);
        //線の後端の位置ベクトル保存
        end.push_back(mPastVec);
        //方向ベクトルの保存
        direction.push_back(d);
    }
    mPastVec.x = x;
    mPastVec.y = y;
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
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
