#ifndef FUNDAMENT_MESH_H
#define FUNDAMENT_MESH_H

//==============================================================================
// This module provides functionality to load 3D meshes from files.
//==============================================================================

#include <fundament/api.h>

#include <stddef.h>
#include <stdint.h>

//
// Represents a 3D mesh.
//
struct fn_mesh {

	// Stores the vertices in float triplets.
	float* vertices;	

	// Stores the normals in float triplets.
	float* normals;

	// Stores the uvs in float doublets.
	float* uvs;

	// Stores the indicies.
	int32_t* indices;	

	size_t	vertex_count;
	size_t  index_count;
};

//
// Loads a mesh from an Wavefront Object file.
//
// Note:
// - This function does NOT support groups, names or materials.
// - Stops after the first object has been parsed.
//
API struct fn_mesh fn_load_obj_mesh_from_file(const char* path);

//
// Loads a mesh from memory.
//
// Note:
// - This function does NOT support groups, names or materials.
// - Stops after the first object has been parsed.
//
API struct fn_mesh fn_load_obj_mesh(const char* text);

#endif 	// FUNDAMENT_MESH_H
