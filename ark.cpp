#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

constexpr int windowWidth{800}, windowHeight{600};
constexpr float ballRadius{8.f}, ballVelocity{6.f};
constexpr float paddleWidth{70.f}, paddleHeight{15.f}, paddleVelocity{14.f};
constexpr float blockWidth{60.f}, blockHeight{20.f};
constexpr int countBlocksX{11}, countBlocksY{4};

class Ball  // "class" for the Ball
{
  public:
    Ball(float mX, float mY) // create the Ball constructor
    {
        // Apply position, radius, color, origin to 'ballobj'
        ballobj.setPosition(mX, mY);
        ballobj.setRadius(ballRadius);
        ballobj.setFillColor(Color::Red);
        ballobj.setOrigin(ballRadius, ballRadius);
    }
    
    void update()  // Move the ball at the current velocity
    {
        ballobj.move(velocity);
        
        // Keep ball inside the window
        if (left() < 0)
            velocity.x = ballVelocity;
        else if (right() > windowWidth)
            velocity.x = -ballVelocity;
            
        if (top() < 0)
            velocity.y = ballVelocity;
        else if (bottom() > windowHeight)
            velocity.y = -ballVelocity;
    }
    
    // Accessor methods
    float x()       { return ballobj.getPosition().x; }
    float y()       { return ballobj.getPosition().y; }
    float left()    { return x() - ballobj.getRadius(); }
    float right()   { return x() + ballobj.getRadius(); }
    float top()     { return y() - ballobj.getRadius(); }
    float bottom()  { return y() + ballobj.getRadius(); }
    CircleShape getobj()    { return ballobj; }
    
    // Mutator methods
    void setpos(float x, float y)   { ballobj.setPosition(x, y); }
    void setvelocity_x(float x)     { velocity.x = x; }
    void setvelocity_y(float y)     { velocity.y = y; }
    
  private:
    CircleShape ballobj;  // SFML class for a circle
    Vector2f velocity{-ballVelocity, -ballVelocity};  // ball velocity
};

class Brick
{
  public:    
    Brick(float mX, float mY)
    {
        brickobj.setPosition(mX, mY);
        brickobj.setSize({blockWidth, blockHeight});
        brickobj.setFillColor(Color::Yellow);
        brickobj.setOrigin(blockWidth / 2.f, blockHeight / 2.f);
    }
    
    // Accessor methods
    float x()       { return brickobj.getPosition().x; }
    float y()       { return brickobj.getPosition().y; }
    float left()    { return x() - brickobj.getSize().x / 2.f; }
    float right()   { return x() + brickobj.getSize().x / 2.f; }
    float top()     { return y() - brickobj.getSize().y / 2.f; }
    float bottom()  { return y() + brickobj.getSize().y / 2.f; }  
    RectangleShape getobj()    { return brickobj; }
    
    bool destroyed = false;  // data member for "destroyed" status
    
  private:
    RectangleShape brickobj;  // SFML class for a rectangle 
};

class Paddle
{
  public:  
    Paddle(float mX, float mY) // create the Ball constructor
    {
        // Apply position, radius, color, origin to 'paddleobj'
        paddleobj.setPosition(mX, mY);
        paddleobj.setSize({paddleWidth, paddleHeight});
        paddleobj.setFillColor(Color::Red);
        paddleobj.setOrigin(paddleWidth / 2.f, paddleHeight / 2.f);
    }
    
    void update()  // Move the paddle
    {
        paddleobj.move(velocity);
        
        // Keep paddle inside the window
        if (Keyboard::isKeyPressed(Keyboard::Key::Left)
            && left() > 0)
            velocity.x = -paddleVelocity;
        else if (Keyboard::isKeyPressed(Keyboard::Key::Right)
                 && right() < windowWidth)
            velocity.x = paddleVelocity;
        else
            velocity.x = 0;
        
    }
    
    // Accessor methods
    float x()       { return paddleobj.getPosition().x; }
    float y()       { return paddleobj.getPosition().y; }
    float left()    { return x() - paddleobj.getSize().x / 2.f; }
    float right()   { return x() + paddleobj.getSize().x / 2.f; }
    float top()     { return y() - paddleobj.getSize().y / 2.f; }
    float bottom()  { return y() + paddleobj.getSize().y / 2.f; }
    RectangleShape getobj()         { return paddleobj; }
    
