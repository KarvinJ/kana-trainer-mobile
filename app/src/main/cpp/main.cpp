#include "raymob.h" // This header can replace 'raylib.h' and includes additional functions related to Android.
#include "assetsManager.h"
#include "dataManager.h"

#define MAX_INPUT_CHARS 4
// #define MAX_INPUT_CHARS 5 for the web build

const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 544;
const int MAX_GAME_TIME = 60;

float gameTimer = MAX_GAME_TIME;

int score = 0;

int highScore = 0;

float showMessageTimer = 0;
bool showMessage = false;
bool isLearningMode = true;

int attempts = 0;

vector<Kana> kanas;

string playerName;

// ui elements
Texture2D backIconTexture;
Rectangle backIconBounds;

Texture2D soundIconTexture;
Rectangle soundIconBounds;

Texture2D muteIconTexture;

Texture2D modeCheckTexture;
Rectangle modeCheckIconBounds;
Texture2D hiraganaCheckTexture;
Rectangle hiraganaCheckIconBounds;

Texture2D highScoresIconTexture;
Rectangle highScoreIconBounds;

void toLowerCase(string

                     &string);

void drawHighScoreScreen(vector<HighScore>

                             &fullScores);

void drawChallengeScreen(float deltaTime, bool isAnswerCorrect, string

                                                                    &previousKanaName);

void drawTextBox(char answer[4], int

                                     &textBoxFrameCounter,
                 int letterCount);

void drawLearningScreen(float

                            &showScoreTimer,
                        float deltaTime,
                        bool isMute,
                        bool isHiraganaMode);

void handleHighScoreScreenUI(const Rectangle

                                 &mouseBounds,
                             char answer[4],
                             int &letterCount, bool &isHighScoreScreen);

void saveChallengeData(float

                           &showScoreTimer,
                       vector<string> &highScores, vector<HighScore> &fullScores);

