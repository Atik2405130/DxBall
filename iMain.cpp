#include "iGraphics.h"
#include "iSound.h"
#include<stdlib.h>
#include<math.h>
#include<cmath>
#include<algorithm>

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
int score=0;
int level=1;
#define ROW 5
#define COLM 10
int brickwidth=80;
int brickheight=20;
int bricks[ROW][COLM];
int brickstartx=60;
int brickstarty=600;
Image LevelSelectBackground;// Rahul is bad
int menuState=0; //0=Main Menu, 1=Game, 2=Instructions, 3=Game Over
int gameOverTimer=0;
Image paddleImage;
bool ballStuck=true;
// Menu click area constants
#define MENU_X 270
#define MENU_WIDTH 300
#define MENU_HEIGHT 50




void Level3Bricks() {
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
                bricks[i][j] = 3; // solid outer ring
            }
            else if ((i + j) % 2 == 0) {
                bricks[i][j] = (r < 50) ? 2 : 3;
            }
            else if ((i == 2 || i == 3) && (j >= 3 && j <= COLM - 4)) {
                bricks[i][j] = (r < 40) ? 1 : 2;
            }
            else if (r < 25) {
                bricks[i][j] = 1;
            }
        }
    }

    // Center explosion (super dense)
    int cx = COLM / 2;
    for (int i = 1; i <= 3; i++) {
        for (int j = cx - 2; j <= cx + 2; j++) {
            bricks[i][j] = 3;
        }
    }

    // Add fang lines diagonally
    for (int i = 0; i < ROW; i++) {
        int j1 = i;
        int j2 = COLM - 1 - i;
        if (j1 >= 0 && j1 < COLM) bricks[i][j1] = 2;
        if (j2 >= 0 && j2 < COLM) bricks[i][j2] = 2;
    }

    // Scatter some more for chaos
    for (int k = 0; k < 10; k++) {
        int i = rand() % ROW;
        int j = rand() % COLM;
        if (bricks[i][j] == 0) bricks[i][j] = (rand() % 3) + 1;
    }
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
                bricks[i][j] = 1;
    }
    else if(level==2){
        for (int i=0;i<ROW;i++) {
            for (int j=0;j<COLM;j++) {
                if (i+j>=2 && j-i<=7) {
                    //3 hit brick
                    int r=rand()%100;
                    if (r<60){
                        bricks[i][j]=1;
                    } else if(r<90){
                        bricks[i][j]=2;
                    } else {
                        bricks[i][j]=3;
                    }
                }
            }
        }
    }
    else if (level == 3) {
        Level3Bricks();
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

void ballChange(){
    if(menuState!=1) return;
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
        iPlaySound("Wall.wav",false,50); 
    } else if (ball_x - ball_radius < 0) {
        ball_x = ball_radius;
        dx = -dx;
        iPlaySound("Wall.wav",false,50); 
    }
    
    if(ball_y + ball_radius > screenHeight){
        ball_y=screenHeight-ball_radius;
        dy = -dy;
        iPlaySound("Wall.wav",false,50);
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
        iPlaySound("Boing.wav",false,50);         
    }

    // Missed ball
    if(ball_y - ball_radius < 0){
        iPlaySound("Fall.wav",false,50);
        life--;
        if(life<=0){
            menuState=3;
            gameOverTimer=100;
            ball_x=screenWidth/2;
            ball_y=-1000;
            ballStuck=true;
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
                    
                    bricks[i][j]--;
                    if(bricks[i][j]==0) score+=10;
                    iPlaySound("Boop.wav",false,50);
                    
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
}
void drawmenu(){
    iSetColor(255, 69, 0); // Orange Red
    iTextAdvanced(270, 520, "MENU", 0.5, 3.5); // Title

    iTextAdvanced(270, 420, "1. Start Game", 0.4, 2.5);
    iTextAdvanced(270, 370, "2. Instructions", 0.4, 2.5);
    iTextAdvanced(270, 320, "3. Exit", 0.4, 2.5);
}
void drawInstructions(){
    iSetColor(255, 69, 0); //Orange Red
    iTextAdvanced(250, 520,"Instructons",0.2,3.5);
    iTextAdvanced(250, 420, "Use Mouse to move the paddle",0.2,3.5);
    iTextAdvanced(250, 370,"Break all bricks without missing the ball",0.2,3.5);
    iTextAdvanced(250, 320, "Press 'B' to go back to menu",0.2,3.5 );
}
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
                //3 hit purple
                if (bricks[i][j] == 3)
                    iSetColor(128, 0, 128);
                else if (bricks[i][j] == 2)//2 hit red
                    iSetColor(255, 0, 0);
                else
                    iSetColor(0, 255, 0);//1 hit green
                // Draw the brick
                iFilledRectangle(x, y, brickwidth, brickheight);
            }
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
    iText(10, 10, "Press p for pause, r for resume, End for exit");
}


