#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <string>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

enum State {MENU, PLAY, WIN};
State s;
bool ai;
const float windowwidth = 800;
const float windowheight = 600;
sf::RenderWindow window(sf::VideoMode(windowwidth, windowheight), "Pong: Slice");

sf::FloatRect bound1;
sf::FloatRect bound2;
sf::FloatRect boundball;

//for drawing the score
sf::Font scoreFont;

//for checking the boost
sf::Clock clock1;
sf::Clock clock2;
bool ballboost;
bool boost1pressed;
bool boost2pressed;
float balldirchange;
const float boostLimit = 10;
const float balldirchangeval = .01f;
const float boostMillies = 500;

sf::SoundBuffer goalBuffer;
sf::Sound goal;
sf::SoundBuffer boopBuffer;
sf::Sound boop;
sf::SoundBuffer engineBuffer;
sf::Sound engine;
sf::SoundBuffer boostBuffer;
sf::Sound usetheboost;

sf::Text score1;
sf::Text score2;
sf::Text winText;
sf::Text win2Text;
sf::Text restText;
sf::Text titleText;
sf::Text playText;
//sf::Text controlsText;

const int winScore = 11;
const int winDifferential = 2;

const float rightbottombound = 35;
const float leftbottombound = 145;
const float righttopbound = 325;
const float lefttopbound = 215;
const float ballstartx = 395;
const float ballstarty = 295;
const float ballvel = .09f;
const float ballsize = 10;
const int randdiroffset = 20;
float ballx;
float bally;
float balldir;

const float padwidth = 25;
const float padheight = 100;
const float padvel = .05f;
const float pad1xstart = 25;
const float pad2xstart = 750;
const float padystart = 250;
float pad1x;
float pad1y;
float pad1vel;
int pad1score;

float pad2x;
float pad2y;
float pad2vel;
int pad2score;

sf::RectangleShape paddle1(sf::Vector2f(padwidth, padheight));
sf::RectangleShape paddle2(sf::Vector2f(padwidth, padheight));

sf::RectangleShape ball(sf::Vector2f(ballsize, ballsize));

//patch for MinGW not recognizing to_string
namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

//set paddles and ball to starting positions
void resetPlay(){
    s = PLAY;

    ballx = ballstartx;
    bally = ballstarty;
    balldir = rand() % 360;
    while ((balldir > rightbottombound && balldir < leftbottombound) || (balldir > lefttopbound && balldir < righttopbound)){
        balldir = rand() % 360;
    }

    pad1x = pad1xstart;
    pad1y = padystart;
    pad1vel = 0;
    pad1score = 0;

    pad2x = pad2xstart;
    pad2y = padystart;
    pad2vel = 0;
    pad2score = 0;

    clock1.restart();
    clock2.restart();
}

//Load required sound/font files
void loadFiles(){
    if (!scoreFont.loadFromFile("Square.ttf")){
        std::cerr << "Could not load Square.ttf" << std::endl;
        exit(1);
    }

    if (!goalBuffer.loadFromFile("goal.wav"))
        exit(1);

    goal.setBuffer(goalBuffer);


    if (!boopBuffer.loadFromFile("boop.wav"))
        exit(1);

    boop.setBuffer(boopBuffer);



    if (!engineBuffer.loadFromFile("engine.wav")){
        exit(1);
    }
    engine.setBuffer(engineBuffer);


    if (!boostBuffer.loadFromFile("usetheboost.wav")){
        exit(1);
    }
    usetheboost.setBuffer(boostBuffer);
}

