#include <iostream>

#include <cstring>

#include <vector>

void loadObj(const char *path, float *vertices, int *indices, int& totalv){
    FILE* obj;
	//fopen(obj, path, "r");
    obj = fopen(path, "r");
	int line = 3;
	int faceline = 0;
    	while (1) {
		char lineHeader[128];

		if (obj == 0) {
			exit(-1);
		}

		int res = fscanf(obj, "%s", lineHeader);
		if (res == EOF) {
			break;
		}

		if (strcmp(lineHeader, "#") == 0) {
			fscanf(obj, "%*[^\n]\n");
		}

		if (strcmp(lineHeader, "s") == 0) {
			fscanf(obj, "%*[^\n]\n");
		}

		if (strcmp(lineHeader, "o") == 0) {
			fscanf(obj, "%*[^\n]\n");
		}

		if (strcmp(lineHeader, "v") == 0) {
			//vertices.resize(vertices.size()+3);
			fscanf(obj, "%f %f %f \n", &vertices[line], &vertices[line+1], &vertices[line+2]);
			line+=3;
		}

        if (strcmp(lineHeader, "f") == 0) {
			fscanf(obj, "%d %d %d \n", &indices[faceline], &indices[faceline+1], &indices[faceline+2]);
			faceline+=3;
		}
	}
	fclose(obj);
	totalv = faceline/3;
}