  private:
    RectangleShape paddleobj;  // SFML class for a rectangle
    Vector2f velocity;  // paddle velocity
};

template<class T1, class T2> bool Collision(T1& mA, T2& mB);
void testCollision(Paddle& mPaddle, Ball& mBall);
void testCollision(Brick& mBrick, Ball& mBall);
bool testBottomCollision(Ball& mBall);
void init(Ball& ball, vector<Brick>& bricks);
bool menu(Font& typeface, RenderWindow& rw);
void gameloop(Paddle& p, Ball& ball, vector<Brick>& bricks, RenderWindow& rw, 
              bool& failed);
void gameover(Paddle& p, Ball& ball, vector<Brick>& bricks, Font& typeface, 
              RenderWindow& rw, bool& failed);

int score;
			  
int main()
{
    // Create instance of a Ball, Paddle, & Brick
    Ball ball{windowWidth / 2, windowHeight / 2};
    Paddle paddle{windowWidth / 2, windowHeight - 50};
    vector<Brick> bricks;
	score = 0;
    
    bool failed, quit;
    
    for (int x = 0; x < countBlocksX; x++) {
        for (int y = 0; y < countBlocksY; y++) {
            bricks.emplace_back((x + 1) * (blockWidth + 3) + 22,
                                (y + 2) * (blockHeight + 3));
        }
    }
    
    // Creation of game window
    RenderWindow window{{windowWidth, windowHeight}, "ArkanoidX"};
    window.setFramerateLimit(60);
    
    Font arial;
    if (!arial.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        return 0;
    }
    
    quit = menu(arial, window);
    if (quit == true)
        return 0;
    
    // Main gameplay loop
    gameloop(paddle, ball, bricks, window, failed);
    
    // Game-over screen w/ option to restart game
    gameover(paddle, ball, bricks, arial, window, failed);
    
    return 0;
}

// Function to test whether two shapes collide
template<class T1, class T2> bool Collision(T1& mA, T2& mB)
{
    return mA.right() >= mB.left() && mA.left() <= mB.right()
           && mA.bottom() >= mB.top() && mA.top() <= mB.bottom();
}

void testCollision(Paddle& mPaddle, Ball& mBall)
{
    // If no collision, exit
    if (!Collision(mPaddle, mBall))
        return;
        
    // Otherwise move the ball back up
    mBall.setvelocity_y(-ballVelocity);
    
    // And direct it depending on where the paddle was hit
    if (mBall.x() < mPaddle.x())
        mBall.setvelocity_x(-ballVelocity);
    else
        mBall.setvelocity_x(ballVelocity);
        
    return;
}

void testCollision(Brick& mBrick, Ball& mBall)
{
    // If no collision, exit
    if (!Collision(mBrick, mBall))
        return;
        
    // Otherwise the brick has been hit
    mBrick.destroyed = true;
	score++;
    
    // Calculate collision from every direction
    float overlapLeft = mBall.right() - mBrick.left();
    float overlapRight = mBrick.right() - mBall.left();
    float overlapTop = mBall.bottom() - mBrick.top();
    float overlapBottom = mBrick.bottom() - mBall.top();
    
    bool ballFromLeft(abs(overlapLeft) < abs(overlapRight));
    bool ballFromTop(abs(overlapTop) < abs(overlapBottom));
    float minOverlapX = ballFromLeft ? overlapLeft : overlapRight;
    float minOverlapY = ballFromTop ? overlapTop : overlapBottom;
    
    // If magnitude of X overlap is less than Y overlap, the ball hit the brick
    // horizontally, otherwise the ball hit the brick vertically
    // Change X or Y velocity as appropriate for a "realistic" collision effect
    if (abs(minOverlapX) < abs(minOverlapY))
        mBall.setvelocity_x(ballFromLeft ? -ballVelocity : ballVelocity);
    else
        mBall.setvelocity_y(ballFromTop ? -ballVelocity : ballVelocity);
        
    return;
}

bool testBottomCollision(Ball& mBall)
{
    if (mBall.y() >= windowHeight - ballRadius)
        return true;
    else
        return false;
}

void init(Ball& ball, vector<Brick>& bricks)
{
    ball.setpos(windowWidth / 2, windowHeight / 2);
    
    bricks.clear();
    for (int iX = 0; iX < countBlocksX; iX++) {
        for (int iY = 0; iY < countBlocksY; iY++) {
            bricks.emplace_back((iX + 1) * (blockWidth + 3) + 22,
                                (iY + 2) * (blockHeight + 3));
        }
    }
    
    return;
}

