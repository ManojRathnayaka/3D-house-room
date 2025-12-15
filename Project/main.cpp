#define _USE_MATH_DEFINES

#include <cmath>
#include <cstdio>
#include <glut.h>
#include <SOIL2.h>

// Camera Variables
bool mouseActive = true;
int lastMouseX = 0, lastMouseY = 0;
float yaw = 180.0f;
float pitch = 0.0f;
float cameraSpeed = 0.05f;

// Input State
bool keys[256] = { false };

// Scene Variables
GLboolean redFlag = GL_TRUE;
bool switchOne = false, switchLamp = false;
double windowHeight = 680, windowWidth = 1340;
double eyeX = 2.8, eyeY = 2.0, eyeZ = 20.0, refX = 0, refY = 0, refZ = 0;
double theta = 180.0, z = 8.05;

// Door Animation Variables
float doorAngle = 0.0f;
bool doorOpening = false;
bool doorClosing = false;
const float doorSpeed = 2.0f;

// Fan Animation Variables
float fanRotationAngle = 0.0f;
const float fanSpeed = 2.5f;
bool isFanOn = false;

bool curtainOpen = false;
bool curtainOpening = false;
bool curtainClosing = false;

// Scale: 1.0f = Fully Closed (flat), 0.1f = Fully Open (shrunk/bunched up)
float curtainScale = 1.0f;
const float curtainSpeed = 0.03f; // Speed of shrinking

// sunIntensity ranges from approx 0.2 (closed) to 1.0 (open)
float sunIntensity = 0.2f;

// Texture IDs
GLuint woodTexture;
GLuint posterTexture;
GLuint carpetTexture;
GLuint floorTexture;

GLUquadric* quad = NULL;

// Cube Vertices
static GLfloat v_cube[8][3] = {
    {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 3.0f}, {3.0f, 0.0f, 3.0f}, {3.0f, 0.0f, 0.0f},
    {0.0f, 3.0f, 0.0f}, {0.0f, 3.0f, 3.0f}, {3.0f, 3.0f, 3.0f}, {3.0f, 3.0f, 0.0f}
};

static GLubyte quadIndices[6][4] = {
    {0, 1, 2, 3}, {4, 5, 6, 7}, {5, 1, 2, 6},
    {0, 4, 7, 3}, {2, 3, 7, 6}, {1, 5, 4, 0}
};

// Normal Vector Helper
static void getNormal3p(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3) {
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;
    Ux = x2 - x1; Uy = y2 - y1; Uz = z2 - z1;
    Vx = x3 - x1; Vy = y3 - y1; Vz = z3 - z1;
    Nx = Uy * Vz - Uz * Vy;
    Ny = Uz * Vx - Ux * Vz;
    Nz = Ux * Vy - Uy * Vx;
    glNormal3f(Nx, Ny, Nz);
}

// Camera Update Logic
void updateCamera() {
    float radYaw = yaw * M_PI / 180.0f;
    float radPitch = pitch * M_PI / 180.0f;
    refX = eyeX + cos(radPitch) * sin(radYaw);
    refY = eyeY + sin(radPitch);
    refZ = eyeZ + cos(radPitch) * cos(radYaw);
}

// Material Helper
void setMaterial(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX, GLfloat ambY, GLfloat ambZ, GLfloat shine = 50) {
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0f };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_shininess[] = { shine };
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
}

// Basic Cube Drawer
void drawCube1(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX = 0, GLfloat ambY = 0, GLfloat ambZ = 0, GLfloat shine = 50, GLuint textureID = 0) {
    if (textureID != 0) {
        setMaterial(1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.8f, shine);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }
    else {
        setMaterial(difX, difY, difZ, ambX, ambY, ambZ, shine);
    }
    glBegin(GL_QUADS);
    for (GLint i = 0; i < 6; i++) {
        getNormal3p(v_cube[quadIndices[i][0]][0], v_cube[quadIndices[i][0]][1], v_cube[quadIndices[i][0]][2],
            v_cube[quadIndices[i][1]][0], v_cube[quadIndices[i][1]][1], v_cube[quadIndices[i][1]][2],
            v_cube[quadIndices[i][2]][0], v_cube[quadIndices[i][2]][1], v_cube[quadIndices[i][2]][2]);

        glTexCoord2f(0.0f, 0.0f); glVertex3fv(&v_cube[quadIndices[i][0]][0]);
        glTexCoord2f(1.0f, 0.0f); glVertex3fv(&v_cube[quadIndices[i][1]][0]);
        glTexCoord2f(1.0f, 1.0f); glVertex3fv(&v_cube[quadIndices[i][2]][0]);
        glTexCoord2f(0.0f, 1.0f); glVertex3fv(&v_cube[quadIndices[i][3]][0]);
    }
    glEnd();
    if (textureID != 0) {
        glDisable(GL_TEXTURE_2D);
    }
}

// Trapezoid Data
static GLfloat v_trapezoid[8][3] = {
    {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 3.0f}, {3.0f, 0.0f, 3.0f}, {3.0f, 0.0f, 0.0f},
    {0.5f, 3.0f, 0.5f}, {0.5f, 3.0f, 2.5f}, {2.5f, 3.0f, 2.5f}, {2.5f, 3.0f, 0.5f}
};

static GLubyte TquadIndices[6][4] = {
    {0, 1, 2, 3}, {4, 5, 6, 7}, {5, 1, 2, 6},
    {0, 4, 7, 3}, {2, 3, 7, 6}, {1, 5, 4, 0}
};

