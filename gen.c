#include <stdio.h>
#include "gen.h"
#include "libbmp.h"


float NOISEMUL = 8.0;

int pointOutOfBounds(float x, float y) {
	if(((x >= (float)MAPSIZE) || (y >= (float)MAPSIZE)) || ((x < 0.0)||(y < 0.0))) {
		return 1;
	}else{
		return 0;
	}
}

Point getPoint(int x, int y, float x_offset, float y_offset, float* layout ) {

	// old code:
	// float coords2[2] = {(x+(topRight[0]*(x_offset-1))), (y+(topRight[1]*(y_offset-1)))};
	Point pt;
	pt.x = ((float)x * x_offset) + (layout[0] * x_offset);
	pt.y = ((float)y * y_offset) + (layout[1] * y_offset);

	// Safety check
	if(pointOutOfBounds(pt.x, pt.y)){
		printf("ErrorOutOfBounds: \nx: %f | y: %f\n", pt.x, pt.y);
	}

	return pt;
}


Map gen_diamond_square(bmp_img* img ) {
	printf("Generating diamond-square map...\n");
	Map gmap;
	int count = 1;
	// For each division
	int noise_i = 0;
	for(float i = (float)MAPSIZE; i > CYCLE_DIV; i = i/4.0f) {

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

		int first_step = 1;

		float tL; 
		float tR;
		float bL;
		float bR;
		float CC; 

		float tC;
		float bC;
		float lC;
		float rC;

		float noise_div = 1.0f;

		// For each quadrant/square
		for(int y=0; y < count; y++) {
			for(int x=0; x < count; x++) {

				// Square points
				Point coords1 = getPoint(x,y,x_offset,y_offset,topLeft); 
				Point coords2 = getPoint(x,y,x_offset,y_offset,topRight); 
				Point coords3 = getPoint(x,y,x_offset,y_offset,bottomLeft); 
				Point coords4 = getPoint(x,y,x_offset,y_offset,bottomRight); 

				Point coords5 = getPoint(x,y,x_offset,y_offset,topCenter); 
				Point coords6 = getPoint(x,y,x_offset,y_offset,bottomCenter); 
				Point coords7 = getPoint(x,y,x_offset,y_offset,leftCenter); 
				Point coords8 = getPoint(x,y,x_offset,y_offset,rightCenter); 

				Point centercoords = getPoint(x,y,x_offset,y_offset,center); 

				// Square step
				if(first_step == 1) {
				    
				    // Square Step init values are random
				    tL = noise(noise_i, 1);
				    tR = noise(noise_i, 2);
				    bL = noise(noise_i, 3);
				    bR = noise(noise_i, 4);
				    CC = ((tL+tR+bL+bR)/4.0f) + (noise(noise_i,5)/noise_div); 

					first_step = 0;
				}else{
				    // Square Step values after first iteration
					tL = gmap.map[(int)coords1.y][(int)coords1.x];
					tR = gmap.map[(int)coords2.y][(int)coords2.x];
					bL = gmap.map[(int)coords3.y][(int)coords3.x];
					bR = gmap.map[(int)coords4.y][(int)coords4.x];
				    CC = ((tL+tR+bL+bR)/4.0f) + (noise(noise_i,5)/noise_div); 

				}

				// Diamond step values
				// only calculating tC and lC since averages will be filled out by interation
			    tC = (noise(noise_i, 1)/noise_div) + ((CC+tL+tR)/3.0);
				bC = gmap.map[(int)coords6.y][(int)coords6.x];
			    //bC = (noise(noise_i, 2)/noise_div) + ((CC+bL+bR)/3.0);
				lC = (noise(noise_i, 3)/noise_div) + ((CC+tL+bL)/3.0);
				rC = gmap.map[(int)coords8.y][(int)coords8.x];
				//rC = (noise(noise_i, 4)/noise_div) + ((CC+tR+bR)/3.0);
				

				// Square Step 
				gmap.map[(int)coords1.y][(int)coords1.x] = tL;

				//gmap.map[(int)coords2.y][(int)coords2.x] = tR;

				//gmap.map[(int)coords3.y][(int)coords3.x] = bL;

				//gmap.map[(int)coords4.y][(int)coords4.x] = bR;

				// Center Average + random
				gmap.map[(int)centercoords.y][(int)centercoords.x] = ((tL+tR+bL+bR)/4.0f) + (noise(noise_i, 5)/noise_div);

				// Diamond Step		
				gmap.map[(int)coords5.y][(int)coords5.x] = tC;

				//gmap.map[(int)coords6.y][(int)coords6.x] = bC;

				gmap.map[(int)coords7.y][(int)coords7.x] = lC;

				//gmap.map[(int)coords8.y][(int)coords8.x] = rC;

				noise_i++;

				noise_div = noise_div + NOISEFADE;

			}
		}

		count = count * 2; 
		// Decrease noise output
		//NOISEMUL = NOISEMUL/2.0f;
	
	}

	printf("DONE!\n");
	return gmap;
}

