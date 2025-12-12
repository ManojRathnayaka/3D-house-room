#define GLUT_DISABLE_ATEXIT_HACK

// Fix for exit() function redefinition issue
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
#define _USE_MATH_DEFINES
#include <glut.h>
#include <SOIL2.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cmath>

// Camera variables for FPS controls
bool mouseActive = true;    // Mouse captured by default
int lastMouseX = 0, lastMouseY = 0;
float yaw = 180.0f;     // Horizontal rotation
float pitch = 0.0f; // Vertical rotation
float cameraSpeed = 0.05f;

// Movement state
bool keys[256] = { false }; // Track key states for smooth movement

// Original variables
GLboolean redFlag = GL_TRUE;
bool switchOne = false, switchLamp = false;
double windowHeight = 680, windowWidth = 1340;
// Initial camera position to be outside, in front of the door
double eyeX = 2.8, eyeY = 2.0, eyeZ = 20.0, refX = 0, refY = 0, refZ = 0;
double theta = 180.0, y = 1.36, z = 7.97888;

float doorAngle = 0.0f; // Door rotation angle (0 = closed, 90 = open)
bool doorOpening = false;
bool doorClosing = false;
const float doorSpeed = 2.0f; // Degrees per frame

// Ceiling Fan Rotation
float fanRotationAngle = 0.0f;
const float fanSpeed = 2.5f;

// Texture IDs
GLuint woodTexture;      // Texture for the door
GLuint posterTexture;    // Texture for the poster
GLuint carpetTexture;    // Texture for the carpet
GLuint floorTexture;     // Texture for the floor

static GLfloat v_cube[8][3] = {
    {0.0f, 0.0f, 0.0f}, //0
    {0.0f, 0.0f, 3.0f}, //1
    {3.0f, 0.0f, 3.0f}, //2
    {3.0f, 0.0f, 0.0f}, //3
    {0.0f, 3.0f, 0.0f}, //4
    {0.0f, 3.0f, 3.0f}, //5
    {3.0f, 3.0f, 3.0f}, //6
    {3.0f, 3.0f, 0.0f}  //7
};

static GLubyte quadIndices[6][4] = {
    {0, 1, 2, 3}, //bottom
    {4, 5, 6, 7}, //top
    {5, 1, 2, 6}, //front
    {0, 4, 7, 3}, // back is clockwise
    {2, 3, 7, 6}, //right
    {1, 5, 4, 0}  //left is clockwise
};

static void getNormal3p
(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3) {
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2 - x1;
    Uy = y2 - y1;
    Uz = z2 - z1;

    Vx = x3 - x1;
    Vy = y3 - y1;
    Vz = z3 - z1;

    Nx = Uy * Vz - Uz * Vy;
    Ny = Uz * Vx - Ux * Vz;
    Nz = Ux * Vy - Uy * Vx;

    glNormal3f(Nx, Ny, Nz);
}

void updateCamera() {
    // Convert spherical coordinates to Cartesian for look direction
    float radYaw = yaw * M_PI / 180.0f;
    float radPitch = pitch * M_PI / 180.0f;

    // Calculate look-at point based on camera orientation
    refX = eyeX + cos(radPitch) * sin(radYaw);
    refY = eyeY + sin(radPitch);
    refZ = eyeZ + cos(radPitch) * cos(radYaw);
}

void drawCube() {
    glBegin(GL_QUADS);
    for (GLint i = 0; i < 6; i++)
    {
        getNormal3p(v_cube[quadIndices[i][0]][0], v_cube[quadIndices[i][0]][1], v_cube[quadIndices[i][0]][2],
            v_cube[quadIndices[i][1]][0], v_cube[quadIndices[i][1]][1], v_cube[quadIndices[i][1]][2],
            v_cube[quadIndices[i][2]][0], v_cube[quadIndices[i][2]][1], v_cube[quadIndices[i][2]][2]);
        glVertex3fv(&v_cube[quadIndices[i][0]][0]);
        glVertex3fv(&v_cube[quadIndices[i][1]][0]);
        glVertex3fv(&v_cube[quadIndices[i][2]][0]);
        glVertex3fv(&v_cube[quadIndices[i][3]][0]);
    }
    glEnd();
}

