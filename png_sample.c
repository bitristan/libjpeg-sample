#include <stdio.h>
#include <stdlib.h>
#include <png.h>

int check_if_png(FILE *fp) {
    if (fp == NULL) {
	return 0;
    }

    const int SIZE = 4;
    char buf[SIZE];

    fseek(fp, 0L, SEEK_SET);

    if (fread(buf, sizeof(char), SIZE, fp) != SIZE) {
	return 0;
    }

    return (!png_sig_cmp(buf, (png_size_t)0, SIZE));
}

void read_png(FILE *fp) {
    if (fp == NULL) {
	return;
    }

    // first put seek to the head
    fseek(fp, 0L, SEEK_SET);

    png_structp png_ptr;
    png_infop info_ptr;
    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type;
    
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (png_ptr == NULL) {
	return;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
	png_destroy_read_struct(&png_ptr, NULL, NULL);
	return;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	return;
    }

    png_init_io(png_ptr, fp);

    png_read_info(png_ptr, info_ptr);
    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    interlace_type = png_set_interlace_handling(png_ptr);

    //    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

    png_read_update_info(png_ptr, info_ptr);

    int i, j;

    //    png_bytep *row_pointers = png_get_rows(png_ptr, info_ptr);
    png_bytep *row_pointers = (png_bytep *) malloc (sizeof(png_bytep) * height);
    for (i = 0; i < height; i++) {
	row_pointers[i] = NULL;
    }

    for (i = 0; i < height; i++) {
	//	row_pointers[i] = png_malloc(png_ptr, png_get_rowbytes(png_ptr, info_ptr));
	row_pointers[i] = (png_byte *) malloc (png_get_rowbytes(png_ptr, info_ptr));
    }

    // read data
    png_read_image(png_ptr, row_pointers);

    /* or read line by line
    for (i = 0; i < interlace_type; i++) {
	for (j = 0; j < height; j++) {
	    png_read_rows(png_ptr, &row_pointers[j], NULL, 1);
	}
    }
    */

    unsigned char *m_gray;
    unsigned char *m_rgb;

    printf("The width is %d\n", width);
    printf("The height is %d\n", height);
    printf("The color type is %d\n", color_type);
    printf("The pixel depth is %d\n", bit_depth);
    printf("interlace_type is %d\n", interlace_type);

    int pos = 0;
    if (color_type == PNG_COLOR_TYPE_GRAY) {
	printf("is png_color_type_gray...\n");

	m_gray = (unsigned char *) malloc (width * height);

	for (i = 0; i < height; i++) {
	    for (j = 0; j < width; j++) {
		m_gray[pos++] = row_pointers[i][j];
	    }
	}

	free(m_gray);
    } else {
	printf ("is png_color_rgb or else\n");
	
	m_rgb = (unsigned char *) malloc (width * height * 3);
	
	for (i = 0; i < height; i++) {
	    for (j = 0; j < width * 3; j++) {
		m_rgb[pos++] = row_pointers[i][j + 2]; //blue
		m_rgb[pos++] = row_pointers[i][j + 1]; //green
		m_rgb[pos++] = row_pointers[i][j]; //red
	    }
	}

	free(m_rgb);
    }


    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
}

void write_png(FILE *fp) {
    if (fp == NULL) {
	return;
    }

    fseek(fp, 0L, SEEK_SET);

    png_structp png_ptr;
    png_infop info_ptr;
    png_colorp palette;

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (png_ptr == NULL) {
	return;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
	return;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
	png_destroy_write_struct(&png_ptr, &info_ptr);
	return;
    }

    png_init_io(png_ptr, fp);

    int width = 300;
    int height = 300;
    int bit_depth = 8;
    int color_type = PNG_COLOR_TYPE_RGB;

    int i, j;

    png_bytep row_pointer[height];
    for (i = 0; i < height; i++) {
	//	row_pointer[i] = png_malloc(png_ptr, png_get_rowbytes(png_ptr, info_ptr));
	row_pointer[i] = (png_byte *) malloc (width * 3 * sizeof(png_byte));
    }

    for (i = 0; i < height; i++) {
	for (j = 0; j < width; j++) {
	    row_pointer[i][j * 3] = 120;
	    row_pointer[i][j * 3 + 1] = 207;
	    row_pointer[i][j * 3 + 2] = 90;
	}
    }

    // write header
    png_set_IHDR(png_ptr, info_ptr, width, height,
		 bit_depth, color_type, PNG_INTERLACE_NONE,
		 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png_ptr, info_ptr);
    
    // write bytes
    png_write_image(png_ptr, row_pointer);
    /* or write line by line.
    for (i = 0; i < height; i++) {
	png_write_rows(png_ptr, &row_pointer[i], 1);
    }
    */

    // write ends
    png_write_end(png_ptr, NULL);

    for (i = 0; i < height; i++) {
	free(row_pointer[i]);
    }

    png_destroy_write_struct(&png_ptr, &info_ptr);
}

int main() {
    const char *in_file_name = "1.png";

    const char *out_file_name = "out.png";
    FILE *in_fp, *out_fp;
    if ((in_fp = fopen(in_file_name, "rb")) == NULL) {
	printf("file read error!\n");
	return 0;
    }

    int result = check_if_png(in_fp);
    printf("result = %d\n", result);

    if (!result) {
	printf("The result is not png!\n");
	fclose(in_fp);
	return 0;
    }

    read_png(in_fp);

    if ((out_fp = fopen(out_file_name, "wb")) == NULL) {
	printf("file read error!\n");
	return 0;
    }
    write_png(out_fp);

    fclose(in_fp);
    fclose(out_fp);
    return 0;
}
