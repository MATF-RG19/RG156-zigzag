#include "lopta.h"



/* Koordinate loptice. */
float x_coord = 0;
float y_coord = 0.75;
float z_coord = 5;

/*Ugao za koji se rotira loptica. */
float ugao_rotacije = 0;

/* Faktor ubrzanja loptice. */
float faktor_ubrzanja = 1.001;


/* Funkcija koja iscrtava loptu. */
void nacrtaj_loptu(){
    
    /* Osvetljenje za loptu. */ 
    GLfloat material_diffuse[] = {0.3, 0.8, 0.8, 1};
    GLfloat material_ambient[] = { 1.0, 0.1, 0.1, 1 };
    GLfloat material_specular[] = { 0.1 , 0.1,0.1, 0.5 };
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);    
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);
    
    glPushMatrix();
    glTranslatef(x_coord, y_coord + 1.6, z_coord);
    glutSolidSphere(0.5, 100, 100);
    glPopMatrix();
}
