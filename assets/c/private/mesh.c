#include <fundament/mesh.h>

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

//
// Advances the cursor to the first character of the next line.
//
// Note:
// - Assumes that the text is separated in lines using the \n character.
//
static const char* fn__advance_line(const char* cursor) {
	for(; *cursor != '\0'; ++cursor)	
		if(*cursor == '\n') 
			return ++cursor;

	return cursor;
}

//
// Stores the contents of an obj face vertex.
//
struct fn__obj_face {
	int64_t vert_index;
	int64_t norm_index;
	int64_t uv_index;
};

//
// Reads a obj face vertex.
//
static const char* fn__read_obj_face(
	const char* cursor,
	struct fn__obj_face* face) {

	face->vert_index = strtoll(cursor, (char**) &cursor, 10);	
	face->norm_index = 0;
	face->uv_index = 0;

	if(*cursor != '/') 
		return cursor;

	++cursor;
	if(*cursor != '/') {
		face->uv_index = strtoll(cursor, (char**) &cursor, 10);
		++cursor;
	}

	if(*cursor == ' ')
		return ++cursor;	

	face->norm_index = strtoll(cursor, (char**) &cursor, 10);

	return *cursor != '\n' ? ++cursor : cursor;
}

struct fn_mesh fn_load_obj_mesh_from_file(const char* path) {
	FILE* fd = fopen(path, "r");

	if(!fd) 
		return (struct fn_mesh) { NULL, };

	fseek(fd, 0L, SEEK_END);
	const size_t file_size = ftell(fd);
	rewind(fd);
	
	char* text = malloc(file_size);
	const size_t read_count = fread(text, 1, file_size, fd);
	text[read_count] = '\0';
	fclose(fd);

	struct fn_mesh mesh = fn_load_obj_mesh(text);
	free(text);

	return mesh;
}

struct fn_mesh fn_load_obj_mesh(const char* text) {
	size_t stash_vertex_count = 0;
	size_t stash_normal_count = 0;
	size_t stash_uv_count = 0;	
	size_t face_count = 0;

	bool break_on_next_object = false;

	for(const char* cursor = text; 
		*cursor != '\0'; 
		cursor = fn__advance_line(cursor)) {
		if(*cursor == '#')
			continue;

		if(*cursor == 'o') {
			if(break_on_next_object)
				break;
			
			break_on_next_object = true;
			continue;
		}
		
		if(*cursor == 'v') {
			++cursor;

			switch(*cursor) {
				case ' ': ++stash_vertex_count; break;
				case 'n': ++stash_normal_count; break;
				case 't': ++stash_uv_count; break;	
				default: break;
			}

			continue;
		}

		if(*cursor == 'f' && *++cursor == ' ') {
			++face_count;
			continue;
		}	
		
	}

	const size_t stash_size =
		sizeof(float) * 3 * stash_vertex_count
		+ sizeof(float) * 3 * stash_normal_count
		+ sizeof(float) * 2 * stash_uv_count;

	float* stash_base = malloc(stash_size);
	float* stash_verts = stash_base;
	float* stash_norms = &stash_verts[stash_vertex_count];
	float* stash_uvs = &stash_norms[stash_normal_count];

	const bool has_norms = stash_normal_count != 0;
	const bool has_uvs = stash_uv_count != 0;

	struct fn_mesh mesh = {
		.vertices = malloc(sizeof(float) * 3 * face_count), 
		.normals = malloc(sizeof(float) * 3 * face_count * has_norms), 
		.uvs = malloc(sizeof(float) * 2 * face_count * has_uvs),
		.indices = NULL,
		.vertex_count = face_count,
		.index_count = 0
	};		

	size_t read_verts = 0;
	size_t read_norms = 0;
	size_t read_uvs = 0;

	size_t write_verts = 0;
	size_t write_norms = 0;
	size_t write_uvs = 0;
	
	for(const char* cursor = text;
		*cursor != '\0';
		cursor = fn__advance_line(cursor)) {
	
		if(*cursor == '#')
			continue;	

		if(*cursor == 'v') {
			++cursor;

			if(*cursor == ' ') {
				++cursor;

				stash_verts[read_verts++] = strtof(cursor, (char**) &cursor);
				++cursor;	
				stash_verts[read_verts++] = strtof(cursor, (char**) &cursor);
				++cursor;
				stash_verts[read_verts++] = strtof(cursor, (char**) &cursor);	

				continue;
			}

			if(*cursor == 'n') {
				cursor += 2;

				stash_norms[read_norms++] = strtof(cursor, (char**) &cursor);
				++cursor;
				stash_norms[read_norms++] = strtof(cursor, (char**) &cursor);
				++cursor;
				stash_norms[read_norms++] = strtof(cursor, (char**) &cursor);

				continue;
			}

			if(*cursor == 't') {
				cursor += 2;

				stash_uvs[read_uvs++] = strtof(cursor, (char**) &cursor);
				++cursor;
				stash_uvs[read_uvs++] = strtof(cursor, (char**) &cursor);

				continue;
			}
		}

		if(*cursor == 'f') {
			cursor += 2;

			struct fn__obj_face face;
			for(uint8_t it = 0; it < 3; ++it) {
				cursor = fn__read_obj_face(cursor, &face);

				mesh.vertices[write_verts++] = stash_verts[face.vert_index]; 

				printf("%"PRId64"/%"PRId64"/%"PRId64" ",
					face.vert_index,
					face.uv_index,
					face.norm_index
				);
			
				if(has_norms)
					mesh.normals[write_norms++] = stash_norms[face.norm_index];

				if(has_uvs)
					mesh.uvs[write_uvs++] = stash_uvs[face.uv_index];
			}

			printf("\n");
			continue;
		}
	}

	free(stash_base);
	
	return mesh;
}