//Make sure all variables have a starting value at beginning of play
void setVariables(){

    loadFiles();
    resetPlay();

    //seed the random generator
    srand(time(NULL));

    //Start on the main menu
    s = MENU;

    //start with AI on
    ai = true;

    //set bounds for the paddles and ball
    bound1 = paddle1.getGlobalBounds();
    bound2 = paddle2.getGlobalBounds();
    boundball = ball.getGlobalBounds();

    //prep the boost-related variables
    ballboost = false;
    boost1pressed = false;
    boost2pressed = false;
    balldirchange = 0;

    //set fill colors for ball and paddles
    ball.setFillColor(sf::Color::White);
    paddle1.setFillColor(sf::Color::Green);
    paddle2.setFillColor(sf::Color::Green);

    //Text for the scoreboard for each player
    score1.setFont(scoreFont);
    score2.setFont(scoreFont);
    score1.setCharacterSize(48);
    score2.setCharacterSize(48);
    score1.setColor(sf::Color::Green);
    score2.setColor(sf::Color::Green);
    score1.setPosition(sf::Vector2f(350, 10));
    score2.setPosition(sf::Vector2f(450, 10));

    //Text for the winning screens for each player
    winText.setString("Player 1 Wins!");
    winText.setFont(scoreFont);
    winText.setCharacterSize(72);
    winText.setColor(sf::Color::Green);
    winText.setPosition(sf::Vector2f(160, 200));

    win2Text.setString("Player 2 Wins!");
    win2Text.setFont(scoreFont);
    win2Text.setCharacterSize(72);
    win2Text.setColor(sf::Color::Green);
    win2Text.setPosition(sf::Vector2f(150, 200));

    restText.setString("Press 'r' to Restart");
    restText.setFont(scoreFont);
    restText.setCharacterSize(48);
    restText.setColor(sf::Color::Green);
    restText.setPosition(sf::Vector2f(170, 300));

    titleText.setString("Pong: Slice");
    titleText.setFont(scoreFont);
    titleText.setCharacterSize(108);
    titleText.setColor(sf::Color::White);
    titleText.setPosition(110, 150);

    playText.setString("Press SPACE to Play");
    playText.setFont(scoreFont);
    playText.setCharacterSize(48);
    playText.setColor(sf::Color::White);
    playText.setPosition(155, 300);
}

void menuScreen(){
    window.draw(titleText);
    window.draw(playText);
    window.display();

    if (sf::Keyboard::isKeyPressed((sf::Keyboard::Space))){
        resetPlay();
        s = PLAY;
    }
}



void winScreen(){
    if (pad1score > pad2score){
        window.draw(winText);
    }
    else{
        window.draw(win2Text);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
        resetPlay();
    }

    window.draw(restText);
    window.display();
}

void checkScores(){
    if (pad1score >= winScore || pad2score >= winScore){
        s = WIN;
    }
}

void paddleMovement(){
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
        pad1vel = -padvel;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
        pad1vel = padvel;
    }

    if (ai){
        if (pad2y + padheight/2 < bally + ballsize/2){
            pad2vel = padvel;
        } else {
            pad2vel = -padvel;
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
        ai = false;
        pad2vel = -padvel;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
        ai = false;
        pad2vel = padvel;
    }
}

void boostCheck(){
    //Checking for boost power from spacebar
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
        sf::Time elapsed = clock1.getElapsedTime();
        if (elapsed.asSeconds() >= boostLimit){
            clock1.restart();
            //std::cout << "PLAYER 1 BOOST!" << std::endl;
            boost1pressed = true;
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad0) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)){
        sf::Time elapsed = clock2.getElapsedTime();
        if (elapsed.asSeconds() >= boostLimit){
            clock2.restart();
            //std::cout << "PLAYER 2 BOOST!" << std::endl;
            boost2pressed = true;
        }
    }
}

void checkPaddleBounds(){
    //Bounds checking on paddles
    pad1y += pad1vel;
    if (pad1y < 0){
        pad1y = 0;
    } else if (pad1y > windowheight - padheight){
        pad1y = windowheight - padheight;
    }

    pad2y += pad2vel;
    if (pad2y < 0){
        pad2y = 0;
    } else if (pad2y > windowheight - padheight){
        pad2y = windowheight - padheight;
    }
}

void updateBallLocation(){
    //updating ball movements
    balldir += balldirchange;
    if (ballboost){
        ballx += cos(balldir * M_PI/180) * ballvel * 2;
        bally += sin(balldir * M_PI/180) * ballvel * 2;
    } else {
        ballx += cos(balldir * (M_PI/180)) * ballvel;
        bally += sin(balldir * (M_PI/180)) * ballvel;
    }
    ball.setPosition(sf::Vector2f(ballx, bally));
}

void checkBallOnWalls(){
    //ball bouncing off top and bottom walls
    if (bally < 0){
        bally = 0;
        balldir = 360 - balldir;
        boop.play();
        //balldir = 45;
    }

    if (bally > windowheight - ballsize){
        bally = windowheight - ballsize;
        balldir = 360 - balldir;
        boop.play();
        //balldir = 315;
    }
}

void resetBall(){
    ballx = ballstartx;
    bally = ballstarty;
    ballboost = false;
    balldirchange = 0;
    if (engine.getStatus() == sf::Sound::Playing){
        engine.stop();
    }
}

void serveBallLeft(){
    balldir = rand() % 360;
    while ((balldir < leftbottombound || balldir > lefttopbound)){
        balldir = rand() % 360;
    }
}

void serveBallRight(){
    balldir = rand() % 360;
    while ((balldir > rightbottombound && balldir < righttopbound)){
        balldir = rand() % 360;
    }
}