// Trapezoid Drawer
void drawTrapezoid(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX, GLfloat ambY, GLfloat ambZ, GLfloat shine = 50) {
    setMaterial(difX, difY, difZ, ambX, ambY, ambZ, shine);
    if (switchLamp == GL_TRUE) {
        GLfloat mat_emission[] = { difX, difY, difZ, 1.0f };
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    }
    glBegin(GL_QUADS);
    for (GLint i = 0; i < 6; i++) {
        getNormal3p(v_trapezoid[TquadIndices[i][0]][0], v_trapezoid[TquadIndices[i][0]][1], v_trapezoid[TquadIndices[i][0]][2],
            v_trapezoid[TquadIndices[i][1]][0], v_trapezoid[TquadIndices[i][1]][1], v_trapezoid[TquadIndices[i][1]][2],
            v_trapezoid[TquadIndices[i][2]][0], v_trapezoid[TquadIndices[i][2]][1], v_trapezoid[TquadIndices[i][2]][2]);

        glVertex3fv(&v_trapezoid[TquadIndices[i][0]][0]);
        glVertex3fv(&v_trapezoid[TquadIndices[i][1]][0]);
        glVertex3fv(&v_trapezoid[TquadIndices[i][2]][0]);
        glVertex3fv(&v_trapezoid[TquadIndices[i][3]][0]);
    }
    glEnd();
}

// Polygon Helper
void polygon(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX, GLfloat ambY, GLfloat ambZ, GLfloat shine) {
    setMaterial(difX, difY, difZ, ambX, ambY, ambZ, shine);
    glBegin(GL_POLYGON);
    glVertex2f(0, 0); glVertex2f(6, 0); glVertex2f(5.8f, 1); glVertex2f(5.2f, 2);
    glVertex2f(5, 2.2f); glVertex2f(4, 2.8f); glVertex2f(3, 3); glVertex2f(2, 2.8f);
    glVertex2f(1, 2.2f); glVertex2f(0.8f, 2); glVertex2f(0.2f, 1);
    glEnd();
}

// Polygon Outline Helper
void polygonLine(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX, GLfloat ambY, GLfloat ambZ, GLfloat shine) {
    setMaterial(difX, difY, difZ, ambX, ambY, ambZ, shine);
    glBegin(GL_LINE_STRIP);
    glVertex2f(6, 0); glVertex2f(5.8f, 1); glVertex2f(5.2f, 2); glVertex2f(5, 2.2f);
    glVertex2f(4, 2.8f); glVertex2f(3, 3); glVertex2f(2, 2.8f); glVertex2f(1, 2.2f);
    glVertex2f(0.8f, 2); glVertex2f(0.2f, 1); glVertex2f(0, 0);
    glEnd();
}

// Sphere Drawer
void drawSphere(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX, GLfloat ambY, GLfloat ambZ, GLfloat shine = 90) {
    setMaterial(difX, difY, difZ, ambX, ambY, ambZ, shine);
    glutSolidSphere(3.0, 20, 16);
}

// Texture Loading
void loadAllTextures() {
    woodTexture = SOIL_load_OGL_texture("wood.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
    if (!woodTexture) printf("Wood texture loading failed: %s\n", SOIL_last_result());

    carpetTexture = SOIL_load_OGL_texture("carpet.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
    if (!carpetTexture) printf("Carpet texture loading failed: %s\n", SOIL_last_result());

    floorTexture = SOIL_load_OGL_texture("floor.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
    if (!floorTexture) printf("Floor texture loading failed: %s\n", SOIL_last_result());

    posterTexture = SOIL_load_OGL_texture("image.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
    if (!posterTexture) printf("Poster texture (image.jpg) loading failed: %s\n", SOIL_last_result());
}

// Custom Textured Cube Drawer
void drawTexturedCube(GLfloat width, GLfloat height, GLfloat depth, GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX = 0.2f, GLfloat ambY = 0.1f, GLfloat ambZ = 0.05f, GLfloat shine = 30) {
    setMaterial(difX, difY, difZ, ambX, ambY, ambZ, shine);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    glBegin(GL_QUADS);

    // Front
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, depth);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(width, 0.0f, depth);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(width, height, depth);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, height, depth);

    // Back
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, height, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(width, height, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(width, 0.0f, 0.0f);

    // Top
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, height, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, height, depth);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(width, height, depth);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(width, height, 0.0f);

    // Bottom
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(width, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(width, 0.0f, depth);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 0.0f, depth);

    // Right
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(width, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(width, height, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(width, height, depth);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(width, 0.0f, depth);

    // Left
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 0.0f, depth);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, height, depth);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, height, 0.0f);

    glEnd();
    glDisable(GL_TEXTURE_2D);
}

// Door Object
void drawHouseDoor() {
    glPushMatrix();
    glTranslatef(2.0f, -1.0f, 14.8f);
    glScalef(1.1f, 1.1f, 1.0f);

    // Left Post
    glPushMatrix();
    glTranslatef(-0.1f, 0.0f, -0.15f);
    drawTexturedCube(0.2f, 4.0f, 0.6f, 0.4f, 0.2f, 0.1f);
    glPopMatrix();

    // Right Post
    glPushMatrix();
    glTranslatef(1.6f, 0.0f, -0.15f);
    drawTexturedCube(0.2f, 4.0f, 0.6f, 0.4f, 0.2f, 0.1f);
    glPopMatrix();

    // Top Header
    glPushMatrix();
    glTranslatef(-0.1f, 3.8f, -0.15f);
    drawTexturedCube(1.9f, 0.2f, 0.6f, 0.4f, 0.2f, 0.1f);
    glPopMatrix();

    // Rotating Door Panel
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.1f);
    glRotatef(doorAngle, 0.0f, 1.0f, 0.0f);
    drawTexturedCube(1.6f, 3.8f, 0.1f, 0.6f, 0.4f, 0.2f);

    // Door Handle
    glPushMatrix();
    glTranslatef(1.45f, 1.8f, 0.15f);
    setMaterial(0.8f, 0.7f, 0.1f, 0.4f, 0.35f, 0.05f, 100);
    glutSolidSphere(0.08, 20, 20);
    glPopMatrix();

    glPopMatrix();
    glPopMatrix();
}

