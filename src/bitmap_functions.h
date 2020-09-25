#ifndef bitmap_functions_h
#define bitmap_functions_h

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <assert.h>

void bitmap_output(float x, float y, char *string, void *font);
void bitmap_output_1(float x, float y, float z, char *string, void *font);

#endif
