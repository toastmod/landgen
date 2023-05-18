#include <stdio.h>
#include "gen.h"
#include "libbmp.h"

float NOISEMUL = 8.0;

typedef struct Point_t {
	float x;
	float y;
} Point;

Point getPoint(int x, int y, float x_offset, float y_offset, float* layout ) {

	// old code:
	// float coords2[2] = {(x+(topRight[0]*(x_offset-1))), (y+(topRight[1]*(y_offset-1)))};
	Point pt;
	pt.x = ((float)x * x_offset) + (layout[0] * x_offset);
	pt.y = ((float)y * y_offset) + (layout[1] * y_offset);

	// Safety check
	if((pt.x >= (float)MAPSIZE) || (pt.y >= (float)MAPSIZE)) {
		printf("ERROR:\nx: %f | y: %f\n", pt.x, pt.y);
	}

	return pt;
}

void checkPoint(Point *pt) {


}

Map gen_diamond_square(bmp_img* img ) {
	Map gmap;
	int count = 0;
	// For each division
	for(float i = (float)MAPSIZE-1; i > 1.0; i = i/4.0f) {

		count = count + 2;
		float topLeft[2] = { 0, 0 };
		float topRight[2] = { 0, 1 };
		float bottomLeft[2] = { 1, 0 };
		float bottomRight[2] = { 1, 1 };
		float center[2] = { 0.5, 0.5 };

		float topCenter[2] = { 0.5, 0 };
		float bottomCenter[2] = { 0.5, 1 };
		float leftCenter[2] = { 0, 0.5 };
		float rightCenter[2] = { 1, 0.5 };

		float x_offset = (float)(MAPSIZE-1)/(float)count;
		float y_offset = (float)(MAPSIZE-1)/(float)count;

		int noise_i = 0;

		// For each quadrant/square
		for(int y=0; y < count; y++) {
			for(int x=0; x < count; x++) {

				// Square Step; Set corners to random values
				float tL = noise(noise_i, 1);
				float tR = noise(noise_i, 2);
				float bL = noise(noise_i, 3);
				float bR = noise(noise_i, 4);
				float CC = ((tL+tR+bL+bR)/4.0f) + noise(noise_i,5); 

				// float coords1[2] = { (x+(topLeft[0]*(x_offset-1))),(y+(topLeft[1]*(y_offset-1)))};
				// float coords1[2] = {
				// 	(x*x_offset)+(topLeft[0]*x_offset),
				// 	(y*y_offset)+(topLeft[1]*y_offset),

				// };

				Point coords1 = getPoint(x,y,x_offset,y_offset,topLeft); 
				gmap.map[(int)coords1.x][(int)coords1.y] = tL;

				Point coords2 = getPoint(x,y,x_offset,y_offset,topRight); 
				gmap.map[(int)coords2.x][(int)coords2.y] = tR;

				Point coords3 = getPoint(x,y,x_offset,y_offset,bottomLeft); 
				gmap.map[(int)coords3.x][(int)coords3.y] = bL;

				Point coords4 = getPoint(x,y,x_offset,y_offset,bottomRight); 
				gmap.map[(int)coords4.x][(int)coords4.y] = bR;

				// Center Average + random
				Point centercoords = getPoint(x,y,x_offset,y_offset,center); 
				gmap.map[(int)centercoords.x][(int)centercoords.y] = ((tL+tR+bL+bR)/4.0f) + noise(noise_i, 5);
				noise_i = noise_i+(int)(noise(5+noise_i,1));

				// Diamond Step
				float tC = noise(noise_i, 1) + ((CC+tL+tR)/3.0);
				float bC = noise(noise_i, 2) + ((CC+bL+bR)/3.0);
				float lC = noise(noise_i, 3) + ((CC+tL+bL)/3.0);
				float rC = noise(noise_i, 4) + ((CC+tR+bR)/3.0);

				Point coords5 = getPoint(x,y,x_offset,y_offset,topCenter); 
				gmap.map[(int)coords5.x][(int)coords5.y] = tC;

				Point coords6 = getPoint(x,y,x_offset,y_offset,bottomCenter); 
				gmap.map[(int)coords6.x][(int)coords6.y] = bC;

				Point coords7 = getPoint(x,y,x_offset,y_offset,leftCenter); 
				gmap.map[(int)coords7.x][(int)coords7.y] = lC;

				Point coords8 = getPoint(x,y,x_offset,y_offset,rightCenter); 
				gmap.map[(int)coords8.x][(int)coords8.y] = rC;

				noise_i++;

			}
		}

		// Decrease noise output
		//NOISEMUL = NOISEMUL/2.0f;
	}

	return gmap;
}

void floodCell(float tolerance, Map* map, int* coords, int* adjcoords) {
	// check if adjancent coordinates are in bounds
	if((adjcoords[0] >= 0) && (adjcoords[1] >= 0)) {
		map->map[adjcoords[0]][adjcoords[1]] += tolerance * map->map[coords[0]][coords[1]];
	}
}

void intoHeatMap(Map* map) {
	for(int y=0; y<MAPSIZE; y++) {
		for(int x=0; x<MAPSIZE; x++) {

			int top[2] = {x, y-1};
			int bottom[2] = {x, y+1};
			int left[2] = {x-1, y};
			int right[2] = {x+1, y};
			int tLeft[2] = {x-1, y-1};
			int tRight[2] = {x+1, y-1};
			int bLeft[2] = {x-1, y+1};
			int bRight[2] = {x+1, +-1};

			int coords[2] = {x, y};

			floodCell(TOLERANCE, map, coords, top);
			floodCell(TOLERANCE, map, coords, bottom);
			floodCell(TOLERANCE, map, coords, left);
			floodCell(TOLERANCE, map, coords, right);
			floodCell(TOLERANCE, map, coords, tLeft);
			floodCell(TOLERANCE, map, coords, tRight);
			floodCell(TOLERANCE, map, coords, bLeft);
			floodCell(TOLERANCE, map, coords, bRight);

		}
	}
}

float noise(int x, int y) {
	int n;

	n = x + y * 57;
	n = (n << 13) ^ n;

	return NOISEMUL*(1.0 - ( (n * ((n * n * 15731) + 789221) +  1376312589) & 0x7fffffff) / 1073741824.0);

}

float perlin2(float x, float y, float gain, int octaves, int hgrid) {
	int i;
	float total = 0.0f;
	float frequency = 1.0f/((float)hgrid);
	float amplitude = gain;
	float lacunarity = 2.0;
	
	for(i=0; i < octaves; i++) {
		total += noise((float)x * frequency, (float)y * frequency) * amplitude;
	}
}
