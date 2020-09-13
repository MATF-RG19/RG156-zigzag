#include <GL/glut.h>


/* Deklaracije callback funkcija. */
static void on_display(void);
static void on_reshape(int width, int height);


int main(int argc, char **argv)
{
    /* Inicijalizuje se GLUT. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    /* Kreira se prozor. */
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Zig Zag");

    /* Registruju se callback funkcije. */
    glutDisplayFunc(on_display);
    glutReshapeFunc(on_reshape);
    /* Obavlja se OpenGL inicijalizacija. */
    glClearColor(0.75, 0.75, 0.75, 0);

    /* Program ulazi u glavnu petlju. */
    glutMainLoop();

    return 0;
}
static void on_reshape(int width, int height)
{
    /* Postavlja se viewport. */
    glViewport(0, 0, width, height);

    /* Postavljaju se parametri projekcije.*/
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 50);
}

static void on_display(void)
{
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Postavlja se kamera. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.5,2,-4,
                2,0,2,
                0.0,1.0,0.0);
    
    /* Iscrtavanje koordinatnog sistema. */
    glBegin(GL_LINES);
        // x osa -> plava
        glColor3f(0, 0, 1);
        glVertex3f(-1000, 0, 0);
        glVertex3f(1000, 0, 0);
        // y osa -> zelena
        glColor3f(0, 1, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 1000, 0);
        // z osa -> crvena
        glColor3f(1, 0, 0);
        glVertex3f(0, 0, -1000);
        glVertex3f(0, 0, 1000);
    glEnd();
 
    glutSwapBuffers();
}
