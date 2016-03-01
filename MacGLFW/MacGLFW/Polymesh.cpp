//
//  Polymesh.cpp
//  Polymesh
//
//  Created by Jeremy Kemery on 2/18/16.
//  Copyright (c) 2016 Jeremy Kemery. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include "ShaderProgram.h"
#include "Utils.h"
#include "Polymesh.hpp"

using std::ifstream;
using std::istringstream;
using std::vector;
using std::cout;
using std::endl;
using std::endl;
using std::string;
using std::getline;

//----------------------------------------------------------------------

void Polymesh::init()
{
    _numPoints = 0;
    _numTriangles = 0;
    _points = NULL;
    _indices = NULL;
    _arrayAndElementBuffers[0] = 0;
    _arrayAndElementBuffers[1] = 0;
}

//----------------------------------------------------------------------

void Polymesh::dealloc()
{
    if (_points) {
        delete [] _points;
    }
    if (_indices) {
        delete [] _indices;
    }
    if (_arrayAndElementBuffers[0] != 0) {
        glDeleteBuffers(2, _arrayAndElementBuffers);
    }
    init();
}

//----------------------------------------------------------------------

void Polymesh::readFromObjectFile(string filename, bool frontFaceIsClockwise)
{
    // Open polymesh file
    filename = pathUsingEnvironmentVariable(filename, "CS377_INPUT_FILES");
    ifstream ifs;
    ifs.open(filename.c_str());
    
    // Read in number of points and polygons
    string line;
    getline(ifs, line);
    trim(line);
    istringstream iss(line);
    
    int numPolygons;
    iss >> _numPoints >> numPolygons;
    
    // Allocate array for points
    _points = new GLfloat[3 * _numPoints];
    
    // Read in points
    for (int i = 0; i < _numPoints; i++) {
        getline(ifs, line);
        trim(line);
        iss = istringstream(line);
        iss >> _points[3 * i] >> _points[3 * i + 1] >> _points[3 * i + 2];
    }
    
    // Read in indices
    vector<GLfloat> indices;
    for (int i = 0; i < numPolygons; i++) {
        getline(ifs, line);
        trim(line);
        iss = istringstream(line);
        
        // Get number of points in polygon
        int npts;
        iss >> npts;
        
        // Get the number of triangles
        GLsizei triangles = npts - 2;
        
        vector<GLfloat> inds;
        // Read in points for polygon
        for (int j = 0; j < npts; j++) {
            int pt;
            iss >> pt;
            inds.push_back(pt);
        }
        
        // Create triangle fan
        for (int j = 0; j < triangles; j++) {
            indices.push_back(inds[j+2]);
            indices.push_back(inds[j+1]);
            indices.push_back(inds[0]);
        }
        
        // Update the total number of triangles
        _numTriangles += triangles;
    }
    
    // Allocate array for indices
    _indices = new GLuint[3 * _numTriangles];
    for (int i = 0; i < indices.size(); i++) {
        _indices[i] = indices[i];
    }
}

//----------------------------------------------------------------------

void Polymesh::bindArrayAndElementBuffers(ShaderProgram &shaderProgram, int vertexPosition)
{
    // Generate 2 buffers
    // Buffer 0 for GL_ARRAY_BUFFER
    // Buffer 1 for GL_ELEMENT_ARRAY_BUFFER
    glGenBuffers(2, _arrayAndElementBuffers);
    
    // Array Buffer
    glBindBuffer(GL_ARRAY_BUFFER, _arrayAndElementBuffers[0]);
    glBufferData(GL_ARRAY_BUFFER, 3 * _numPoints * sizeof(GLfloat), _points, GL_STATIC_DRAW);
    
    // Element Array Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _arrayAndElementBuffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * _numTriangles * sizeof(GLfloat), _indices, GL_STATIC_DRAW);
    
    // Layout value for vPosition
    glEnableVertexAttribArray(vertexPosition);
}

//----------------------------------------------------------------------

void Polymesh::glDraw()
{
    // Buffer for each triangle
    glBindBuffer(GL_ARRAY_BUFFER, _arrayAndElementBuffers[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _arrayAndElementBuffers[1]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), BUFFER_OFFSET(0));

    // draw the buffer for the triangles
    glDrawElements(GL_TRIANGLES, _numTriangles * 3, GL_UNSIGNED_INT, NULL);
}

//----------------------------------------------------------------------
