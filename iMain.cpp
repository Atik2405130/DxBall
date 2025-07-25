#include "iGraphics.h"
#include "iSound.h"
#include<stdlib.h>
#include<math.h>
#include<cmath>
#include<algorithm>
#include <vector> // Required for std::vector

char playerName[50] = ""; // rahul new
int nameIndex = 0;
bool askingName = false;
#define MENU_CONTINUE 7
bool isPaused = false;
int winTimer = 0;
// Atik is dead
int screenWidth = 1000, screenHeight = 800;
// Ball
int ball_x, ball_y;
int dx=6, dy=8;
int ball_radius = 7;
// Paddle
int paddle_x, paddle_y=30;
int paddle_width = 120,paddle_height = 15;
int lifeIcon;
int life=3;
int musicID=-1;//Menu music
//Bricks related
Image img;
Image scoreIcon;
Image level1;
Image level2;
Image levelupImage;
int score=0;
int level=1;
int level1MusicID=-1;
#define ROW 5
#define COLM 10
int brickwidth=80;
int brickheight=20;
int bricks[ROW][COLM];
int brickstartx=60;
int brickstarty=600;
Image LevelSelectBackground;// Rahul is bad
Image leaderboardImage; // rahul new
int menuState=0; //0=Main Menu, 1=Game, 2=Instructions, 3=Game Over
int gameOverTimer=0;
Image paddleImage;
bool ballStuck=true;
// Menu click area constants
#define MENU_X 270
#define MENU_WIDTH 300
#define MENU_HEIGHT 50
#define MENU_WIN 7

// *** NEW: Perk Definitions ***
#define BRICK_TYPE_NORMAL_1HIT 1 // Green
#define BRICK_TYPE_NORMAL_2HIT 2 // Red
#define BRICK_TYPE_NORMAL_3HIT 3 // Purple

// Special brick type that spawns a perk when destroyed
#define BRICK_TYPE_SPAWN_PERK 7

// Perk types for falling objects
enum PerkType {
    PERK_LIFE_UP = 0, // 1.bmp
    PERK_BALL_SPEED_UP, // 3.bmp
    PERK_LIFE_DOWN_RESET // 13.bmp
};

// Structure for a falling perk object
struct FallingPerk {
    int x, y;
    PerkType type;
    int speed;
    bool active;
    Image perkImage; // Holds the image for this specific perk
    int width, height; // Size of the falling perk image
};

std::vector<FallingPerk> fallingPerks; // Vector to store active falling perks

// *** NEW: Perk Images ***
Image perkImage_Life;
Image perkImage_Speed;
Image perkImage_Trap;

// *** NEW: Function to handle falling perks ***
void updateFallingPerks();

void drawNameInput()
{ // rahul new
    iSetColor(255, 255, 255);
    iTextAdvanced(300, 400, "Enter Your Name:", 0.3, 2.5);

    char displayName[100] = "Name: ";
    strcat(displayName, playerName);

    iSetColor(200, 200, 0);
    iTextAdvanced(320, 350, displayName,0.3,2.5);

    iSetColor(150, 150, 150);
    iTextAdvanced(320, 300, "Press ENTER to continue",0.3,2.5);
}


// atik wow


void Level3Bricks() { // rahul new
    // Clear all first
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLM; j++) {
            bricks[i][j] = 0;
        }
    }

    // Fill all rows with bricks (upper half coverage)
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLM; j++) {
            int r = rand() % 100;

            // Create a layered difficulty
            if (i == 0 || i == 4 || j == 0 || j == COLM - 1) {
                bricks[i][j] = BRICK_TYPE_NORMAL_3HIT; // solid outer ring
            }
            else if ((i + j) % 2 == 0) {
                bricks[i][j] = (r < 50) ? BRICK_TYPE_NORMAL_2HIT : BRICK_TYPE_NORMAL_3HIT;
            }
            else if ((i == 2 || i == 3) && (j >= 3 && j <= COLM - 4)) {
                bricks[i][j] = (r < 40) ? BRICK_TYPE_NORMAL_1HIT : BRICK_TYPE_NORMAL_2HIT;
            }
            else if (r < 25) {
                bricks[i][j] = BRICK_TYPE_NORMAL_1HIT;
            }
        }
    }

    // Center explosion (super dense)
    int cx = COLM / 2;
    for (int i = 1; i <= 3; i++) {
        for (int j = cx - 2; j <= cx + 2; j++) {
            bricks[i][j] = BRICK_TYPE_NORMAL_3HIT;
        }
    }

    // Add fang lines diagonally
    for (int i = 0; i < ROW; i++) {
        int j1 = i;
        int j2 = COLM - 1 - i;
        if (j1 >= 0 && j1 < COLM) bricks[i][j1] = BRICK_TYPE_NORMAL_2HIT;
        if (j2 >= 0 && j2 < COLM) bricks[i][j2] = BRICK_TYPE_NORMAL_2HIT;
    }

    // Scatter some more for chaos
    for (int k = 0; k < 10; k++) {
        int i = rand() % ROW;
        int j = rand() % COLM;
        if (bricks[i][j] == 0) bricks[i][j] = (rand() % 3) + 1;
    }
}


