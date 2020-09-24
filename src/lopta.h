#ifndef LOPTA_H
#define LOPTA_H

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>


/* Koordinate loptice. */
float x_coord;
float y_coord;
float z_coord;

/*Ugao za koji se rotira loptica. */
float ugao_rotacije;

/* Faktor ubrzanja loptice. */
float faktor_ubrzanja;

/* Funkcija koja iscrtava lopticu. */
void nacrtaj_loptu();


#endif
