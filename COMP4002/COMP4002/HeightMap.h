#include "Renderable.h"

class HeightMap
{
public:
	HeightMap();
	HeightMap(const char* filename);
	float lookup(float x, float y);
	int width, length, channels;
	unsigned char *ht_map;
};

inline HeightMap::HeightMap()
{

}

inline HeightMap::HeightMap(const char* filename)
{	
	ht_map = SOIL_load_image(filename, &width, &length, &channels, SOIL_LOAD_L);
}

inline float HeightMap::lookup(float x, float z)
{
	if (x > 1.0f) return 0;
	if (z > 1.0f) return 0;
	if (x < 0.0f) return 0;
	if (z < 0.0f) return 0;
	
	int mapX = x * width;
	int mapZ = z * length;

	return ht_map[mapX * width + mapZ] / 255.0f;
}