// Modified to accept an optional texture ID
void drawCube1(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX = 0, GLfloat ambY = 0, GLfloat ambZ = 0, GLfloat shine = 50, GLuint textureID = 0) {
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0f };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_shininess[] = { shine };

    // If a texture is provided, make the material color white to not tint the texture
    if (textureID != 0) {
        mat_ambient[0] = mat_ambient[1] = mat_ambient[2] = 0.8f;
        mat_diffuse[0] = mat_diffuse[1] = mat_diffuse[2] = 1.0f;
    }

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

    if (textureID != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    glBegin(GL_QUADS);
    for (GLint i = 0; i < 6; i++)
    {
        getNormal3p(v_cube[quadIndices[i][0]][0], v_cube[quadIndices[i][0]][1], v_cube[quadIndices[i][0]][2],
            v_cube[quadIndices[i][1]][0], v_cube[quadIndices[i][1]][1], v_cube[quadIndices[i][1]][2],
            v_cube[quadIndices[i][2]][0], v_cube[quadIndices[i][2]][1], v_cube[quadIndices[i][2]][2]);

        // Generic texture coordinates for each face of the cube
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


static GLfloat v_trapezoid[8][3] = {
    {0.0f, 0.0f, 0.0f}, //0
    {0.0f, 0.0f, 3.0f}, //1
    {3.0f, 0.0f, 3.0f}, //2
    {3.0f, 0.0f, 0.0f}, //3
    {0.5f, 3.0f, 0.5f}, //4
    {0.5f, 3.0f, 2.5f}, //5
    {2.5f, 3.0f, 2.5f}, //6
    {2.5f, 3.0f, 0.5f}  //7
};

static GLubyte TquadIndices[6][4] = {
    {0, 1, 2, 3}, //bottom
    {4, 5, 6, 7}, //top
    {5, 1, 2, 6}, //front
    {0, 4, 7, 3}, // back is clockwise
    {2, 3, 7, 6}, //right
    {1, 5, 4, 0}  //left is clockwise
};

void drawTrapezoid(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX, GLfloat ambY, GLfloat ambZ, GLfloat shine = 50) {
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0f };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_emission[] = { difX, difY, difZ, 0.0f };
    GLfloat mat_shininess[] = { shine };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    if (switchLamp == GL_TRUE) {
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    }
    else {
        glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
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

static GLubyte p_Indices[4][3] = {
    {4, 1, 2},
    {4, 2, 3},
    {4, 3, 0},
    {4, 0, 1}
};

static GLubyte PquadIndices[1][4] = {
    {0, 3, 2, 1}
};

void polygon(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX, GLfloat ambY, GLfloat ambZ, GLfloat shine) {
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0f };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_shininess[] = { shine };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glBegin(GL_POLYGON);
    glVertex2f(0, 0);
    glVertex2f(6, 0);
    glVertex2f(5.8f, 1);
    glVertex2f(5.2f, 2);
    glVertex2f(5, 2.2f);
    glVertex2f(4, 2.8f);
    glVertex2f(3, 3);
    glVertex2f(2, 2.8f);
    glVertex2f(1, 2.2f);
    glVertex2f(0.8f, 2);
    glVertex2f(0.2f, 1);
    glEnd();
}

void polygonLine(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX, GLfloat ambY, GLfloat ambZ, GLfloat shine) {
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0f };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_shininess[] = { shine };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glBegin(GL_LINE_STRIP);
    glVertex2f(6, 0);
    glVertex2f(5.8f, 1);
    glVertex2f(5.2f, 2);
    glVertex2f(5, 2.2f);
    glVertex2f(4, 2.8f);
    glVertex2f(3, 3);
    glVertex2f(2, 2.8f);
    glVertex2f(1, 2.2f);
    glVertex2f(0.8f, 2);
    glVertex2f(0.2f, 1);
    glVertex2f(0, 0);
    glEnd();
}

void drawSphere(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX, GLfloat ambY, GLfloat ambZ, GLfloat shine = 90) {
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0f };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_shininess[] = { shine };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glutSolidSphere(3.0, 20, 16);
}

void loadAllTextures() {
    // Load wood texture for the door
    woodTexture = SOIL_load_OGL_texture(
        "wood.jpg",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
    );
    if (!woodTexture) {
        printf("Wood texture loading failed: %s\n", SOIL_last_result());
    }
    // Load carpet texture
    carpetTexture = SOIL_load_OGL_texture(
        "carpet.jpg",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
    );
    if (!carpetTexture) {
        printf("Carpet texture loading failed: %s\n", SOIL_last_result());
    }
    // Load floor texture
    floorTexture = SOIL_load_OGL_texture(
        "floor.jpg",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
    );
    if (!floorTexture) {
        printf("Floor texture loading failed: %s\n", SOIL_last_result());
    }
    // Load poster texture
    posterTexture = SOIL_load_OGL_texture(
        "image.jpg",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
    );
    if (!posterTexture) {
        printf("Poster texture (image.jpg) loading failed: %s\n", SOIL_last_result());
    }
}


