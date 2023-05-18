#include <math.h>
#define MAPSIZE 400
#define TOLERANCE 0.5f

typedef struct Map_t {
	float map[MAPSIZE][MAPSIZE];
} Map;

Map gen_diamond_square();

void intoHeatMap(Map* map);

float noise(int x, int y);

float perlin2(float x, float y, float gain, int octaves, int hgrid);


