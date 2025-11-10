#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofEnableDepthTest();

	// gui setup
	gui.setup("Prototype Box Particle Structure");

	// particle setup
	pointSize.set("Point size (px)", 2.0f, 1.0f, 30.0f);
	particleCount.set("Particle Count", 100000, 1000, 400000);

	// @todo: these values are hardcoded for now
	protoWidth.set("Prototype Width (X)", 200.0f, 20.0f, 1000.0f);	
	protoHeight.set("Prototype Height (Y)", 15.0f, 20.0f, 200.0f);
	protoDepth.set("Prototype Depth  (Z)", 100.0f, 20.0f, 1000.0f);

	bgColor.set("Background", ofColor(0, 0, 0));
	showAxes.set("Show Axes", false);

	cellSize.set("Cell Size (spacing)", 5.0f, 1.0f, 50.0f);

	neighbourRadiusMul.set("Neighbor Radius ×", 5.0f, 0.5f, 15.0f);
	surfaceThreshold.set("Surface Threshold", 8, 2, 40);       

	// add gui elements
	gui.add(pointSize);
	gui.add(particleCount);
	gui.add(protoWidth);
	gui.add(protoHeight);
	gui.add(protoDepth);
	gui.add(bgColor);
	gui.add(showAxes);
	gui.add(cellSize);

	gui.add(cellSize);
	gui.add(neighbourRadiusMul);
	gui.add(surfaceThreshold);

	// listeners for sliders
	sizeListenerW = protoWidth.newListener([&](float&) { regenCloud(); });
	sizeListenerD = protoHeight.newListener([&](float&) { regenCloud(); });
	sizeListenerH = protoDepth.newListener([&](float&) { regenCloud(); });
	countListener = particleCount.newListener([&](int&) { regenCloud(); });
	cellSizeListener = cellSize.newListener([&](float&) { regenCloud(); });

	radiusMulListener = neighbourRadiusMul.newListener([&](float&) {
		rebuildGrid();
		tagSurfaceParticles();
		});
	thresholdListener = surfaceThreshold.newListener([&](int&) {
		tagSurfaceParticles();
		});

	// setup mesh
	mesh.setMode(OF_PRIMITIVE_POINTS);

	// generate the point cloud
	regenCloud();


}

//--------------------------------------------------------------
void ofApp::update(){

}

