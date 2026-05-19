#include <GL/glut.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#endif

int W = 900, H = 600;

enum State
{
    MENU,
    PLAYING,
    PAUSED,
    GAMEOVER
};
State state = MENU;

// ---------- BASKET ----------
float basketX = 400;
float basketW = 100;

// ---------- CHICKEN ----------
struct Chicken
{
    float x;
    float dir;
};
std::vector<Chicken> chickens;

// ---------- EGG ----------
struct Egg
{
    float x, y;
    int type;
};
std::vector<Egg> eggs;

// ---------- TREE ----------
struct Tree
{
    float x;
};
std::vector<Tree> trees;

// ---------- CLOUD ----------
struct Cloud
{
    float x, y;
};
std::vector<Cloud> clouds;

// ---------- GAME ----------
int score = 0;
int highScore = 0;
int timeLeft = 60;

int slowTimer = 0;
float wind = 0;
float wingAngle = 0;
float hillOffset = 0;

// ---------- SOUND ----------
void playSound()
{
#ifdef _WIN32
    Beep(800, 100);
#endif
}

// ---------- FILE ----------
void loadHighScore()
{
    std::ifstream file("highscore.txt");
    if (file.is_open())
        file >> highScore;
}

void saveHighScore()
{
    std::ofstream file("highscore.txt");
    file << highScore;
}

// ---------- DRAW HELPERS ----------
void drawText(float x, float y, std::string s)
{
    glRasterPos2f(x, y);
    for (char c : s)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

void drawCircle(float cx, float cy, float r)
{
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < 100; i++)
    {
        float a = 2 * 3.1416f * i / 100;
        glVertex2f(cx + cos(a) * r, cy + sin(a) * r);
    }
    glEnd();
}

// ---------- CLOUD ----------
void drawCloud(float x, float y)
{
    glColor3f(1, 1, 1);
    drawCircle(x, y, 18);
    drawCircle(x + 20, y + 5, 22);
    drawCircle(x + 40, y, 18);
}

// ---------- HILLS ----------
void drawHills()
{
    glColor3f(0.3, 0.7, 0.3);
    for (int i = -100; i < W + 100; i += 200)
    {
        glBegin(GL_TRIANGLES);
        glVertex2f(i + hillOffset * 0.3, 200);
        glVertex2f(i + 100 + hillOffset * 0.3, 350);
        glVertex2f(i + 200 + hillOffset * 0.3, 200);
        glEnd();
    }

    glColor3f(0.1, 0.5, 0.1);
    for (int i = -150; i < W + 150; i += 250)
    {
        glBegin(GL_TRIANGLES);
        glVertex2f(i + hillOffset * 0.6, 150);
        glVertex2f(i + 120 + hillOffset * 0.6, 300);
        glVertex2f(i + 240 + hillOffset * 0.6, 150);
        glEnd();
    }
}

// ---------- TREE ----------
void drawTree(float x)
{
    glColor3f(0.55, 0.27, 0.07);
    glBegin(GL_QUADS);
    glVertex2f(x - 10, 80);
    glVertex2f(x + 10, 80);
    glVertex2f(x + 10, 140);
    glVertex2f(x - 10, 140);
    glEnd();

    glColor3f(0.0, 0.6, 0.0);
    drawCircle(x, 160, 30);
}

bool isBlocked(float newX)
{
    for (auto &t : trees)
    {
        if (newX + basketW > t.x - 15 && newX < t.x + 15)
            return true;
    }
    return false;
}
#include <GL/glut.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#endif

int W = 900, H = 600;

enum State
{
    MENU,
    PLAYING,
    PAUSED,
    GAMEOVER
};
State state = MENU;

// ---------- BASKET ----------
float basketX = 400;
float basketW = 100;

// ---------- CHICKEN ----------
struct Chicken
{
    float x;
    float dir;
};
std::vector<Chicken> chickens;

// ---------- EGG ----------
struct Egg
{
    float x, y;
    int type;
};
std::vector<Egg> eggs;

// ---------- TREE ----------
struct Tree
{
    float x;
};
std::vector<Tree> trees;

// ---------- CLOUD ----------
struct Cloud
{
    float x, y;
};
std::vector<Cloud> clouds;

// ---------- GAME ----------
int score = 0;
int highScore = 0;
int timeLeft = 60;

int slowTimer = 0;
float wind = 0;
float wingAngle = 0;
float hillOffset = 0;

// ---------- SOUND ----------
void playSound()
{
#ifdef _WIN32
    Beep(800, 100);
#endif
}

// ---------- FILE ----------
void loadHighScore()
{
    std::ifstream file("highscore.txt");
    if (file.is_open())
        file >> highScore;
}

void saveHighScore()
{
    std::ofstream file("highscore.txt");
    file << highScore;
}

// ---------- DRAW HELPERS ----------
void drawText(float x, float y, std::string s)
{
    glRasterPos2f(x, y);
    for (char c : s)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

void drawCircle(float cx, float cy, float r)
{
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < 100; i++)
    {
        float a = 2 * 3.1416f * i / 100;
        glVertex2f(cx + cos(a) * r, cy + sin(a) * r);
    }
    glEnd();
}

// ---------- CLOUD ----------
void drawCloud(float x, float y)
{
    glColor3f(1, 1, 1);
    drawCircle(x, y, 18);
    drawCircle(x + 20, y + 5, 22);
    drawCircle(x + 40, y, 18);
}

// ---------- HILLS ----------
void drawHills()
{
    glColor3f(0.3, 0.7, 0.3);
    for (int i = -100; i < W + 100; i += 200)
    {
        glBegin(GL_TRIANGLES);
        glVertex2f(i + hillOffset * 0.3, 200);
        glVertex2f(i + 100 + hillOffset * 0.3, 350);
        glVertex2f(i + 200 + hillOffset * 0.3, 200);
        glEnd();
    }

    glColor3f(0.1, 0.5, 0.1);
    for (int i = -150; i < W + 150; i += 250)
    {
        glBegin(GL_TRIANGLES);
        glVertex2f(i + hillOffset * 0.6, 150);
        glVertex2f(i + 120 + hillOffset * 0.6, 300);
        glVertex2f(i + 240 + hillOffset * 0.6, 150);
        glEnd();
    }
}

// ---------- TREE ----------
void drawTree(float x)
{
    glColor3f(0.55, 0.27, 0.07);
    glBegin(GL_QUADS);
    glVertex2f(x - 10, 80);
    glVertex2f(x + 10, 80);
    glVertex2f(x + 10, 140);
    glVertex2f(x - 10, 140);
    glEnd();

    glColor3f(0.0, 0.6, 0.0);
    drawCircle(x, 160, 30);
}

bool isBlocked(float newX)
{
    for (auto &t : trees)
    {
        if (newX + basketW > t.x - 15 && newX < t.x + 15)
            return true;
    }
    return false;
}
