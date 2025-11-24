#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofEnableDepthTest();
    ofSetBackgroundColor(ofColor::black);

    initSimulation();
}

void ofApp::initSimulation() {
    float spacing = cbrtf(1.0f / particleDensity);
    cellSize = spacing;

    // define half-extents of box
    float hX = protoWidth * 0.5f;
    float hY = protoHeight * 0.5f;
    float hZ = protoDepth * 0.5f;

    pPos.clear();

    // generate a regular lattice of particules inside box
    for (float y = -hY; y <= hY; y += spacing) {
        for (float z = -hZ; z <= hZ; z += spacing) {
            for (float x = -hX; x <= hX; x += spacing) {
                pPos.emplace_back(x, y, z);
            }
        }
    }

    // assign normals (optional?)
    int N = (int)pPos.size();
    pNrm.assign(N, glm::vec3(0, 1, 0));
    pState.assign(N, 0);

    // create bounds within a small margin
    minBounds = { -hX - cellSize, -hY - cellSize, -hZ - cellSize };
    maxBounds = { hX + cellSize,  hY + cellSize,  hZ + cellSize };

    // grid setup
    const glm::vec3 size = maxBounds - minBounds;
    dim = glm::ivec3(
        std::max(1, (int)ceil(size.x / cellSize)),
        std::max(1, (int)ceil(size.y / cellSize)),
        std::max(1, (int)ceil(size.z / cellSize))
    );

    // allocate buckest
    buckets.clear();
    buckets.resize(dim.x * dim.y * dim.z);

    //setup mesh
    mesh.clear();
    mesh.setMode(OF_PRIMITIVE_POINTS);
    mesh.addVertices(pPos);
    mesh.addColors(std::vector<ofFloatColor>(pPos.size(), ofFloatColor(1)));

    // build grid and detect surface
    rebuildGrid();
    //detectSurface();

}

//--------------------------------------------------------------
glm::ivec3 ofApp::getCell(const glm::vec3& p) const {
    glm::vec3 rel = (p - minBounds) / cellSize.get();
    return glm::ivec3(
        (int)floor(rel.x),
        (int)floor(rel.y),
        (int)floor(rel.z)
    );
}

//--------------------------------------------------------------
int ofApp::getCellIndex(const glm::ivec3& c) const {
    return (c.x * dim.y + c.y) * dim.z + c.z;
}

//--------------------------------------------------------------
void ofApp::rebuildGrid() {
    for (auto& cell : buckets) cell.clear();

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
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    cam.begin();

    if (showAxes) {
        ofDrawAxis(100);
    }

    glPointSize(pointSize);
    mesh.draw();

    cam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