void ofApp::regenCloud() {
	
	// save dimensions

	
	// save half-dimensions
	const float hX = protoWidth * 0.5f, hY = protoHeight * 0.5f, hZ = protoDepth * 0.5f;

	std::vector<glm::vec3> temp;
	temp.reserve(particleCount);

	// spacing
	float s = cellSize.get();

	// used to stop early
	bool filled = false;

	// build lattice, though this runtime isn't great
	std::vector<glm::vec3> all;
	all.reserve((size_t)((2 * hX / s + 1) * (2 * hY / s + 1) * (2 * hZ / s + 1)));
	for (float y = -hY; y <= hY; y += s)
		for (float z = -hZ; z <= hZ; z += s)
			for (float x = -hX; x <= hX; x += s)
				all.push_back({ x + ofRandomf() * 0.2f * s,
								y + ofRandomf() * 0.2f * s,
								z + ofRandomf() * 0.2f * s });

	// honestly this seems like a cheap solution to the core issue
	if ((int)all.size() <= particleCount) {
		// top up particles if under-filled
		temp = std::move(all);
		while ((int)temp.size() < particleCount) {
			temp.push_back({ ofRandom(-hX, hX), ofRandom(-hY, hY), ofRandom(-hZ, hZ) });
		}
	}
	else {
		// if overfilled, down-sample
		static std::mt19937 rng{ std::random_device{}() }; // apparntly rand is deprecated
		std::shuffle(all.begin(), all.end(), rng);
		temp.assign(all.begin(), all.begin() + particleCount);
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
	const float cS = std::max(1e-6f, cellSize.get());
	dim = glm::ivec3(
		std::max(1, (int)ceil((maxBounds.x - minBounds.x) / cS)),
		std::max(1, (int)ceil((maxBounds.y - minBounds.y) / cS)),
		std::max(1, (int)ceil((maxBounds.z - minBounds.z) / cS))
	);

	mesh.clear();
	mesh.setMode(OF_PRIMITIVE_POINTS);
	mesh.addVertices(pPos);
	mesh.addColors(std::vector<ofFloatColor>(N, ofFloatColor(1)));

	rebuildGrid();
	tagSurfaceParticles();


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


template <class F>
void ofApp::forNeighbours(int i, F&& fn) {


	// find which grid cell particle i belongs to
	glm::ivec3 c = getCell(pPos[i]);

	// loop through cell and neighbouring cells
	for (int dZ = -1; dZ <= 1; ++dZ) {
		for (int dY = -1; dY <= 1; ++dY) {
			for (int dX = -1; dX <= 1; ++dX) {

				// find coords of neighbouring cell
				glm::ivec3 nC = c + glm::ivec3(dX, dY, dZ);

				// if outside bounds, skip
				if (nC.x < 0 || nC.y < 0 || nC.z < 0 ||
					nC.x >= dim.x || nC.y >= dim.y || nC.z >= dim.z)
					continue;

				// retrieve list of particle indices
				const auto& cell = buckets[getCellIndex(nC)];

				// for each particle index in cell, call provided func
				for (int j : cell) {
					if (j != i) // don't compare particle to self
						fn(j);
				}
			}
		}
	}
}


void ofApp::tagSurfaceParticles() {
	// set params for neighbourhood search
	const float cS = std::max(1e-6f, cellSize.get());
	const float r = cS * neighbourRadiusMul;
	const float r2 = r * r; // squared radius for distance checks

	// reset particle states/normals
	pState.assign(pPos.size(), 0);
	pNrm.assign(pPos.size(), glm::vec3(0, 1, 0));

	// make sure mesh has colour array that matches vertex count
	if ((int)mesh.getVertices().size() != pPos.size()) {
		mesh.clear();
		mesh.setMode(OF_PRIMITIVE_POINTS);
		mesh.addVertices(pPos);
		mesh.addColors(std::vector<ofFloatColor>(pPos.size(), ofFloatColor(1)));
	}
	auto& cols = mesh.getColors();

	// define threshold for surface detection
	//   - if less than this many neighbours, ON surface
	const int thr = surfaceThreshold.get();

	// analyze each particle's neighbourhood
	for (int i = 0; i < (int)pPos.size(); ++i) {
		const glm::vec3 p = pPos[i]; // curremt particle position

		int neighbourCount = 0;		// how many neighbours within radius?
		glm::vec3 grad(0.0f);		// accumulator for "repulsion" direction 

		// use helper to iterate through nearby particles
		forNeighbours(i, [&](int j) {
			const glm::vec3 d = pPos[j] - p; // vector to neighbour
			const float d2 = glm::dot(d, d); // squared distance
			if (d2 <= r2) { // inside neighbour radius?
				neighbourCount++;
				if (d2 > 1e-6f)
					grad -= d / d2; // push away from neighbour
			}
		});


		// is the particle at the surface ?
		if (neighbourCount < thr) {
			pState[i] = 1; // mark as surface (state 1)

			// estimate normal - direction of least density (away from neighbours)
			if (glm::length2(grad) > 1e-10f)
				pNrm[i] = glm::normalize(grad);

			cols[i] = ofFloatColor(1.0f, 0.6f, 0.25f);
		}
		else {
			// not at surface
			pState[i] = 0;
			pNrm[i] = glm::vec3(0, 1, 0);
			cols[i] = ofFloatColor(0.75f);
		}
	}
}




//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(bgColor);
	cam.begin();

	if (showAxes) ofDrawAxis(100);
	ofSetColor(220, 200, 140);
	glPointSize(pointSize);
	mesh.draw();

	cam.end();

	ofDisableDepthTest(); //otherwise gui doesn't render
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