void drawLeaderboard() {
    iShowLoadedImage(0, 0, &leaderboardImage); // Show the background image
    iSetColor(255, 255, 255); // White text for contrast

    FILE *fp = fopen("scores.txt", "r");
    if (fp != NULL) {
        char line[100];
        int y = 500;

        iSetColor(255, 255, 255);
        iTextAdvanced(350, y + 40, "PLAYER", 0.3, 2.5);
        iTextAdvanced(600, y + 40, "SCORE", 0.3, 2.5);

        while (fgets(line, sizeof(line), fp) != NULL && y > 100) {
            // Remove newline
            line[strcspn(line, "\n")] = 0;

            // Split by last space
            char *lastSpace = strrchr(line, ' ');
            if (lastSpace) {
                int score = atoi(lastSpace + 1);
                *lastSpace = '\0'; // terminate name

                char *name = line;

                char scoreStr[10];
                sprintf(scoreStr, "%d", score);

                iTextAdvanced(350, y, name, 0.3, 2.0);
                iTextAdvanced(600, y, scoreStr, 0.3, 2.0);

                y -= 40;
            }
        }

        fclose(fp);
    } else {
        iSetColor(255, 255, 255);
        iTextAdvanced(300, 400, "Unable to load scores.", 0.3, 2.5);
    }

    iSetColor(255, 255, 255);
    iTextAdvanced(350, 50, "Press 'B' to go back", 0.25, 2.0);
}



// MARK: initBricks() -- Changed
void initBricks();  // Declare before use
void startNextLevel() {
    if (level > 3) level = 3; // Safety check
    menuState = 1;
    initBricks();
    paddle_x = screenWidth / 2 - paddle_width / 2;
    ball_x = paddle_x + paddle_width / 2;
    ball_y = paddle_y + paddle_height;
    dx = 6;
    dy = 8;
    ballStuck = true;
}

void initBricks()
{
    for(int i=0;i<ROW;i++)
    {
        for(int j=0;j<COLM;j++)
        {
            bricks[i][j]=0;
        }
    }
    if(level==1) {
        for (int i = 0; i < ROW; i++)
            for (int j = 0; j < COLM; j++)
                bricks[i][j] = BRICK_TYPE_NORMAL_1HIT; // Use defined constants
    }
    else if(level==2){
        for (int i=0;i<ROW;i++) {
            for (int j=0;j<COLM;j++) {
                if (i+j>=2 && j-i<=7) {
                    int r=rand()%100;
                    if (r<60){
                        bricks[i][j]=BRICK_TYPE_NORMAL_1HIT;
                    } else if(r<90){
                        bricks[i][j]=BRICK_TYPE_NORMAL_2HIT;
                    } else {
                        bricks[i][j]=BRICK_TYPE_NORMAL_3HIT;
                    }
                }
            }
        }
    }
    else if (level == 3) {
        Level3Bricks();
    }

    // Add perks to 6-7 random bricks in all levels
    int perksAdded = 0;
    int numPerks = 6 + (rand() % 2); // 6 or 7 perks
    while(perksAdded < numPerks) {
        int r = rand() % ROW;
        int c = rand() % COLM;
        // Ensure it's a valid brick and not already a perk-spawning brick
        if(bricks[r][c] > 0 && bricks[r][c] != BRICK_TYPE_SPAWN_PERK) {
            bricks[r][c] = BRICK_TYPE_SPAWN_PERK; // This brick will spawn a perk
            perksAdded++;
        }
    }
    // Clear any existing falling perks when starting a new level
    fallingPerks.clear();
}

