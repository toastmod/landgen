#include <stdio.h>
#include <math.h>
#include <float.h>
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
	rgb.g = gray*COLOR_DENSITY;
    rgb.b = 0.0;
	if(gray < WATERLEVEL) {
		rgb.g = (1.0f/rgb.g);
		rgb.b = (gray*COLOR_DENSITY);
	}
	return rgb;
}

RGB gray_to_bw(float gray) {
    RGB bw;
	bw.r = gray*COLOR_DENSITY;
	bw.g = gray*COLOR_DENSITY;
	bw.b = gray*COLOR_DENSITY;

	return bw;

}

float mapMedian(Map *map) {
    float min = FLT_MAX;
    float max = FLT_MIN;
    for(int y=0; y<MAPSIZE; y++){
		for(int x=0; x<MAPSIZE; x++){
		    if(max < map->map[y][x]){
				max = map->map[y][x];
			}
		    if(min > map->map[y][x]){
				min = map->map[y][x];
			}
		}
	}
	return ((min)+(max))/2.0f;
}

void truncate(Map *map, float level) {
	printf("Truncating...\n");
    for(int y=0; y<MAPSIZE; y++){
		for(int x=0; x<MAPSIZE; x++){
			int upscale = (int)(map->map[y][x]*level);
			map->map[y][x] = (((float)(upscale))/level);
		}
	}
}

// Normalize min and max to 0.0 and 1.0 
// Assumes input is always within min and max
float normalize(float min, float max, float input) {
    return (1.0+((input-min)/(max-min)))/2.0f;
	
}

float normalize_map(Map* map) {

    float min = FLT_MAX;
    float max = FLT_MIN;
    printf("Scanning min/max: ");
    for(int y=0; y<MAPSIZE; y++){
		for(int x=0; x<MAPSIZE; x++){
		    if(max < map->map[y][x]){
				max = map->map[y][x];
			}
		    if(min > map->map[y][x]){
				min = map->map[y][x];
			}
			//printf("%f ",map->map[y][x]);
		}
		//printf("\n");
	}
	map->min = min;
	map->max = max;

	float median = ((min)+(max))/2.0f;
    printf("[min: %f | med: %f | max: %f]\n", min, median, max);

    printf("Normalizing...\n");
    float nmin = FLT_MAX;
    float nmax = FLT_MIN;
    for(int y=0; y<MAPSIZE; y++){
		for(int x=0; x<MAPSIZE; x++){
		    map->map[y][x] = normalize(min, max, map->map[y][x]);
		    if(nmax < map->map[y][x]){
				nmax = map->map[y][x];
			}
		    if(nmin > map->map[y][x]){
				nmin = map->map[y][x];
			}
		}
	}

	median = ((nmin)+(nmax))/2.0f;
    printf("normalized: [min: %f | med: %f | max: %f]\n", nmin, nmax);


}

// for debugging
void donothing() {
	int i = 0;
	i = i+5;
	int a = 0;
	a = i;	
}

int main(int argc, char** argv) { 
	bmp_img img;
	bmp_img hmap_img;
	bmp_img_init_df(&img, MAPSIZE, MAPSIZE);
	bmp_img_init_df(&hmap_img, MAPSIZE, MAPSIZE);

	Map map = gen_diamond_square();
	normalize_map(&map);
	truncate(&map, TRUNC_LEVEL);
	float med = mapMedian(&map);

	printf("median: %f\n",med);

	Map heatmap;

	// for(int i=0; i<HEATMAPDEPTH; i++) {
	// 	intoHeatMap(&map, &heatmap);
	// 	map = heatmap;
	// }


	FILE* csv = fopen("map.csv", "w");
	FILE* hmap_csv = fopen("heightmap.csv", "w");

	donothing();	

	for(int y=0; y<MAPSIZE; y++) {
		for(int x=0; x<MAPSIZE; x++) {
			fprintf(hmap_csv, "%f,", map.map[y][x]);
			RGB rgb = gray_to_rgb(powf(map.map[y][x], EXCENTUATION));
			RGB bw = gray_to_bw(powf(map.map[y][x], EXCENTUATION));
			bmp_pixel_init(&img.img_pixels[y][x], rgb.r, rgb.g, rgb.b);
			bmp_pixel_init(&hmap_img.img_pixels[y][x], bw.r, bw.g, bw.b);
			fprintf(csv, "%f,", rgb.g);

		}
		fprintf(hmap_csv, "\n");
		fprintf(csv, "\n");
	}

	// CMY Positions
	// Cyan = 0,0
	// Magenta = 0,1
	// Yellow = 1,1
    bmp_pixel_init(&img.img_pixels[0][0], 0.0f, 255.0f, 255.0f);
    bmp_pixel_init(&img.img_pixels[0][MAPSIZE-1], 255.0f, 0.0f, 255.0f);
    bmp_pixel_init(&img.img_pixels[MAPSIZE-1][MAPSIZE-1], 255.0f, 255.0f, 0.0f);

	fclose(csv);
	fclose(hmap_csv);
	bmp_img_write(&img, "map.bmp");
	bmp_img_free(&img);
	bmp_img_write(&hmap_img, "heightmap.bmp");
	bmp_img_free(&hmap_img);
	return 0;
}