// Door Animation Logic
void updateDoorAnimation()
{
    if (doorOpening && doorAngle < 90.0f) {
        doorAngle += doorSpeed;
        if (doorAngle >= 90.0f) {
            doorAngle = 90.0f;
            doorOpening = false;
        }
        glutPostRedisplay();
    }
    else if (doorClosing && doorAngle > 0.0f) {
        doorAngle -= doorSpeed;
        if (doorAngle <= 0.0f) {
            doorAngle = 0.0f;
            doorClosing = false;
        }
        glutPostRedisplay();
    }
}

// Cupboard Object
void cupboard() {
    glPushMatrix();
    glTranslatef(-1.0f, 0.0f, -1.0f);

    // Body
    glPushMatrix();
    glTranslatef(4, 0, 4.4f);
    glScalef(0.5f, 1, 0.5f);
    drawCube1(0.5f, 0.2f, 0.2f, 0.25f, 0.1f, 0.1f);
    glPopMatrix();

    // Striplines
    glPushMatrix();
    glTranslatef(4, 1, 5.9f);
    glScalef(0.5f, 0.01f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4, 0.5f, 5.9f);
    glScalef(0.5f, 0.01f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4, 0, 5.9f);
    glScalef(0.5f, 0.01f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5.5f, 0, 5.9f);
    glScalef(0.01f, 1, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.75f, 1, 5.9f);
    glScalef(0.01f, 0.67f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4, 0, 5.9f);
    glScalef(0.01f, 1, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    // Handles
    glPushMatrix();
    glTranslatef(5, 1.4f, 5.9f);
    glScalef(0.02f, 0.18f, 0.01f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5.02f, 1.9f, 5.91f);
    glScalef(0.02f, 0.02f, 0.01f);
    drawSphere(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.5f, 1.4f, 5.9f);
    glScalef(0.02f, 0.18f, 0.01f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.52f, 1.9f, 5.91f);
    glScalef(0.02f, 0.02f, 0.01f);
    drawSphere(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f, 10);
    glPopMatrix();

    // Drawer Handles
    glPushMatrix();
    glTranslatef(4.5f, 0.7f, 5.9f);
    glScalef(0.16f, 0.02f, 0.01f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.5f, 0.25f, 5.9f);
    glScalef(0.16f, 0.02f, 0.01f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();
    glPopMatrix();
}

// Room Structure
void room() {
    // Back Wall
    glPushMatrix();
    glTranslatef(-1.5f, -1, .5f);
    glScalef(5, 2, 0.1f);
    drawCube1(1, 0.8f, 0.7f, 0.5f, 0.4f, 0.35f);
    glPopMatrix();

    // Left Wall
    glPushMatrix();
    glTranslatef(-4.5f, -1, 0);
    glScalef(1, 2, 5);
    drawCube1(1, 0.8f, 0.7f, 0.5f, 0.4f, 0.35f);
    glPopMatrix();

    // Right Wall
    glPushMatrix();
    glTranslatef(8, -1, 0);
    glScalef(0.2f, 2, 5);
    drawCube1(1, 0.8f, 0.7f, 0.5f, 0.4f, 0.35f);
    glPopMatrix();

    // Ceiling
    glPushMatrix();
    glTranslatef(-2, 5.1f, 0);
    glScalef(5, 0.1f, 7);
    drawCube1(1.0f, 0.9f, 0.8f, 0.5f, 0.45f, 0.4f);
    glPopMatrix();

    // Floor
    glPushMatrix();
    glScalef(5, 0.1f, 7);
    glTranslatef(-1, -5, 0);
    drawCube1(1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.8f, 50, floorTexture);
    glPopMatrix();

    // Front Wall Sections
    glPushMatrix();
    glTranslatef(-1.5f, -1.0f, 14.8f);
    glScalef(3.5f / 3.0f, 6.1f / 3.0f, 0.1f);
    drawCube1(1, 0.8f, 0.7f, 0.5f, 0.4f, 0.35f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(3.76f, -1.0f, 14.8f);
    glScalef(4.24f / 3.0f, 6.1f / 3.0f, 0.1f);
    drawCube1(1, 0.8f, 0.7f, 0.5f, 0.4f, 0.35f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.0f, 3.18f, 14.8f);
    glScalef(1.76f / 3.0f, 1.92f / 3.0f, 0.1f);
    drawCube1(1, 0.8f, 0.7f, 0.5f, 0.4f, 0.35f);
    glPopMatrix();
}

// Bed Object
void bed() {
    // Headboard
    glPushMatrix();
    glScalef(0.1f, 0.5f, 0.9f);
    glTranslatef(-2, -0.5f, 6.2f);
    drawCube1(0.5f, 0.2f, 0.2f, 0.25f, 0.1f, 0.1f);
    glPopMatrix();

    // Mattress
    glPushMatrix();
    glScalef(1, 0.2f, 0.9f);
    glTranslatef(0, -0.5f, 6.2f);
    drawCube1(0.824f, 0.706f, 0.549f, 0.412f, 0.353f, 0.2745f);
    glPopMatrix();

    // Pillow (Right)
    glPushMatrix();
    glTranslatef(0.5f, 0.5f, 6);
    glRotatef(20, 0, 0, 1);
    glScalef(0.1f, 0.15f, 0.28f);
    drawCube1(0.5f, 0.7f, 0.9f, 0.25f, 0.35f, 0.45f);
    glPopMatrix();

    // Pillow (Left)
    glPushMatrix();
    glTranslatef(0.5f, 0.5f, 7.2f);
    glRotatef(22, 0, 0, 1);
    glScalef(0.1f, 0.15f, 0.28f);
    drawCube1(0.5f, 0.7f, 0.9f, 0.25f, 0.35f, 0.45f);
    glPopMatrix();

    // Blanket
    glPushMatrix();
    glTranslatef(1.4f, 0.45f, 5.5f);
    glScalef(0.5f, 0.05f, 0.95f);
    drawCube1(0.7f, 0.1f, 0.1f, 0.35f, 0.05f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.4f, -0.3f, 8.16f);
    glScalef(0.5f, 0.25f, 0.05f);
    drawCube1(0.7f, 0.1f, 0.1f, 0.35f, 0.05f, 0.05f);
    glPopMatrix();
}

// Bedside Drawer Object
void bedsideDrawer() {
    // Body
    glPushMatrix();
    glTranslatef(0.5f, -0.1f, 8.7f);
    glScalef(0.12f, 0.2f, 0.23f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    // Drawer Face
    glPushMatrix();
    glTranslatef(0.88f, 0, 8.8f);
    glScalef(0.0001f, 0.11f, 0.18f);
    drawCube1(0.3f, 0.2f, 0.2f, 0.15f, 0.1f, 0.1f);
    glPopMatrix();

    // Knob
    glPushMatrix();
    glTranslatef(0.9f, 0.15f, 9.05f);
    glScalef(0.01f, 0.02f, 0.02f);
    drawSphere(0.3f, 0.1f, 0.0f, 0.15f, 0.05f, 0.0f);
    glPopMatrix();
}

// Lamp Object
void lamp() {
    // Base
    glPushMatrix();
    glTranslatef(.6f, 0.5f, 8.95f);
    glScalef(0.07f, 0.02f, 0.07f);
    drawCube1(0, 0, 1, 0, 0, 0.5f);
    glPopMatrix();

    // Stand
    glPushMatrix();
    glTranslatef(.7f, 0.35f, 9.05f);
    glScalef(0.01f, 0.2f, 0.01f);
    drawCube1(1, 0, 0, 0.5f, 0.0f, 0.0f);
    glPopMatrix();

    // Shade
    glPushMatrix();
    glTranslatef(.6f, 0.9f, 8.9f);
    glScalef(0.08f, 0.09f, 0.08f);
    drawTrapezoid(0.000f, 0.000f, 0.545f, 0, 0, 0.2725f);
    glPopMatrix();
}

// Poster Object
void drawPoster() {
    glPushMatrix();
    glTranslatef(-1.49f, 1.5f, 11.0f);

    float height = 1.95f;
    float width = 2.4f;
    float frameThickness = 0.1f;
    float frameDepth = 0.05f;

    GLfloat frameDif[] = { 0.3f, 0.15f, 0.05f };
    GLfloat frameAmb[] = { 0.15f, 0.075f, 0.025f };

    // Frames
    glPushMatrix();
    glTranslatef(0.0f, -frameThickness, -frameThickness);
    glScalef(frameDepth / 3.0f, frameThickness / 3.0f, (width + 2 * frameThickness) / 3.0f);
    drawCube1(frameDif[0], frameDif[1], frameDif[2], frameAmb[0], frameAmb[1], frameAmb[2]);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, height, -frameThickness);
    glScalef(frameDepth / 3.0f, frameThickness / 3.0f, (width + 2 * frameThickness) / 3.0f);
    drawCube1(frameDif[0], frameDif[1], frameDif[2], frameAmb[0], frameAmb[1], frameAmb[2]);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, -frameThickness, -frameThickness);
    glScalef(frameDepth / 3.0f, (height + 2 * frameThickness) / 3.0f, frameThickness / 3.0f);
    drawCube1(frameDif[0], frameDif[1], frameDif[2], frameAmb[0], frameAmb[1], frameAmb[2]);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, -frameThickness, width);
    glScalef(frameDepth / 3.0f, (height + 2 * frameThickness) / 3.0f, frameThickness / 3.0f);
    drawCube1(frameDif[0], frameDif[1], frameDif[2], frameAmb[0], frameAmb[1], frameAmb[2]);
    glPopMatrix();

    // Canvas
    setMaterial(1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.8f, 20.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, posterTexture);

    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 0.0f, width);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, height, width);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, height, 0.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