void saveGameState() {
    FILE* fp = fopen("saved_game.txt", "w");
    if (fp) {
        fprintf(fp, "%d %d %d %d %d %d %d %d %d\n", level, score, life, ball_x, ball_y, dx, dy, paddle_x, ballStuck);
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COLM; j++) {
                fprintf(fp, "%d ", bricks[i][j]);
            }
            fprintf(fp, "\n");
        }
        fclose(fp);
    }
}
    



void loadGameState() {
    FILE* fp = fopen("saved_game.txt", "r");
    if (fp) {
        fscanf(fp, "%d %d %d %d %d %d %d %d %d", &level, &score, &life, &ball_x, &ball_y, &dx, &dy, &paddle_x, &ballStuck);
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COLM; j++) {
                fscanf(fp, "%d", &bricks[i][j]);
            }
        }
        fclose(fp);
        menuState = 1;
    }
}





void drawLevelSelectMenu() {
    iShowLoadedImage(0,0,&LevelSelectBackground);
    iSetColor(246,190,0);
    //iTextAdvanced(300, 520, "Select Level", 0.5, 3.5);
    //iTextAdvanced(300, 420, "1. Level 1", 0.4, 2.5);
    //iTextAdvanced(300, 370, "2. Level 2", 0.4, 2.5);
    //iTextAdvanced(300, 320, "3. Level 3", 0.4, 2.5);
    iTextAdvanced(230,250,"Click to go back to Main Menu", 0.3, 2.0);
}
void gameOverCountdown() {
    if (menuState==3) {
        if(gameOverTimer>0) {
            gameOverTimer--;
        } else{
            menuState=0;
        }
    }
}
bool isLevelCleared() {
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLM; j++) {
            if (bricks[i][j] > 0) return false;
        }
    }
    return true;
}