void handleTextBoxTyping(int

                             &letterCount,
                         char answer[4]);

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "KANA-TRAINER");
    SetTargetFPS(60);

    soundIconTexture = LoadTexture("icons/sound-icon.png");
    soundIconBounds = {SCREEN_WIDTH - 32, 10, (float)soundIconTexture.width,
                       (float)soundIconTexture.height};

    muteIconTexture = LoadTexture("icons/mute-icon.png");

    hiraganaCheckTexture = LoadTexture("icons/check-icon.png");
    hiraganaCheckIconBounds = {8, 10, (float)hiraganaCheckTexture.width,
                               (float)hiraganaCheckTexture.height};

    modeCheckTexture = hiraganaCheckTexture;
    modeCheckIconBounds = {8, SCREEN_HEIGHT - 32, (float)hiraganaCheckTexture.width,
                           (float)hiraganaCheckTexture.height};

    highScoresIconTexture = LoadTexture("icons/high-scores-icon.png");
    highScoreIconBounds = {SCREEN_WIDTH - 32, SCREEN_HEIGHT - 32,
                           (float)highScoresIconTexture.width,
                           (float)highScoresIconTexture.height};

    backIconTexture = LoadTexture("icons/back-icon.png");
    backIconBounds = {8, SCREEN_HEIGHT - 32, (float)highScoresIconTexture.width,
                      (float)highScoresIconTexture.height};

    highScore = loadHighScore();

    playerName = loadPlayerName();

    bool isHighScoreScreen = false;

    //    if (playerName.compare("aaa") == 0) {
    //        isHighScoreScreen = true;
    //    }

    vector<string> highScores = loadHighScores();

    vector<HighScore> fullScores = getFullScoreData(highScores);

    // need to explicitly define local variable values, if not I'll get a segmentation fault.
    float soundTimer = 0;
    float showScoreTimer = 5;
    bool isMute = false;
    bool isAnswerCorrect = false;

    InitAudioDevice();

    Texture2D hiraganaSpriteSheet = LoadTexture("img/hiraganas.png");
    Texture2D katakanaSpriteSheet = LoadTexture("img/katakanas.png");

    vector<Kana> kanas;
    kanas.reserve(142);
    loadKanas(kanas);

    // there are 71 hiragana + 71 katakanas = 142.
    int totalElements = kanas.size() - 1;

    int hiraganasInitialIndex = 0;
    int totalHiraganas = kanas.size() / 2 - 1;

    int katakanasInitialIndex = totalHiraganas + 1;

    int actualKanaIndex = GetRandomValue(hiraganasInitialIndex, totalHiraganas);
    int previousKanaIndex = 0;

    bool isHiraganaMode = true;

    Rectangle mouseBounds = {0, 0, 8, 8};

    char answer[MAX_INPUT_CHARS] = "\0"; // NOTE: One extra space required for null terminator char '\0'
    int letterCount = 0;

    int textBoxFrameCounter = 0;

    while (!WindowShouldClose())
    {
        int totalKanas = totalElements;

        if (isHiraganaMode)
        {
            totalKanas = totalHiraganas;
        }

        Kana actualKana = kanas[actualKanaIndex];

        float deltaTime = GetFrameTime();

        handleTextBoxTyping(letterCount, answer);

        Vector2 mousePosition = GetMousePosition();

        mouseBounds.x = mousePosition.x;
        mouseBounds.y = mousePosition.y;

        string actualKanaName = answer;

        if (!isHighScoreScreen)
        {
            if (IsKeyPressed(KEY_ENTER) /*|| (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                                            CheckCollisionRecs(mouseBounds, modeCheckIconBounds)*)*/)
            {
                isLearningMode = !isLearningMode;
                score = 0;
                attempts = 0;
                gameTimer = MAX_GAME_TIME;
                showMessageTimer = 0;
                showMessage = false;

                answer[0] = '\0';
                letterCount = 0;

                if (isHiraganaMode)
                {
                    actualKanaIndex = GetRandomValue(hiraganasInitialIndex, totalKanas);
                }
                else
                {
                    actualKanaIndex = GetRandomValue(katakanasInitialIndex, totalKanas);
                }
            }

            if (!isLearningMode)
            {
                if (gameTimer < 1)
                {
                    saveChallengeData(showScoreTimer, highScores, fullScores);
                }

                if (IsKeyPressed(KEY_SPACE))
                {
                    // removing the last character, that is always a blank space.
                    actualKanaName.pop_back();
                    // always converting to lower case, in case anyone writes in uppercase.
                    toLowerCase(actualKanaName);

                    if (actualKana.name.compare(actualKanaName) == 0)
                    {
                        isAnswerCorrect = true;
                        score++;
                    }
                    else
                    {
                        isAnswerCorrect = false;
                    }

                    if (!isMute)
                    {
                        PlaySound(actualKana.sound);
                    }

                    showMessage = true;

                    // clearing the textbox array.
                    answer[0] = '\0';
                    letterCount = 0;
                    previousKanaIndex = actualKanaIndex;
                    if (isHiraganaMode)
                    {
                        actualKanaIndex = GetRandomValue(hiraganasInitialIndex, totalKanas);
                    }
                    else
                    {
                        actualKanaIndex = GetRandomValue(katakanasInitialIndex, totalKanas);
                    }

                    attempts++;

                    if (attempts == 20)
                    {
                        saveChallengeData(showScoreTimer, highScores, fullScores);
                    }
                }
            }
            else
            {
                if (!isMute)
                {
                    soundTimer += deltaTime;
                }

                if (IsKeyPressed(KEY_SPACE))
                {
                    if (soundTimer > 0.6)
                    {
                        PlaySound(actualKana.sound);
                        soundTimer = 0;
                    }

                    actualKanaName.pop_back();
                    toLowerCase(actualKanaName);

                    int actualInitialIndex = hiraganasInitialIndex;

                    if (!isHiraganaMode)
                    {
                        actualInitialIndex = katakanasInitialIndex;
                    }

                    for (int i = actualInitialIndex; i < totalKanas + 1; i++)
                    {
                        if (kanas[i].name.compare(actualKanaName) == 0)
                        {
                            actualKana = kanas[i];
                            actualKanaIndex = i;

                            answer[0] = '\0';
                            letterCount = 0;
                            break;
                        }
                    }
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                         CheckCollisionRecs(mouseBounds, soundIconBounds))
                {
                    isMute = !isMute;
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                         CheckCollisionRecs(mouseBounds, hiraganaCheckIconBounds))
                {
                    isHiraganaMode = !isHiraganaMode;

                    if (isHiraganaMode)
                    {
                        actualKanaIndex = GetRandomValue(hiraganasInitialIndex, totalHiraganas);
                    }
                    else
                    {
                        actualKanaIndex = GetRandomValue(katakanasInitialIndex, kanas.size() - 1);
                    }

                    if (!isMute)
                    {
                        Kana nextKana = kanas[actualKanaIndex];
                        PlaySound(nextKana.sound);
                    }
                }
//                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
//                         CheckCollisionRecs(mouseBounds, highScoreIconBounds))
//                {
//                    isHighScoreScreen = true;
//                }

                Rectangle goRightBounds = {SCREEN_WIDTH / 2, 40, SCREEN_WIDTH / 2, 268};
                Rectangle goLeftBounds = {0, 40, SCREEN_WIDTH / 2, 268};

                if (IsKeyPressed(KEY_RIGHT) || (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                                                CheckCollisionRecs(mouseBounds, goRightBounds)))
                {
                    actualKanaIndex++;

                    if (isHiraganaMode && actualKanaIndex > totalKanas)
                    {
                        actualKanaIndex = hiraganasInitialIndex;
                    }
                    else if (!isHiraganaMode && actualKanaIndex > totalKanas)
                    {
                        actualKanaIndex = katakanasInitialIndex;
                    }

                    if (!isMute)
                    {
                        Kana nextKana = kanas[actualKanaIndex];
                        PlaySound(nextKana.sound);
                    }
                }
                else if (IsKeyPressed(KEY_LEFT) || (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                                                    CheckCollisionRecs(mouseBounds,
                                                                       goLeftBounds)))
                {
                    actualKanaIndex--;

                    if (isHiraganaMode && actualKanaIndex < hiraganasInitialIndex)
                    {
                        actualKanaIndex = totalKanas;
                    }
                    else if (!isHiraganaMode && actualKanaIndex < katakanasInitialIndex)
                    {
                        actualKanaIndex = totalKanas;
                    }

                    if (!isMute)
                    {
                        Kana nextKana = kanas[actualKanaIndex];
                        PlaySound(nextKana.sound);
                    }
                }
            }
        }
        else
        {
            handleHighScoreScreenUI(mouseBounds, answer, letterCount, isHighScoreScreen);
        }

        BeginDrawing();

        ClearBackground({29, 29, 27, 255});

        if (isHighScoreScreen)
        {
            drawHighScoreScreen(fullScores);
        }
        else if (!isHighScoreScreen)
        {
            DrawRectangleRec({40, 40, 320, 268}, WHITE);

//            DrawRectangleRounded(modeCheckIconBounds, 0.3, 6, LIGHTGRAY);
//
//            if (isLearningMode)
//            {
//                DrawTexture(modeCheckTexture, modeCheckIconBounds.x, modeCheckIconBounds.y, WHITE);
//            }

            if (isHiraganaMode)
            {
                DrawTextureRec(hiraganaSpriteSheet, actualKana.bounds, {40, 40}, WHITE);
            }
            else
            {
                DrawTextureRec(katakanaSpriteSheet, actualKana.bounds, {40, 40}, WHITE);
            }

            if (isLearningMode)
            {
                drawLearningScreen(showScoreTimer, deltaTime, isMute, isHiraganaMode);
            }
//            else
//            {
//                Kana previousKana = kanas[previousKanaIndex];
//                drawChallengeScreen(deltaTime, isAnswerCorrect, previousKana.name);
//            }
        }

//        drawTextBox(answer, textBoxFrameCounter, letterCount);

        EndDrawing();
    }

    UnloadTexture(hiraganaSpriteSheet);
    UnloadTexture(katakanaSpriteSheet);
    UnloadTexture(muteIconTexture);
    UnloadTexture(muteIconTexture);
    UnloadTexture(soundIconTexture);
    UnloadTexture(hiraganaCheckTexture);
    UnloadTexture(highScoresIconTexture);
    UnloadTexture(backIconTexture);

    int index = 0;
    for (auto &kana : kanas)
    {
        // I just have sounds loaded for the hiraganas, and for the katakanas I just copied.
        if (index < totalHiraganas)
        {
            UnloadSound(kana.sound);
        }

        index++;
    }

    CloseAudioDevice();
    CloseWindow();
}

