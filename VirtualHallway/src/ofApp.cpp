#include "ofApp.h"

#define SIMULATION_UPDATE_PERIOD 250 //Once every second

#define SIMULATION_MODE_FREE 1 //Let person explore by themselves
#define SIMULATION_MODE_FREE_SILENT 2 //Let person explore by themselves in the dark
#define SIMULATION_MODE_DISCRETE 3 //
#define SIMULATION_MODE_DONE 4 //
#define SIMULATION_DISCRETE_PERIOD 8000 //Let person explore by themselves

#define SIMULATION_SOUND_PERIOD 2000
#define SIMULATION_SOUND_NOTE 263.3

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofBackground(34, 34, 34);
    
    // 2 output channels,
    // 0 input channels
    // 22050 samples per second
    // 512 samples per buffer
    // 4 num buffers (latency)
    
    int bufferSize          = 512;
    sampleRate              = 44100;
    
    phaseLeft 				= 0;
    phaseRight 				= 0;
    
    phaseAdderLeft 			= 0.0f;
    phaseAdderRight 		= 0.0f;
    
    phaseAdderTargetLeft 	= 0.0f;
    phaseAdderTargetRight 	= 0.0f;
    
    volumeLeft				= 0.1f;
    volumeRight				= 0.1f;
    
    bNoise                  = false;
    
    periodRight  = 500;
    periodLeft = 500;
    
    lAudio.assign(bufferSize, 0.0);
    rAudio.assign(bufferSize, 0.0);
    
    soundStream.printDeviceList();
    

    
    
    playerPositionMaximum = 10;
    worldPositionMaximum = 2*playerPositionMaximum; //+1
    //Come up with the sounds:
    for (int i=0; i <worldPositionMaximum; ++i) {
        leftFrequencies.push_back(440 + 10.*(i+1));
    }
    
    for (int i=0; i <worldPositionMaximum; ++i) {
        rightFrequencies.push_back(440 + 10.*(i+1));
    }

    centerPosition = playerPositionMaximum;
    
    playerPositionActual = centerPosition;
    
    update_time.push_back(ofGetElapsedTimeMillis());
    lastPlayedRight = update_time.back();
    lastPlayedLeft = update_time.back();
    
    frequencyTargetLeft = SIMULATION_SOUND_NOTE;
    frequencyTargetRight = 392.; //SIMULATION_SOUND_NOTE;
    
    
    soundStream.setup(this, 2, 0, sampleRate, bufferSize, 4);
    
    phaseAdderTargetLeft = (frequencyTargetLeft / (float) sampleRate) * TWO_PI;
    phaseAdderTargetRight = (frequencyTargetRight / (float) sampleRate) * TWO_PI;
    
    ofSetFrameRate(60);
    
    applicationMode = SIMULATION_MODE_FREE;

    
    ofSetLogLevel(OF_LOG_VERBOSE);

    dataToLog.append("Time (seconds), Player Position, Offset \n");
    
    lastSimulationUpdate = ofGetElapsedTimeMillis();
    
    simulationDiscreteSteps.push_back(0+10); //    playerPositionMaximum = 10;
    simulationDiscreteSteps.push_back(8+10); //    playerPositionMaximum = 10;
    simulationDiscreteSteps.push_back(-5+10); //    playerPositionMaximum = 10;
    simulationDiscreteSteps.push_back(-10+10); //    playerPositionMaximum = 10;
    simulationDiscreteSteps.push_back(3+10); //    playerPositionMaximum = 10;
    simulationDiscreteSteps.push_back(6+10); //    playerPositionMaximum = 10;
    simulationDiscreteSteps.push_back(-2+10); //    playerPositionMaximum = 10;
    simulationDiscreteSteps.push_back(1+10); //    playerPositionMaximum = 10;
    simulationDiscreteSteps.push_back(5+10); //    playerPositionMaximum = 10;
    simulationDiscreteSteps.push_back(-9+10); //    playerPositionMaximum = 10;
    simulationPtr = 0;
}


