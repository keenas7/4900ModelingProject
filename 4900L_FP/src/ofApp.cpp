#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofEnableDepthTest();

	// gui setup
	gui.setup("Prototype Box Particle Structure");

	// particle setup
	pointSize.set("Point size (px)", 2.0f, 1.0f, 30.0f);
	particleCount.set("Particle Count", 60000, 1000, 400000);

	// @todo: these values are hardcoded for now
	protoWidth.set("Prototype Width (X)", 200.0f, 20.0f, 300.0f);	
	protoHeight.set("Prototype Height (Y)", 400.0f, 20.0f, 500.0f);
	protoDepth.set("Prototype Depth  (Z)", 200.0f, 20.0f, 1000.0f);

	bgColor.set("Background", ofColor(0, 0, 0));
	showAxes.set("Show Axes", false);

	cellSize.set("Cell Size (spacing)", 8.0f, 1.0f, 50.0f);

	// add gui elements
	gui.add(pointSize);
	gui.add(particleCount);
	gui.add(protoWidth);
	gui.add(protoHeight);
	gui.add(protoDepth);
	gui.add(bgColor);
	gui.add(showAxes);
	gui.add(cellSize);

	// listeners for sliders
	sizeListenerW = protoWidth.newListener([&](float&) { regenCloud(); });
	sizeListenerD = protoHeight.newListener([&](float&) { regenCloud(); });
	sizeListenerH = protoDepth.newListener([&](float&) { regenCloud(); });
	countListener = particleCount.newListener([&](int&) { regenCloud(); });
	cellSizeListener = cellSize.newListener([&](float&) { regenCloud(); });

	// setup mesh
	mesh.setMode(OF_PRIMITIVE_POINTS);

	// generate the point cloud
	regenCloud();


}

//--------------------------------------------------------------
void ofApp::update(){

}

void ofApp::regenCloud() {
	//pPos.clear();
	
	// save half-dimensions
	float hX = protoWidth * 0.5f, hY = protoHeight * 0.5f, hZ = protoDepth * 0.5f;

	std::vector<glm::vec3> temp;
	temp.reserve(particleCount);

	// spacing
	float s = cellSize.get();

	// used to stop early
	bool filled = false;

	// NOTE: ooo, this runtime is not great.
	for (float y = -hY; y <= hY; y += s) {
		for (float z = -hZ; z <= hZ && !filled; z += s) {
			for (float x = -hX; x <= hX; x += s) {
				glm::vec3 p = {
					x + ofRandomf() * 0.2f * s,
					y + ofRandomf() * 0.2f * s,
					z + ofRandomf() * 0.2f * s
				};
				temp.push_back(p);
				if ((int)temp.size() >= particleCount) {
					filled = true;
					break;
				}
			}			
		}
	}

	// now process ?
	pPos = std::move(temp); 
	int N = (int)pPos.size();
	pNrm.assign(N, { 0,1,0 }); // assign normal
	pState.assign(N, 0); // assign state

	// add some extra margin for the particle bounds
	minBounds = { -hX - s, -hY - s, -hZ - s };
	maxBounds = { hX + s,  hY + s,  hZ + s };

	// set dimensions 
	dim = glm::ivec3(
		std::max(1, (int)ceil((maxBounds.x - minBounds.x) / cellSize)),
		std::max(1, (int)ceil((maxBounds.y - minBounds.y) / cellSize)),
		std::max(1, (int)ceil((maxBounds.z - minBounds.z) / cellSize))
	);

	mesh.clear();
	mesh.setMode(OF_PRIMITIVE_POINTS);
	mesh.addVertices(pPos);
	mesh.addColors(std::vector<ofFloatColor>(N, ofFloatColor(1)));

	rebuildGrid();
	//tagSurfaceParticles();


}

void ofApp::rebuildGrid() {
	buckets.clear();
	buckets.resize(dim.x * dim.y * dim.z);

	for (int i = 0; i < (int)pPos.size(); ++i) {
		glm::ivec3 c = getCell(pPos[i]);
		if (c.x < 0 || c.y < 0 || c.z < 0 ||
			c.x >= dim.x || c.y >= dim.y || c.z >= dim.z) {
			continue;
		}
		buckets[getCellIndex(c)].push_back(i);
	}


}


//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(bgColor);
	cam.begin();

	ofSetColor(220, 200, 140);
	glPointSize(pointSize);
	mesh.draw();

	cam.end();

	ofDisableDepthTest(); //otherwise gui doesn't render
	if (showAxes) ofDrawAxis(100); // @todo: bug - cant figure out how to do this
	if (showGui) 
		gui.draw();
	ofEnableDepthTest();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'g') showGui = !showGui;
	if (key == 'r') regenCloud();
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
