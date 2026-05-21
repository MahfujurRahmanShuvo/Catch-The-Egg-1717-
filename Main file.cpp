
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
// ---------- WORLD ----------
void drawGrass()
{
    glColor3f(0.1, 0.7, 0.1);

    for (int i = 0; i < W; i += 20)
    {
        glBegin(GL_TRIANGLES);
        glVertex2f(i, 0);
        glVertex2f(i + 10, 40);
        glVertex2f(i + 20, 0);
        glEnd();
    }

    glColor3f(0.0, 0.5, 0.0);
    glBegin(GL_LINES);
    glVertex2f(0, 80);
    glVertex2f(W, 80);
    glEnd();
}

void drawStick()
{
    glColor3f(0.6, 0.3, 0.1);
    glBegin(GL_LINES);
    glVertex2f(0, 480);
    glVertex2f(W, 480);
    glEnd();
}

// ---------- CHICKEN ----------
void drawChicken(float x)
{
    glColor3f(1, 1, 0);
    drawCircle(x, 500, 20);

    drawCircle(x + 22, 515, 10);

    glColor3f(0, 0, 0);
    drawCircle(x + 25, 518, 2);

    glColor3f(1, 0.5, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(x + 30, 515);
    glVertex2f(x + 40, 520);
    glVertex2f(x + 30, 525);
    glEnd();

    float wingOffset = sin(wingAngle) * 6;
    glColor3f(1, 1, 0);
    drawCircle(x - 5, 500 + wingOffset, 10);

    glColor3f(0.8, 0.3, 0.1);
    glBegin(GL_LINES);
    glVertex2f(x - 5, 480);
    glVertex2f(x - 5, 470);
    glVertex2f(x + 5, 480);
    glVertex2f(x + 5, 470);
    glEnd();
}

// ---------- BASKET ----------
void drawBasket()
{
    glColor3f(0.8, 0.4, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(basketX, 50);
    glVertex2f(basketX + basketW, 50);
    glVertex2f(basketX + basketW, 80);
    glVertex2f(basketX, 80);
    glEnd();
}

// ---------- EGG ----------
void spawnEgg()
{
    if (chickens.empty())
        return;
    int i = rand() % chickens.size();

    Egg e;
    e.x = chickens[i].x;
    e.y = 480;
    e.type = rand() % 5;
    eggs.push_back(e);
}

// ---------- DISPLAY ----------
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (state == MENU)
    {
        drawText(330, 350, "LEGEND CATCH THE EGGS");
        drawText(330, 300, "Press S to Start");
        drawText(330, 270, "Move: A/D or Mouse");
    }

    else if (state == PLAYING || state == PAUSED)
    {

        // 🌄 BACKGROUND
        drawHills();

        for (auto &c : clouds)
            drawCloud(c.x, c.y);

        drawGrass();

        // 🌳 TREES
        for (auto &t : trees)
            drawTree(t.x);

        drawStick();

        // 🐔 CHICKENS
        for (auto &c : chickens)
            drawChicken(c.x);

        // 🧺 BASKET
        drawBasket();

        // 🥚 EGGS
        for (auto &e : eggs)
        {
            if (e.type == 0)
                glColor3f(1, 1, 1);
            else if (e.type == 1)
                glColor3f(0, 0, 1);
            else if (e.type == 2)
                glColor3f(1, 0.8, 0);
            else if (e.type == 3)
                glColor3f(0.3, 0.2, 0);
            else
                glColor3f(0, 1, 0);

            glBegin(GL_POLYGON);
            for (int i = 0; i < 100; i++)
            {
                float t = 2 * 3.1416f * i / 100;
                glVertex2f(e.x + cos(t) * 10, e.y + sin(t) * 14);
            }
            glEnd();
        }

        drawText(10, H - 20, "Score: " + std::to_string(score));
        drawText(150, H - 20, "High: " + std::to_string(highScore));
        drawText(W - 120, H - 20, "Time: " + std::to_string(timeLeft));

        if (state == PAUSED)
            drawText(400, 300, "PAUSED");
    }

    else if (state == GAMEOVER)
    {
        drawText(350, 320, "GAME OVER");
        drawText(350, 280, "Score: " + std::to_string(score));
        drawText(350, 250, "High: " + std::to_string(highScore));
        drawText(300, 220, "Press R to Restart");
    }

    glutSwapBuffers();
}

// ---------- UPDATE ----------
void update(int v)
{
    if (state == PLAYING)
    {

        wingAngle += 0.1;
        hillOffset += 0.5;

        // ☁️ clouds move
        for (auto &c : clouds)
        {
            c.x += 0.3;
            if (c.x > W + 60)
                c.x = -60;
        }

        for (auto &c : chickens)
        {
            c.x += c.dir * 2;
            if (c.x < 40 || c.x > W - 40)
                c.dir *= -1;
        }

        wind = sin(glutGet(GLUT_ELAPSED_TIME) / 500.0) * 2;

        for (int i = 0; i < eggs.size(); i++)
        {

            float speed = (slowTimer > 0) ? 2 : 5;
            eggs[i].y -= speed;
            eggs[i].x += wind;

            if (eggs[i].y <= 80 &&
                eggs[i].x >= basketX &&
                eggs[i].x <= basketX + basketW)
            {

                playSound();

                if (eggs[i].type == 0)
                    score += 1;
                else if (eggs[i].type == 1)
                    score += 5;
                else if (eggs[i].type == 2)
                    score += 10;
                else if (eggs[i].type == 3)
                    score -= 10;
                else
                {
                    int p = rand() % 3;
                    if (p == 0)
                        basketW = 150;
                    else if (p == 1)
                        slowTimer = 200;
                    else
                        timeLeft += 5;
                }

                eggs.erase(eggs.begin() + i);
                i--;
            }
            else if (eggs[i].y < 0)
            {
                eggs.erase(eggs.begin() + i);
                i--;
            }
        }

        if (rand() % 20 == 0)
            spawnEgg();

        if (slowTimer > 0)
            slowTimer--;
    }

    glutPostRedisplay();
    glutTimerFunc(30, update, 0);
}

// ---------- TIMER ----------
void timer(int v)
{
    if (state == PLAYING && timeLeft > 0)
        timeLeft--;

    if (timeLeft <= 0)
    {
        state = GAMEOVER;

        if (score > highScore)
        {
            highScore = score;
            saveHighScore();
        }
    }

    glutTimerFunc(1000, timer, 0);
}

// ---------- INPUT ----------
void keyboard(unsigned char key, int x, int y)
{

    if (state == MENU && key == 's')
        state = PLAYING;

    else if (state == PLAYING)
    {

        if (key == 'a')
        {
            float newX = basketX - 20;
            if (!isBlocked(newX))
                basketX = newX;
        }

        if (key == 'd')
        {
            float newX = basketX + 20;
            if (!isBlocked(newX))
                basketX = newX;
        }

        if (key == 'p')
            state = PAUSED;
    }

    else if (state == PAUSED && key == 'p')
        state = PLAYING;

    else if (state == GAMEOVER && key == 'r')
    {
        score = 0;
        timeLeft = 60;
        eggs.clear();
        chickens.clear();
        trees.clear();

        chickens.push_back({400, 1});

        for (int i = 100; i < W; i += 200)
            trees.push_back({(float)i});

        basketW = 100;
        state = PLAYING;
    }

    if (key == 27)
        exit(0);
}

void mouseMove(int x, int y)
{
    float newX = x - basketW / 2;
    if (!isBlocked(newX))
        basketX = newX;
}

// ---------- INIT ----------
void init()
{
    glClearColor(0.5, 0.8, 1, 1);
    gluOrtho2D(0, W, 0, H);

    srand(time(0));
    loadHighScore();

    chickens.push_back({400, 1});

    for (int i = 100; i < W; i += 200)
        trees.push_back({(float)i});

    for (int i = 0; i < 5; i++)
        clouds.push_back({(float)(rand() % W), 400 + rand() % 150});
}

// ---------- MAIN ----------
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(W, H);
    glutCreateWindow("Egg Catcher - Full Ultimate Edition");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutPassiveMotionFunc(mouseMove);
    glutTimerFunc(30, update, 0);
    glutTimerFunc(1000, timer, 0);

    glutMainLoop();
    return 0;
}