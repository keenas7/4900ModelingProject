#pragma once

#include "ofMain.h"
#include "ofxGui.h"

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



		// basic scene setup
		ofEasyCam cam;
		ofVbo vbo;
		std::vector<glm::vec3> positions;

		ofParameter<ofColor> bgColor;

		//debug
		ofParameter<bool> showAxes;

		// gui stuff
		ofxPanel gui;
		bool showGui = true;

		// prototype structure setup
		ofParameter<float> protoWidth;
		ofParameter<float> protoDepth;
		ofParameter<float> protoHeight;

		// particle setup
		ofParameter<float> pointSize;
		ofParameter<float> particleRadius;
		ofParameter<int> particleCount;

		// listeners for sliders to regen particles
		ofEventListener sizeListenerW, sizeListenerD, sizeListenerH, countListener;

		void regenCloud();

		size_t vboVertexCount = 0;
		

		

};