void handleTextBoxTyping(int

                             &letterCount,
                         char answer[4])
{
    // Get char pressed (unicode character) on the queue
    int character = GetCharPressed();

    // Check if more characters have been pressed on the same frame
    while (character > 0)
    {
        // NOTE: Only allow keys in range [32..125]
        if ((character >= 32) && (character <= 125) && (letterCount < MAX_INPUT_CHARS))
        {
            answer[letterCount] = (char)
                character;
            answer[letterCount + 1] = '\0'; // Add null terminator at the end of the string.
            letterCount++;
        }

        character = GetCharPressed(); // Check next character in the queue
    }

    if (
        IsKeyPressed(KEY_BACKSPACE))
    {
        letterCount--;

        if (letterCount < 0)
        {
            letterCount = 0;
        }

        answer[letterCount] = '\0';
    }
}

void saveChallengeData(float

                           &showScoreTimer,
                       vector<string> &highScores, vector<HighScore> &fullScores)
{
    isLearningMode = true;
    attempts = 0;
    showScoreTimer = 0;

    score *=
        gameTimer;

    if (score > 0)
    {
        highScores = saveActualHighScores(highScores, score, playerName);
        fullScores = getFullScoreData(highScores);
        updateHighScore(score, highScore);
        gameTimer = MAX_GAME_TIME;
    }
}

