#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "image.h"
#include "lopta.h"
#include "bitmap_functions.h"
#define TIMER_ID 0

/* Makroi za imena fajlova sa teksturama. */
#define ZID "../Teksture/zid.bmp"
#define VODA "../Teksture/voda.bmp"
#define START_GAME "../Teksture/start_game.bmp"

/* Identifikatori tekstura. */
static GLuint names[3];

/* Struktura koja predstavlja jednu prepreku. */ 
typedef struct
{
    float x;
    float y;
    float z;
    int tip;
} Prepreka;

/* Nizovi koji sadrze koordinate prepreka. */
Prepreka prepreke_1[50];
Prepreka prepreke_2[50];

/* Brojaci za prepreke u oba niza. */
static int poz1;
static int poz2;

/* Identifikatori pocetka i kraja igrice, kao i identifikator za restartovanje. */ 
static int start = 0;
static int restart = 0;

/* Pomocne vrednosti prilikom kretanja loptice. */
static float brzina_lopte = 0.35;
static int ulevo  = 0 ;
static int udesno  = 0;

/* Identifikator koji sluzi za iscrtavanje scene na samom pocetku igre. */
static int prva_prepreka = 1;

/* Deklaracije callback funkcija. */
static void on_display(void);
static void on_reshape(int width, int height);
static void on_keyboard(unsigned char key, int x, int y);
static void on_release(unsigned char key, int x, int y);
static void move_objects();

/* Dimenzije ravni po kojima ide loptica. */
static float x_plane = 10;
static float y_plane = 1;
static float z_plane = 50;
static float x_plane2 = 10;
static float y_plane2 = 1;
static float z_plane2 = 150;

/* z koordinate ravni levo i desno od glavne ravni(dodate zbog tekstura). */
static float z_coord_left_first = 50;
static float z_coord_right_first = 50;
static float z_coord_left_second = 150;
static float z_coord_right_second = 150;

/* Duzina staze. */
static float duzina_staze = 100;

/* Tekuci skor. */
static int score = 0;

/* Pomocne funkcije za iscrtavanje delova scene. */
static void nacrtaj_ravan();
static void nacrtaj_prepreke(int tip);
static void postavi_prepreke(int tip);
static void postavi_prepreke_na_pocetku();

/* Pomocne funkcije za ispitivanje preseka lopte sa nekom od prepreka. */
static float rastojanje_lopta_prepreka();
static void upit_preseka();

/* Pomocna funkcija  za resetovanje parametara. */
static void postavi_pocetne_vrednosti();

/* Funkcija za posatavljanje tekstura. */
static void postavi_teksture();

/* Funkcija koja prikazuje tekuci rezultat. */
static void  prikazi_tekuci_rezultat();

/* Funkcije za postavaljanje ekrana na pocetku i kraju igrice. */
void start_game();
void game_over();

int main(int argc, char **argv)
{
    /* Inicijalizuje se GLUT. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    /* Kreira se prozor. */
    glutInitWindowSize(1100, 700);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Zig Zag");
    
    srand(time(NULL));

    /* Postavljaju se teksture. */
    postavi_teksture();
    
    /* Registruju se callback funkcije. */    
    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);
    glutKeyboardUpFunc(on_release);
    glutDisplayFunc(start_game);
    
    
    /* Obavlja se OpenGL inicijalizacija. */
    glClearColor(0, 0, 0.2, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

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
    gluLookAt(x_coord, y_coord + 5, z_coord - 5, x_coord, y_coord, z_coord + 7, 0, 1, 0);

    /* Postavlja se osvetljenje. */
    GLfloat light_position[] = {0, 1, 0, 0};
    GLfloat light_ambient[] = {0.5, 0.6, 0.5, 1.0};
    GLfloat light_diffuse[] = {0.8, 0.8, 0.9, 1};
    GLfloat light_specular[] = {1, 1, 1, 1};

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    
    /* U svakom koraku prikazujem tekuci rezultat. */
    prikazi_tekuci_rezultat();
    
    /* Iscrtava se staza. */
    nacrtaj_ravan();
    
    /* Iscrtava se lopta. */
    nacrtaj_loptu();
    
    /* Pocetno iscrtavanje prepreka na samom pocetku igre. */
    if (prva_prepreka){
        postavi_prepreke_na_pocetku();
        postavi_prepreke(2);
        prva_prepreka= 0;
    }
    else
    {
        
    
    /* Iscrtavam prepreke. */
    nacrtaj_prepreke(1);
    nacrtaj_prepreke(2);
    }
    
    /* Nova slika se salje na ekran. */
    glutSwapBuffers();
}