// MARK: ballChange() -- Modified for Perk Spawning
void ballChange(){
    if(menuState!=1 || isPaused) return; // Add pause check here
    remove("saved_game.txt");
    if (ballStuck) {
        ball_x=paddle_x+paddle_width/2;
        ball_y=paddle_y+paddle_height+ball_radius;
        return;
    }

    // Predict next position (useful for advanced collision detection, but for this
    // geometric accuracy, we'll revert if collision happens)
    int prev_ball_x = ball_x;
    int prev_ball_y = ball_y;

    ball_x+= dx;
    ball_y+= dy;

    // Boundary collisions
    if(ball_x + ball_radius > screenWidth) {
        ball_x = screenWidth - ball_radius;
        dx = -dx;
        iPlaySound("Wall.wav",false,10);
    } else if (ball_x - ball_radius < 0) {
        ball_x = ball_radius;
        dx = -dx;
        iPlaySound("Wall.wav",false,10);
    }

    if(ball_y + ball_radius > screenHeight){
        ball_y=screenHeight-ball_radius;
        dy = -dy;
        iPlaySound("Wall.wav",false,10);
    }

    // Paddle collision
    if(ball_y-ball_radius<=paddle_y+paddle_height && ball_y-ball_radius>=paddle_y&&
       ball_x>=paddle_x && ball_x<=paddle_x+paddle_width && dy<0){
        dy=-dy;
        double hitPos = (double)(ball_x - paddle_x) / paddle_width; // 0.0 to 1.0
        if (hitPos<0.25) dx=-8;
        else if(hitPos<0.5) dx=-4;//left middle
        else if (hitPos<0.75) dx=4;//right middle
        else dx = 8;//right quarter

        ball_y=paddle_y+paddle_height+ball_radius;// Prevent sticking
        iPlaySound("Boing.wav",false,10);
    }

    // Missed ball
    if(ball_y - ball_radius < 0){
        iPlaySound("Fall.wav",false,10);
        life--;
        if(life<=0){
            menuState=3;
            gameOverTimer=100;
            ball_x=screenWidth/2;
            ball_y=-1000;
            ballStuck=true;

        FILE *fp = fopen("scores.txt", "a");
        if (fp != NULL)
        { // rahul new
            fprintf(fp, "%s %d\n", playerName[0] ? playerName : "Anonymous", score);
            fclose(fp);
        }
        }
        else{
            paddle_x=screenWidth/2-paddle_width / 2;
            ball_x=paddle_x+paddle_width/2;
            ball_y=paddle_y+paddle_height;
            dx=6;
            dy=8;
            ballStuck=true;
        }
        return;
    }
    // Geometrically accurate Brick collision
    for(int i=0;i<ROW;i++) {
        for(int j=0;j<COLM;j++) {
            if(bricks[i][j]>0) {
                int bx=brickstartx+j*(brickwidth+10);
                int by=brickstarty-i*(brickheight+10);
                int closestX=std::max(bx,std::min(ball_x, bx + brickwidth));
                int closestY=std::max(by,std::min(ball_y, by + brickheight));
                int distX=ball_x-closestX;
                int distY=ball_y-closestY;
                double distanceSq=(distX*distX)+(distY*distY);
                if (distanceSq<(ball_radius*ball_radius)) {
                    ball_x=prev_ball_x;
                    ball_y=prev_ball_y;
                    double overlapLeft=(ball_x+ball_radius)-bx;
                    double overlapRight=(bx+brickwidth)-(ball_x-ball_radius);
                    double overlapBottom=(ball_y+ball_radius)-by;
                    double overlapTop=(by+brickheight)-(ball_y-ball_radius);

                    bool bouncedX=false;
                    bool bouncedY=false;

                    // Determine the minimum overlap to find the collision axis
                    double minOverlapX = std::min(overlapLeft, overlapRight);
                    double minOverlapY = std::min(overlapBottom, overlapTop);

                    if (minOverlapX<minOverlapY){
                        dx=-dx;
                        bouncedX=true;
                    } else if(minOverlapY<minOverlapX){
                        dy=-dy;
                        bouncedY=true;
                    } else{
                        dy=-dy;
                        bouncedY=true;
                    }

                    if (bricks[i][j] == BRICK_TYPE_SPAWN_PERK) {
                        // Spawn a new falling perk object
                        FallingPerk newPerk;
                        newPerk.x = bx + brickwidth / 2; // Center of the broken brick
                        newPerk.y = by; // Just below the broken brick
                        newPerk.speed = 3 + (rand() % 3); // Random speed for falling perks
                        newPerk.active = true;
                        newPerk.width = 30; // Example size, adjust as needed
                        newPerk.height = 30; // Example size, adjust as needed

                        int perkChoice = rand() % 3; // 0, 1, or 2
                        if (perkChoice == 0) {
                            newPerk.type = PERK_LIFE_UP;
                            newPerk.perkImage = perkImage_Life;
                        } else if (perkChoice == 1) {
                            newPerk.type = PERK_BALL_SPEED_UP;
                            newPerk.perkImage = perkImage_Speed;
                        } else {
                            newPerk.type = PERK_LIFE_DOWN_RESET;
                            newPerk.perkImage = perkImage_Trap;
                        }
                        fallingPerks.push_back(newPerk);
                        bricks[i][j] = 0; // Destroy the brick
                        score += 10;
                    } else {
                        bricks[i][j]--;
                        if(bricks[i][j]==0) score+=10;
                    }
                    iPlaySound("Boop.wav",false,10);

                    // Move ball out of collision to prevent multiple hits in one frame
                    // This is handled by ball_x/y += dx/dy after the loop,
                    // but we ensure it's moving away from the brick.
                    if (bouncedX) ball_x+=dx;
                    if (bouncedY) ball_y+=dy;
                    break; // Only hit one brick per frame
                }
            }
        }
    }
    // Check if level is cleared
if (isLevelCleared()) {
    ballStuck = true;
    if (level < 3) {
        level++;
        menuState = 6; // Show "Level Complete!" screen
        //iResumeTimer(0); // Ensure timers are running
        //initBricks();
        //paddle_x = screenWidth / 2 - paddle_width / 2;
        //ball_x = paddle_x + paddle_width / 2;
        //ball_y = paddle_y + paddle_height;
        //dx = 6;
        //dy = 8;
        ballStuck = true;
    } else {
        // All levels done, show win screen
        menuState = MENU_WIN;
        winTimer = 150; // ~3 seconds at 20ms per frame
        ball_x = screenWidth / 2;
        ball_y = -1000;
        ballStuck = true;

        // Save final score
        FILE *fp = fopen("scores.txt", "a");
        if (fp != NULL) {
            fprintf(fp, "%s %d\n", playerName[0] ? playerName : "Anonymous", score);
            fclose(fp);
        }
    }
  }
}