void handleHighScoreScreenUI(const Rectangle

                                 &mouseBounds,
                             char answer[4],
                             int &letterCount, bool &isHighScoreScreen)
{
    if (
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionRecs(mouseBounds, backIconBounds))
    {
        isHighScoreScreen = false;
    }

    if (
        IsKeyPressed(KEY_SPACE))
    {
        string name = answer;
        name.

            pop_back();

        toLowerCase(name);

        if (!name.

             empty()

        )
        {
            playerName = name;
            savePlayerName(name);
        }

        answer[0] = '\0';
        letterCount = 0;
    }
}

void drawLearningScreen(float

                            &showScoreTimer,
                        float deltaTime,
                        bool isMute,
                        bool isHiraganaMode)
{
    DrawRectangleRounded(hiraganaCheckIconBounds,
                         0.3, 6, LIGHTGRAY);

    if (isHiraganaMode)
    {
        DrawText("Hiragana mode", 110, 10, 24, LIGHTGRAY);
        DrawTexture(hiraganaCheckTexture, hiraganaCheckIconBounds.x, hiraganaCheckIconBounds.y, WHITE);
    }
    else
    {
        DrawText("katakana mode", 110, 10, 24, LIGHTGRAY);
    }

    DrawRectangleRounded(soundIconBounds,
                         0.3, 6, LIGHTGRAY);

    if (!isMute)
    {
        DrawTexture(soundIconTexture, soundIconBounds.x, soundIconBounds.y, WHITE);
    }
    else
    {
        DrawTexture(muteIconTexture, soundIconBounds.x, soundIconBounds.y, WHITE);
    }

    if (showScoreTimer < 5)
    {
        if (highScore > 0)
        {
            DrawText(TextFormat(

                         "High score: %i", highScore),
                     110, 320, 24, LIGHTGRAY);
            showScoreTimer +=
                deltaTime;
        }

        if (score > 0)
        {
            DrawText(TextFormat(

                         "Actual score: %i", score),
                     100, 360, 24, DARKGRAY);
        }
    }

//    DrawText("SEARCH", 90, 400, 20, LIGHTGRAY);

//    DrawRectangleRounded(highScoreIconBounds,0.3, 6, LIGHTGRAY);
//    DrawTexture(highScoresIconTexture, highScoreIconBounds.x, highScoreIconBounds.y, WHITE);
}