static void on_keyboard(unsigned char key, int x, int y){
    
    switch (key)
    {
    case 27:
        /* Igra se zavrsava. */
        exit(0);
        break;
    case 'S':
    case 's':
        /* Igra se startuje. */
        if (!start)
        {
            /* Podesava se pozadina i poziva se on_display funkcija. */
            glClearColor(0, 0, 0.2, 0);
            glutDisplayFunc(on_display);
            
            /* Funkcija koja translira loptu. */
            glutTimerFunc(50,move_objects, TIMER_ID);
            
            /* Identifikator da je startovana igrica. */
            start = 1;
        }
        break;
    case 'a':
    case 'A':
        /* Pomeranje ulevo, azurira se indikator za pomeranje ulevo. */
        ulevo = 1;
        glutPostRedisplay();
        break;
    case 'd':
    case 'D':
        /* Pomeranje udesno, azurira se indikator za pomeranje udesno. */
        udesno = 1;
        glutPostRedisplay();
        break;
        /* Pauziranje igre. */
    case 'p':
    case 'P':
        start = 0;
        break;
        /* Restartovanje igrice. */
    case 'r':
    case 'R':
        restart  = 1;
        glutPostRedisplay();
        break;
       
    }
}


/* Azuriranje vec realizovane vrednosti unete sa tastature. */
static void on_release(unsigned char key, int x, int y){
    switch (key)
    {
    case 'a':
    case 'A':
        ulevo-=1;
        break;
    case 'd':
    case 'D':
        udesno -= 1;
        break;
    }
}

/* Funkcija koja crta 2 ravni po kojima ide lopta, kao i ravni sa njihove obe  strane. */
static void nacrtaj_ravan(){
    
    /* Postavljanje prve ravni. */
    glPushMatrix();
    
    /* Dodavanje tekstura na prvu ravan. */
    glBindTexture(GL_TEXTURE_2D, names[0]);
    glEnable(GL_TEXTURE_2D);
    
    glBegin(GL_QUAD_STRIP);
    glNormal3f(0, 1, 0);
    int first = 0;
    int second = 0;
    for (int i = 0; i <= 5; i++)
    {
        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        glVertex3f(-5, -0.2, z_plane - 50 + i * 20);
        
        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        second = (second + 1) % 2;
        glVertex3f(5, -0.2, z_plane - 50 + i * 20);
    }   
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();

    /* Postavljanje druge ravni. */
    glPushMatrix();
    
    /* Dodavanje teksture na drugu ravan. */
    glBindTexture(GL_TEXTURE_2D, names[0]);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUAD_STRIP);
    glNormal3f(0, 1, 0);
    first = 0;
    second = 0;
    for (int i = 0; i <= 5; i++)
    {
        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        glVertex3f(-5, -0.2, z_plane2 - 50 + i * 20);
        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        second = (second + 1) % 2;
        glVertex3f(5, -0.2, z_plane2 - 50 + i * 20);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    /* Prva ravan desno od glavne i njene teksture. */
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, names[1]);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUAD_STRIP);
    glNormal3f(0, 1, 0);
    first = 0;
    second = 0;
    for (int i = 0; i <= 5; i++)
    {
        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        glVertex3f(-5, -0.2, z_coord_right_first - 50 + i * 20);

        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        second = (second + 1) % 2;
        glVertex3f(-35, -0.2, z_coord_right_first - 50 + i * 20);
    }

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
    
    /* Druga ravan desno od glavne i njene teksture. */
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, names[1]);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUAD_STRIP);
    glNormal3f(0, 1, 0);
    first = 0;
    second = 0;
    for (int i = 0; i <= 5; i++)
    {
        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        glVertex3f(-5, -0.2, z_coord_right_second - 50 + i * 20);

        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        second = (second + 1) % 2;
        glVertex3f(-35, -0.2, z_coord_right_second - 50 + i * 20);
    }

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();

    /* Prva ravan levo od glavne i njene teksture. */
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, names[1]);
    glEnable(GL_TEXTURE_2D);
        
    glBegin(GL_QUAD_STRIP);
    glNormal3f(0, 1, 0);
    first = 0;
    second = 0;
    for (int i = 0; i <= 5; i++)
    {
        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        glVertex3f(5, -0.2, z_coord_left_first - 50 + i * 20);

        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        second = (second + 1) % 2;
        glVertex3f(35, -0.2, z_coord_left_first - 50 + i * 20);
    }

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
    
     /* Druga ravan levo od glavne i njene teksture. */
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, names[1]);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUAD_STRIP);
    glNormal3f(0, 1, 0);
    first = 0;
    second = 0;
    for (int i = 0; i <= 5; i++)
    {
        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        glVertex3f(5, -0.2, z_coord_left_second - 50 + i * 20);

        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        second = (second + 1) % 2;
        glVertex3f(35, -0.2, z_coord_left_second - 50 + i * 20);
    }

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();

}

