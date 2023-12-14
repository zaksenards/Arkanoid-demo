#include <raylib.h>
#include <cstdlib>
#include <cstdio>
#include <vector>

// Window
#define WND_WIDTH  800
#define WND_HEIGHT 600
#define WND_TITLE  "Hit'n break"

// Brick
#define BRK_ROWS    20
#define BRK_COLUMNS 40
#define BRK_WIDTH   WND_WIDTH/BRK_ROWS
#define BRK_HEIGHT  WND_HEIGHT/BRK_COLUMNS

struct Brick
{
    Vector2 position;
    Vector2 scale;
    Color color;

    Brick(Vector2 pos, Color clr):position(pos)
    {
        scale = {BRK_WIDTH, BRK_HEIGHT};
        color = clr;
    }

    Rectangle getRect()
    {
        Rectangle rect = {0};
        rect.x = position.x;
        rect.y = position.y;
        rect.width = scale.x;
        rect.height = scale.y;
        return rect;
    }

    void render()
    {
        DrawRectangleV(position, scale, color);
    }
};
struct Ball
{
    Vector2 position;
    Vector2 force;
    Color color;
    float scale;

    // control
    bool aiming;
    float aimAngle;
    float aimTarget;

    Ball(Vector2 pos):position(pos)
    {
        force = {0,0};
        color = WHITE;
        scale = 5;
        position.x-=scale/2;

        aimTarget = (position.x+scale/2)+50;
        aimAngle = position.x+scale/2;
        aiming = true;
    }

    void render()
    {
        if(aiming)
        {
            DrawLineV({position.x, position.y}, {aimAngle, position.y-90}, RED);

            // Dinamicaly changes angle
            aimAngle+=((aimTarget-aimAngle) >= 0? 1 : -1);

            // Change aim target
            if(aimAngle == aimTarget)
            {
                if(aimTarget > (position.x+scale/2))
                    aimTarget = (position.x+scale/2)-50;
                else 
                    aimTarget = (position.x+scale/2)+50;
            }
        }

        DrawCircleV(position, scale, color);
    }

    void update()
    {
        double x = force.x/17;
        double y = force.y/17;

        if(position.x-x <= 0 || position.x-x >= WND_WIDTH){
            force.x = -force.x;
            x = -x;
        }

        if(position.y-y <= 0 || position.y-y >= WND_HEIGHT){
            force.y = -force.y;
            y = -y;
        }

        position.x-=x;
        position.y-=y;
    }

    void shoot()
    {
        aiming = false;
        force = {position.x-aimAngle, position.y-(position.y-90)};
    }

    bool isPlaying()
    {
        return !aiming;
    }

    void collide(Rectangle rect)
    {
        if(CheckCollisionCircleRec(position, scale, rect))
        {
            if( (position.x - rect.x) > (position.y - rect.y))
                force.y = -force.y;
            else
                force.x = -force.x;
        }
    }

};
struct Paddle
{
    Vector2 position;
    float velocity;
    Vector2 scale;
    Color color;

    Paddle(Vector2 pos):position(pos)
    {
        scale = {90, 15};
        position.x-=scale.x/2;
        color = WHITE;
        velocity = 3;
    }

    void render()
    {
        DrawRectangleV(position, scale, color);
    }

    void update()
    {
        float motionX = (IsKeyDown(KEY_A)-IsKeyDown(KEY_D))*velocity;
        position.x-= motionX;
    }

    Rectangle getRect()
    {
        Rectangle rect;
        rect.x = position.x;
        rect.y = position.y;
        rect.width = scale.x;
        rect.height = scale.y;
        return rect;
    }
};
struct CeneManager
{
    int rows, columns;
    std::vector<Brick*> bricks;

    CeneManager(int r, int c):rows(r),columns(c)
    {
        Color colors[] = {WHITE, RED, BLUE, GRAY, GREEN, YELLOW, PURPLE};
        int l_color = -1;
        int c_color = 0;

        for(int j = 0; j <= c; j++)
        {
            for(int i = 0; i <= r; i++)
            {
                do{
                    c_color = rand()%6;
                }while(c_color == l_color);
                Color c = colors[c_color];
                l_color = c_color;

                Brick* b = new Brick({i*(BRK_WIDTH), j*(BRK_HEIGHT)}, c);
                bricks.push_back(b);
            }
        }

    }

    void render()
    {
        for(int i = 0; i < bricks.size(); i++)
            if(bricks[i] != nullptr)
                bricks[i]->render();
    }

    void collide(Ball* ball)
    {
        for(int i = 0; i < bricks.size(); i++)
        {
            Brick* b = bricks[i];
            if(b != nullptr)
            {
                Rectangle rect = b->getRect();
                if(CheckCollisionCircleRec(ball->position, ball->scale, rect)){
                    ball->collide(rect);
                    bricks[i] = nullptr;
                }
            }
        }
    }
};

int main()
{
    InitWindow(WND_WIDTH, WND_HEIGHT, WND_TITLE);
    SetTargetFPS(60);

    CeneManager cene = CeneManager(BRK_ROWS, 5);
    Paddle player = Paddle({WND_WIDTH/2, WND_HEIGHT/1.2});
    Ball ball = Ball({WND_WIDTH/2, WND_HEIGHT/1.3});

    while(!WindowShouldClose())
    {
        ball.collide(player.getRect());
        cene.collide(&ball);
        player.update();
        ball.update();

        if(!ball.isPlaying() && IsKeyDown(KEY_ENTER))
            ball.shoot();

        BeginDrawing();
        {
            ClearBackground(BLACK);
            player.render();
            ball.render();
            cene.render();
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
