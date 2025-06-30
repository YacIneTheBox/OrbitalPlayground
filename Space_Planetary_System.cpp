#include <iostream>
#include "raylib.h"
#include <cmath>
using namespace std;

const int screenWidth = 1200;
const int screenHeight = 800;

const float DISTANCE_MAX = 100.0f;
const float G = 1.0f;
const float FRICTION =0.99999f;
const int MAX_INIT_BODY = 20;

const float MAX_SIZE_BODY = 350;
const float MIN_SIZE_BODY = 5;

const Color DEFAULT_COLOR = WHITE;


struct CelestBody {
    Vector2 pos = { 0,0 };
    float size = rand() % 50 + 40;
    Vector2 velocity = { 0,0 };
    float rotation = 0;
    bool deleted = true;
    Color color = DEFAULT_COLOR;
};

Vector2 AttractionLaw(Vector2 pos1, Vector2 pos2, float mass1, float mass2);
void creatingCBody(CelestBody CBodyList[]);
void DestroyBody(CelestBody CBodyList[]);
void Editing(CelestBody CBodyList[]);
void simpleDelete(CelestBody *b);
void Mooving(CelestBody CBodyList[],bool isMoving);
void CollisionChecking(CelestBody c[]);

int main()
{
    InitWindow(screenWidth, screenHeight, "Space Simulation");
    SetTargetFPS(60);

    // Celestial Body liste et initialisation
    CelestBody CBodyList[MAX_INIT_BODY];

    for (int i = 0; i < MAX_INIT_BODY; i++) {
        CBodyList[i].deleted = true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    while (!WindowShouldClose()) {

        // update
        Vector2 mouse = GetMousePosition();

        // basic controls
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) creatingCBody(CBodyList);
        if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) DestroyBody(CBodyList);
        bool isMoving = false;
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            isMoving = true;
        }
        else {
            isMoving = false;
        }
            
        Mooving(CBodyList,isMoving);







        Editing(CBodyList); // increasing size 



        Vector2 force = { 0,0 };
        
        for (int i = 0; i < MAX_INIT_BODY; i++) {
            if (CBodyList[i].deleted == false) {
                for (int j = 0; j < MAX_INIT_BODY - 1; j++) {
                    if (CBodyList[j].deleted == false && i != j) {
                        force = AttractionLaw(CBodyList[i].pos, CBodyList[j].pos, CBodyList[i].size, CBodyList[j].size);
                    }
                }
                CBodyList[i].velocity.x += force.x;
                CBodyList[i].velocity.y += force.y;

                CBodyList[i].pos.x += CBodyList[i].velocity.x;
                CBodyList[i].pos.y += CBodyList[i].velocity.y;

                CBodyList[i].velocity.x *= FRICTION;
                CBodyList[i].velocity.y *= FRICTION;
            }
            
        }
        
        CollisionChecking(CBodyList);
        BeginDrawing();
        ClearBackground(BLACK);
        
        for (int i = 0; i < MAX_INIT_BODY; i++) {
            if (CBodyList[i].deleted == false) {
                DrawCircle(CBodyList[i].pos.x, CBodyList[i].pos.y, CBodyList[i].size, CBodyList[i].color);
            }
        }



        EndDrawing();
    }
    CloseWindow();
    return 0;
}

void CollisionChecking(CelestBody c[]) {
    bool colid = false;
    for (int i = 0; i < MAX_INIT_BODY; i++) {
        if (c[i].deleted == false) {

            for (int j = 0; j < MAX_INIT_BODY; j++) {
                colid = false;
                colid = CheckCollisionCircles(c[i].pos, c[i].size, c[j].pos, c[j].size);

                if (i != j && c[j].deleted == false && colid == true) {
                    if (c[i].size > c[j].size) {
                        c[i].size += c[j].size / 2;
                        simpleDelete(&c[j]);
                    }
                    if (c[i].size < c[j].size) {
                        c[j].size += c[i].size / 2;
                        simpleDelete(&c[i]);
                    }
                    if (c[i].size == c[j].size) {
                        cout << "les deux deleted";
                        simpleDelete(&c[j]);
                        simpleDelete(&c[i]);
                    }
                }
            }
        }

    }
}