void checkForGoal(){
    //ball going behind paddles to score
    if (ballx < 0 - ballsize){
        pad2score += 1;
        resetBall();
        serveBallRight();
        goal.play();
    }

    if (ballx > windowwidth){
        pad1score += 1;
        resetBall();
        serveBallLeft();
        goal.play();
    }
}

void checkBallOnPaddles(){
    //create bounding boxes for the paddles
    bound1 = paddle1.getGlobalBounds();
    bound2 = paddle2.getGlobalBounds();
    boundball = ball.getGlobalBounds();

    if (bound1.intersects(boundball)){
        ballboost = false;
        balldirchange = 0;
        if (ballx > pad1x){
            engine.stop();
            balldir = 180 + (360 - balldir);
            ballx = pad1x + padwidth + .1f;
            if (bally <= pad1y + padheight/3){
                float ra = rand() % randdiroffset;
                balldir = 315 + ra;
            } else if (bally >= pad1y + (padheight/3 * 2)){
                float ra = rand() % randdiroffset;
                balldir = 35 + ra;
            }

            if (boost1pressed && clock1.getElapsedTime().asMilliseconds() < boostMillies){
                ballboost = true;
                if (pad1vel < 0 && pad1x > 0){
                    balldirchange = balldirchangeval;
                } else if (pad1vel > 0 && pad1x < windowheight - padheight){
                    balldirchange = -balldirchangeval;
                }
                usetheboost.play();
                engine.play();
            }
        } else {
            balldir = 360 - balldir;
        }

        boop.play();
    }

    if (bound2.intersects(boundball)){
        engine.stop();
        ballboost = false;
        balldirchange = 0;
        if (ballx < pad2x){
            balldir = 180 + (360 - balldir);
            ballx = pad2x - (ballsize + .1f);
            if (bally <= pad2y + padheight/3){
                float ra = rand() % randdiroffset;
                balldir = 215 + ra;
            }else if (bally >= pad2y + (padheight/3 * 2)){
                float ra = rand() % randdiroffset;
                balldir = 125 + ra;
            }

            if (boost2pressed && clock2.getElapsedTime().asMilliseconds() < boostMillies){
                ballboost = true;
                if (pad2vel < 0 && pad2x > 0){
                    balldirchange = -balldirchangeval;
                } else if (pad2vel > 0 && pad2y < windowheight - padheight){
                    balldirchange = balldirchangeval;
                }

                usetheboost.play();
                engine.play();
            }
        } else {
            balldir = 360 - balldir;
        }

        boop.play();
    }


}

void setPaddleColors(){
    if (clock1.getElapsedTime().asSeconds() >= boostLimit){
        paddle1.setFillColor(sf::Color::Yellow);
    } else {
        paddle1.setFillColor(sf::Color::Green);
    }

    if (clock2.getElapsedTime().asSeconds() >= boostLimit){
        paddle2.setFillColor(sf::Color::Yellow);
    } else {
        paddle2.setFillColor(sf::Color::Green);
    }
}

void drawPaddles(){
    //draw paddles in new positions
    paddle1.setPosition(sf::Vector2f(pad1x, pad1y));
    paddle2.setPosition(sf::Vector2f(pad2x, pad2y));
    window.draw(paddle1);
    window.draw(paddle2);
}

void drawScores(){
    //draw score
    score1.setString(patch::to_string(pad1score));
    score2.setString(patch::to_string(pad2score));
    window.draw(score1);
    window.draw(score2);
}

void drawBall(){
    //draw ball
    window.draw(ball);
}

void checkIfResetBoost(){
    if (boost1pressed && clock1.getElapsedTime().asMilliseconds() > boostMillies){
        boost1pressed = false;
    }

    if (boost2pressed && clock2.getElapsedTime().asMilliseconds() > boostMillies){
        boost2pressed = false;
    }
}

void setPaddleVelocitiesToZero(){
    pad1vel = 0;
    pad2vel = 0;
}

void updateGame(){
    paddleMovement();

    boostCheck();

    checkPaddleBounds();

    updateBallLocation();

    checkBallOnWalls();

    checkForGoal();

    checkBallOnPaddles();

    setPaddleColors();

    drawPaddles();

    drawScores();

    drawBall();

    setPaddleVelocitiesToZero();

    checkIfResetBoost();

    checkScores();

    window.display();

}

int main()
{
    setVariables();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        //Keyboard input for moving paddles

        if (s == MENU){
            menuScreen();
        }
        else if (s == WIN){
            winScreen();
        }
        else {
            updateGame();
        }
    }

    return 0;
}