// MARK: New Function: updateFallingPerks()
void updateFallingPerks() {
    if (menuState != 1 || isPaused) return; // Only update if game is active and not paused

    for (size_t i = 0; i < fallingPerks.size(); ) {
        if (!fallingPerks[i].active) {
            // Remove inactive perks
            fallingPerks.erase(fallingPerks.begin() + i);
            continue;
        }

        fallingPerks[i].y -= fallingPerks[i].speed; // Move perk downwards

        // Check for collision with paddle
        if (fallingPerks[i].y <= paddle_y + paddle_height &&
            fallingPerks[i].y + fallingPerks[i].height >= paddle_y &&
            fallingPerks[i].x + fallingPerks[i].width >= paddle_x &&
            fallingPerks[i].x <= paddle_x + paddle_width) {

            // Paddle hit the perk! Apply effect.
            if (fallingPerks[i].type == PERK_LIFE_UP) {
                life++;
                iPlaySound("Boop.wav", false, 50); // Good perk sound
            } else if (fallingPerks[i].type == PERK_BALL_SPEED_UP) {
                dx = (dx > 0) ? dx + 2 : dx - 2;
                dy = (dy > 0) ? dy + 2 : dy - 2;
                // Cap speed to prevent it from becoming too fast
                if (abs(dx) > 15) dx = (dx > 0) ? 15 : -15;
                if (abs(dy) > 15) dy = (dy > 0) ? 15 : -15;
                iPlaySound("Boop.wav", false, 50); // Good perk sound
            } else if (fallingPerks[i].type == PERK_LIFE_DOWN_RESET) {
                life--;
                iPlaySound("Fall.wav", false, 50); // Trap perk sound
                if (life <= 0) {
                    menuState = 3; // Game Over
                    gameOverTimer = 100;
                    ball_x = screenWidth / 2;
                    ball_y = -1000;
                    ballStuck = true;

                    FILE *fp = fopen("scores.txt", "a");
                    if (fp != NULL)
                    { // rahul new
                        fprintf(fp, "%s %d\n", playerName[0] ? playerName : "Anonymous", score);
                        fclose(fp);
                    }
                } else {
                    // Reset ball to paddle
                    paddle_x = screenWidth / 2 - paddle_width / 2;
                    ball_x = paddle_x + paddle_width / 2;
                    ball_y = paddle_y + paddle_height;
                    dx = 6; // Reset ball speed to default
                    dy = 8;
                    ballStuck = true;
                }
            }
            fallingPerks[i].active = false; // Deactivate the perk
            // Do NOT increment 'i' as erase() shifts elements
        }
        // Check if perk has fallen off screen
        else if (fallingPerks[i].y + fallingPerks[i].height < 0) {
            fallingPerks[i].active = false; // Deactivate
            // Do NOT increment 'i' as erase() shifts elements
        }
        else {
            i++; // Only increment if no element was removed
        }
    }
}


