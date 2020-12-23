#include <fundament/mesh.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	if(argc < 2) {
		printf("Usage: ./mesh_text path\n");
		return EXIT_FAILURE;	
	}

	struct fn_mesh mesh = fn_load_obj_mesh_from_file(argv[1]);
	printf("Vertex Count: %zu\n", mesh.vertex_count);

	printf("Successfully loaded mesh from: %s\n", argv[1]);

	return EXIT_SUCCESS; 
}
