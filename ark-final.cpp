#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <iostream>
#include <sstream>

using namespace std;
using namespace sf;

constexpr int windowWidth{800}, windowHeight{600};
constexpr float ballRadius{8.f}, ballVelocity{4.f};
constexpr float paddleWidth{70.f}, paddleHeight{15.f}, paddleVelocity{14.f};
constexpr float blockWidth{60.f}, blockHeight{20.f};
constexpr int countBlocksX{11}, countBlocksY{4};

class Ball
{
  public:
    Ball() { }
    Ball(float mX, float mY, Color mC)
    {
        ballobj.setPosition(mX, mY);
        ballobj.setRadius(ballRadius);
        ballobj.setFillColor(mC);
        ballobj.setOrigin(ballRadius, ballRadius);
    }

    void update() // Move the ball at the current velocity
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
    CircleShape ballobj; // SFML class for a circle
    Vector2f velocity{-ballVelocity, -ballVelocity};
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

    bool destroyed = false; // "destroyed" status for an individual brick

  private:
    RectangleShape brickobj; // SFML class for a rectangle
};

class Paddle
{
  public:
    Paddle(float mX, float mY)
    {
        paddleobj.setPosition(mX, mY);
        paddleobj.setSize({paddleWidth, paddleHeight});
        paddleobj.setFillColor(Color::White);
        paddleobj.setOrigin(paddleWidth / 2.f, paddleHeight / 2.f);
    }

    void update() // Move the paddle
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
    RectangleShape paddleobj; // SFML class for a rectangle
    Vector2f velocity;
};

class GameEngine
{
  public:
    GameEngine() { }
    ~GameEngine() { bricks.clear(); }
	void init();
	void reinit();
	void menu();
	void gameloop();
	void gameover();
    void pause_game();
    void reallyquit();
	void testCollision(Paddle& mPaddle, Ball& mBall);
	bool testBottomCollision(Ball& mBall);
    template<class T1, class T2> void testCollision(T1& mA, T2& mB);
    template<class T1, class T2> bool Collision(T1& mA, T2& mB);

  private:
    bool failed;
    bool quit;
    int score;

	Ball ball{windowWidth / 2, windowHeight / 2, Color::White};
    Ball badBall{windowWidth / 3, windowHeight / 3, Color::Red};
	Paddle paddle{windowWidth / 2, windowHeight - 50};
	vector<Brick> bricks;

    Font typeface;
	RenderWindow window{{windowWidth, windowHeight}, "ArkanoidX"};
};

int main()
{
    GameEngine game;
    game.init();

    return EXIT_SUCCESS;
}

void GameEngine::init()
{
    score = 0;

    for (int x = 0; x < countBlocksX; x++) {
        for (int y = 0; y < countBlocksY; y++) {
            bricks.emplace_back((x + 1) * (blockWidth + 3) + 22,
                                (y + 2) * (blockHeight + 3));
        }
    }

    window.setFramerateLimit(60);

    if (!typeface.loadFromFile("arial.ttf")) {
        return;
    }

    menu();
}

void GameEngine::reinit()
{
    ball.setpos(windowWidth / 2, windowHeight / 2);
    ball.setvelocity_x(-ballVelocity);
    ball.setvelocity_y(-ballVelocity);

    badBall.setpos(windowWidth / 3, windowHeight / 3);
    badBall.setvelocity_x(ballVelocity);
    badBall.setvelocity_y(-ballVelocity);

    bricks.clear();
    for (int iX = 0; iX < countBlocksX; iX++) {
        for (int iY = 0; iY < countBlocksY; iY++) {
            bricks.emplace_back((iX + 1) * (blockWidth + 3) + 22,
                                (iY + 2) * (blockHeight + 3));
        }
    }

    score = 0;
    gameloop();
}