void floodCell(float tolerance, Map* map, Map* heatmap, int* coords, int* adjcoords) {

	// check if adjancent coordinates are in bounds
	if(!(pointOutOfBounds((float)adjcoords[0], (float)adjcoords[1]))) {
		heatmap->map[adjcoords[1]][adjcoords[0]] = map->map[adjcoords[1]][adjcoords[0]] + tolerance * map->map[coords[1]][coords[0]];
		//heatmap->map[coords[1]][coords[0]] = map->map[coords[1]][coords[0]] / tolerance;
		// heatmap->map[adjcoords[0]][adjcoords[1]] += map->map[adjcoords[0]][adjcoords[1]] + tolerance * map->map[coords[0]][coords[1]];
		// heatmap->map[coords[0]][coords[1]] += 1.0;
	}
}

void genHeatMap(Map* map, Map* heatmap) {
	printf("Generating heatmap...\n");
	int count = 1;
	// For each division
	int noise_i = 0;
	float center[2] = {0.0, 0.0};
	float progress = 0.0;
	float prev_progress = 0.0;
	for (float i = (float)MAPSIZE - 1; i > CYCLE_DIV; i = i / 4.0f)
	{

		progress = (CYCLE_DIV/i)*100.0;
		if(progress >= prev_progress+2.0) {
			printf("%f\n", progress);
			prev_progress = progress;
		} 

		count = count * 2;
		float x_offset = (float)(MAPSIZE - 1) / (float)count;
		float y_offset = (float)(MAPSIZE - 1) / (float)count;

		// For each quadrant/square
		for (int y = 0; y < count; y++)
		{
			for (int x = 0; x < count; x++)
			{
				Point pt = getPoint(x, y, x_offset, y_offset, center );
				//map->map[(int)pt.x][(int)pt.y];

				// Average quadrant
				float sum = 0.0; 
				for (int in_y = 0; in_y < y_offset; in_y++)
				{
					for (int in_x = 0; in_x < x_offset; in_x++)
					{
						sum += map->map[(int)pt.y + in_y][(int)pt.x + in_x];
					}
				}
				float average = sum / x_offset*y_offset;

				// Paint quadrant
				for (int in_y2 = 0; in_y2 < y_offset; in_y2++)
				{
					for (int in_x2 = 0; in_x2 < x_offset; in_x2++)
					{
						heatmap->map[(int)pt.y + in_y2][(int)pt.x + in_x2] += average*HEATMAP_DENSITY;
						// heatmap->map[(int)pt.y + in_y2][(int)pt.x + in_x2] = heatmap->map[(int)pt.y + in_y2][(int)pt.x + in_x2] / 10.0;
					}
				}


			}
		}

		// Decrease noise output
		// NOISEMUL = NOISEMUL/2.0f;
	}
}

void intoHeatMap(Map *map, Map *heatmap)
{

	for(int y=0; y<MAPSIZE; y++) {
		for(int x=0; x<MAPSIZE; x++) {

			int top[2] = {x, y - 1};
			int bottom[2] = {x, y + 1};
			int left[2] = {x - 1, y};
			int right[2] = {x + 1, y};
			int tLeft[2] = {x - 1, y - 1};
			int tRight[2] = {x + 1, y - 1};
			int bLeft[2] = {x - 1, y + 1};
			int bRight[2] = {x + 1, y + 1};

			int coords[2] = {x, y};

			// Not all due to overlap in interation
			floodCell(TOLERANCE, map, heatmap, coords, top);
			//floodCell(TOLERANCE, map, heatmap, coords, bottom);
			floodCell(TOLERANCE, map, heatmap, coords, left);
			//floodCell(TOLERANCE, map, heatmap, coords, right);
			floodCell(TOLERANCE, map, heatmap, coords, tLeft);
			//floodCell(TOLERANCE, map, heatmap, coords, tRight);
			//floodCell(TOLERANCE, map, heatmap, coords, bLeft);
			//floodCell(TOLERANCE, map, heatmap, coords, bRight);

		}
	}
}

float raw_noise(int x, int y) {
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

	return total;
}


float scalar = 1.0;
float noise(int x, int y) {
	// return perlin2((float)x, (float)y, 1.0, 1.0, 1.0);
	return raw_noise((int)((float)x * scalar), (int)((float)y * scalar));

}