void drawmenu(){
    iSetColor(255, 69, 0); // Orange Red
    iTextAdvanced(270, 520, "MENU", 0.5, 3.5); // Title

    iTextAdvanced(270, 420, "1. Start Game", 0.4, 2.5);
    iTextAdvanced(270, 370, "2. Instructions", 0.4, 2.5);
    iTextAdvanced(270, 320, "3. Leaderboard", 0.4, 2.5); // rahul new
    iTextAdvanced(270, 270, "4. Exit", 0.4, 2.5);
    iTextAdvanced(270, 220, "5. Continue", 0.4, 2.5);
}
void drawInstructions(){
    iSetColor(255, 69, 0); //Orange Red
    iTextAdvanced(250, 520,"Instructons",0.2,3.5);
    iTextAdvanced(250, 420, "Use Mouse to move the paddle",0.2,3.5);
    iTextAdvanced(250, 370,"Break all bricks without missing the ball",0.2,3.5);
    iTextAdvanced(250, 320, "Press 'B' to go back to menu",0.2,3.5 );
}
// MARK: drawgame() -- Modified to draw falling perks
void drawgame(){
    //if(level==1){
        //iShowLoadedImage(0,0,&level1);// Draw at (0,0) to cover the whole screen
    //}
    iSetColor(255, 255, 255);
    iFilledCircle(ball_x, ball_y, ball_radius);
    //iSetColor(255 , 0, 0);
    //iFilledRectangle(paddle_x, paddle_y, paddle_width, paddle_height);
    iShowLoadedImage(paddle_x, paddle_y,&paddleImage);

    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLM; j++) {
            if (bricks[i][j] > 0) {
                int x = brickstartx + j * (brickwidth + 10);
                int y = brickstarty - i * (brickheight + 10);

                // Color for normal bricks based on hits left
                if (bricks[i][j] == BRICK_TYPE_NORMAL_3HIT) // 3-hit purple
                    iSetColor(128, 0, 128);
                else if (bricks[i][j] == BRICK_TYPE_NORMAL_2HIT)//2 hit red
                    iSetColor(255, 0, 0);
                else if (bricks[i][j] == BRICK_TYPE_NORMAL_1HIT) //1 hit green
                    iSetColor(0, 255, 0);
                else if (bricks[i][j] == BRICK_TYPE_SPAWN_PERK) // Perk spawning brick: Distinct color (e.g., Orange)
                    iSetColor(255, 165, 0);

                // Draw the brick
                iFilledRectangle(x, y, brickwidth, brickheight);
            }
        }
    }

    // Draw falling perks
    for (auto& perk : fallingPerks) {
        if (perk.active) {
            iShowLoadedImage(perk.x - perk.width/2, perk.y, &perk.perkImage); // Draw centered horizontally
        }
    }


    for(int i=0;i<life;i++)
    {
        iShowLoadedImage(screenWidth-40*(i+1), screenHeight-40 ,&img);
    }
    iShowLoadedImage(20,screenHeight-50,&scoreIcon);
    char scoreStr[20];
    sprintf(scoreStr,"%d",score);
    iSetColor(255, 255, 255);
    iTextAdvanced(120, screenHeight - 50, scoreStr,0.2,2.5);
    char levelstr[20];
    sprintf(levelstr,"%d",level);
    iTextAdvanced(screenWidth / 2 - 50, screenHeight - 40, levelstr, 0.25, 2.5);
    iSetColor(255, 255, 255);
    iText(10, 10, "Press right mouse button for pause & resume, End for exit");
}


/*
function iDraw() is called again and again by the system.
*/
void iDraw()
{
    // place your drawing codes here
    iClear();

    if(askingName)
    { // rahul new
        drawNameInput();
        return; // Don't draw menu or game yet
    }

    if (menuState == 4) drawLevelSelectMenu();
    //show background image
    if(menuState==0){
        if(musicID==-1){
            musicID=iPlaySound("bgm1.mp3", true, 50);
        }
        drawmenu();
    }
    else{
        if(musicID!=-1){
            iStopSound(musicID);
            musicID=-1;
        }
    }
    if(menuState==1){
        drawgame();
    }
    else if(menuState==2){
        drawInstructions();
    }
    //iText(320, 300, "Welcome to DxBall Game");
    else if(menuState==3)
    {
        iSetColor(255,0,0);
        iTextAdvanced(320,400,"Game Over!",0.4,4.0);
        iTextAdvanced(300,340,"Returning to Menu...",0.3,2.5);
    }
    else if(menuState == MENU_WIN) {
    iSetColor(0, 255, 0);
    iTextAdvanced(300, 400, "🎉 Congratulations!", 0.5, 4.0);
    iTextAdvanced(250, 330, "You have completed all levels!", 0.4, 3.0);
    iTextAdvanced(250, 260, "Returning to Main Menu...", 0.3, 2.5);
}

    else if(menuState == 5) // rahul new
    {
    drawLeaderboard();
    }
    else if(menuState == 6) { // New state: Level Up screen
    //iShowLoadedImage(0, 0, &levelupImage); // Draw level up image full screen
    iSetColor(255, 255, 255);
    char levelText[50];
    sprintf(levelText, "Level %d Complete!", level - 1); // Display the level just completed
    iTextAdvanced(screenWidth / 2 - 150, screenHeight / 2 + 50, levelText, 0.4, 3.0);
    iTextAdvanced(screenWidth / 2 - 150, screenHeight / 2 - 50, "Get Ready for Next Level!", 0.3, 2.5);
    iTextAdvanced(screenWidth / 2 - 400, screenHeight / 2 - 100, "Click left mouse button to go to next level", 0.3, 2.5);
    }
    else if(menuState == MENU_CONTINUE){
    iSetColor(255, 255, 255);
    iTextAdvanced(350, 450, "GAME PAUSED", 0.4, 3.0);
    iTextAdvanced(350, 350, "Click to CONTINUE", 0.3, 2.5);
    iTextAdvanced(350, 300, "Click to EXIT to Main Menu", 0.3, 2.5);
}
}
void winCountdown() {
    if (menuState == MENU_WIN) {
        if (winTimer > 0) winTimer--;
        else menuState = 0; // Return to main menu
    }
}


