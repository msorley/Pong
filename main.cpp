#include <raylib.h>
#include <raymath.h>
#include <cmath>

const static float maxTimer = 0.2f;
const static float maxSpeed = 650.0f;

float SqrtInterpolate(float a, float b, float t) {
    const float y = std::sqrt(t);
    return Lerp(a, b, y);
}

enum class ActivePlayer {
    Left,
    Right,
};

struct Player {
    Vector2 position = {};
    float direction = 1;
    float timer = 0;
    int score = 0;

    void Update(float deltaTime, float padding, int heightBound, bool isActive) {
        if (isActive && (IsKeyDown(KEY_W) || IsKeyDown(KEY_S))) {
            timer += deltaTime;
            if (IsKeyDown(KEY_W)) {
                direction = -1;
            } else {
                direction = 1;
            }
        } else {
            timer -= deltaTime;
        }
        timer = Clamp(timer, 0, maxTimer);

        const float t = std::sqrt(timer / maxTimer);
        position.y += direction * maxSpeed * t * deltaTime;
        position.y = Clamp(position.y, padding, heightBound);
    }
};

struct Ball {
    Vector2 position = {};
    float radius = 0;
    Vector2 velocity = {};
};

Vector2 GetRandomDirection(float maxAngle) {
    const float random = GetRandomValue(0, INT_MAX) / (float)INT_MAX;
    const int direction = GetRandomValue(0, 1);
    const float angle = (random - 0.5) * maxAngle;
    return Vector2Normalize({
        direction == 0 ? 1.0f : -1.0f,
        std::sin(angle),
    });
}

int main() {
    InitWindow(1280, 720, "Pong");
    SetTargetFPS(120);

    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();
    const float padding = 16;
    const float ballRadius = 8;
    const Vector2 playerSize = { 16, 128 };
    const float fixedDeltaTime = 1 / 120.0;

    const float maxAngleHeight = 2 * std::atan(screenHeight / (float)screenWidth);
    const int heightBound = screenHeight - playerSize.y - padding;

    float ballSpeed = 650;

    Player playerLeft = { {
        padding,
        screenHeight / 2 - playerSize.y / 2 - padding,
    } };
    Player playerRight = { {
        screenWidth - playerSize.x - padding,
        screenHeight / 2 - playerSize.y / 2 - padding,
    } };
    Ball ball = {
        {
            screenWidth / 2 - ballRadius,
            screenHeight / 2 - ballRadius,
        },
        ballRadius,
        Vector2Scale(GetRandomDirection(maxAngleHeight), ballSpeed),
    };

    ActivePlayer activePlayer = ball.velocity.x < 0
        ? ActivePlayer::Left
        : ActivePlayer::Right;

    bool shouldRun = true;
    bool isGameOver = false;
    while (shouldRun && !WindowShouldClose()) {
        const int keyPressed = GetKeyPressed();
        switch (keyPressed) {
            case KEY_ESCAPE: {
                shouldRun = false;
                break;
            }
        }

        Player* currentPlayer = nullptr;
        if (activePlayer == ActivePlayer::Left) {
            currentPlayer = &playerLeft;
        } else {
            currentPlayer = &playerRight;
        }
        Vector2& playerPosition = currentPlayer->position;
        Rectangle playerRectangle = {
            playerPosition.x,
            playerPosition.y,
            playerSize.x,
            playerSize.y,
        };

        playerLeft.Update(fixedDeltaTime, padding, heightBound, activePlayer == ActivePlayer::Left);
        playerRight.Update(fixedDeltaTime, padding, heightBound, activePlayer == ActivePlayer::Right);

        ball.position = Vector2Add(ball.position, Vector2Scale(ball.velocity, fixedDeltaTime));

        if (CheckCollisionCircleRec(ball.position, ball.radius, playerRectangle)) {
            const float hitPosition = (ball.position.y - playerPosition.y) / playerSize.y;
            ball.velocity.x = -ball.velocity.x;
            ball.velocity.y = (hitPosition - 0.5f) * 2 * ballSpeed;
            currentPlayer->score += 1;
            ballSpeed += 2;
            activePlayer = activePlayer == ActivePlayer::Left
                ? ActivePlayer::Right
                : ActivePlayer::Left;
        }

        if (ball.position.y <= padding || ball.position.y >= screenHeight - padding) {
            ball.velocity.y = -ball.velocity.y;
        }
        if (ball.position.x <= padding || ball.position.x >= screenWidth - padding) {
            DrawText("Game Over", screenWidth / 2 - 64, screenHeight / 2 - 8, 32, WHITE);
            shouldRun = false;
            isGameOver = true;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        DrawRectangleV(playerLeft.position, playerSize, WHITE);
        DrawRectangleV(playerRight.position, playerSize, WHITE);
        DrawCircleV(ball.position, ball.radius, WHITE);

        DrawFPS(screenWidth / 2 - 24, 8);
        DrawText(TextFormat("Score: %d - %d", playerLeft.score, playerRight.score), screenWidth / 2 - 96, 32, 32, WHITE);
        EndDrawing();
    }

    if (isGameOver) {
        WaitTime(1);
    }
    CloseWindow();
    return 0;
}
