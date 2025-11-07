#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofEnableDepthTest();

	// gui setup
	gui.setup("Prototype Box Particle Structure");
	pointSize.set("Point size (px)", 2.0f, 1.0f, 30.0f);
	particleCount.set("Particle Count", 60000, 1000, 400000);

	// @todo: these values are hardcoded for now
	protoWidth.set("Prototype Width (X)", 200.0f, 20.0f, 300.0f);	
	protoHeight.set("Prototype Height (Y)", 400.0f, 20.0f, 500.0f);
	protoDepth.set("Prototype Depth  (Z)", 200.0f, 20.0f, 1000.0f);

	bgColor.set("Background", ofColor(0, 0, 0));
	showAxes.set("Show Axes", false);

	// add gui elements
	gui.add(pointSize);
	gui.add(particleCount);
	gui.add(protoWidth);
	gui.add(protoHeight);
	gui.add(protoDepth);
	gui.add(bgColor);
	gui.add(showAxes);

	// listeners for sliders
	sizeListenerW = protoWidth.newListener([&](float&) { regenCloud(); });
	sizeListenerD = protoHeight.newListener([&](float&) { regenCloud(); });
	sizeListenerH = protoDepth.newListener([&](float&) { regenCloud(); });
	countListener = particleCount.newListener([&](int&) { regenCloud(); });

	// generate the point cloud
	regenCloud();


}

//--------------------------------------------------------------
void ofApp::update(){

}

void ofApp::regenCloud() {
	positions.clear();
	positions.reserve(particleCount);

	// Randomly fill particles within a box structure
	for (int i = 0; i < particleCount; ++i) {
		float x = ofRandom(-protoWidth * 0.5f, protoWidth * 0.5f);
		float y = ofRandom(-protoHeight * 0.5f, protoHeight * 0.5f);
		float z = ofRandom(-protoDepth * 0.5f, protoDepth * 0.5f);
		positions.emplace_back(x, y, z);
	}

	// update vertex buffer
	if (!vbo.getIsAllocated() || vboVertexCount != positions.size()) {
		// reallocate buffer for new size otherwise particle slider breaks
		vbo.setVertexData(positions.data(), positions.size(), GL_DYNAMIC_DRAW);
		vboVertexCount = positions.size();
	} else {
		vbo.updateVertexData(positions.data(), positions.size());
	}

}


//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(bgColor);
	cam.begin();

	ofSetColor(220, 200, 140);
	glPointSize(pointSize);
	vbo.draw(GL_POINTS, 0, positions.size());

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