/*
function iDraw() is called again and again by the system.
*/
void iDraw()
{
    // place your drawing codes here
    iClear();
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
}


/*
function iMouseMove() is called when the user moves the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouseMove(int mx, int my)
{
    if(menuState == 1){
        paddle_x = mx-paddle_width/2;
        if(paddle_x< 0 ) paddle_x=0;
        if(paddle_x+paddle_width>screenWidth)
        paddle_x=screenWidth-paddle_width;
    }


    // place your codes here
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
void iMouse(int button,int state,int mx,int my){
    if (button==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
        if (menuState == 1 && ballStuck) {
            ballStuck=false;
            return;
        }
        if(menuState==0) {
            if(mx>=MENU_X && mx<=MENU_X+MENU_WIDTH && my>=420 && my<=420+MENU_HEIGHT){
                menuState=4; // Open level selection screen
            }
            else if (mx>=MENU_X && mx<=MENU_X+MENU_WIDTH && my>=370 && my<=370+MENU_HEIGHT){
                menuState=2; // Go to Instructions screen
            }
        } else if(menuState==4){ // Level Select Menu clicks
            // Coordinates based on the 'image.png' provided (estimated)
            // You might need to adjust these precise pixel values based on your image's exact layout.
            // Level 1: Around (345, 500) to (445, 570)
            if(mx>=300 && mx<=400 && my>=300 && my<=400){
                level=1;
            }
            // Level 2: Around (475, 500) to (575, 570)
            else if (mx>=450 && mx<=550 && my>=300 && my<=400){
                level=2;
            }
            // Level 3: Around (605, 500) to (705, 570)
            else if (mx>=600 && mx<=705 && my>=300 && my<=400) {
                level=3;
            }
            // Back to Main Menu
            else if (mx>=230 && mx<=850 && my>=250 && my<=310){// Keep text if desired
                menuState=0;
                return;
            } else { // If clicked outside level numbers, don't change level.
                return;
            }

            if (level>=1 && level<=3) { // If a valid level was selected
                score=0;
                life=3;
                initBricks();
                paddle_x=screenWidth / 2 - paddle_width / 2;
                ball_x=paddle_x+paddle_width/2;
                ball_y=paddle_y+paddle_height;
                dx=6; dy=8; // Reset ball speed
                menuState=1; // Go to Game Play state
                ballStuck=true; // Ball starts stuck to paddle
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
    if(menuState==0){
        if(key=='1'){
            life=3;
            menuState=1;
            paddle_x = screenWidth/2 - paddle_width/2;
            ball_x = paddle_x + paddle_width/2;
            ball_y = paddle_y + paddle_height;
            //paddle_x= 350;//start Game
            dx=6;
            dy=8;
            gameOverTimer=0;
            initBricks();
            ballStuck=true;
        }
        else if(key=='2'){
            menuState=2;//Instructions
        }
        else if(key=='3'){
        exit(0);//exit game
        }
    }
        else if(menuState==2 && (key=='b'|| key=='B')){
            menuState = 0;//Go back to main menu
        }
        else if(menuState==1){
            if(key== 'p' || key== 'P') iPauseTimer(0);
            if(key== 'r' || key== 'R') iResumeTimer(0);
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

    iSetTimer(20, ballChange); // place your own initialization codes here.
    iSetTimer(20, gameOverCountdown);
    iLoadImage(&img,"life.png");
    iLoadImage(&scoreIcon,"score.png");
    iLoadImage(&paddleImage,"paddle_M1.bmp");
    iLoadImage(&LevelSelectBackground,"image.png");
    //iLoadImage(&level1,"1.png");
    iOpenWindow(1000, 800, "DxBall");
    return 0;
} 
