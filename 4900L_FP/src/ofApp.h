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

		// particle data
		std::vector<glm::vec3> pPos; // particle positions
		std::vector<glm::vec3> pNrm; // particle normals
		std::vector<uint8_t> pState; // 0 = solid, 1 = surface

		// mesh for rendering
		ofVboMesh mesh;

		// create grid
		ofParameter<float> cellSize = 8.0f;
		glm::vec3 minBounds, maxBounds;
		glm::ivec3 dim;
		std::vector<std::vector<int>> buckets;

		// helpers to map to grid
		inline glm::ivec3 getCell(const glm::vec3& p) const {
			glm::vec3 relativePos = (p - minBounds) / cellSize.get();
			return glm::ivec3(floor(relativePos.x), floor(relativePos.y), floor(relativePos.z));
		}
		inline int getCellIndex(const glm::ivec3& cell) const {
			return (cell.x * dim.y + cell.y) * dim.z + cell.z;
		}


		// other functions
		void regenCloud();
		void rebuildGrid();
		void tagSurfaceParticles();

		template <class F>
		void forNeighbours(int i, F&& fn);

		int computeThreshold();


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
		ofEventListener sizeListenerW, sizeListenerD, sizeListenerH, countListener, cellSizeListener;

		// debug

		

		size_t vboVertexCount = 0;

		// debugging something right now
		ofParameter<float> neighbourRadiusMul;
		ofParameter<int>   surfaceThreshold;
		ofEventListener radiusMulListener, thresholdListener;

		ofParameter<float> computedThr;

		ofParameter<float> beta;

};