void GameEngine::menu()
{
    Text line1, line2, line3;
    Text line4a, line4b, line4c, line4d;
    Text line5a, line5b, line5c;
    line1.setFont(typeface); line2.setFont(typeface); line3.setFont(typeface);
    line4a.setFont(typeface); line4b.setFont(typeface);
    line4c.setFont(typeface); line4d.setFont(typeface);
    line5a.setFont(typeface); line5b.setFont(typeface);
    line5c.setFont(typeface);

    Texture title;
    if (!title.loadFromFile("arkanoid-main.png"))
        return;

    Sprite s_title;
    s_title.setTexture(title);
    s_title.setPosition(90, 10);

    while (true) {
        window.clear(Color::Black);

        line1.setString("How To Play:");
        line1.setPosition(50, 405);
        line1.setCharacterSize(20); line1.setColor(Color::White);

        line2.setString("- Left and Right arrow keys move the paddle");
        line2.setPosition(50, 430);
        line2.setCharacterSize(16); line2.setColor(Color::White);

        line3.setString("- Press Space to start playing!");
        line3.setPosition(50, 450); line3.setStyle(Text::Bold);
        line3.setCharacterSize(16); line3.setColor(Color::Green);

        line4a.setString("Scoring:"); line4a.setStyle(Text::Bold);
        line4b.setString("+5 points for every destroyed brick");
        line4c.setString("-20 points if you touch the red ball");
        line4d.setString("Game ends if your score drops below 0!");
        line4a.setPosition(480, 480); line4b.setPosition(480, 500);
        line4c.setPosition(480, 520); line4d.setPosition(480, 540);
        line4a.setCharacterSize(16); line4b.setCharacterSize(16);
        line4c.setCharacterSize(16); line4d.setCharacterSize(16);
        line4a.setColor(Color::White); line4b.setColor(Color::White);
        line4c.setColor(Color::White); line4d.setColor(Color::White);

        line5a.setString("Keyboard Commands:"); line5a.setStyle(Text::Bold);
        line5b.setString("- Esc or Q to quit");
        line5c.setString("- P to pause the game while playing");
        line5a.setPosition(50, 500); line5b.setPosition(50, 520);
        line5c.setPosition(50, 540);
        line5a.setCharacterSize(16); line5b.setCharacterSize(16);
        line5c.setCharacterSize(16);
        line5a.setColor(Color::White); line5b.setColor(Color::White);
        line5c.setColor(Color::White);

        window.draw(s_title);
        window.draw(line1); window.draw(line2); window.draw(line3);
        window.draw(line4a); window.draw(line4b);
        window.draw(line4c); window.draw(line4d);
        window.draw(line5a); window.draw(line5b); window.draw(line5c);

        if (Keyboard::isKeyPressed(Keyboard::Key::Space))
            reinit();

        if (Keyboard::isKeyPressed(Keyboard::Key::Escape) ||
            Keyboard::isKeyPressed(Keyboard::Key::Q))
            reallyquit();

        window.display();
    }
}

void GameEngine::gameloop()
{
    Text scorelabel, current_score;
    stringstream ss;
    scorelabel.setFont(typeface);
    current_score.setFont(typeface);

    while (true)
    {
        window.clear(Color::Black);

        scorelabel.setString("Score:");
        scorelabel.setPosition(700, 565); scorelabel.setCharacterSize(16);
        scorelabel.setColor(Color::White);

        current_score.setString(ss.str());
        current_score.setPosition(750, 565);
        current_score.setCharacterSize(16);
        current_score.setColor(Color::White);

        // If "Esc" or "q" is pressed, go back to the main menu
        if (Keyboard::isKeyPressed(Keyboard::Key::Escape) ||
            Keyboard::isKeyPressed(Keyboard::Key::Q)) {
            menu();
        }

        ball.update();
        paddle.update();
        testCollision(paddle, ball);

        badBall.update();
        testCollision(paddle, badBall);
        ss.str("");
        ss.clear();
        ss << score;

        if ((testBottomCollision(ball) == true) || (score < 0)) {
            failed = true;
            gameover();
            break;
        }

        if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
            bricks.erase(end(bricks));
        } // "cheat" key to manually destroy bricks (for testing)

        if (Keyboard::isKeyPressed(Keyboard::Key::P)) {
            pause_game();
        }

        // Test for ball & brick collision, and update the screen for a hit
        for(auto& brick : bricks) testCollision(brick, ball);
        bricks.erase(remove_if(begin(bricks), end(bricks),
            [](const Brick& mBrick){ return mBrick.destroyed; }),
            end(bricks)); // STL algorithm & C++11 lambda

        if (bricks.size() == 0) {
            failed = false;
            gameover();
            break;
        }

        // Render the objects to the window
        window.draw(ball.getobj());
        window.draw(paddle.getobj());
        window.draw(badBall.getobj());
        window.draw(scorelabel);
        window.draw(current_score);
        for(auto& brick : bricks) window.draw(brick.getobj()); // C++11 'foreach'
        window.display();
    }

    return;
}