/*
function iMouseMove() is called when the user moves the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouseMove(int mx, int my)
{
    if(menuState == 1 && !isPaused) { //Paddle only moves if not paused
        paddle_x = mx - paddle_width / 2;
        if (paddle_x < 0) paddle_x = 0;
        if (paddle_x + paddle_width > screenWidth)
            paddle_x = screenWidth - paddle_width;
    }
}


/*
function iMouseDrag() is called when the user presses and drags the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouseDrag(int mx, int my)
{
    // place your codes here
}

/*
function iMouse() is called when the user presses/releases the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouse(int button, int state, int mx, int my){

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        if (menuState == 6) {
        startNextLevel();
        return;
        }
        // Game start on ball click
        if (menuState == 1 && ballStuck) {
            ballStuck = false;
            return;
        }
        // Main Menu clicks
        if(menuState == 0) {
            // Start Game
            if(mx >= MENU_X && mx <= MENU_X + MENU_WIDTH && my >= 420 && my <= 420 + MENU_HEIGHT){
                menuState = 4; // Open level selection screen
            }
            // Instructions
            else if (mx >= MENU_X && mx <= MENU_X + MENU_WIDTH && my >= 370 && my <= 370 + MENU_HEIGHT){
                menuState = 2; // Go to Instructions screen
            }
            // Leaderboard (moved from keyboard to mouse click)
            else if (mx >= MENU_X && mx <= MENU_X + MENU_WIDTH && my >= 320 && my <= 320 + MENU_HEIGHT) {
                menuState = 5;
            }
            // Exit Button
            else if (mx >= MENU_X && mx <= MENU_X + MENU_WIDTH && my >= 270 && my <= 270 + MENU_HEIGHT){
                exit(0); // Exit game
            }
            else if(mx >= MENU_X && mx <= MENU_X + MENU_WIDTH && my >= 220 && my <= 270){
                loadGameState();
                menuState = 1; // Go to game
            }

        }

        // Level Select Menu clicks
        else if(menuState == 4){
            if(mx >= 300 && mx <= 400 && my >= 300 && my <= 400){
                level = 1;
            }
            else if(mx >= 450 && mx <= 550 && my >= 300 && my <= 400){
                level = 2;
            }
            else if(mx >= 600 && mx <= 705 && my >= 300 && my <= 400){
                level = 3;
            }
            else if(mx >= 230 && mx <= 850 && my >= 250 && my <= 310){
                menuState = 0;
                return;
            }
            else {
                return;
            }

            if(level >= 1 && level <= 3){
                // Removed redundant game start logic here as it's handled after name input
                askingName = true; // Ask for name before starting the game
                nameIndex = 0;
                playerName[0] = '\0';
                return; // Wait for name input before starting game
            }
        }
        if (menuState == MENU_CONTINUE && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    // Continue
    if(mx >= 350 && mx <= 650 && my >= 340 && my <= 390) {
        iResumeTimer(0);
        isPaused = false;
        menuState = 1;
    }
    // Exit
    else if(mx >= 350 && mx <= 650 && my >= 290 && my <= 340) {
        saveGameState(); // Save paused game
        menuState = 0;   // Back to main menu
        isPaused = false;
    }
}


    }

    // ✅ Right click to pause/resume (anywhere)
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
        if (menuState == 1){ // Game state
            if (!isPaused) {
                iPauseTimer(0);
                isPaused = true;
                menuState = MENU_CONTINUE; // Go to pause menu
            }
        }
    }
}


/*
function iMouseWheel() is called when the user scrolls the mouse wheel.
dir = 1 for up, -1 for down.
*/
void iMouseWheel(int dir, int mx, int my)
{
    // place your code here
}