//--------------------------------------------------------------
void ofApp::update(){
    
    
    if (ofGetElapsedTimeMillis() - (lastPlayedRight)   >periodRight){
        volumeRight = ofLerp(volumeRight, 1, 0.8); // jump quickly to 1
        lastPlayedRight = ofGetElapsedTimeMillis();
    }
    else{
        volumeRight = ofLerp(volumeRight, 0, 0.1); // fade slowly to 0
    }
    //Check if the left source (@ ?? Hz) needs to be played
    if(ofGetElapsedTimeMillis() - lastPlayedLeft  >periodLeft){
        volumeLeft = ofLerp(volumeLeft, 1, 0.8); // jump quickly to 1
        lastPlayedLeft = ofGetElapsedTimeMillis();
    }
    else{
        volumeLeft = ofLerp(volumeLeft, 0, 0.1); // fade slowly to 0
    }
    
    
    
    if (ofGetElapsedTimeMillis() - update_time.back()>SIMULATION_UPDATE_PERIOD){
        
        
        int distance_from_right = worldPositionMaximum - playerPositionActual;
        int distance_from_left = playerPositionActual;
        
        //cout << "Distance from Right: " << distance_from_right << ", From Left: " << distance_from_left << "\n";
        
        periodRight = ofMap(distance_from_right, 0, worldPositionMaximum-1, 10, SIMULATION_SOUND_PERIOD);
        periodLeft = ofMap(distance_from_left, 0, worldPositionMaximum-1, 10, SIMULATION_SOUND_PERIOD);
        
//        frequencyTargetLeft = SIMULATION_SOUND_NOTE;
//        frequencyTargetRight = SIMULATION_SOUND_NOTE;
//        
//        //Quick hack to make the synch up the beats line up
//        if ((centerPosition-playerPositionActual == 0) && needsSynch){
//            lastPlayedRight = 0; // Say some long time ago to force update
//            lastPlayedLeft =lastPlayedRight;
//            needsSynch = false;
//            cout << "Just synched\n";
//        }
//        
//        if (centerPosition-playerPositionActual != 0){
//            cout << "Yes we need synch\n";
//            needsSynch = true;
//        }
        
        //Update the player world state
//        update_time.push_back(ofGetElapsedTimeMillis());
    }
    
    if (applicationMode == SIMULATION_MODE_DISCRETE
        && ofGetElapsedTimeMillis() - lastSimulationUpdate>SIMULATION_DISCRETE_PERIOD)
    {
        lastSimulationUpdate = ofGetElapsedTimeMillis();
        
        simulationPtr +=1;
        if (simulationPtr > simulationDiscreteSteps.size()-1){
            applicationMode = SIMULATION_MODE_FREE;
            simulationPtr--;
        }
        playerPositionActual = simulationDiscreteSteps[simulationPtr];
        cout << "DISCRETE " << simulationPtr << "\n";
    }
    
    dataToLog.append("" + ofToString(ofGetElapsedTimeMillis()*1.0/1000) + ", " + ofToString(playerPositionActual)
                     + "," + ofToString(simulationDiscreteSteps[simulationPtr]) + "\n");
//    logFile <<  << ",";
//    logFile << playerPositionActual << ",";
//    logFile << 0 << "\n";
   
//    playerPositionHistory.push_back(5);
//    artificialOffsetHistory.push_back(0);
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetColor(225);
    if (applicationMode == SIMULATION_MODE_FREE){
        ofDrawBitmapString("VISUAL TO AUDIO PROSHETIC TEST - FREE RANGE MODE", 32, 32);
        ofDrawBitmapString("Press Left and Right to navigate in the virtual hallway\nPress '1' to stop the visual cues", 31, 92);

    }
    else if (applicationMode == SIMULATION_MODE_FREE_SILENT)
    {
        ofDrawBitmapString("VISUAL TO AUDIO PROSHETIC TEST - FREE RANGE MODE DARK", 32, 32);
        ofDrawBitmapString("Navigate freely to enter\nPress '2' when you're done", 31, 92);
    }
    
    else {
        ofDrawBitmapString("VISUAL TO AUDIO PROSHETIC TEST - EXPERIMENT\n", 32, 32);
        ofDrawBitmapString("When you hear your position change, navigate back to center\n", 31, 92);
        ofDrawBitmapString("Cue " + ofToString(simulationPtr ) + "/ 9", 31, 92);
    }
    
    ofNoFill();
    
    // draw the left channel:
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(32, 150, 0);
    
    ofSetColor(225);
    ofDrawBitmapString("Left Channel", 4, 18);
    
    ofSetLineWidth(1);
    ofDrawRectangle(0, 0, 900, 200);
    
    ofSetColor(245, 58, 135);
    ofSetLineWidth(3);
    
    ofBeginShape();
    if (applicationMode == SIMULATION_MODE_FREE ){
        for (unsigned int i = 0; i < lAudio.size(); i++){
            float x =  ofMap(i, 0, lAudio.size(), 0, 900, true);
            ofVertex(x, 100 -lAudio[i]*180.0f);
        }
    }
    ofEndShape(false);
    
    ofPopMatrix();
    ofPopStyle();
    
    // draw the right channel:
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(32, 350, 0);
    
    ofSetColor(225);
    ofDrawBitmapString("Right Channel", 4, 18);
    
    ofSetLineWidth(1);
    ofDrawRectangle(0, 0, 900, 200);
    
    ofSetColor(245, 58, 135);
    ofSetLineWidth(3);
    
    ofBeginShape();
        if (applicationMode == SIMULATION_MODE_FREE){
            for (unsigned int i = 0; i < rAudio.size(); i++){
                float x =  ofMap(i, 0, rAudio.size(), 0, 900, true);
                ofVertex(x, 100 -rAudio[i]*180.0f);
            }
        }
    ofEndShape(false);
    
    ofPopMatrix();
    ofPopStyle();
    
    

    // In Free mode, give the user a visual sense of what's going on
    
    float xPos = ofMap(playerPositionActual, 0, worldPositionMaximum, 32+15, 932-15);;
    ofSetColor(225);
    if (applicationMode == SIMULATION_MODE_FREE){
        ofDrawLine(32+900/2, 550, 32+900/2, 650);
        ofSetColor(245, 58, 135);
    
        ofDrawRectangle(xPos-15, 600, 30, 30);
    }

}


