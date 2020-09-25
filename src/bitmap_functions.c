#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <string.h>

#include "bitmap_functions.h"

/*kod skinut sa interneta https://www.opengl.org/archives/resources/code/samples/glut_examples/examples/examples.html
*/

void bitmap_output(float x, float y, char *string, void *font)
{
    
    int len, i;
    glRasterPos2f(x, y);
    len = strlen(string);

    for (i = 0; i < len; i++)
    {

        glutBitmapCharacter(font, string[i]);
    }

}
void bitmap_output_1(float x, float y, float z, char *string, void *font)
{
    
    int len, i;
    glRasterPos3f(x, y,z);
    len = strlen(string);

    for (i = 0; i < len; i++)
    {

        glutBitmapCharacter(font, string[i]);
    }
     
}