bool menu(Font& typeface, RenderWindow& rw)
{   
    Text title, info1, play, quit;
    title.setFont(typeface);
    info1.setFont(typeface);
    play.setFont(typeface);
    quit.setFont(typeface);
    
    while (true) {
        rw.clear(Color::Black);
        title.setPosition(340, 20);
        title.setString("ArkanoidX");
        title.setCharacterSize(24);
        title.setColor(Color::White);
        
        info1.setPosition(300, 300);
        info1.setString("Arrow keys move the paddle");
        info1.setCharacterSize(16);
        info1.setColor(Color::White);
        
        play.setPosition(330, 500);
        play.setString("Hit Spacebar to play");
        play.setCharacterSize(16);
        play.setColor(Color::White);
        
        quit.setPosition(302, 530);
        quit.setString("Hit Esc or q to exit the game");
        quit.setCharacterSize(16);
        quit.setColor(Color::White);
        
        rw.draw(title);
        rw.draw(info1);
        rw.draw(play);
        rw.draw(quit);

        if (Keyboard::isKeyPressed(Keyboard::Key::Space))
            return false;
            
        if (Keyboard::isKeyPressed(Keyboard::Key::Escape) ||
            Keyboard::isKeyPressed(Keyboard::Key::Q))
            return true;
            
        rw.display();
    }
    
    return true;
}

void gameloop(Paddle& p, Ball& ball, vector<Brick>& bricks, RenderWindow& rw, bool& failed)
{
	Text myScore;
    myScore.setFont(typeface);
    while (true)
    {
        rw.clear(Color::Black);  // clear the window
        title.setPosition(640, 20);
        title.setString(score);
        // If "Esc" or "q" is pressed, quit the game
        if (Keyboard::isKeyPressed(Keyboard::Key::Escape) ||
            Keyboard::isKeyPressed(Keyboard::Key::Q))
            break;
        
        ball.update();
        p.update();
        testCollision(p, ball);
        
        if (testBottomCollision(ball) == true) {
            failed = true;
            break;
        }
        
        // Test for ball & brick collision, and update the screen for a hit
        for(auto& brick : bricks) testCollision(brick, ball);
        bricks.erase(remove_if(begin(bricks), end(bricks),
            [](const Brick& mBrick){ return mBrick.destroyed; }),
            end(bricks)); // STL algorithm & C++11 lambda
            
        if (bricks.size() == 0) {
            failed = false;
            break;
        }
        
        // Render Ball on the window
        rw.draw(ball.getobj());
        rw.draw(p.getobj());
        for(auto& brick : bricks) rw.draw(brick.getobj()); // C++11 'foreach'
        rw.display();
    }
    
    return;
}

void gameover(Paddle& p, Ball& ball, vector<Brick>& bricks, Font& typeface, 
              RenderWindow& rw, bool& failed)
{
    while (true)
    {
        rw.clear(Color::Black);  // clear the window
        
        Text end, retry;
        
        if (failed == true) {
            end.setFont(typeface);
            end.setPosition(356, 300);
            end.setString("Game Over");
            end.setCharacterSize(24);
            end.setColor(Color::White);
            
            retry.setFont(typeface);
            retry.setPosition(362, 340);
            retry.setString("Try Again? (Y/N)");
            retry.setCharacterSize(16);
            retry.setColor(Color::White);
        }
        else if (failed == false) {
            end.setPosition(348, 300);
            end.setString("Level Passed!");
            end.setCharacterSize(24);
            end.setColor(Color::White);
        }
        
        // If "Esc" or "q" is pressed, quit the game
        if (Keyboard::isKeyPressed(Keyboard::Key::Escape) ||
            Keyboard::isKeyPressed(Keyboard::Key::Q))
            break;
            
        if (Keyboard::isKeyPressed(Keyboard::Key::Y)) {
            init(ball, bricks);\
			score = 0;
            gameloop(p, ball, bricks, rw, failed);
        }
            
        else if (Keyboard::isKeyPressed(Keyboard::Key::N))
            break;
        
        rw.draw(retry);
        rw.draw(end);
        rw.display();
    }
    
    return;
}
