#include <math.h>
#define MAPSIZE 601 
#define HEATMAPDEPTH 1 
#define TOLERANCE 5.0f // DEPRECATED
#define HEATMAP_DENSITY 0.5
#define CYCLE_DIV 0.1
#define COLOR_DENSITY 255.0
#define WATERLEVEL 0.0
#define NOISEFADE 5.0

typedef struct Point_t {
	float x;
	float y;
} Point;

int pointOutOfBounds(float x, float y);

Point getPoint(int x, int y, float x_offset, float y_offset, float* layout );

typedef struct Map_t {
	float map[MAPSIZE][MAPSIZE];
} Map;

Map gen_diamond_square();

void intoHeatMap(Map* map, Map* heatmap);

void genHeatMap(Map* map, Map* heatmap);

float raw_noise(int x, int y);

float noise(int x, int y);

float perlin2(float x, float y, float gain, int octaves, int hgrid);