Vector2 AttractionLaw(Vector2 pos1, Vector2 pos2, float mass1, float mass2) {
    Vector2 force = { 0, 0 };

    // Calcul du vecteur direction entre les deux positions
    Vector2 direction;
    direction.x = pos2.x - pos1.x;
    direction.y = pos2.y - pos1.y;

    // Calcul de la distance
    float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

    // Éviter la division par zéro
    if (distance < 1.0f) distance = 1.0f;

    // Normalisation du vecteur direction
    Vector2 directionNormalized;
    directionNormalized.x = direction.x / distance;
    directionNormalized.y = direction.y / distance;

    // Calcul de la force scalaire selon la loi de Newton
    float strength = (G * mass1 * mass2) / (distance * distance);

    // Appliquer la direction à la force
    force.x = directionNormalized.x * strength;
    force.y = directionNormalized.y * strength;

    return force;
}

void Mooving(CelestBody CBodyList[],bool isMoving){
    Vector2 mouse = GetMousePosition();
    Vector2 initPos;
    Vector2 direction;
    Vector2 inertia = { 0,0 };
    if (isMoving) {
        for (int i = 0; i < MAX_INIT_BODY; i++) {
            if (CheckCollisionPointCircle(mouse, CBodyList[i].pos, CBodyList[i].size+20)) {

                initPos.x = CBodyList[i].pos.x;
                initPos.y = CBodyList[i].pos.y;

                direction.x = mouse.x - initPos.x * 1;
                direction.y = mouse.y - initPos.y * 1;
                float distance = sqrt(direction.x * direction.x + direction.y * direction.y);
                if (distance > CBodyList[i].size/2.0f) {
                    direction.x /= distance;
                    direction.y /= distance;
                    inertia.x = direction.x * 0.5f;
                    inertia.y = direction.y * 0.5f;
                    CBodyList[i].velocity.x += inertia.x;
                    CBodyList[i].velocity.y += inertia.y;
                }


            }
        }
    }

}

void Editing(CelestBody CBodyList[]) {
    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < MAX_INIT_BODY; i++) {
        if (CheckCollisionPointCircle(mouse, CBodyList[i].pos, CBodyList[i].size)) {
                if (IsKeyDown(KEY_R) && CBodyList[i].size < MAX_SIZE_BODY) {
                    cout << "size increasing";
                    CBodyList[i].size += 1;
                }
                if (IsKeyDown(KEY_E) && CBodyList[i].size > MIN_SIZE_BODY) {
                    cout << "size deacreasing";
                    CBodyList[i].size -= 1;
                }
                break;
            }
        
    }
}

void creatingCBody(CelestBody CBodyList[]) {
        bool isAvailable = false;
        Vector2 mouse = GetMousePosition();
        for (int i = 0; i < MAX_INIT_BODY; i++) {

            if (CBodyList[i].deleted == true) {
                isAvailable = true;
                CBodyList[i].pos.x = mouse.x;
                CBodyList[i].pos.y = mouse.y;
                CBodyList[i].deleted = false;
                break;
            }
        }
        /*if (isAvailable) { cout << "place available !"; }
        else { cout << "no place available ! "; }*/
   
}
void simpleDelete(CelestBody *b) {
    b->deleted = true;
    b->size = 20;
    b->pos = { 0,0 };
    b->size = 20;
    b->velocity = { 0,0 };
    b->rotation = 0;
    b->color = DEFAULT_COLOR;
    cout << "////////////////////////////////////////////////////////////////";
}

void DestroyBody(CelestBody CBodyList[]) {
    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < MAX_INIT_BODY; i++) {
        if (CheckCollisionPointCircle(mouse, CBodyList[i].pos, CBodyList[i].size)) {
            simpleDelete(&CBodyList[i]);
        }
    }
}

