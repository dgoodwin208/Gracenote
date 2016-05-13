#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
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
		
		void audioOut(float * input, int bufferSize, int nChannels);
		
		

    
		ofSoundStream soundStream;

		float 	pan;
		int		sampleRate;
		bool 	bNoise;
		

		vector <float> lAudio;
		vector <float> rAudio;
		
		//------------------- for the simple sine wave synthesis
        float frequencyTargetLeft;
        float frequencyTargetRight;
	
        float 	volumeLeft;
        float 	volumeRight;
    
        float 	phaseLeft;
		float 	phaseRight;
    
        float 	phaseAdderLeft;
        float   phaseAdderRight;
    
		float 	phaseAdderTargetLeft;
        float 	phaseAdderTargetRight;
    
        float   periodRight;
        float   periodLeft;
    
        double  lastPlayedLeft;
        double  lastPlayedRight;

    
        //player state variables:
//        void mapSound(int leftOrRight,int distance);
    
        int applicationMode;
        vector <float> leftFrequencies;
        vector <float> rightFrequencies;
   
        int centerPosition;
        int playerPositionMaximum; //Say 0 is center, +/- Maximum
        int worldPositionMaximum;
        int playerPositionPerceived; //start in the middle
        int playerPositionActual; //start in the middle
    
        bool needsSynch;
        vector <int> playerPositionHistory;
        vector <int> artificialOffsetHistory;
        vector <double> update_time;
    
        double lastSimulationUpdate;
        vector <int> simulationDiscreteSteps;
        int simulationPtr;
        ofBuffer dataToLog;
};