void drawTextBox(char answer[4], int

                                     &textBoxFrameCounter,
                 int letterCount)
{
    Rectangle textBoxBounds = {90, 425, 225, 50};

    DrawRectangleRec(textBoxBounds, LIGHTGRAY);
    DrawRectangleLines((int)textBoxBounds.x, (int)textBoxBounds.y, (int)textBoxBounds.width, (int)textBoxBounds.height, DARKGRAY);
    DrawText(answer,
             (int)textBoxBounds.x + 5, (int)textBoxBounds.y + 8, 40, DARKGRAY);

    textBoxFrameCounter++;

    if (letterCount < MAX_INPUT_CHARS)
    {
        // Draw blinking underscore char
        if (((textBoxFrameCounter / 20) % 2) == 0)
        {
            DrawText("_", (int)textBoxBounds.x + 8 + MeasureText(answer, 40), (int)textBoxBounds.y + 12, 40, DARKGRAY);
        }
    }
}

void drawChallengeScreen(float deltaTime, bool isAnswerCorrect, string

                                                                    &previousKanaName)
{
    if (gameTimer > 0)
    {
        gameTimer -=
            deltaTime;
    }

    DrawText(TextFormat(

                 "%i", (int)gameTimer),
             SCREEN_WIDTH - 40, 10, 24, WHITE);

    DrawText(TextFormat(

                 "%i", score),
             200, 10, 24, WHITE);

    DrawText(TextFormat(

                 "%i", highScore),
             20, 10, 24, WHITE);
    DrawRectangle(160, SCREEN_HEIGHT / 2 - 10, 80, 45, WHITE);

    // drawing text box
    DrawText("WRITE THE ANSWER", 90, 400, 20, LIGHTGRAY);

    if (showMessage)
    {
        if (isAnswerCorrect)
        {
            DrawText("CORRECT!", 145, 500, 20, LIME);
        }
        else
        {
            DrawText("WRONG!", 145, 500, 20, RED);
            DrawText(previousKanaName
                         .

                     c_str(),

                     235, 495, 25, LIME);
        }

        showMessageTimer +=
            deltaTime;

        if (showMessageTimer > 1.5)
        {
            showMessageTimer = 0;
            showMessage = false;
        }
    }
}

void drawHighScoreScreen(vector<HighScore>

                             &fullScores)
{
    DrawText("Top Players", 120, 10, 24, LIGHTGRAY);

    DrawText("Rank", 20, 40, 24, LIGHTGRAY);
    DrawText("Name", 170, 40, 24, LIGHTGRAY);
    DrawText("Score", 310, 40, 24, LIGHTGRAY);

    int yPosition = 70;
    for (auto &fullScore : fullScores)
    {
        DrawText(TextFormat(

                     "%i", fullScore.rank),
                 20, 0 + yPosition, 20, LIGHTGRAY);
        DrawText(fullScore
                     .name.

                 c_str(),

                 170, 0 + yPosition, 20, LIGHTGRAY);

        DrawText(TextFormat(

                     "%i", fullScore.score),
                 310, 0 + yPosition, 20, LIGHTGRAY);
        yPosition += 25;
    }

    DrawText("WRITE YOUR NAME", 90, 400, 20, LIGHTGRAY);

    if (!playerName.

         empty()

    )
    {
        DrawText(("Current Player: " + playerName).

                 c_str(),

                 90, 500, 20, LIGHTGRAY);
    }

    DrawRectangleRounded(backIconBounds,
                         0.3, 6, LIGHTGRAY);
    DrawTexture(backIconTexture, backIconBounds.x, backIconBounds.y, WHITE);
}

void toLowerCase(string

                     &string)
{
    // Manual converting each character to lowercase using ASCII values
    for (char &character : string)
    {
        if (character >= 'A' && character <= 'Z')
        {
            // Convert uppercase to lowercase by adding 32
            character += 32;
        }
    }
}