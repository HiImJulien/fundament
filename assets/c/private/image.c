#include <fundament/image.h>
#include <fundament/log.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// Reperesents the header of a TGA file.  
//
#pragma pack(push, 1)
struct fn__tga_header {
	uint8_t		image_id_length;

	uint8_t 	color_map_type;
	uint8_t 	image_type;

	uint16_t 	color_map_offset;
	uint16_t 	color_map_length;
	uint8_t 	color_map_entry_length;

	uint16_t	origin_x;
	uint16_t	origin_y;

	uint16_t	width;
	uint16_t	height;

	uint8_t		bits_per_pixel;
	uint8_t		attr;
};
#pragma pack(pop)

struct fn_image fn_load_tga_image_from_file(const char* path) {
	FILE* fd = fopen(path, "r");
	if(!fd)
		return (struct fn_image) { NULL };

	fseek(fd, 0L, SEEK_END);
	const size_t file_size = ftell(fd);		
	rewind(fd);

	const uint8_t* memory = malloc(file_size);	
	fread((char*) memory, 1, file_size, fd);
	fclose(fd);

	struct fn_image img = fn_load_tga_image(memory, file_size);	
	free((void*) memory);

	return img;
}

struct fn_image fn_load_tga_image(const uint8_t* memory, size_t size) {
	if(size < 18) {
		fn_err("fundament.assets", "Not a valid TGA file.\n");
		return (struct fn_image) { NULL };
	}

	struct fn__tga_header* header = (struct fn__tga_header*) memory;
	if(header->image_type == 0)
		return (struct fn_image) { NULL };

	if(header->image_id_length) {
		fn_err("fundament.assets", 
				"TGA images with IDs are not supported yet.\n");		

		return (struct fn_image) { NULL };
	}

	if(header->color_map_type 
		|| header->color_map_offset 
		|| header->color_map_length) {
		fn_err("fundament.assets",
				"TGA images with color maps are not supported yet.\n");	

		return (struct fn_image) { NULL };
	}

	if(header->origin_x || header->origin_y) {
		fn_err("fundament.assets",
				"TGA images with origin offsets are not supported yet.\n");

		return (struct fn_image) { NULL };	
	}

	if(header->image_type != 2) {
		fn_err("fundament.assets",
				"TGA images must be 24 or 32 bits and uncompressed.\n");

		return (struct fn_image) { NULL };
	}

	const size_t data_size = header->width 
		* header->height
		* header->bits_per_pixel;	

	struct fn_image img = {
		.data = malloc(data_size),
		.width = header->width,
		.height = header->height
	};

	if(header->bits_per_pixel == 24)
		img.format = fn_image_format_r8g8b8_unorm;
	else if(header->bits_per_pixel == 32)
		img.format = fn_image_format_r8g8b8a8_unorm;
	else {
		fn_err("fundament.assets",
				"Unknown pixel format!\n");	

		free((void*) img.data);	
		return (struct fn_image) { NULL };
	}
		
	memcpy(img.data, memory + 18, data_size);

	const size_t pixel_step = img.format == fn_image_format_r8g8b8_unorm
		? 3
		: 4;	
		
	// TGA actually stores its data BGR(A);
	// swap the bytes to RGBA in order to make it useable 
	// for the graphics APIs.
	for(size_t it = 0; it < data_size - pixel_step; it += pixel_step) {
		uint8_t tmp = img.data[it];	
		img.data[it] = img.data[it + 2];
		img.data[it + 2] = tmp;
	}

	// TODO: Check endianness and swap on demand.
	// TODO: Support image IDs.
	// TODO: Flip images to right direction.
	// TODO: Origin offset?
	// TODO: RLE?

	return img;	
}