/*Pomeranje objekta se vrsi tako sto ne pomeramo objekat vec samu stazu 
 * sa preprekama ka objektu. */
static void move_objects(int id)
{
    
    if(id != TIMER_ID)
    {
        return;
    }
    
    if(restart)
    {
        /* Resetovanje pocetih vrednosti. */
        postavi_pocetne_vrednosti();
        glutPostRedisplay();
    }
    else
    {
    /* Ubrzavam lopticu za dati faktor. */
    brzina_lopte *= faktor_ubrzanja;
    
    /* Pomeram koordinate obe ravni ka objektu. */
       z_plane -= brzina_lopte;
       z_plane2 -= brzina_lopte;
       
    /* Pomeram koordiate ravni levo i desno od glavne. */
    z_coord_left_first -= brzina_lopte;
    z_coord_left_second -= brzina_lopte;
    z_coord_right_first -= brzina_lopte;
    z_coord_right_second -= brzina_lopte;


    /* Pomeram prepreke ka objektu. */
     for (int i = 0; i < poz1; i++)
         prepreke_1[i].z -= brzina_lopte;
     
     for (int i = 0; i < poz2; i++)
        prepreke_2[i].z -= brzina_lopte;

    /* Sprecavam da lopta sleti sa staze. */
     if (ulevo && x_coord < 4.5)
        x_coord += 0.25;
    
    if (udesno && x_coord > -4.5)
        x_coord -= 0.25;    

    
    /* Pocetak jedne ravni nadovezujemo na kraj druge, i tako sve u krug. */
    if (z_plane + 50 <= 0)
    {
        z_plane = 149;
        postavi_prepreke(1);
    }
    if (z_plane2 + 50 <= 0)
    {
        z_plane2 = 149;
        postavi_prepreke(2);
    }
    
    if (z_coord_left_first + 50 <= 0)
    {
        z_coord_left_first = 149;
    }
    if (z_coord_left_second + 50 <= 0)
    {
        z_coord_left_second = 149;
    }

    if (z_coord_right_first + 50 <= 0)
    {
        z_coord_right_first = 149;
    }
    if (z_coord_right_second + 50 <= 0)
    {
        z_coord_right_second = 149;
    }
    
    /* Rotiranje loptice. */
    ugao_rotacije += 30;
    if (ugao_rotacije >= 360)
        ugao_rotacije += -360;
    
    glRotated(ugao_rotacije, 1, 0, 0);

    
    /* Svaki put ispitujem da li je doslo  do kolizije. */
    upit_preseka();
    
    /* Ponovo se iscrtava ekran. */
    glutPostRedisplay();
    if (start)
        glutTimerFunc(50, move_objects, 0);
}
}

/* Iscrtavanje prepreka. */
static void nacrtaj_prepreke(int tip)
{
    int broj_prepreka;
    
    if (tip == 1)
        broj_prepreka = poz1;
    else
        broj_prepreka = poz2;

    for (int i = 0; i < broj_prepreka; i++)
    {
        Prepreka p;
        if (tip == 1)
            p = prepreke_1[i];
        else
            p = prepreke_2[i];
        
        /* Osvetljenje za prepreke. */
        GLfloat material_ambient[] = {0.22, 0.15, 0.05, 1.0};
        GLfloat material_diffuse[] = {0.7, 0.45, 0.2, 1.0};
        GLfloat material_specular[] = {0.4, 0.3, 0.15, 1.0};
        GLfloat shininess = 0.5;

        glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);

        /* Tip prepreke 0 je dijamant. */
        if (p.tip == 0)
        {
            glPushMatrix();
            glTranslatef(p.x, p.y, p.z);
            glScalef(0.7, 0.7, 0.7);
            glutSolidIcosahedron();
            glPopMatrix();
        }
        /* Inace se crta kocka. */
        else
        {
            glPushMatrix();
            glTranslatef(p.x, p.y, p.z);
            glScalef(2, 2, 2);
            glutSolidCube(1);
            glPopMatrix();
        }
    }
}