//--------------------------------------------------------------
void ofApp::keyPressed  (int key){

    if(key == '1') {
        cout << "Changed modes";
        applicationMode = SIMULATION_MODE_FREE_SILENT;
    }
    
    if(key == '2') {
        cout << "Changed modes";
        applicationMode = SIMULATION_MODE_DISCRETE;
    }
    
    if(key == OF_KEY_ESC) {
        cout << "TRYING TO SAVE FILE\n";

        ostringstream filename;
        time_t seconds = time(NULL);
        filename << "outputfile_timestamp_" << seconds << ".txt";
        
        cout << "Filename: " << filename.str().c_str() <<"\n";

//        dataToLog.append("write some shite\n");
//        dataToLog.append(ofToString(ofGetSeconds()));
        string fileName = "test" + ofToString(time(NULL)) + ".txt";
        bool fileWritten = ofBufferToFile(fileName, dataToLog);

        
    }

    
    if(key == OF_KEY_LEFT) {
        playerPositionActual--;
        if (playerPositionActual < 0)
            playerPositionActual = 0;
    }
    if(key == OF_KEY_RIGHT) {
        playerPositionActual++;
        if (playerPositionActual > worldPositionMaximum)
            playerPositionActual = worldPositionMaximum;
    }
    phaseAdderTargetLeft = (frequencyTargetLeft / (float) sampleRate) * TWO_PI;
    phaseAdderTargetRight = (frequencyTargetRight / (float) sampleRate) * TWO_PI;
    
}

//--------------------------------------------------------------
void ofApp::keyReleased  (int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    int width = ofGetWidth();
    pan = (float)x / (float)width;
    float height = (float)ofGetHeight();
    float heightPct = ((height-y) / height);
    //    targetFrequency = 2000.0f * heightPct;
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    int width = ofGetWidth();
    pan = (float)x / (float)width;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    bNoise = true;
}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    bNoise = false;
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
void ofApp::audioOut(float * output, int bufferSize, int nChannels){
    
    
    // sin (n) seems to have trouble when n is very large, so we
    // keep phase in the range of 0-TWO_PI like this:
    while (phaseLeft > TWO_PI){
        phaseLeft -= TWO_PI;
    }
    while (phaseRight > TWO_PI){
        phaseRight -= TWO_PI;
    }
    
    
    phaseAdderLeft = 0.95f * phaseAdderLeft + 0.05f * phaseAdderTargetLeft;
    phaseAdderRight = 0.95f * phaseAdderRight + 0.05f * phaseAdderTargetRight;
    
    for (int i = 0; i < bufferSize; i++){
        phaseLeft += phaseAdderLeft;
        phaseRight += phaseAdderRight;
        
        float sampleLeft = sin(phaseLeft);
        float sampleRight = sin(phaseRight);
        lAudio[i] = output[i*nChannels    ] = sampleLeft * volumeLeft ;
        rAudio[i] = output[i*nChannels + 1] = sampleRight * volumeRight ;
    }
    
    
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