// Carpet Object
void drawCarpet() {
    glPushMatrix();
    glTranslatef(3.0f, -0.199f, 7.0f);
    setMaterial(1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.8f, 10.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, carpetTexture);

    float width = 3.9f;
    float depth = 5.1f;

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, depth);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(width, 0.0f, depth);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(width, 0.0f, 0.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

// Wardrobe Object
void wardrobe() {
    // Body
    glPushMatrix();
    glTranslatef(0, 0, 4);
    glScalef(0.12f, 0.6f, 0.4f);
    drawCube1(0.3f, 0.1f, 0, 0.15f, 0.05f, 0);
    glPopMatrix();

    // Drawers
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glTranslatef(0.36f, 1.4f - (i * 0.4f), 4.05f);
        glScalef(0.0001f, 0.11f, 0.38f);
        drawCube1(0.5f, 0.2f, 0.2f, 0.25f, 0.1f, 0.1f);
        glPopMatrix();

        // Handle
        glPushMatrix();
        glTranslatef(0.37f, 1.5f - (i * 0.4f), 4.3f);
        glScalef(0.01f, 0.03f, 0.2f);
        drawCube1(0.3f, 0.1f, 0, 0.15f, 0.05f, 0.0f);
        glPopMatrix();
    }
}

// Dressing Table Object
void dressingTable() {
    glPushMatrix();
    glTranslatef(-0.5f, 0.0f, -0.2f);

    // Side Bodies
    glPushMatrix();
    glTranslatef(5.9f, 0, 4.6f);
    glScalef(0.2f, 0.2f, 0.2f);
    drawCube1(0.545f, 0.271f, 0.075f, 0.2725f, 0.1355f, 0.0375f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(7, 0, 4.6f);
    glScalef(0.2f, 0.2f, 0.2f);
    drawCube1(0.545f, 0.271f, 0.075f, 0.2725f, 0.1355f, 0.0375f);
    glPopMatrix();

    // Upper Body
    glPushMatrix();
    glTranslatef(5.9f, 0.6f, 4.6f);
    glScalef(0.57f, 0.1f, 0.2f);
    drawCube1(0.545f, 0.271f, 0.075f, 0.2725f, 0.1355f, 0.0375f);
    glPopMatrix();

    // Stripes and Handles
    glPushMatrix();
    glTranslatef(5.9f, 0.6f, 5.2f);
    glScalef(0.57f, 0.01f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5.9f, 0.9f, 5.2f);
    glScalef(0.57f, 0.01f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(6.5f, 0.75f, 5.2f);
    glScalef(0.16f, 0.02f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(6.4f, 0.1f, 5.2f);
    glScalef(0.02f, 0.13f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(7.1f, 0.1f, 5.2f);
    glScalef(0.02f, 0.13f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    // Mirrors
    glPushMatrix();
    glTranslatef(6.2f, 0.9f, 4.7f);
    glScalef(0.36f, 0.5f, 0.0001f);
    drawCube1(0.690f, 0.878f, 0.902f, 0.345f, 0.439f, 0.451f, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5.92f, 0.9f, 4.7f);
    glScalef(0.1f, 0.48f, 0.0001f);
    drawCube1(0.690f, 0.878f, 0.902f, 0.345f, 0.439f, 0.451f, 10);
    glPopMatrix();

    // Mirror Borders and Stripes
    glPushMatrix();
    glTranslatef(5.92f, 0.9f, 4.71f);
    glScalef(0.019f, 0.48f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(6.17f, 0.9f, 4.71f);
    glScalef(0.019f, 0.48f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5.92f, 0.9f, 4.71f);
    glScalef(0.55f, 0.019f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5.92f, 2.3f, 4.71f);
    glScalef(0.1f, 0.019f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(7.25f, 0.9f, 4.7f);
    glScalef(0.1f, 0.48f, 0.0001f);
    drawCube1(0.690f, 0.878f, 0.902f, 0.345f, 0.439f, 0.451f, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(7.25f, 2.3f, 4.71f);
    glScalef(0.1f, 0.019f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(7.25f, 0.9f, 4.71f);
    glScalef(0.019f, 0.48f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(7.5f, 0.9f, 4.71f);
    glScalef(0.019f, 0.48f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    // Mirror Top Polygon
    glPushMatrix();
    glTranslatef(6.2f, 2.4f, 4.7f);
    glScalef(0.18f, 0.18f, 2);
    polygon(0.690f, 0.878f, 0.902f, 0.345f, 0.439f, 0.451f, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(6.2f, 2.4f, 4.71f);
    glScalef(.18f, .18f, 1);
    polygonLine(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f, 50);
    glPopMatrix();
    glPopMatrix();
}

void Clock() {
    // Clock Body
    glPushMatrix();
    glTranslatef(-1.49f, 1.8f, 7.87f);
    glScalef(0.08f, 0.25f, 0.1f);
    drawCube1(0.545f, 0.271f, 0.075f, 0.271f, 0.1335f, 0.0375f, 50);
    glPopMatrix();

    // Clock Face
    glPushMatrix();
    glTranslatef(-1.42f, 1.9f, 7.9f);
    glScalef(0.06f, 0.2f, 0.08f);
    drawCube1(1.000f, 0.894f, 0.710f, 1.000f, 0.894f, 0.710f);
    glPopMatrix();

    // Hands 
    glPushMatrix();
    glTranslatef(-1.24f, 2.18f, 8.01f);
    glRotatef(45, 1, 0, 0);
    glScalef(0.0001f, 0.01f, 0.04f);
    drawCube1(0, 0, 0, 0, 0, 0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1.24f, 2.18f, 8.01f);
    glRotatef(90, 1, 0, 0);
    glScalef(0.0001f, 0.012f, 0.08f);
    drawCube1(0, 0, 0, 0, 0, 0);
    glPopMatrix();

    // pendulum
    glPushMatrix();
    glTranslatef(-1.29f, 2.0f, 8.05f);
    glRotatef((GLfloat)theta, 1, 0, 0);

    glPushMatrix();
    glScalef(0.0001f, 0.2f, 0.03f);
    drawCube1(0.8f, 0.7f, 0.1f, 0.4f, 0.35f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.02f, 0.6f, 0.05f);
    glScalef(0.035f, 0.035f, 0.035f);
    drawSphere(0.8f, 0.7f, 0.1f, 0.4f, 0.35f, 0.05f, 10);
    glPopMatrix();
    glPopMatrix();
}

void window() {
    glPushMatrix();
    // Position on the right wall
    glTranslatef(7.95f, 2.5f, 9.0f);
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);

    float winWidth = 3.4f;
    float winHeight = 2.8f;
    float frameThick = 0.15f;

    // The Sky
    float glow = sunIntensity * 0.8f;
    if (glow > 1.0f) glow = 1.0f;
    GLfloat skyColor[] = { 0.529f, 0.808f, 0.922f };
    GLfloat skyEmission[] = { skyColor[0] * glow, skyColor[1] * glow, skyColor[2] * glow, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, skyColor);
    glMaterialfv(GL_FRONT, GL_EMISSION, skyEmission);

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-winWidth / 2, -winHeight / 2, -0.02f); // Fixed Z
    glVertex3f(winWidth / 2, -winHeight / 2, -0.02f);
    glVertex3f(winWidth / 2, winHeight / 2, -0.02f);
    glVertex3f(-winWidth / 2, winHeight / 2, -0.02f);
    glEnd();

    // Turn off emission
    GLfloat no_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);

    // Frame & Bars
    setMaterial(0.4f, 0.25f, 0.1f, 0.2f, 0.1f, 0.05f);

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.05f);

    // Outer Frame
    glPushMatrix(); glTranslatef(0, winHeight / 2 + frameThick / 2, 0); glScalef(winWidth + frameThick * 2, frameThick, frameThick); glutSolidCube(1.0); glPopMatrix();
    glPushMatrix(); glTranslatef(0, -winHeight / 2 - frameThick / 2, 0); glScalef(winWidth + frameThick * 2, frameThick, frameThick); glutSolidCube(1.0); glPopMatrix();
    glPushMatrix(); glTranslatef(-winWidth / 2 - frameThick / 2, 0, 0); glScalef(frameThick, winHeight, frameThick); glutSolidCube(1.0); glPopMatrix();
    glPushMatrix(); glTranslatef(winWidth / 2 + frameThick / 2, 0, 0); glScalef(frameThick, winHeight, frameThick); glutSolidCube(1.0); glPopMatrix();

    // Bars
    glPushMatrix(); glScalef(0.1f, winHeight, 0.05f); glutSolidCube(1.0); glPopMatrix();
    glPushMatrix(); glScalef(winWidth, 0.1f, 0.05f); glutSolidCube(1.0); glPopMatrix();

    glPopMatrix(); // End Frame Shift

    // Curtains
    setMaterial(0.6f, 0.1f, 0.1f, 0.3f, 0.05f, 0.05f, 10);

    float curtainTotalWidth = winWidth + 1.0f;
    float curtainHeight = winHeight + 1.2f;
    float curtainYOffset = 0.1f;

    float fullPanelWidth = curtainTotalWidth / 2.0f;
    float currentPanelWidth = fullPanelWidth * curtainScale;

    // Left Panel
    glPushMatrix();
    glTranslatef((-curtainTotalWidth / 2.0f) + (currentPanelWidth / 2.0f), curtainYOffset, 0.2f);
    glScalef(currentPanelWidth, curtainHeight, 0.1f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Right Panel
    glPushMatrix();
    glTranslatef((curtainTotalWidth / 2.0f) - (currentPanelWidth / 2.0f), curtainYOffset, 0.2f);
    glScalef(currentPanelWidth, curtainHeight, 0.1f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Rod
    setMaterial(0.2f, 0.2f, 0.2f, 0.1f, 0.1f, 0.1f, 50);
    glPushMatrix();
    glTranslatef(0.0f, curtainYOffset + (curtainHeight / 2.0f) - 0.1f, 0.25f);
    glScalef(curtainTotalWidth + 0.2f, 0.05f, 0.05f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();
}

// Stool Object
void drawWoodenStool() {
    glPushMatrix();
    glTranslatef(5.0f, -0.2f, 10.0f);

    // Seat
    glPushMatrix();
    glTranslatef(-0.45f, 0.8f, -0.45f);
    glScalef(0.3f, 0.05f, 0.3f);
    drawCube1(1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.8f, 30, woodTexture);
    glPopMatrix();

    // Legs
    float legPositions[4][2] = { {-0.4f, -0.4f}, {0.25f, -0.4f}, {-0.4f, 0.25f}, {0.25f, 0.25f} };
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glTranslatef(legPositions[i][0], 0.0f, legPositions[i][1]);
        glScalef(0.05f, 0.28f, 0.05f);
        drawCube1(1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.8f, 30, woodTexture);
        glPopMatrix();
    }
    glPopMatrix();
}

// Ceiling Fan Object
void drawCeilingFan() {
    const float centerX = 0.3f;
    const float centerZ = 0.3f;

    glPushMatrix();
    glTranslatef(2.5f, 5.0f, 7.5f);

    // Mount
    glPushMatrix();
    glTranslatef(centerX, 0.0f, centerZ);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    setMaterial(0.8f, 0.8f, 0.8f, 0.4f, 0.4f, 0.4f);
    gluCylinder(quad, 0.15, 0.05, 0.2, 32, 32);
    gluDisk(quad, 0.0, 0.15, 32, 1);
    glPopMatrix();

    // Rod
    glPushMatrix();
    glTranslatef(centerX, -0.2f, centerZ);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    setMaterial(0.7f, 0.7f, 0.7f, 0.35f, 0.35f, 0.35f);
    gluCylinder(quad, 0.03, 0.03, 0.8, 32, 32);
    glPopMatrix();

    // Motor
    glPushMatrix();
    glTranslatef(centerX, -1.0f, centerZ);
    setMaterial(0.9f, 0.9f, 0.9f, 0.45f, 0.45f, 0.45f);
    glScalef(1.0f, 0.6f, 1.0f);
    gluSphere(quad, 0.25, 32, 32);
    glPopMatrix();

    // Blades
    glPushMatrix();
    glTranslatef(centerX, -1.1f, centerZ);
    glRotatef(fanRotationAngle, 0.0f, 1.0f, 0.0f);
    setMaterial(0.9f, 0.9f, 0.9f, 0.45f, 0.45f, 0.45f, 50);

    for (int i = 0; i < 4; ++i) {
        glPushMatrix();
        glRotatef(i * 90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.35f, 0.0f, 0.0f);
        glScalef(0.6f, 0.02f, 0.15f);
        gluSphere(quad, 1.0, 32, 32);
        glPopMatrix();
    }
    glPopMatrix();
    glPopMatrix();
}

void vaseWithFlowers() {
    glPushMatrix();
    glTranslatef(5.6f, 0.9f, 4.75f); // On dressing table

    // Vase body (tapered cylinder)
    glRotatef(-90, 1, 0, 0);
    setMaterial(0.5f, 0.5f, 1.0f, 0.2f, 0.2f, 0.2f, 80);
    gluCylinder(quad, 0.08, 0.15, 0.3, 24, 1);
    gluDisk(quad, 0, 0.08, 24, 1);

    // Flowers (spheres on stems)
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glRotatef(i * 40.0f - 20, 0, 0, 1);

        // Stem
        setMaterial(0.1f, 0.4f, 0.1f, 0.05f, 0.2f, 0.05f);
        gluCylinder(quad, 0.01, 0.01, 0.5, 8, 1);

        // Flower head
        glTranslatef(0, 0, 0.5f);
        float colors[3][3] = { {0.9f,0.2f,0.2f}, {0.9f,0.7f,0.2f}, {0.8f,0.2f,0.8f} };
        setMaterial(colors[i][0], colors[i][1], colors[i][2],
            colors[i][0] * 0.5f, colors[i][1] * 0.5f, colors[i][2] * 0.5f);
        gluSphere(quad, 0.06, 12, 12);
        glPopMatrix();
    }
    glPopMatrix();
}

// Light Bulb Mesh
void lightBulb() {
    glPushMatrix();
    glTranslatef(5, 5, 8);
    glScalef(0.2f, 0.2f, 0.2f);
    setMaterial(1.0f, 0.843f, 0.0f, 0.0f, 0.0f, 0.0f, 100.0f);

    if (switchOne == GL_TRUE) {
        GLfloat mat_emission[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    }
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();
}

void lightBulb3() {
    glPushMatrix();
    glTranslatef(0.7f, 1.5f, 9.0f);
    glScalef(0.2f, 0.2f, 0.2f);
    setMaterial(1.000f, 0.843f, 0.000f, 0.0f, 0.0f, 0.0f, 100.0f);

    if (switchLamp == GL_TRUE) {
        GLfloat mat_emission[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    }
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();
}

// Main Light Logic
void lightOne() {
    glPushMatrix();
    GLfloat light_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_position[] = { 5.0f, 5.0f, 8.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    if (switchOne == GL_TRUE) {
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
        glEnable(GL_LIGHT0);
    }
    else {
        glDisable(GL_LIGHT0);
    }
    glPopMatrix();
}

// Lamp Light Logic
void lampLight() {
    glPushMatrix();
    GLfloat light_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_position[] = { 0.7f, 1.5f, 9.0f, 1.0f };

    glLightfv(GL_LIGHT2, GL_POSITION, light_position);
    GLfloat spot_direction[] = { 0.3f, -1, -0.8f };
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 35.0f);

    if (switchLamp == GL_TRUE) {
        glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient);
        glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
        glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);
        glEnable(GL_LIGHT2);
    }
    else {
        glDisable(GL_LIGHT2);
    }
    glPopMatrix();
}

void sunLight() {
    glPushMatrix();
    GLfloat light_position[] = { 15.0f, 6.0f, 9.0f, 1.0f };
    GLfloat localIntensity = sunIntensity;
    if (localIntensity < 0.01f) localIntensity = 0.0f;

    // 3. Colors
    // Ambient is very low to allow for total darkness
    GLfloat light_ambient[] = { localIntensity * 0.1f, localIntensity * 0.1f, localIntensity * 0.1f, 1.0f };

    // Diffuse/Specular are high to simulate bright sun
    GLfloat light_diffuse[] = { localIntensity, localIntensity, localIntensity * 0.9f, 1.0f };
    GLfloat light_specular[] = { localIntensity, localIntensity, localIntensity, 1.0f };

    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

    // Turn off light 1 entirely if intensity is 0 to save processing/prevent artifacts
    if (localIntensity > 0.0f) {
        glEnable(GL_LIGHT1);
    }
    else {
        glDisable(GL_LIGHT1);
    }
    glPopMatrix();
}

// Camera Movement Logic
void processMovement() {
    float radYaw = yaw * M_PI / 180.0f;
    if (keys['w'] || keys['W']) {
        eyeX += sin(radYaw) * cameraSpeed;
        eyeZ += cos(radYaw) * cameraSpeed;
    }
    if (keys['s'] || keys['S']) {
        eyeX -= sin(radYaw) * cameraSpeed;
        eyeZ -= cos(radYaw) * cameraSpeed;
    }
    if (keys['a'] || keys['A']) {
        eyeX += cos(radYaw) * cameraSpeed;
        eyeZ -= sin(radYaw) * cameraSpeed;
    }
    if (keys['d'] || keys['D']) {
        eyeX -= cos(radYaw) * cameraSpeed;
        eyeZ += sin(radYaw) * cameraSpeed;
    }
    if (keys['q'] || keys['Q']) {
        eyeY += cameraSpeed;
    }
    if (keys['e'] || keys['E']) {
        eyeY -= cameraSpeed;
    }
    updateCamera();
}

// Mouse Controls
void mouseMotion(int x, int y) {
    if (!mouseActive) return;
    int deltaX = x - lastMouseX;
    int deltaY = y - lastMouseY;
    float sensitivity = 0.2f;
    yaw -= deltaX * sensitivity;
    pitch -= deltaY * sensitivity;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    if (yaw > 360.0f) yaw -= 360.0f;
    if (yaw < 0.0f) yaw += 360.0f;

    updateCamera();
    int centerX = windowWidth / 2;
    int centerY = windowHeight / 2;
    glutWarpPointer(centerX, centerY);
    lastMouseX = centerX;
    lastMouseY = centerY;
    glutPostRedisplay();
}

void mousePassiveMotion(int x, int y) {
    mouseMotion(x, y);
}

// Main Display Loop
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (GLfloat)windowWidth / (GLfloat)windowHeight, 1, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ, refX, refY, refZ, 0, 1, 0);

    glEnable(GL_LIGHTING);
    lightOne();
    lampLight();
    sunLight();

    room();
    drawCarpet();
    bed();
    bedsideDrawer();
    lamp();
    drawPoster();
    wardrobe();
    cupboard();
    dressingTable();
    Clock();
    window();
    drawWoodenStool();
    drawCeilingFan();
    lightBulb();
    drawHouseDoor();
    vaseWithFlowers();

    glDisable(GL_LIGHTING);
    glFlush();
    glutSwapBuffers();
}

// Keyboard Controls
void myKeyboardFunc(unsigned char key, int x, int y) {
    keys[key] = true;
    switch (key) {
    case 27: // ESC
        mouseActive = !mouseActive;
        if (mouseActive) {
            glutSetCursor(GLUT_CURSOR_NONE);
            int centerX = windowWidth / 2;
            int centerY = windowHeight / 2;
            glutWarpPointer(centerX, centerY);
            lastMouseX = centerX;
            lastMouseY = centerY;
        }
        else {
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        }
        break;
    case 'f': case 'F':
        if (doorAngle < 90.0f) {
            doorOpening = true; doorClosing = false;
        }
        else if (doorAngle > 0.0f) {
            doorClosing = true; doorOpening = false;
        }
        break;
    case '1':
        switchOne = !switchOne;
        break;
    case '2':
        switchLamp = !switchLamp;
        break;
    case 'r': case 'R':
        isFanOn = !isFanOn;
        break;
    case 'c': case 'C':
        if (curtainOpen) {
            curtainClosing = true; curtainOpening = false;
        }
        else {
            curtainOpening = true; curtainClosing = false;
        }
        break;
    }
    glutPostRedisplay();
}

void myKeyboardUpFunc(unsigned char key, int x, int y) {
    keys[key] = false;
}

void animate() {
    processMovement();
    updateDoorAnimation();

    // Curtain Scaling Logic
    if (curtainOpening) {
        curtainScale -= curtainSpeed;
        if (curtainScale <= 0.15f) { // Stop when bunched up (0.15 width)
            curtainScale = 0.15f;
            curtainOpening = false;
            curtainOpen = true;
        }
    }
    else if (curtainClosing) {
        curtainScale += curtainSpeed;
        if (curtainScale >= 1.0f) { // Stop when fully flat (1.0 width)
            curtainScale = 1.0f;
            curtainClosing = false;
            curtainOpen = false;
        }
    }
    float openFactor = (1.0f - curtainScale) / (1.0f - 0.15f); // Normalized 0.0 to 1.0
    if (openFactor < 0.0f) openFactor = 0.0f; // Clamp to 0
    // Max intensity 1.3 for a bright day
    sunIntensity = openFactor * 1.3f;
    if (isFanOn) {
        fanRotationAngle += fanSpeed;
        if (fanRotationAngle > 360.0f) fanRotationAngle -= 360.0f;
    }
    // Pendulum Logic
    if (redFlag == GL_TRUE) {
        theta += 0.5;
        if (theta >= 210) redFlag = GL_FALSE;
    }
    else {
        theta -= 0.5;
        if (theta <= 150) redFlag = GL_TRUE;
    }
    glutPostRedisplay();
}

// Reshape Handler
void fullScreen(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    if (h == 0) h = 1;
    float ratio = (GLfloat)w / (GLfloat)h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(60, ratio, 1, 100);
    glMatrixMode(GL_MODELVIEW);
}

// Main
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(10, 10);
    glutInitWindowSize((int)windowWidth, (int)windowHeight);
    glutCreateWindow("3D Bedroom");

    quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);
    gluQuadricTexture(quad, GL_TRUE);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    loadAllTextures();

    glutSetCursor(GLUT_CURSOR_NONE);
    lastMouseX = windowWidth / 2;
    lastMouseY = windowHeight / 2;
    glutWarpPointer(lastMouseX, lastMouseY);

    glutReshapeFunc(fullScreen);
    glutDisplayFunc(display);
    glutKeyboardFunc(myKeyboardFunc);
    glutKeyboardUpFunc(myKeyboardUpFunc);
    glutMotionFunc(mouseMotion);
    glutPassiveMotionFunc(mousePassiveMotion);
    glutIdleFunc(animate);
    glutMainLoop();
    return 0;
}