/* Postavljanje koordinata prepreka. */
static void postavi_prepreke(int tip)
{
    if (tip == 1)
        poz1 = 0;
    else
        poz2 = 0;

    /* Po ravni cu imati 10 redova sa preprekama.
     * Po redu cu imati max 4 prepreke. */
    int broj_prepreka;
    for (int i = 0; i < 10; i++)
    {
        broj_prepreka = (int)rand() % 5;
        if (broj_prepreka == 0)
            broj_prepreka = 2;
        int dijamant = 0;
        /* Niz koji govori koje su slobodne pozicije u jednom redu prepreka.*/
        int slobodne_pozicije[] = {1, 1, 1, 1, 1};
        for (int j = 0; j < broj_prepreka; j++)
        {
            Prepreka p;
            
            /* Niz koji oznacava x koordinate pozicija koje odgovaraju slobodnim pozicijama
             * u nizu slobodne_pozicije. */
            int x_koordinate_slobodnih_pozicija[] = {4, 2, 0, -2, -4};
            int pos = (int)rand() % 5;
            if (slobodne_pozicije[pos] == 1)
            {
                slobodne_pozicije[pos] = 0;
                
                /* Nasumicno biramo da li ce se crtati kocka ili dijamant sa podjednakom verovatnocom. */
                int t = (int)rand() % 2;
                if (t == 0 && !dijamant)
                {
                    p.tip = 0;
                    p.y = 0.5;
                    dijamant = 1;
                }
                else
                {
                    p.tip = 1;
                    p.y = 0.75;
                }

                p.x = x_koordinate_slobodnih_pozicija[pos];
                if (tip == 1)
                {
                    p.z = z_plane + 50 - i * 10;
                    prepreke_1[poz1++] = p;
                }
                    else
                {
                    p.z = z_plane2 + 50 - i * 10;
                    prepreke_2[poz2++] = p;
                }
            }
        }
    }
}

/* Funkcija koja se poziva samo na pocetku igrice,
 * vrsi pocetno iscrtavanje prepreka na drugi deo ravni 1. */
static void postavi_prepreke_na_pocetku()
{
    int broj_prepreka;
    
    for (int i = 0; i <= 2; i++)
    {
        broj_prepreka = (int)rand() % 5;
        if (broj_prepreka < 4)
            broj_prepreka++;
        int dijamant = 0;
        int slobodne_pozicije[] = {0, 0, 0, 0, 0};
        for (int j = 0; j < broj_prepreka; j++)
        {
            Prepreka o;
            int x_koordinate_slobodnih_pozicija[] = {4, 2, 0, -2, -4};
            int pos = (int)rand() % 5;
            if (slobodne_pozicije[pos] == 0)
            {
                slobodne_pozicije[pos] = 1;
                int t = (int)rand() % 2;
                if (t == 0 && !dijamant)
                {
                    o.tip = 0;
                    o.y = 0.5;
                    dijamant = 1;
                }
                else
                {
                    o.tip = 1;
                    o.y = 0.75;
                }
                o.x = x_koordinate_slobodnih_pozicija[pos];
                o.z = z_plane + 50 - i * 20;
                prepreke_1[poz1++] = o;
            }
        }
    }
}

/* Funkcija koja racuna rastojanje izmedju lopte i prepreke. */
static float rastojanje_lopta_prepreka(Prepreka p){
    
    float x = (p.x - x_coord) * (p.x - x_coord);
    float y = (p.y - y_coord) * (p.y - y_coord);
    float z = (p.z - z_coord) * (p.z - z_coord);


    return sqrt(x + y + z);
    
}

/* Funkcija koja racuna presek izmedju lopte i prepreke. */
static void upit_preseka()
{
    /* Ako smo na prvoj ravni, ispitujemo  preseke tj. kolizije. */
    if (z_plane < z_plane2)
    {
        for (int i = 0; i < poz1; i++)
        {
            Prepreka p = prepreke_1[i];
            if (rastojanje_lopta_prepreka(p) < 1.5)
            {
                /* Ako je dijamat, uvecava se skor za 1. */
                if (prepreke_1[i].tip == 0)
                {
                   score += 1;
                   /* Sklanjam dijamant da izgleda kao da se stopio sa loptom. */
                   prepreke_1[i].x = -100;
                }
                /* Ako je presek pozitivan, kraj igre. */
                else if (prepreke_1[i].tip == 1)
                {
                    start = 0;
                    glutDisplayFunc(game_over);
                    postavi_pocetne_vrednosti();
                    glutPostRedisplay();
            }
        }
    }
  }
    /* Analogno ispitujem preseke na ravni 2. */
    else
    {
        for (int i = 0; i < poz2; i++)
        {
            Prepreka p = prepreke_2[i];
            if (rastojanje_lopta_prepreka(p) < 1.22)
            {
                if (prepreke_2[i].tip == 0)
                {
                    score += 1;
                    prepreke_2[i].x = -100;

                }
                else if (prepreke_2[i].tip == 1)
                {
                    start = 0;
                    glutDisplayFunc(game_over);
                    postavi_pocetne_vrednosti();
                    glutPostRedisplay();
                }
            }
        }
    }
}

