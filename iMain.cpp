#include "iGraphics.h"
// Atik is a bad boy.
int screenWidth = 1000, screenHeight = 800;
// Ball
int ball_x, ball_y;
int dx=6, dy=8;
int ball_radius = 8;
// Paddle
int paddle_x, paddle_y=30;
int paddle_width = 120,paddle_height = 15;

int menuState=0; //0=Main Menu, 1=Game, 2=Instructions

void ballChange(){
    ball_x+= dx;
    ball_y+= dy;
    
    if(ball_x + ball_radius > screenWidth || ball_x - ball_radius < 0)
    {
        if(ball_x + ball_radius > screenWidth)
           ball_x= screenWidth - ball_radius;
        else
           ball_x = ball_radius;
        dx = -dx;   
    }

    if(ball_y + ball_radius > screenHeight || ball_y - ball_radius < 0)
    {
        if(ball_y + ball_radius > screenHeight)
           ball_y = screenHeight - ball_radius;
        else
           ball_y= ball_radius;
        dy = -dy;
    }
    if(ball_y - ball_radius <= paddle_y + paddle_height && ball_x>= paddle_x && ball_x <= paddle_x + paddle_width && dy<0){
        dy=-dy;
        ball_y = paddle_y + paddle_height + ball_radius;// Prevent sticking        
    }
    if(ball_y - ball_radius < 0){
        ball_x = paddle_x;
        ball_y =paddle_y;
        dx=5;
        dy=7;
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
    iSetColor(255, 255, 255);
    iFilledCircle(ball_x, ball_y, ball_radius);
    iSetColor(255 , 0, 0);
    iFilledRectangle(paddle_x, paddle_y, paddle_width, paddle_height);
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
    //show background image
    if(menuState==0){
        drawmenu();
    }
    else if(menuState==1){
        drawgame();
    }
    else if(menuState==2){
        drawInstructions();
    }
    //iText(320, 300, "Welcome to DxBall Game");
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
        if(paddle_x + paddle_width > screenWidth)
        paddle_x = screenWidth - paddle_width;
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
void iMouse(int button, int state, int mx, int my)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {        

    }
        // place your codes here
    
    if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        // place your codes here
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
void iKeyboard(unsigned char key){
    if(menuState==0){
        if(key=='1'){
            menuState=1;
            paddle_x = screenWidth/2 - paddle_width/2;
            ball_x = paddle_x + paddle_width/2;
            ball_y = paddle_y + paddle_height;
            paddle_x= 350;//start Game
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
void iSpecialKeyboard(unsigned char key)
{
    switch (key)
    {
    case GLUT_KEY_END:exit(0);
        // do something
        break;
    // place your codes for other keys here
    default:
        break;
    }
}

int main(int argc, char *argv[])
{   
    glutInit(&argc, argv);
    iSetTimer(20, ballChange); // place your own initialization codes here.
    iInitialize(1000, 800, "DxBall - Menu");
    return 0;
}