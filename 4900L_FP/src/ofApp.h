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

		// camera
		ofEasyCam cam;

		// particle data
		std::vector<glm::vec3> pPos;   // positions
		std::vector<glm::vec3> pNrm;   // normals
		std::vector<uint8_t>   pState; // 0 = interior, 1 = surface

		ofVboMesh mesh;

		// grid/buckets
		ofParameter<float> cellSize = 3.0f;   // grid cell size
		glm::vec3 minBounds, maxBounds;
		glm::ivec3 dim;                // grid dimensions: # cells in x,y,z
		std::vector<std::vector<int>> buckets;

		// model constants
		float protoWidth = 200.0f;    // X size of box
		float protoHeight = 50.0f;     // Y size of box
		float protoDepth = 200.0f;    // Z size of box

		float particleDensity = 0.5f;

		float pointSize = 5.0f;
		bool showAxes = false;

		glm::ivec3 getCell(const glm::vec3& p) const;
		int getCellIndex(const glm::ivec3& c) const;

		void initSimulation(); // build particles, grid and mark surface
		void rebuildGrid(); // fill buckets from pPos
		void detectSurface(); // detect if we're on a surface
};