void GameEngine::gameover()
{
    Text end1, end2, retry;
    end1.setFont(typeface); end2.setFont(typeface);
    retry.setFont(typeface);

    // check how the game ended
    if (failed == true) {
        end1.setString("Game Over");
        end1.setPosition(339, 270); end1.setCharacterSize(24);
        end1.setColor(Color::White);

        retry.setString("Try Again? (Y/N)");
        retry.setPosition(345, 300); retry.setCharacterSize(16);
        retry.setColor(Color::White);
    }
    else {
        end1.setString("Level Passed!");
        end1.setPosition(326, 270); end1.setCharacterSize(24);
        end1.setColor(Color::White);

        end2.setString("Would you like to play again? (Y/N)");
        end2.setPosition(282, 300); end2.setCharacterSize(16);
        end2.setColor(Color::White);
    }

    while (true)
    {
        window.clear(Color::Black);

        if (failed == true) {
            window.draw(end1);
            window.draw(retry);
        }
        else {
            window.draw(end1);
            window.draw(end2);
        }

        if (Keyboard::isKeyPressed(Keyboard::Key::N))
            menu();

        if (Keyboard::isKeyPressed(Keyboard::Key::Y))
            reinit();

        window.display();
    }

    return;
}

void GameEngine::pause_game()
{
    Text paused, unpause;
    paused.setFont(typeface); unpause.setFont(typeface);

    paused.setString("Game Paused");
    paused.setPosition(340, 250); paused.setCharacterSize(20);
    paused.setColor(Color::White);

    unpause.setString("(R) Resume game");
    unpause.setPosition(340, 300); unpause.setCharacterSize(16);
    unpause.setColor(Color::White);

    while (true)
    {
        window.clear(Color::Black);
        window.draw(paused);
        window.draw(unpause);

        if (Keyboard::isKeyPressed(Keyboard::Key::R)) {
            break;
        }

        if (Keyboard::isKeyPressed(Keyboard::Key::Escape) ||
            Keyboard::isKeyPressed(Keyboard::Key::Q))
            return;

        window.display();
    }
}

void GameEngine::reallyquit()
{
    Text quit1, quit2;
    quit1.setFont(typeface); quit2.setFont(typeface);

    quit1.setString("Are you sure you want to quit? (Y/N)");
    quit1.setPosition(242, 270); quit1.setCharacterSize(20);
    quit1.setColor(Color::White);

    quit2.setString("N will end any games in progress and go back to the "
                    "main menu");
    quit2.setPosition(182, 300); quit2.setCharacterSize(16);
    quit2.setColor(Color::White);

    while (true)
    {
        window.clear(Color::Black);
        window.draw(quit1);
        window.draw(quit2);

        // This is the only point where we have to forcefully end the program
        // since we can't go back anywhere else
        if (Keyboard::isKeyPressed(Keyboard::Key::Y)) {
            exit(0);
        }
        else if (Keyboard::isKeyPressed(Keyboard::Key::N)) {
            return;
        }

        window.display();
    }
}

void GameEngine::testCollision(Paddle& mPaddle, Ball& mBall)
{
    // If no collision, exit
    if (!Collision(mPaddle, mBall))
        return;

    // Is the ball the bad one? If so, decrement the score
    if (mBall.getobj().getFillColor() == Color::Red)
        score -= 20;

    // Otherwise move the ball back up
    mBall.setvelocity_y(-ballVelocity);

    // And direct it depending on where the paddle was hit
    if (mBall.x() < mPaddle.x())
        mBall.setvelocity_x(-ballVelocity);
    else
        mBall.setvelocity_x(ballVelocity);

    return;
}

template<class T1, class T2>
void GameEngine::testCollision(T1& mA, T2& mB)
{
    // If no collision, exit
    if (!Collision(mA, mB))
        return;

    // Otherwise the brick has been hit
    mA.destroyed = true;
    score += 5;

    // Calculate collision from every direction
    float overlapLeft = mB.right() - mA.left();
    float overlapRight = mA.right() - mB.left();
    float overlapTop = mB.bottom() - mA.top();
    float overlapBottom = mA.bottom() - mB.top();

    bool ballFromLeft(abs(overlapLeft) < abs(overlapRight));
    bool ballFromTop(abs(overlapTop) < abs(overlapBottom));
    float minOverlapX = ballFromLeft ? overlapLeft : overlapRight;
    float minOverlapY = ballFromTop ? overlapTop : overlapBottom;

    // If magnitude of X overlap is less than Y overlap, the ball hit the brick
    // horizontally, otherwise the ball hit the brick vertically
    // Change X or Y velocity as appropriate for a "realistic" collision effect
    if (abs(minOverlapX) < abs(minOverlapY))
        mB.setvelocity_x(ballFromLeft ? -ballVelocity : ballVelocity);
    else
        mB.setvelocity_y(ballFromTop ? -ballVelocity : ballVelocity);

    return;
}

bool GameEngine::testBottomCollision(Ball& mBall)
{
    if (mBall.y() >= windowHeight - ballRadius)
        return true;
    else
        return false;
}

template<class T1, class T2>
bool GameEngine::Collision(T1& mA, T2& mB)
{
    return mA.right() >= mB.left() && mA.left() <= mB.right()
           && mA.bottom() >= mB.top() && mA.top() <= mB.bottom();
}
