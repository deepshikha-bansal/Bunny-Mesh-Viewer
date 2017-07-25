#include <stdio.h>
#define NUM_Faces 90000
#define NUM_Vertices  50000

struct Normal
{
	float x;
	float y;
	float z;
	int normalID;

};
typedef struct Normal normal;
struct Face
{
	int x;
	int y;
	int z;
	int faceID;
};
typedef struct Face face;
struct Vertex
{
	float x;
	float y;
	float z;
	int vertexID;
	normal Vertex_norm;
};
typedef struct Vertex vertex;
vertex vertices[NUM_Vertices];
face Faces[NUM_Faces];

