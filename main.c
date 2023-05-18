#include<stdio.h>
#include<math.h>
#include "gen.h"
#include "libbmp.h"

const char* gradient5_1 = " ░▒▓█";
const char* gradient5_2 = " *!#O";
const char* gradient50 = "@@@@@@@@@@@@%%%%%%%%#########********+++++++++===="; 
const char* gradient93 = "  `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";

typedef struct RGB_t {
	float r;
	float g;
	float b;
} RGB;

RGB gray_to_rgb(float gray) {
	RGB rgb;
	rgb.r = 0.0;
	rgb.g = gray;
	rgb.b = (1.0/gray)*2.0;
	return rgb;
}

void donothing() {
	int i = 0;
	i = i+5;
	int a = 0;
	a = i;	
}

int main(int argc, char** argv) { 
	bmp_img img;
	bmp_img_init_df(&img, MAPSIZE, MAPSIZE);

	Map map = gen_diamond_square(&img);
	Map heatmap;

	genHeatMap(&map, &heatmap);
	map = heatmap;

	donothing();	

	for(int y=0; y<MAPSIZE; y++) {
		for(int x=0; x<MAPSIZE; x++) {
			//float output = perlin2(x,y,120.0,1.0,4.0);
			//float precalc = (5.0f * ((1.0 + output)/2.0f));
			float precalc = (((1.0 + heatmap.map[y][x])/2.0f));
			// int calc = (int)precalc; 
			// char c = gradient5_1[calc];
			// printf("%c",c);
			float gray = precalc*(COLOR_DENSITY);
			RGB rgb = gray_to_rgb(gray);
			bmp_pixel_init(&img.img_pixels[y][x], rgb.r, rgb.g, rgb.b);

			// RGB rgb = gray_to_rgb(noise(x,y));
			// bmp_pixel_init(&img.img_pixels[y][x], rgb.r, rgb.g, rgb.b);
		}
		// printf("\n");
	}
	bmp_img_write(&img, "map.bmp");
	bmp_img_free(&img);
	return 0;
}