/* Funkcija za inicijalizaciju vrednosti pri restartovanju igrice. */
static void postavi_pocetne_vrednosti(){
    
        restart = 0;
        start = 0;
        
        brzina_lopte = 0.35;
        faktor_ubrzanja = 1.0005;;
        
        ulevo = 0 ;
        udesno = 0;
        
        prva_prepreka = 1;
        
        x_plane = 10;
        y_plane = 1;
        z_plane = 50;
        x_plane2 = 10;
        y_plane2 = 1;
        z_plane2 = 150;

        duzina_staze = 100;

        x_coord = 0;
        y_coord = 0.75;
        z_coord = 5;
        
        ugao_rotacije = 0;
        
        score = 0;
        
        poz1 = 0;
        poz2 = 0;
}

/* Funkcija za posatavljanje tekstura. */
static void postavi_teksture()
{
    
    Image *image;

    /* Ukljucuju se teksture. */
    glEnable(GL_TEXTURE_2D);

    glTexEnvf(GL_TEXTURE_ENV,
              GL_TEXTURE_ENV_MODE,
              GL_REPLACE);

    /*
     * Inicijalizuje se objekat koji ce sadrzati teksture ucitane iz
     * fajla.
     */
    image = image_init(0, 0);

    /* Generisu se identifikatori tekstura. */
    glGenTextures(3, names);
    
    /* Kreira se prva tekstura. */
    image_read(image, ZID);

    glBindTexture(GL_TEXTURE_2D, names[0]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
    
   
    /* Kreira se druga tekstura. */
    image_read(image, VODA);

    glBindTexture(GL_TEXTURE_2D, names[1]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
    
    /* Kreira se treca tekstura. */
    image_read(image, START_GAME);

    glBindTexture(GL_TEXTURE_2D, names[2]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);    
        
    
    /* Iskljucujemo aktivnu teksturu */
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Unistava se objekat za citanje tekstura iz fajla. */
    image_done(image);
    
}

/* Funkcija koja prikazuje tekuci rezultat. */
static void prikazi_tekuci_rezultat(){
    
    /* Karakterske promenljive za rasterizaciju rezultata. */
    char vrednost_na_ekranu[25] = "SCORE: ";
    char rezultat[25];
        
    sprintf(rezultat, " %d ", score);
    strcat(vrednost_na_ekranu, rezultat);

    bitmap_output_1(x_coord-3,y_coord+5,5,vrednost_na_ekranu,GLUT_BITMAP_TIMES_ROMAN_24);

}

/* Funkcija koja postavlja teksturu  na pocetku igre. */
void start_game(void)
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 2, 0,
              0, 0, 0,
              1, 0, 0);

    GLfloat light_position[] = {0, 0, 0, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, names[2]);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);

    glTexCoord2f(0, 0);
    glVertex3f(-1, 0, -1);

    glTexCoord2f(0, 1);
    glVertex3f(1, 0, -1);

    glTexCoord2f(1, 1);
    glVertex3f(1, 0, 1);

    glTexCoord2f(1, 0);
    glVertex3f(-1, 0, 1);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glutSwapBuffers();
}

/* Funkcija koja posatavlja tekst na ekran na kraju igre. */
void game_over(){


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 5,
              0, 0, 0,
              0, 1, 0);


    glDisable(GL_LIGHTING);
    glColor3f(1, 0, 0);
    bitmap_output(-1, 0.5, " GAME OVER ", GLUT_BITMAP_TIMES_ROMAN_24);

    glColor3f(0.8, 0.8, 0.8);
    bitmap_output(-4.9, 2.5, " Press - s - to start   ", GLUT_BITMAP_TIMES_ROMAN_24);
    bitmap_output(-4.9, 2.3, " Press - a - to go left ", GLUT_BITMAP_TIMES_ROMAN_24);
    bitmap_output(-4.9, 2.1, " Press - d- to go right ", GLUT_BITMAP_TIMES_ROMAN_24);
    bitmap_output(-4.9, -2.2, " Press - esc - for exit ", GLUT_BITMAP_TIMES_ROMAN_24);
    bitmap_output(-4.9, -2.4, " Press - p - to pause the game ", GLUT_BITMAP_TIMES_ROMAN_24);
    bitmap_output(-4.9, -2.6, " Press - r - to restart the game ", GLUT_BITMAP_TIMES_ROMAN_24);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);

    
    glutSwapBuffers();
}