// Textured cube for the door
void drawTexturedCube(GLfloat width, GLfloat height, GLfloat depth,
    GLfloat difX, GLfloat difY, GLfloat difZ,
    GLfloat ambX = 0.2f, GLfloat ambY = 0.1f, GLfloat ambZ = 0.05f,
    GLfloat shine = 30) {
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0f };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0f };
    GLfloat mat_specular[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat mat_shininess[] = { shine };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);

    glBegin(GL_QUADS);

    // Front face
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, depth);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(width, 0.0f, depth);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(width, height, depth);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, height, depth);

    // Back face
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, height, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(width, height, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(width, 0.0f, 0.0f);

    // Top face
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, height, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, height, depth);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(width, height, depth);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(width, height, 0.0f);

    // Bottom face
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(width, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(width, 0.0f, depth);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 0.0f, depth);

    // Right face
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(width, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(width, height, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(width, height, depth);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(width, 0.0f, depth);

    // Left face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 0.0f, depth);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, height, depth);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, height, 0.0f);

    glEnd();
    glDisable(GL_TEXTURE_2D);
}

// House door function with a spherical doorknob
void drawHouseDoor() {
    glPushMatrix();

    // Position the entire door assembly on the front wall
    glTranslatef(2.0f, -1.0f, 14.8f);
    glScalef(1.1f, 1.1f, 1.0f);
    // Door frame - left post
    glPushMatrix();
    glTranslatef(-0.1f, 0.0f, -0.15f);
    drawTexturedCube(0.2f, 4.0f, 0.6f, 0.4f, 0.2f, 0.1f);
    glPopMatrix();

    // Door frame - right post
    glPushMatrix();
    glTranslatef(1.6f, 0.0f, -0.15f);
    drawTexturedCube(0.2f, 4.0f, 0.6f, 0.4f, 0.2f, 0.1f);
    glPopMatrix();

    // Door frame - top
    glPushMatrix();
    glTranslatef(-0.1f, 3.8f, -0.15f);
    drawTexturedCube(1.9f, 0.2f, 0.6f, 0.4f, 0.2f, 0.1f);
    glPopMatrix();

    // Door itself
    glPushMatrix();

    // Move to hinge point (left edge of door)
    glTranslatef(0.0f, 0.0f, 0.1f);

    // Rotate around Y-axis (the hinge)
    glRotatef(doorAngle, 0.0f, 1.0f, 0.0f);

    //  Adjusted door size (slightly smaller than frame opening)
    drawTexturedCube(1.6f, 3.8f, 0.1f, 0.6f, 0.4f, 0.2f);

    // Spherical Door handle
    glPushMatrix();
    // Position the sphere on the door's surface
    glTranslatef(1.45f, 1.8f, 0.15f);

    // Material properties for the golden doorknob
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_ambient[] = { 0.4f, 0.35f, 0.05f, 1.0f };
    GLfloat mat_diffuse[] = { 0.8f, 0.7f, 0.1f, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_shininess[] = { 100 };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

    // Use built-in glutSolidSphere
    glutSolidSphere(0.08, 20, 20);
    glPopMatrix();

    glPopMatrix(); // End door rotation
    glPopMatrix(); // End door positioning
}

// Animation update function for the door
void updateDoorAnimation()
{
    if (doorOpening && doorAngle < 90.0f) {
        doorAngle += doorSpeed;
        if (doorAngle >= 90.0f) {
            doorAngle = 90.0f;
            doorOpening = false;
        }
        glutPostRedisplay();
    }else if (doorClosing && doorAngle > 0.0f) {
        doorAngle -= doorSpeed;
        if (doorAngle <= 0.0f) {
            doorAngle = 0.0f;
            doorClosing = false;
        }
        glutPostRedisplay();
    }
}

void cupboard() {
    //Cupboard/Almari
    glPushMatrix();
    glTranslatef(-1.0f, 0.0f, -1.0f);
    //cupboard
    glPushMatrix();
    glTranslatef(4, 0, 4.4f);
    glScalef(0.5f, 1, 0.5f);
    drawCube1(0.5f, 0.2f, 0.2f, 0.25f, 0.1f, 0.1f);
    glPopMatrix();

    //cupboard's 1st vertical stripline
    glPushMatrix();
    glTranslatef(4, 1, 5.9f);
    glScalef(0.5f, 0.01f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //cupboard's 2nd vertical stripline
    glPushMatrix();
    glTranslatef(4, 0.5f, 5.9f);
    glScalef(0.5f, 0.01f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //cupboard's last stripline
    glPushMatrix();
    glTranslatef(4, 0, 5.9f);
    glScalef(0.5f, 0.01f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //cupboard's lst horizontal stripline
    glPushMatrix();
    glTranslatef(5.5f, 0, 5.9f);
    glScalef(0.01f, 1, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //cupboard's right side horizontal stripline
    glPushMatrix();
    glTranslatef(4.75f, 1, 5.9f);
    glScalef(0.01f, 0.67f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //cupboard's left side horizontal stripline
    glPushMatrix();
    glTranslatef(4, 0, 5.9f);
    glScalef(0.01f, 1, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //cupboard's handle right
    glPushMatrix();
    glTranslatef(5, 1.4f, 5.9f);
    glScalef(0.02f, 0.18f, 0.01f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //cupboard's handle right sphere
    glPushMatrix();
    glTranslatef(5.02f, 1.9f, 5.91f);
    glScalef(0.02f, 0.02f, 0.01f);
    drawSphere(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f, 10);
    glPopMatrix();

    //cupboard's handle left
    glPushMatrix();
    glTranslatef(4.5f, 1.4f, 5.9f);
    glScalef(0.02f, 0.18f, 0.01f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //cupboard's handle left sphere
    glPushMatrix();
    glTranslatef(4.52f, 1.9f, 5.91f);
    glScalef(0.02f, 0.02f, 0.01f);
    drawSphere(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f, 10);
    glPopMatrix();

    //cupboard's drawer's 1st handle
    glPushMatrix();
    glTranslatef(4.5f, 0.7f, 5.9f);
    glScalef(0.16f, 0.02f, 0.01f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //cupboard's drawer's 2nd handle
    glPushMatrix();
    glTranslatef(4.5f, 0.25f, 5.9f);
    glScalef(0.16f, 0.02f, 0.01f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();
    glPopMatrix();
}

void room() {
    // back wall
    glPushMatrix();
    glTranslatef(-1.5f, -1, .5f);
    glScalef(5, 2, 0.1f);
    drawCube1(1, 0.8f, 0.7f, 0.5f, 0.4f, 0.35f);
    glPopMatrix();

    // left wall
    glPushMatrix();
    glTranslatef(-4.5f, -1, 0);
    glScalef(1, 2, 5);
    drawCube1(1, 0.8f, 0.7f, 0.5f, 0.4f, 0.35f);
    glPopMatrix();

    // right wall
    glPushMatrix();
    glTranslatef(8, -1, 0);
    glScalef(0.2f, 2, 5);
    drawCube1(1, 0.8f, 0.7f, 0.5f, 0.4f, 0.35f);
    glPopMatrix();

    //ceiling
    glPushMatrix();
    glTranslatef(-2, 5.1f, 0);
    glScalef(5, 0.1f, 7);
    drawCube1(1.0f, 0.9f, 0.8f, 0.5f, 0.45f, 0.4f);
    glPopMatrix();

    // floor with texture
    glPushMatrix();
    glScalef(5, 0.1f, 7);
    glTranslatef(-1, -5, 0);
    // The color arguments are overridden by the function when a texture is present.
    // pass the shininess value and the floorTexture ID.
    drawCube1(1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.8f, 50, floorTexture);
    glPopMatrix();

    // front wall section (left of door)
    glPushMatrix();
    glTranslatef(-1.5f, -1.0f, 14.8f);
    glScalef(3.5f / 3.0f, 6.1f / 3.0f, 0.1f);
    drawCube1(1, 0.8f, 0.7f, 0.5f, 0.4f, 0.35f);
    glPopMatrix();

    // front wall section (right of door)
    glPushMatrix();
    glTranslatef(3.76f, -1.0f, 14.8f);
    glScalef(4.24f / 3.0f, 6.1f / 3.0f, 0.1f);
    drawCube1(1, 0.8f, 0.7f, 0.5f, 0.4f, 0.35f);
    glPopMatrix();

    // front wall section (above door)
    glPushMatrix();
    glTranslatef(2.0f, 3.18f, 14.8f);
    glScalef(1.76f / 3.0f, 1.92f / 3.0f, 0.1f);
    drawCube1(1, 0.8f, 0.7f, 0.5f, 0.4f, 0.35f);
    glPopMatrix();
}

void bed() {
    //bed headboard
    glPushMatrix();
    glScalef(0.1f, 0.5f, 0.9f);
    glTranslatef(-2, -0.5f, 6.2f);
    drawCube1(0.5f, 0.2f, 0.2f, 0.25f, 0.1f, 0.1f);
    glPopMatrix();

    //bed body
    glPushMatrix();
    glScalef(1, 0.2f, 0.9f);
    glTranslatef(0, -0.5f, 6.2f);
    drawCube1(0.824f, 0.706f, 0.549f, 0.412f, 0.353f, 0.2745f);
    glPopMatrix();

    //pillow right far
    glPushMatrix();
    glTranslatef(0.5f, 0.5f, 6);
    glRotatef(20, 0, 0, 1);
    glScalef(0.1f, 0.15f, 0.28f);
    drawCube1(0.5f, 0.7f, 0.9f, 0.25f, 0.35f, 0.45f);
    glPopMatrix();

    //pillow left near
    glPushMatrix();
    glTranslatef(0.5f, 0.5f, 7.2f);
    glRotatef(22, 0, 0, 1);
    glScalef(0.1f, 0.15f, 0.28f);
    drawCube1(0.5f, 0.7f, 0.9f, 0.25f, 0.35f, 0.45f);
    glPopMatrix();

    //blanket
    glPushMatrix();
    glTranslatef(1.4f, 0.45f, 5.5f);
    glScalef(0.5f, 0.05f, 0.95f);
    drawCube1(0.7f, 0.1f, 0.1f, 0.35f, 0.05f, 0.05f);
    glPopMatrix();

    //blanket side left part
    glPushMatrix();
    glTranslatef(1.4f, -0.3f, 8.16f);
    glScalef(0.5f, 0.25f, 0.05f);
    drawCube1(0.7f, 0.1f, 0.1f, 0.35f, 0.05f, 0.05f);
    glPopMatrix();
}

void bedsideDrawer() {
    //bedside drawer

    //side drawer
    glPushMatrix();
    glTranslatef(0.5f, -0.1f, 8.7f);
    glScalef(0.12f, 0.2f, 0.23f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //side drawer's drawer
    glPushMatrix();
    glTranslatef(0.88f, 0, 8.8f);
    glScalef(0.0001f, 0.11f, 0.18f);
    drawCube1(0.3f, 0.2f, 0.2f, 0.15f, 0.1f, 0.1f);
    glPopMatrix();

    //side drawer's knob
    glPushMatrix();
    glTranslatef(0.9f, 0.15f, 9.05f);
    glScalef(0.01f, 0.02f, 0.02f);
    drawSphere(0.3f, 0.1f, 0.0f, 0.15f, 0.05f, 0.0f);
    glPopMatrix();
}

void lamp() {
    //lamp base
    glPushMatrix();
    glTranslatef(.6f, 0.5f, 8.95f);
    glScalef(0.07f, 0.02f, 0.07f);
    drawCube1(0, 0, 1, 0, 0, 0.5f);
    glPopMatrix();

    //lamp stand
    glPushMatrix();
    glTranslatef(.7f, 0.35f, 9.05f);
    glScalef(0.01f, 0.2f, 0.01f);
    drawCube1(1, 0, 0, 0.5f, 0.0f, 0.0f);
    glPopMatrix();

    //lamp shade
    glPushMatrix();
    glTranslatef(.6f, 0.9f, 8.9f);
    glScalef(0.08f, 0.09f, 0.08f);
    drawTrapezoid(0.000f, 0.000f, 0.545f, 0, 0, 0.2725f);
    glPopMatrix();
}

void drawPoster() {
    glPushMatrix();
    glTranslatef(-1.0f, 1.5f, 11.0f);

    float height = 1.95f;
    float width = 2.4f;
    float frameThickness = 0.1f;
    float frameDepth = 0.05f; // How far it sticks out from the wall

    // A simple dark wood color for the frame
    GLfloat frameDif[] = { 0.3f, 0.15f, 0.05f };
    GLfloat frameAmb[] = { 0.15f, 0.075f, 0.025f };

    // Bottom frame piece
    glPushMatrix();
    glTranslatef(0.0f, -frameThickness, -frameThickness);
    glScalef(frameDepth / 3.0f, frameThickness / 3.0f, (width + 2 * frameThickness) / 3.0f);
    drawCube1(frameDif[0], frameDif[1], frameDif[2], frameAmb[0], frameAmb[1], frameAmb[2]);
    glPopMatrix();

    // Top frame piece
    glPushMatrix();
    glTranslatef(0.0f, height, -frameThickness);
    glScalef(frameDepth / 3.0f, frameThickness / 3.0f, (width + 2 * frameThickness) / 3.0f);
    drawCube1(frameDif[0], frameDif[1], frameDif[2], frameAmb[0], frameAmb[1], frameAmb[2]);
    glPopMatrix();

    // Left frame piece
    glPushMatrix();
    glTranslatef(0.0f, -frameThickness, -frameThickness);
    glScalef(frameDepth / 3.0f, (height + 2 * frameThickness) / 3.0f, frameThickness / 3.0f);
    drawCube1(frameDif[0], frameDif[1], frameDif[2], frameAmb[0], frameAmb[1], frameAmb[2]);
    glPopMatrix();

    // Right frame piece
    glPushMatrix();
    glTranslatef(0.0f, -frameThickness, width);
    glScalef(frameDepth / 3.0f, (height + 2 * frameThickness) / 3.0f, frameThickness / 3.0f);
    drawCube1(frameDif[0], frameDif[1], frameDif[2], frameAmb[0], frameAmb[1], frameAmb[2]);
    glPopMatrix();

    // Set material properties to be bright so the texture shows clearly
    GLfloat mat_ambient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_specular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);

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


// Draws a textured quad for the carpet on the floor.
void drawCarpet() {
    glPushMatrix();
    glTranslatef(3.0f, -0.199f, 7.0f);

    // Set material properties
    GLfloat mat_ambient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, carpetTexture);

    float width = 3.9f;
    float depth = 5.1f;

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f); // Normal pointing up from the floor
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, depth);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(width, 0.0f, depth);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(width, 0.0f, 0.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void wardrobe() {
    //wardrobe
    glPushMatrix();
    glTranslatef(0, 0, 4);
    glScalef(0.12f, 0.6f, 0.4f);
    drawCube1(0.3f, 0.1f, 0, 0.15f, 0.05f, 0);
    glPopMatrix();

    //wardrobe's 1st drawer
    glPushMatrix();
    glTranslatef(0.36f, 1.4f, 4.05f);
    glScalef(0.0001f, 0.11f, 0.38f);
    drawCube1(0.5f, 0.2f, 0.2f, 0.25f, 0.1f, 0.1f);
    glPopMatrix();

    //wardrobe's 2nd drawer
    glPushMatrix();
    glTranslatef(0.36f, 1, 4.05f);
    glScalef(0.0001f, 0.11f, 0.38f);
    drawCube1(0.5f, 0.2f, 0.2f, 0.25f, 0.1f, 0.1f);
    glPopMatrix();

    //wardrobe's 3rd drawer
    glPushMatrix();
    glTranslatef(0.36f, 0.6f, 4.05f);
    glScalef(0.0001f, 0.11f, 0.38f);
    drawCube1(0.5f, 0.2f, 0.2f, 0.25f, 0.1f, 0.1f);
    glPopMatrix();

    //wardrobe's 4th drawer
    glPushMatrix();
    glTranslatef(0.36f, 0.2f, 4.05f);
    glScalef(0.0001f, 0.11f, 0.38f);
    drawCube1(0.5f, 0.2f, 0.2f, 0.25f, 0.1f, 0.1f);
    glPopMatrix();

    //wardrobe's 1st drawer handle
    glPushMatrix();
    glTranslatef(0.37f, 1.5f, 4.3f);
    glScalef(0.01f, 0.03f, 0.2f);
    drawCube1(0.3f, 0.1f, 0, 0.15f, 0.05f, 0.0f);
    glPopMatrix();

    //wardrobe's 2nd drawer handle
    glPushMatrix();
    glTranslatef(0.37f, 1.1f, 4.3f);
    glScalef(0.01f, 0.03f, 0.2f);
    drawCube1(0.3f, 0.1f, 0, 0.15f, 0.05f, 0.0f);
    glPopMatrix();

    //wardrobe's 3rd drawer handle
    glPushMatrix();
    glTranslatef(0.37f, 0.7f, 4.3f);
    glScalef(0.01f, 0.03f, 0.2f);
    drawCube1(0.3f, 0.1f, 0, 0.15f, 0.05f, 0.0f);
    glPopMatrix();

    //wardrobe's 4th drawer handle
    glPushMatrix();
    glTranslatef(0.37f, 0.3f, 4.3f);
    glScalef(0.01f, 0.03f, 0.2f);
    drawCube1(0.3f, 0.1f, 0, 0.15f, 0.05f, 0.0f);
    glPopMatrix();
}

void dressingTable() {
    //Dressing table
    glPushMatrix();
    glTranslatef(-0.5f,0.0f,-0.2f);
    //dressing table left body
    glPushMatrix();
    glTranslatef(5.9f, 0, 4.6f);
    glScalef(0.2f, 0.2f, 0.2f);
    drawCube1(0.545f, 0.271f, 0.075f, 0.2725f, 0.1355f, 0.0375f);
    glPopMatrix();

    //dressing table right body
    glPushMatrix();
    glTranslatef(7, 0, 4.6f);
    glScalef(0.2f, 0.2f, 0.2f);
    drawCube1(0.545f, 0.271f, 0.075f, 0.2725f, 0.1355f, 0.0375f);
    glPopMatrix();

    //dressing table upper body
    glPushMatrix();
    glTranslatef(5.9f, 0.6f, 4.6f);
    glScalef(0.57f, 0.1f, 0.2f);
    drawCube1(0.545f, 0.271f, 0.075f, 0.2725f, 0.1355f, 0.0375f);
    glPopMatrix();

    //dressing table upper body bottom stripe
    glPushMatrix();
    glTranslatef(5.9f, 0.6f, 5.2f);
    glScalef(0.57f, 0.01f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //dressing table upper body upper stripe
    glPushMatrix();
    glTranslatef(5.9f, 0.9f, 5.2f);
    glScalef(0.57f, 0.01f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //dressing table upper body handle
    glPushMatrix();
    glTranslatef(6.5f, 0.75f, 5.2f);
    glScalef(0.16f, 0.02f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //dressing table left body handle
    glPushMatrix();
    glTranslatef(6.4f, 0.1f, 5.2f);
    glScalef(0.02f, 0.13f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //dressing table right body handle
    glPushMatrix();
    glTranslatef(7.1f, 0.1f, 5.2f);
    glScalef(0.02f, 0.13f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //dressing table main mirror
    glPushMatrix();
    glTranslatef(6.2f, 0.9f, 4.7f);
    glScalef(0.36f, 0.5f, 0.0001f);
    drawCube1(0.690f, 0.878f, 0.902f, 0.345f, 0.439f, 0.451f, 10);
    glPopMatrix();

    //dressing table left mirror
    glPushMatrix();
    glTranslatef(5.92f, 0.9f, 4.7f);
    glScalef(0.1f, 0.48f, 0.0001f);
    drawCube1(0.690f, 0.878f, 0.902f, 0.345f, 0.439f, 0.451f, 10);
    glPopMatrix();

    //dressing table left mirror left stripe
    glPushMatrix();
    glTranslatef(5.92f, 0.9f, 4.71f);
    glScalef(0.019f, 0.48f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //dressing table left mirror right stripe
    glPushMatrix();
    glTranslatef(6.17f, 0.9f, 4.71f);
    glScalef(0.019f, 0.48f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //dressing table mirror stripe
    glPushMatrix();
    glTranslatef(5.92f, 0.9f, 4.71f);
    glScalef(0.55f, 0.019f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //dressing table left mirror upper stripe
    glPushMatrix();
    glTranslatef(5.92f, 2.3f, 4.71f);
    glScalef(0.1f, 0.019f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //dressing table right mirror
    glPushMatrix();
    glTranslatef(7.25f, 0.9f, 4.7f);
    glScalef(0.1f, 0.48f, 0.0001f);
    drawCube1(0.690f, 0.878f, 0.902f, 0.345f, 0.439f, 0.451f, 10);
    glPopMatrix();

    //dressing table right mirror upper stripe
    glPushMatrix();
    glTranslatef(7.25f, 2.3f, 4.71f);
    glScalef(0.1f, 0.019f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //dressing table right mirror left stripe
    glPushMatrix();
    glTranslatef(7.25f, 0.9f, 4.71f);
    glScalef(0.019f, 0.48f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //dressing table right mirror right stripe
    glPushMatrix();
    glTranslatef(7.5f, 0.9f, 4.71f);
    glScalef(0.019f, 0.48f, 0.0001f);
    drawCube1(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f);
    glPopMatrix();

    //dressing table main mirror polygon part
    glPushMatrix();
    glTranslatef(6.2f, 2.4f, 4.7f);
    glScalef(0.18f, 0.18f, 2);
    polygon(0.690f, 0.878f, 0.902f, 0.345f, 0.439f, 0.451f, 10);
    glPopMatrix();

    //dressing table upper round stripe
    glPushMatrix();
    glTranslatef(6.2f, 2.4f, 4.71f);
    glScalef(.18f, .18f, 1);
    polygonLine(0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f, 50);
    glPopMatrix();
    glPopMatrix();
}

void Clock() {
    //clock body
    glPushMatrix();
    glTranslatef(-0.9f, 1.8f, 7.87f);
    glScalef(0.08f, 0.25f, 0.1f);
    drawCube1(0.545f, 0.271f, 0.075f, 0.271f, 0.1335f, 0.0375f, 50);
    glPopMatrix();

    //clock body white
    glPushMatrix();
    glTranslatef(-0.83f, 1.9f, 7.9f);
    glScalef(0.06f, 0.2f, 0.08f);
    drawCube1(1.000f, 0.894f, 0.710f, 1.000f, 0.894f, 0.710f);
    glPopMatrix();

    //clock hour handle
    glPushMatrix();
    glTranslatef(-0.65f, 2.18f, 8.01f);
    glRotatef(45, 1, 0, 0);
    glScalef(0.0001f, 0.01f, 0.04f);
    drawCube1(0, 0, 0, 0, 0, 0);
    glPopMatrix();

    //clock minute handle
    glPushMatrix();
    glTranslatef(-0.65f, 2.18f, 8.01f);
    glRotatef(90, 1, 0, 0);
    glScalef(0.0001f, 0.012f, 0.08f);
    drawCube1(0, 0, 0, 0, 0, 0);
    glPopMatrix();

    //clock pendulum stick
    glPushMatrix();
    glTranslatef(-0.7f, 2, 8.1f);
    glRotatef((GLfloat)theta, 1, 0, 0);
    glScalef(0.0001f, 0.2f, 0.03f);
    drawCube1(0.8f, 0.7f, 0.1f, 0.4f, 0.35f, 0.05f); // Gold color
    glPopMatrix();

    //clock pendulum ball
    glPushMatrix();
    glTranslatef(-0.72f, 1.42f, (GLfloat)z);
    glScalef(0.035f, 0.035f, 0.035f);
    drawSphere(0.8f, 0.7f, 0.1f, 0.4f, 0.35f, 0.05f, 10);
    glPopMatrix();
}

void window() {
    const GLfloat winX = -1.49f; // X position on the wall
    const GLfloat winY = 1.0f;   // Y position (bottom edge)
    const GLfloat winZ = 8.9f;   // Z position (right edge)
    const GLfloat winHeight = 1.8f;
    const GLfloat winWidth = 0.9f;
    const GLfloat frameThickness = 0.12f;
    const GLfloat barThickness = 0.06f;
    glPushMatrix();
    glTranslatef(winX, winY, winZ);
    glScalef(0.0001f, winHeight / 3.0f, winWidth / 3.0f);
    // A slightly blueish white for glass
    drawCube1(0.8f, 0.9f, 1.0f, 0.4f, 0.45f, 0.5f);
    glPopMatrix();
    const GLfloat frameX = winX + 0.02f;

    // Frame: Bottom horizontal bar
    glPushMatrix();
    glTranslatef(frameX, winY, winZ);
    glScalef(frameThickness / 3.0f, frameThickness / 3.0f, winWidth / 3.0f);
    drawCube1(0.7f, 0.6f, 0.5f, 0.35f, 0.3f, 0.25f);
    glPopMatrix();

    // Frame: Top horizontal bar
    glPushMatrix();
    glTranslatef(frameX, winY + winHeight - frameThickness, winZ);
    glScalef(frameThickness / 3.0f, frameThickness / 3.0f, winWidth / 3.0f);
    drawCube1(0.7f, 0.6f, 0.5f, 0.35f, 0.3f, 0.25f);
    glPopMatrix();

    // Frame: Right vertical post
    glPushMatrix();
    glTranslatef(frameX, winY, winZ);
    glScalef(frameThickness / 3.0f, winHeight / 3.0f, frameThickness / 3.0f);
    drawCube1(0.8f, 0.6f, 0.4f, 0.4f, 0.3f, 0.2f);
    glPopMatrix();

    // Frame: Left vertical post
    glPushMatrix();
    glTranslatef(frameX, winY, winZ + winWidth - frameThickness);
    glScalef(frameThickness / 3.0f, winHeight / 3.0f, frameThickness / 3.0f);
    drawCube1(0.8f, 0.6f, 0.4f, 0.4f, 0.3f, 0.2f);
    glPopMatrix();


    // Window Bars (inside the frame)
    const GLfloat barX = frameX - 0.01f; // Place bars slightly behind the frame

    // Bar: Middle horizontal
    glPushMatrix();
    glTranslatef(barX, winY + (winHeight / 2.0f) - (barThickness / 2.0f), winZ);
    glScalef(0.0001f, barThickness / 3.0f, winWidth / 3.0f);
    drawCube1(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5);
    glPopMatrix();

    // Bar: Middle vertical
    glPushMatrix();
    glTranslatef(barX, winY, winZ + (winWidth / 2.0f) - (barThickness / 2.0f));
    glScalef(0.0001f, winHeight / 3.0f, barThickness / 3.0f);
    drawCube1(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5);
    glPopMatrix();
}

void drawWoodenStool() {
    glPushMatrix();
    // Position the stool
    glTranslatef(5.0f, -0.2f, 10.0f);

    // Stool Top
    glPushMatrix();
    glTranslatef(-0.45f, 0.8f, -0.45f);
    glScalef(0.3f, 0.05f, 0.3f);
    // Use the woodTexture by passing its ID to drawCube1
    drawCube1(1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.8f, 30, woodTexture);
    glPopMatrix();

    // Leg 1 (front-left)
    glPushMatrix();
    glTranslatef(-0.4f, 0.0f, -0.4f);
    glScalef(0.05f, 0.28f, 0.05f);
    drawCube1(1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.8f, 30, woodTexture);
    glPopMatrix();

    // Leg 2 (front-right)
    glPushMatrix();
    glTranslatef(0.25f, 0.0f, -0.4f);
    glScalef(0.05f, 0.28f, 0.05f);
    drawCube1(1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.8f, 30, woodTexture);
    glPopMatrix();

    // Leg 3 (back-left)
    glPushMatrix();
    glTranslatef(-0.4f, 0.0f, 0.25f);
    glScalef(0.05f, 0.28f, 0.05f);
    drawCube1(1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.8f, 30, woodTexture);
    glPopMatrix();

    // Leg 4 (back-right)
    glPushMatrix();
    glTranslatef(0.25f, 0.0f, 0.25f);
    glScalef(0.05f, 0.28f, 0.05f);
    drawCube1(1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.8f, 30, woodTexture);
    glPopMatrix();

    glPopMatrix();
}

void drawCeilingFan() {
    glPushMatrix();
    // Position the fan in the center of the ceiling
    glTranslatef(2.5f, 4.8f, 7.5f);

    // This is the intended center point for the fan assembly, based on the rotation pivot
    const float centerX = 0.3f;
    const float centerZ = 0.3f;

    // Fan base attached to the ceiling. Base is 0.3 wide (3.0 * 0.1).
    glPushMatrix();
    glTranslatef(centerX - 0.15f, 0.0f, centerZ - 0.15f);
    glScalef(0.1f, 0.1f, 0.1f);
    drawCube1(0.8f, 0.8f, 0.8f, 0.4f, 0.4f, 0.4f);
    glPopMatrix();

    // Rod connecting base to motor. Rod is 0.06 wide (3.0 * 0.02).
    glPushMatrix();
    glTranslatef(centerX - 0.03f, -1.0f, centerZ - 0.03f);
    glScalef(0.02f, 0.3f, 0.02f);
    drawCube1(0.7f, 0.7f, 0.7f, 0.35f, 0.35f, 0.35f);
    glPopMatrix();

    // Motor housing. Motor is 0.6 wide (3.0 * 0.2).
    glPushMatrix();
    glTranslatef(centerX - 0.3f, -1.0f, centerZ - 0.3f);
    glScalef(0.2f, 0.08f, 0.2f);
    drawCube1(0.9f, 0.9f, 0.9f, 0.45f, 0.45f, 0.45f);
    glPopMatrix();

    // Rotating part of the fan
    glPushMatrix();
    glTranslatef(centerX, -1.1f, centerZ); // Move pivot to the center of the motor
    glRotatef(fanRotationAngle, 0.0f, 1.0f, 0.0f); // Rotate around the Y-axis

    // Draw 4 blades
    for (int i = 0; i < 4; ++i) {
        glPushMatrix();
        // Rotate each blade into position
        glRotatef(i * 90.0f, 0.0f, 1.0f, 0.0f);

        // Position and draw the blade
        glTranslatef(0.2f, 0.0f, 0.0f); // Move out from the center
        glScalef(0.6f, 0.01f, 0.15f);
        // Changed to a solid off-white color and removed texture
        drawCube1(0.9f, 0.9f, 0.9f, 0.45f, 0.45f, 0.45f, 50);
        glPopMatrix();
    }
    glPopMatrix(); // End rotating part
    glPopMatrix(); // End fan
}


void lightBulb() {
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat mat_ambient_color[] = { 0.8f, 0.8f, 0.2f, 1.0f };
    GLfloat mat_diffuse[] = { 1.000f, 0.843f, 0.000f, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat high_shininess[] = { 100.0f };
    GLfloat mat_emission[] = { 1.000f, 1, 1, 0.0f };
    glPushMatrix();
    glTranslatef(5, 5, 8);
    glScalef(0.2f, 0.2f, 0.2f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    if (switchOne == GL_TRUE) {
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    }
    else {
        glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    }
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();
}



void lightBulb3() {
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat mat_ambient_color[] = { 0.8f, 0.8f, 0.2f, 1.0f };
    GLfloat mat_diffuse[] = { 1.000f, 0.843f, 0.000f, 1.0f };
    GLfloat high_shininess[] = { 100.0f };
    GLfloat mat_emission[] = { 1, 1, 1, 1.0f };

    glPushMatrix();
    glTranslatef(0.7f, 1.5f, 9.0f);
    glScalef(0.2f, 0.2f, 0.2f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    if (switchLamp == GL_TRUE) {
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    }
    else {
        glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    }
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();
}

void lightOne() {
    glPushMatrix();
    GLfloat light_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_position[] = { 5.0f, 5.0f, 8.0f, 1.0f };

    // Always set position
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    if (switchOne == GL_TRUE) {
        // Apply all light components
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

void lampLight() {
    glPushMatrix();
    GLfloat light_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_position[] = { 0.7f, 1.5f, 9.0f, 1.0f };

    // Always set position and spot direction
    glLightfv(GL_LIGHT2, GL_POSITION, light_position);
    GLfloat spot_direction[] = { 0.3f, -1, -0.8f };
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 35.0f);

    if (switchLamp == GL_TRUE) {
        // Apply all light components
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

// FPS Camera Movement Functions
void processMovement() {
    float radYaw = yaw * M_PI / 180.0f;
    // Forward/Backward movement
    if (keys['w'] || keys['W']) {
        eyeX += sin(radYaw) * cameraSpeed;
        eyeZ += cos(radYaw) * cameraSpeed;
    }
    if (keys['s'] || keys['S']) {
        eyeX -= sin(radYaw) * cameraSpeed;
        eyeZ -= cos(radYaw) * cameraSpeed;
    }
    // Strafe left/right
    if (keys['a'] || keys['A']) {
        eyeX += cos(radYaw) * cameraSpeed;
        eyeZ -= sin(radYaw) * cameraSpeed;
    }
    if (keys['d'] || keys['D']) {
        eyeX -= cos(radYaw) * cameraSpeed;
        eyeZ += sin(radYaw) * cameraSpeed;
    }
    // Vertical movement
    if (keys['q'] || keys['Q']) {
        eyeY += cameraSpeed;
    }
    if (keys['e'] || keys['E']) {
        eyeY -= cameraSpeed;
    }
    updateCamera();
}

// Mouse motion callback for FPS camera
void mouseMotion(int x, int y) {
    if (!mouseActive) return;
    int deltaX = x - lastMouseX;
    int deltaY = y - lastMouseY;
    // Mouse sensitivity
    float sensitivity = 0.2f;
    // rotation angles
    yaw -= deltaX * sensitivity;
    pitch -= deltaY * sensitivity;
    // Clamp pitch to prevent camera flipping
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    // Wrap yaw around 360 degrees
    if (yaw > 360.0f) yaw -= 360.0f;
    if (yaw < 0.0f) yaw += 360.0f;
    // Update camera
    updateCamera();
    // Warp mouse back to center to prevent hitting screen edges
    int centerX = windowWidth / 2;
    int centerY = windowHeight / 2;
    glutWarpPointer(centerX, centerY);
    lastMouseX = centerX;
    lastMouseY = centerY;
    glutPostRedisplay();
}

// Passive mouse motion (when no button pressed)
void mousePassiveMotion(int x, int y) {
    mouseMotion(x, y);
}

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
    //lightBulb3();
    glDisable(GL_LIGHTING);
    glFlush();
    glutSwapBuffers();
}

void myKeyboardFunc(unsigned char key, int x, int y) {
    keys[key] = true;  // Mark key as pressed
    switch (key) {
    case 27:   // ESC key - toggle mouse capture
        mouseActive = !mouseActive;
        if (mouseActive) {
            glutSetCursor(GLUT_CURSOR_NONE);
            // Center mouse
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
    case 'f':
    case 'F':
        if (doorAngle < 90.0f) {
            doorOpening = true;
            doorClosing = false;
        }
        else if (doorAngle > 0.0f) {
            doorClosing = true;
            doorOpening = false;
        }
        break;
    case '1': // Master switch for Light One
        switchOne = !switchOne; // Toggles between True and False
        break;

    case '2': // Master switch for Lamp Light
        switchLamp = !switchLamp; // Toggles between True and False
        break;

    case 'r': case 'R':
        // Reset camera to default starting position
        eyeX = 2.8; eyeY = 2.0; eyeZ = 16.0;
        yaw = 180.0f; pitch = 0.0f;
        updateCamera();
        std::cout << "Camera reset to default position" << std::endl;
        break;
    }
    glutPostRedisplay();
}

void myKeyboardUpFunc(unsigned char key, int x, int y) {
    keys[key] = false;  // Mark key as released
}

void animate() {
    // Process continuous movement
    processMovement();
    updateDoorAnimation();
    // fan rotation
    fanRotationAngle += fanSpeed;
    if (fanRotationAngle > 360.0f) {
        fanRotationAngle -= 360.0f;
    }
    if (redFlag == GL_TRUE) {
        theta += 0.5; // Slower speed
        z -= 0.005;  // Adjusted z-movement to match slower speed
        if (theta >= 196 && theta <= 210) {
            y = 1.44;
        } else if (theta >= 180 && theta <= 194) {
            y = 1.42;
        } else if (theta >= 180 && theta <= 194) {
            y = 1.4;
        } else if (theta >= 164 && theta <= 178) {
            y = 1.42;
        }
        if (theta >= 210) {
            redFlag = GL_FALSE;
        }
    } else if (redFlag == GL_FALSE) {
        theta -= 0.5; // speed
        z += 0.005;
        if (theta >= 196 && theta <= 210) {
            y = 1.44;
        } else if (theta >= 180 && theta <= 194) {
            y = 1.42;
        } else if (theta >= 180 && theta <= 194) {
            y = 1.4;
        } else if (theta >= 164 && theta <= 178) {
            y = 1.42;
        }
        if (theta <= 150) {
            redFlag = GL_TRUE;
        }
    }
    glutPostRedisplay();
}

void fullScreen(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    if (h == 0)
        h = 1;
    float ratio = (GLfloat)w / (GLfloat)h;
    //Set the perspective coordinate system
    glMatrixMode(GL_PROJECTION);           //Use the Projection Matrix
    glLoadIdentity();                   //Reset Matrix
    glViewport(0, 0, w, h);       //Set the viewport to be the entire window
    gluPerspective(60, ratio, 1, 100); //Set the correct perspective.
    glMatrixMode(GL_MODELVIEW);           //Get Back to the Modelview
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(10, 10);
    glutInitWindowSize((int)windowWidth, (int)windowHeight);
    glutCreateWindow("Bedroom - FPS Controls");

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    // Load all textures
    loadAllTextures();

    // Initialize mouse capture
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