/*
function iKeyboard() is called whenever the user hits a key in keyboard.
key- holds the ASCII value of the key pressed.
*/
void iKeyboard(unsigned char key, int state){

    if (state != GLUT_DOWN) return; //  Prevent duplicates
        if (askingName) { // rahul new
        if (key == '\r') { // ENTER key
            askingName = false;
            // Go to game
            score = 0;
            life = 3;
            initBricks();
            paddle_x = screenWidth / 2 - paddle_width / 2;
            ball_x = paddle_x + paddle_width / 2;
            ball_y = paddle_y + paddle_height;
            dx = 6; dy = 8;
            menuState = 1;
            ballStuck = true;
        }
        else if (key == '\b') { // BACKSPACE
            if (nameIndex > 0) {
                nameIndex--;
                playerName[nameIndex] = '\0';
            }
        }
        else if (nameIndex < 49 && key >= 32 && key <= 126) { // visible characters
            playerName[nameIndex++] = key;
            playerName[nameIndex] = '\0';
        }
        return;
    }


    if(menuState==0){
        // Removed numeric keys 1, 2, 3 for menu navigation as it's now handled by mouse clicks.
        // The old key '3' for exit is now associated with the mouse click for 'Leaderboard'.
        // Exit is now bound to key '4' as per original menu.
        if(key=='1' || key=='2' || key=='3' || key=='4') { // Prevent unintended actions for these keys if menu navigation is now mouse-driven
             // No action if menu is mouse controlled.
        }
    }
        else if(menuState==2 && (key=='b'|| key=='B')){
            menuState = 0;//Go back to main menu
        }
        else if(menuState==1){
            if(key== 'p' || key== 'P') {
                iPauseTimer(0);
                isPaused = true; // Update pause state
            }
            if(key== 'r' || key== 'R') {
                iResumeTimer(0);
                isPaused = false; // Update pause state
            }
        }
        else if(menuState == 5 && (key == 'b' || key == 'B')) // rahul new
        {
            menuState = 0;
        }

    }


/*
function iSpecialKeyboard() is called whenver user hits special keys likefunction
keys, home, end, pg up, pg down, arraows etc. you have to use
appropriate constants to detect them. A list is:
GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11,
GLUT_KEY_F12, GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN,
GLUT_KEY_PAGE_UP, GLUT_KEY_PAGE_DOWN, GLUT_KEY_HOME, GLUT_KEY_END,
GLUT_KEY_INSERT */
void iSpecialKeyboard(unsigned char key,int state)
{
    if(key==GLUT_KEY_END) exit(0);
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    iInitializeSound();
    iSetTimer(20, winCountdown); // Timer for win countdown
    iSetTimer(20, ballChange); // place your own initialization codes here.
    iSetTimer(20, gameOverCountdown);
    iSetTimer(20, updateFallingPerks); // *** NEW: Timer for falling perks ***
    iLoadImage(&img,"life.png");
    iLoadImage(&scoreIcon,"score.png");
    iLoadImage(&paddleImage,"paddle_M1.bmp");
    iLoadImage(&LevelSelectBackground,"image.png");
    iLoadImage(&leaderboardImage, "leaderboard.jpg"); // rahul new

    // *** NEW: Load Perk Images ***
    iLoadImage(&perkImage_Life, "1.bmp"); // Assuming 1.bmp is the life perk image
    iLoadImage(&perkImage_Speed, "3.bmp"); // Assuming 3.bmp is the speed perk image
    iLoadImage(&perkImage_Trap, "13.bmp"); // Assuming 13.bmp is the trap perk image
    iLoadImage(&levelupImage, "lvlup.bmp");
    //iLoadImage(&level1,"1.png");
    iOpenWindow(1000, 800, "DxBall");
    return 0;
}