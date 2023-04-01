#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <iostream>
#include <cmath>
#include <vector>
#include <map>

// #include <thread> // for playing continuous sounds

// #ifdef _WIN32
//     #include <windows.h>
// #else
//     #include <unistd.h>
// #endif

#include <fstream> // for reading and writing HIGHSCORE.txt

#include <algorithm> // for min() and max()

#include <cstdlib>
#include <ctime>

#include "RenderWindow.hpp"
#include "Utils.hpp"
#include "Entity.hpp"
#include "Text.hpp"
#include "Button.hpp"

using namespace std;

// const int screenWidth = 700, screenHeight = 600;
const int screenWidth = 900, screenHeight = 770;
int playerScore = 0;

struct Asteroid {
    Entity entity;
    int angle;
    vector<float> movement;
    int size;
    int angleChange = 2;
};

struct SmallSaucer {
    Entity entity;
    int angleDeviation; // greater angleDeviation results in worse accuracy
    float xMovement;
    float timeBetweenShots;
    float lastShot;
    float lastYJump;
    float yJumpDuration = 0.5;
    float yJumpStart;
    float yJumpDistance;
    bool yJumping = false;
};

void playSound(Mix_Chunk* sound, int channel = -1) {
    Mix_PlayChannel(channel, sound, 0);
}

// void playSaucerSound(Mix_Chunk* sound, int channel, float soundLength, vector<SmallSaucer>& saucers) {
//     while (saucers.size() > 0) {
//         Mix_PlayChannel(channel, sound, 0);
//         Sleep(soundLength * 1000);
//     }
// }

void breakAsteroid(Asteroid& asteroidObj, vector<Asteroid>& asteroids, int asteroidSpeed, int sizeLarge, int sizeMedium, int sizeSmall, vector<SDL_Texture*> asteroidSpritesMedium, vector<SDL_Texture*> asteroidSpritesSmall, map<float, int>& asteroidScores, Mix_Chunk* explosionSound, int explosionSoundChannel, vector<vector<vector<float>>> asteroidVertices = {}) {
    playSound(explosionSound, explosionSoundChannel);

    int newSize;
    vector<SDL_Texture*> asteroidTextures;

    playerScore += asteroidScores[asteroidObj.size];

    int randAngleChange1 = utils::randChoice(vector<int>({-2, 2}));
    int randAngleChange2 = utils::randChoice(vector<int>({-2, 2}));

    if (asteroidObj.size == sizeLarge) {
        newSize = sizeMedium;
        asteroidTextures = asteroidSpritesMedium;
    }
    else if (asteroidObj.size == sizeMedium) {
        newSize = sizeSmall;
        asteroidTextures = asteroidSpritesSmall;
    }
    else if (asteroidObj.size == sizeSmall) return;

    Entity& asteroid = asteroidObj.entity;

    int randChoiceNum1 = utils::randNum(0, asteroidTextures.size() - 1);
    int randChoiceNum2 = utils::randNum(0, asteroidTextures.size() - 1);

    Entity newAsteroid1(
        asteroid.getX(),
        asteroid.getY(),
        newSize,
        newSize,
        newSize,
        newSize,
        asteroidTextures[randChoiceNum1],
        asteroidVertices[randChoiceNum1]
    );
    newAsteroid1.setAngle(asteroid.getAngle());

    Entity newAsteroid2(
        asteroid.getX(),
        asteroid.getY(),
        newSize,
        newSize,
        newSize,
        newSize,
        asteroidTextures[randChoiceNum2],
        asteroidVertices[randChoiceNum2]
    );
    newAsteroid2.setAngle(asteroid.getAngle());

    int newAngle1 = (asteroidObj.angle + 60) % 360;
    int newAngle2 = (asteroidObj.angle - 60) % 360;

    vector<float> movement1 = {
        cos(newAngle1 * (M_PI / 180)) * asteroidSpeed,
        -sin(newAngle1 * (M_PI / 180)) * asteroidSpeed
    };

    vector<float> movement2 = {
        cos(newAngle2 * (M_PI / 180)) * asteroidSpeed,
        -sin(newAngle2 * (M_PI / 180)) * asteroidSpeed
    };

    Asteroid asteroidObj1 = {newAsteroid1, newAngle1, movement1, newSize, randAngleChange1};
    Asteroid asteroidObj2 = {newAsteroid2, newAngle2, movement2, newSize, randAngleChange2};

    asteroids.push_back(asteroidObj1);
    asteroids.push_back(asteroidObj2);
}

struct Bullet {
    Entity entity;
    vector<float> movement;
    char origin = 'p'; // tracks where bullet came from (p = player, s = saucer)
    float distTravelled = 0;
};

struct Particle {
    Entity entity;
    int frame = 0;
    int totalFrames;
    float lastFrameUpdate = 0;
    float timeBetweenFrames;
};

void delayRefreshRate(float frameTicks, float refreshRate) {
    if (frameTicks < 1000 / refreshRate) {
        SDL_Delay(1000 / refreshRate - frameTicks);
    }
}

void startButtonClick(float x, float y) {
}

int startMenu(RenderWindow window, SDL_Event event, int refreshRate, SDL_Texture* font) {
    Text mainTitle(0, 0, 64, "Asteroids", font);
    mainTitle.setX(screenWidth / 2 - mainTitle.getW() / 2);
    mainTitle.setY(screenHeight / 3 - mainTitle.getH() / 2);

    Text buttonText(0, 0, 40, "Start Game", font);
    buttonText.center(screenWidth, screenHeight);
    buttonText.setY(screenHeight / 1.5 - buttonText.getH() / 2);

    Text buttonText2(0, 0, 40, "Highscores", font);
    buttonText2.center(screenWidth, screenHeight);
    buttonText2.setY(screenHeight / 1.3 - buttonText2.getH() / 2);

    float lastTextRender = utils::hireTimeInSeconds();
    float textRenderDelay = 0.5;
    bool textRender = true;

    SDL_Texture* mainButtonSprite = window.loadTexture("./res/start_game_button.png");
    Entity newButtonEntity(screenWidth / 2 - 500 / 2, screenHeight / 1.5 - 70 / 2, 500, 70, 500, 70, mainButtonSprite);
    Button mainButton(&newButtonEntity, &startButtonClick);

    Entity newButtonEntity2 = newButtonEntity;
    newButtonEntity2.setY(newButtonEntity2.getY() + newButtonEntity2.getH() + 10);
    Button highscoreButton(&newButtonEntity2, &startButtonClick);

    bool startMenuRunning = true;
    while (startMenuRunning) {
        float startTicks = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return 0;
                // startMenuRunning = false;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (mainButton.isClicked(event.button.x, event.button.y)) {
                        // mainButton.click(event.button.x, event.button.y);
                        textRender = true;
                        startMenuRunning = false;
                    }
                    if (highscoreButton.isClicked(event.button.x, event.button.y)) {
                        startMenuRunning = false;
                        return 2;
                    }
                }
            }
        }
        window.clear();
        mainTitle.render(window);
        Entity buttonEntity = mainButton.getEntity();
        window.render(buttonEntity);

        Entity buttonEntity2 = highscoreButton.getEntity();
        window.render(buttonEntity2);

        buttonText2.render(window);

        if (textRender) {
            buttonText.render(window);
        }

        if (utils::hireTimeInSeconds() - lastTextRender >= textRenderDelay) {
            lastTextRender = utils::hireTimeInSeconds();
            textRender = !textRender;
        }

        window.display();

        delayRefreshRate(SDL_GetTicks() - startTicks, refreshRate);
    }
    return 1;
}

void cleanupAudio(vector<Mix_Chunk*> sounds) {
    for (int i = 0; i < sounds.size(); i++) {
        Mix_FreeChunk(sounds[i]);
        sounds[i] = NULL;
    }
}

int mainGame(RenderWindow window, SDL_Event event, int refreshRate, SDL_Texture* monospaceFont) {
    bool gameRunning = true;

    // SDL_Texture* playerSprite = window.loadTexture("./res/player_sprite_32.png");
    SDL_Texture* playerSprite = window.loadTexture("./res/player_sprite_28.png");
    SDL_Texture* bulletSprite = window.loadTexture("./res/bullet_sprite.png");
    // SDL_Texture* asteroidExplosion = window.loadTexture("./res/particles/explosion.png");
    // SDL_Texture* asteroidExplosion = window.loadTexture("./res/particles/explosion4.png");
    SDL_Texture* asteroidExplosion = window.loadTexture("./res/particles/explosion5.png");
    SDL_Texture* playerExplosion = window.loadTexture("./res/particles/player_explosion.png");
    SDL_Texture* heartEmpty = window.loadTexture("./res/hearts/heart_empty.png");
    SDL_Texture* heartFull = window.loadTexture("./res/hearts/heart_full.png");
    // SDL_Texture* buttonSprite = window.loadTexture("./res/button_sprite.png");
    // SDL_Texture* smallSaucerSprite = window.loadTexture("./res/saucers/small_saucer.png");
    SDL_Texture* smallSaucerSprite = window.loadTexture("./res/saucers/small_saucer_48.png");

    // load audio files
    Mix_Chunk* explosionSound = Mix_LoadWAV("./res/audio/bangLarge.wav");
    Mix_Chunk* fireSound = Mix_LoadWAV("./res/audio/fire.wav");
    Mix_Chunk* extraLifeSound = Mix_LoadWAV("./res/audio/extraShip.wav");
    Mix_Chunk* smallSaucerSound = Mix_LoadWAV("./res/audio/saucerSmall.wav");

    Mix_Chunk* musicBeat1 = Mix_LoadWAV("./res/audio/beat1.wav");
    Mix_Chunk* musicBeat2 = Mix_LoadWAV("./res/audio/beat2.wav");
    // float musicBeatLength = 0.118;
    float musicBeatLength = 0.5;
    float lastMusicBeatPlayed;
    bool lastPlayedBeat = false;

    // float smallSaucerSoundLength = 0.12; // sound length in seconds
    float smallSaucerSoundLength = 0.118; // sound length in seconds
    float lastSmallSaucerSoundPlay;

    // int smallSaucerSoundChannel = 6; // sound channel to play small saucer sound
    int smallSaucerSoundChannel = 1; // sound channel to play small saucer sound
    Mix_Volume(smallSaucerSoundChannel, MIX_MAX_VOLUME / 7); // make small saucer sound a little quieter
    // Mix_Volume(smallSaucerSoundChannel, MIX_MAX_VOLUME); // make small saucer sound a little quieter

    int explosionSoundChannel = 5; // sound channel to play explosion sound
    Mix_Volume(explosionSoundChannel, MIX_MAX_VOLUME);

    int fireSoundChannel = 4;
    Mix_Volume(fireSoundChannel, MIX_MAX_VOLUME / 2);

    int extraLifeSoundChannel = 3;
    Mix_Volume(extraLifeSoundChannel, 2 * MIX_MAX_VOLUME / 3);

    int musicSoundChannel = 10;
    Mix_Volume(musicSoundChannel, 2 * MIX_MAX_VOLUME / 3);

    vector<Mix_Chunk*> allSounds = {
        explosionSound,
        fireSound,
        extraLifeSound,
        smallSaucerSound,
        musicBeat1,
        musicBeat2
    };

    vector<SDL_Texture*> asteroidSpritesLarge = {
        window.loadTexture("./res/asteroids/asteroid_sprite_100_1.png"),
        window.loadTexture("./res/asteroids/asteroid_sprite_100_2.png"),
        window.loadTexture("./res/asteroids/asteroid_sprite_100_3.png")
        // window.loadTexture("./res/asteroids/asteroid_test_100.png")
    };

    vector<SDL_Texture*> asteroidSpritesMedium = {
        window.loadTexture("./res/asteroids/asteroid_sprite_64_1.png"),
        window.loadTexture("./res/asteroids/asteroid_sprite_64_2.png"),
        window.loadTexture("./res/asteroids/asteroid_sprite_64_3.png")
        // window.loadTexture("./res/asteroids/asteroid_test_64.png")
    };

    vector<SDL_Texture*> asteroidSpritesSmall = {
        window.loadTexture("./res/asteroids/asteroid_sprite_32_1.png"),
        window.loadTexture("./res/asteroids/asteroid_sprite_32_2.png"),
        window.loadTexture("./res/asteroids/asteroid_sprite_32_3.png")
        // window.loadTexture("./res/asteroids/asteroid_test_32.png")
    };

    // const int playerWidth = 32, playerHeight = 32;
    const int playerWidth = 28, playerHeight = 28;
    // const int bulletWidth = 8, bulletHeight = 8;
    const int bulletWidth = 4, bulletHeight = 4;
    const int asteroidLarge = 100, asteroidMedium = 64, asteroidSmall = 32;
    // const int smallSaucerWidth = 32, smallSaucerHeight = 17;
    const int smallSaucerWidth = 48, smallSaucerHeight = 26;

    // define vertices for asteroids and player
    vector<vector<float>> asteroidMedium1Vertices = {{0, 18}, {30, 0}, {45, 5}, {64, 30}, {53, 64}, {16, 64}, {0, 46}, {16, 30}};
    vector<vector<float>> asteroidLarge1Vertices = utils::multiplyAllNums(1.5625, asteroidMedium1Vertices);
    vector<vector<float>> asteroidSmall1Vertices = utils::multiplyAllNums(0.5, asteroidMedium1Vertices);

    vector<vector<float>> asteroidMedium2Vertices = {{8, 0}, {30, 1}, {43, 6}, {64, 32}, {57, 47}, {52, 48}, {56, 53}, {53, 64}, {16, 64}, {0, 47}, {16, 30}, {15, 20}, {7, 13}};
    vector<vector<float>> asteroidLarge2Vertices = utils::multiplyAllNums(1.5625, asteroidMedium2Vertices);
    vector<vector<float>> asteroidSmall2Vertices = utils::multiplyAllNums(0.5, asteroidMedium2Vertices);

    // test for concave shape:
    // vector<vector<float>> asteroidMedium3Vertices = {{12, 0}, {64, 2}, {15, 8}, {7, 34}, {12, 50}, {43, 64}, {5, 57}, {0, 23}};

    // vector<vector<float>> asteroidMedium3Vertices = {{7, 6}, {52, 0}, {55, 10}, {53, 15}, {57, 17}, {64, 33}, {42, 64}, {22, 52}, {0, 64}, {2, 47}, {13, 43}, {7, 20}, {16, 15}, {128, 128}};
    vector<vector<float>> asteroidMedium3Vertices = {{7, 6}, {52, 0}, {55, 10}, {53, 15}, {57, 17}, {64, 33}, {42, 64}, {22, 52}, {0, 64}, {2, 47}, {13, 43}, {7, 20}, {16, 15}};
    vector<vector<float>> asteroidLarge3Vertices = utils::multiplyAllNums(1.5625, asteroidMedium3Vertices);
    vector<vector<float>> asteroidSmall3Vertices = utils::multiplyAllNums(0.5, asteroidMedium3Vertices);


    vector<vector<vector<float>>> asteroidLargeVerticesAll = {asteroidLarge1Vertices, asteroidLarge2Vertices, asteroidLarge3Vertices};
    vector<vector<vector<float>>> asteroidMediumVerticesAll = {asteroidMedium1Vertices, asteroidMedium2Vertices, asteroidMedium3Vertices};
    vector<vector<vector<float>>> asteroidSmallVerticesAll = {asteroidSmall1Vertices, asteroidSmall2Vertices, asteroidSmall3Vertices};

    // for (int i = 0; i < asteroidLarge1Vertices.size(); i++) {
        // cout << asteroidLarge1Vertices[i][0] << "," << asteroidLarge1Vertices[i][1] << " : " << asteroidMedium1Vertices[i][0] * 100/64 << "," << asteroidMedium1Vertices[i][1] * 100/64 << "\n";
    // }

    vector<vector<float>> playerVertices = {{14, 0}, {28, 28}, {0, 28}};
    Entity player(screenWidth / 2 - playerWidth / 2, screenHeight / 2 - playerHeight / 2, playerWidth, playerHeight, 28, 28,  playerSprite, playerVertices);

    // float playerMass = 60; // player's mass
    float playerMass = 55; // player's mass
    // float thrusterForce = 15; // force of thrusters (was 12)
    // float thrusterForce = 15; // force of thrusters (was 18)
    float thrusterForce = 12; // force of thrusters (was 18)

    float decelFactor = 0.98; // decelerration factor
    // float decelFactor = 0.95;

    float timeStep =  1; // scaling for player physics speed
    // float timeStep =  0.033;

    // player's x and y velocity
    float playerXVel = 0;
    float playerYVel = 0;

    int playerDeg = 90; // player's rotation in degs
    const int playerDegChange = 5; // degree change every arrow press

    int playerLives = 5;
    int extraLifeScore = 10000; // new life is added once player adds another {extraLifeScore} to their score

    // total force on player for x and y
    float totalForceX = 0;
    float totalForceY = 0;

    // bools representing status of keys
    bool upHeld = false;
    bool leftHeld = false;
    bool rightHeld = false;
    bool spaceUp = true;

    float bulletSpeed = 10; // bullet speed
    float lastBulletShoot = 0; // time last bullet was shot
    float bulletDelay = 0.1; // time between each bullet in seconds
    float bulletDistance = screenHeight - 100; // distance a bullet can travel

    float asteroidSpeedLarge = 1.5;
    float asteroidSpeedMedium = 2.25;
    float asteroidSpeedSmall = 2.75;

    map<float, int> asteroidScoring = {
        {asteroidLarge, 20},
        {asteroidMedium, 50},
        {asteroidSmall, 100}
    };

    float lastAsteroidSpawn = 0;
    float asteroidDelay = 3;
    int levelAsteroidCount = 5;
    int currentLevel = 0;
    int asteroidsSpawned = 0;
    // const float levelDelay = 2;
    const float levelDelay = 0.5;

    float playerInvincibleDuration = 3;
    float playerInvincibleStart = 0;
    bool playerRender = true;
    bool playerIsRespawning = false;
    float lastRenderSwitch = 0;

    float saucerSpawnDelay = 20; // time between each saucer spawned in seconds
    float lastSaucerSpawn = utils::hireTimeInSeconds() - 10; // -10 to spawn in first ten seconds

    float smallSaucerSpeed = 2;
    // int smallSaucerAngleDeviation = 30;
    int smallSaucerAngleDeviation = 45;
    // float smallSaucerTimeBetweenShots = 1;
    float smallSaucerTimeBetweenShots = 0.95;
    float smallSaucerTimeBetweenJumps = 2;
    float smallSaucerJumpDistance = 3;
    float smallSaucerPoints = 1000;
    float smallSaucerFirstShotDelay = 0.75; // delay after spawn before first shot is fired

    int oldPlayerScore = playerScore; // var storing previous frame player score

    // vectors storing different objects
    vector<Bullet> bullets;
    vector<Asteroid> asteroids;
    vector<Particle> particles;
    vector<Text*> allText;
    vector<Entity> hearts;
    vector<Button> buttons;
    vector<SmallSaucer> saucers;

    for (int i = 0; i < playerLives; i++) {
        Entity newHeart(5 + (i * (32 + 5)), 5, 32, 32, 32, 32, heartEmpty);
        hearts.push_back(newHeart);
    }

    Text scoreText(0, 5, 32, to_string(playerScore), monospaceFont);
    scoreText.setX(screenWidth - scoreText.getW() - 5);
    allText.push_back(&scoreText);

    // TRACKING FRAMERATE ---------------

    // Text frameRate(0, 0, 32, "hello", monospaceFont);
    // frameRate.center(screenWidth, screenHeight);
    // frameRate.setY(5);
    // float endFrameTicks;

    // END TRACKING FRAMERATE --------------

    vector<vector<vector<float>>> testPoints;

    while (gameRunning) {
        int startTicks = SDL_GetTicks();

        float addedForceX = 0;
        float addedForceY = 0;

        float clickedX = -1;
        float clickedY = -1;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                cleanupAudio(allSounds);
                return 0;
                gameRunning = false;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    clickedX = event.button.x;
                    clickedY = event.button.y;
                }
            }

            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_z:
                        window.setDebug(!window.getDebug());
                        break;
                    case SDLK_UP:
                        upHeld = true;
                        break;
                    case SDLK_LEFT:
                        leftHeld = true;
                        break;
                    case SDLK_RIGHT:
                        rightHeld = true;
                        break;
                    case SDLK_SPACE:
                        if (spaceUp && utils::hireTimeInSeconds() - lastBulletShoot >= bulletDelay) {
                            playSound(fireSound, fireSoundChannel);

                            lastBulletShoot = utils::hireTimeInSeconds();
                            Entity newBullet = Entity(player.getX() + playerWidth / 2 - bulletWidth / 2, player.getY() + playerHeight / 2 - bulletHeight / 2, bulletWidth, bulletHeight, 16, 16, bulletSprite);
                            vector<float> movement = {cos(playerDeg * (M_PI / 180)) * bulletSpeed, -sin(playerDeg * (M_PI / 180)) * bulletSpeed};
                            Bullet bulletObj = {newBullet, movement};
                            bullets.push_back(bulletObj);
                            spaceUp = false;
                        }
                        break;
                }
            }

            if (event.type == SDL_KEYUP) {
                switch(event.key.keysym.sym) {
                    case SDLK_UP:
                        upHeld = false;
                        break;
                    case SDLK_LEFT:
                        leftHeld = false;
                        break;
                    case SDLK_RIGHT:
                        rightHeld = false;
                        break;
                    case SDLK_SPACE:
                        spaceUp = true;
                        break;
                }
            }
        }

        if (upHeld) {
            addedForceY -= sin(playerDeg * (M_PI / 180)) * thrusterForce;
            addedForceX += cos(playerDeg * (M_PI / 180)) * thrusterForce;
        }

        // update players rotation if left or right is held:
        playerDeg = (playerDeg + (playerDegChange * leftHeld) - (playerDegChange * rightHeld)) % 360;

        totalForceX = addedForceX;
        totalForceY = addedForceY;

        playerXVel += (totalForceX / playerMass) * timeStep;
        playerYVel += (totalForceY / playerMass) * timeStep;

        playerXVel *= decelFactor;
        playerYVel *= decelFactor;

        if (abs(playerXVel) <= 0.05) {
            playerXVel = 0;
        }

        if (abs(playerYVel) <= 0.05) {
            playerYVel = 0;
        }

        if (player.isOffScreen(screenWidth, screenHeight)) {
            player.wrap(screenWidth, screenHeight);
        }

        player.setX(player.getX() + (playerXVel * timeStep));
        player.setY(player.getY() + (playerYVel * timeStep));
        player.setAngle(playerDeg - 90);

        // add extra life for every {extraLifeScore} points
        if (floor(oldPlayerScore / extraLifeScore) < floor(playerScore / extraLifeScore)) {
            playSound(extraLifeSound, extraLifeSoundChannel);
            playerLives++;
            Entity newHeart(5 + (hearts.size() * (32 + 5)), 5, 32, 32, 32, 32, heartEmpty);
            hearts.push_back(newHeart);
        }
        oldPlayerScore = playerScore;

        // if (utils::hireTimeInSeconds() - lastAsteroidSpawn >= asteroidDelay && asteroidCounts[currentLevel] > asteroidsSpawned) {
        // create new asteroid if enough time has passen since last asteroid creation
        if (utils::hireTimeInSeconds() - lastAsteroidSpawn >= asteroidDelay && levelAsteroidCount > asteroidsSpawned) {
            lastAsteroidSpawn = utils::hireTimeInSeconds();
            asteroidsSpawned++;

            int asteroidY = utils::randNum(0, screenHeight);
            int asteroidX = utils::randNum(0, screenWidth);

            int randRotation = utils::randNum(0, 359);
            int randAngleChange = utils::randChoice(vector<int>({-2, 2}));

            int newAsteroidSpriteIndex = utils::randNum(0, asteroidSpritesLarge.size() - 1);
            SDL_Texture* newAsteroidSprite = asteroidSpritesLarge[newAsteroidSpriteIndex];
            vector<vector<float>> newAsteroidVertices;
            if (newAsteroidSpriteIndex == 0) {
                newAsteroidVertices = asteroidLarge1Vertices;
            } else if (newAsteroidSpriteIndex == 1) {
                newAsteroidVertices = asteroidLarge2Vertices;
            } else if (newAsteroidSpriteIndex == 2) {
                newAsteroidVertices = asteroidLarge3Vertices;
            }
            
            // {x, y, angle}
            vector<vector<float>> spawnChoices = {
                // spawned on left edge:
                {-asteroidLarge, asteroidY, utils::randNum(-90, 90) % 360},
                // spawned on right edge:
                {screenWidth, asteroidY, utils::randNum(90, 270)},
                // spawned on top edge:
                {asteroidX, -asteroidLarge, utils::randNum(180, 360) % 360},
                // spawned on bottom edge
                {asteroidX, screenHeight, utils::randNum(0, 180)}
            };

            vector<float> spawnChoice = utils::randChoice(spawnChoices);

            Entity newAsteroid(
                spawnChoice[0],
                spawnChoice[1],
                asteroidLarge,
                asteroidLarge,
                asteroidLarge,
                asteroidLarge,
                newAsteroidSprite,
                newAsteroidVertices
            );
            newAsteroid.setAngle(randRotation);

            vector<float> movement = {
                cos(spawnChoice[2] * (M_PI / 180)) * asteroidSpeedLarge,
                -sin(spawnChoice[2] * (M_PI / 180)) * asteroidSpeedLarge
            };

            Asteroid asteroidObj = {newAsteroid, spawnChoice[2], movement, asteroidLarge, randAngleChange};
            asteroids.push_back(asteroidObj);
        }
        
        // SPAWN SAUCER ------------------------
        if (saucers.size() == 0 && utils::hireTimeInSeconds() - lastSaucerSpawn >= saucerSpawnDelay) {
            vector<vector<float>> spawnChoices = {
                {-smallSaucerWidth, utils::randNum(0, screenHeight - smallSaucerHeight)},
                {screenWidth, utils::randNum(0, screenHeight - smallSaucerHeight)}
            };

            vector<float> spawnPos = utils::randChoice(spawnChoices);

            Entity saucerEntity(spawnPos[0], spawnPos[1], smallSaucerWidth, smallSaucerHeight, smallSaucerWidth, smallSaucerHeight, smallSaucerSprite);
            SmallSaucer newSaucer = {saucerEntity, smallSaucerAngleDeviation, utils::randChoice(vector<float>({-smallSaucerSpeed, smallSaucerSpeed})), smallSaucerTimeBetweenShots};
            newSaucer.lastShot = utils::hireTimeInSeconds() + smallSaucerFirstShotDelay;
            saucers.push_back(newSaucer);
        }

        // play saucer sounds
        if (saucers.size() != 0 && utils::hireTimeInSeconds() - lastSmallSaucerSoundPlay >= smallSaucerSoundLength) {
            lastSmallSaucerSoundPlay = utils::hireTimeInSeconds();
            playSound(smallSaucerSound, smallSaucerSoundChannel);
        } 

        // play music
        // if (utils::hireTimeInSeconds() - lastMusicBeatPlayed >= musicBeatLength) {
        //     lastMusicBeatPlayed = utils::hireTimeInSeconds();
        //     playSound(lastPlayedBeat ? musicBeat1 : musicBeat2, musicSoundChannel);
        //     lastPlayedBeat = !lastPlayedBeat;
        // }

        window.clear();

        // UPDATING FRAMERATE TEXT: ------------
        // frameRate.setText(to_string(floor(1 / ((SDL_GetTicks() - endFrameTicks) / 1000))));
        // frameRate.render(window);
        // END UPDATING FRAMERATE TEXT --------

        for (int i = 0; i < bullets.size(); i++) {
            Entity& bullet = bullets[i].entity;  

            if (bullet.isOffScreen(screenWidth, screenHeight)) {
                bullet.wrap(screenWidth, screenHeight);
            }            

            bullet.setX(bullet.getX() + bullets[i].movement[0]);
            bullet.setY(bullet.getY() + bullets[i].movement[1]);
            bullets[i].distTravelled += bulletSpeed;

            if (bullets[i].distTravelled >= bulletDistance) {
                bullets.erase(bullets.begin() + i);
                continue;
            }

            bool bulletErased = false;

            for (int j = 0; j < asteroids.size(); j++) {
                if (bullet.isCollidingWith(asteroids[j].entity, testPoints)) {
                    Entity& asteroid = asteroids[j].entity;
                    Asteroid& oldAsteroidObj = asteroids[j];
                    // vector<SDL_Texture*> newAsteroidTextures;

                    Entity newExplosion(
                        bullet.getX() + bullet.getW() / 2 - 32 / 2,
                        bullet.getY() + bullet.getH() / 2 - 32 / 2,
                        32,
                        32,
                        32,
                        32,
                        asteroidExplosion
                    );

                    Particle explosionObj = {newExplosion, 0, 5, 0, 0.05};

                    particles.push_back(explosionObj);
                    float asteroidSpeed;
                    vector<vector<vector<float>>> asteroidVertices;

                    if (oldAsteroidObj.size == asteroidLarge) {
                        asteroidSpeed = asteroidSpeedMedium;
                        asteroidVertices = asteroidMediumVerticesAll;
                    }
                    else if (oldAsteroidObj.size == asteroidMedium) {
                        asteroidSpeed = asteroidSpeedSmall;
                        asteroidVertices = asteroidSmallVerticesAll;
                    }
                    else if (oldAsteroidObj.size == asteroidSmall) {
                        asteroidSpeed = asteroidSpeedLarge;
                        asteroidVertices = asteroidLargeVerticesAll;
                    }
                    
                    int oldPlayerScore = playerScore;
                    breakAsteroid(
                        oldAsteroidObj,
                        asteroids,
                        asteroidSpeed,
                        asteroidLarge,
                        asteroidMedium,
                        asteroidSmall,
                        asteroidSpritesMedium,
                        asteroidSpritesSmall,
                        asteroidScoring,
                        explosionSound,
                        explosionSoundChannel,
                        asteroidVertices
                    );

                    // if (floor(oldPlayerScore / extraLifeScore) < floor(playerScore / extraLifeScore)) {
                    //     playerLives++;
                    //     Entity newHeart(5 + (hearts.size() * (32 + 5)), 5, 32, 32, 32, 32, heartEmpty);
                    //     hearts.push_back(newHeart);
                    // }

                    asteroids.erase(asteroids.begin() + j);
                    bullets.erase(bullets.begin() + i);
                    bulletErased = true;

                    // if (asteroids.size() == 0) {
                    //     currentLevel++;
                    //     asteroidsSpawned = 0;
                    //     lastAsteroidSpawn = utils::hireTimeInSeconds() + levelDelay;
                    //     levelAsteroidCount++;
                    // }
                    if (asteroids.size() == 0) {
                        currentLevel++;
                        asteroidsSpawned = 0;
                        lastAsteroidSpawn = utils::hireTimeInSeconds() + levelDelay;
                        levelAsteroidCount++;
                        saucerSpawnDelay = max(2.0f, saucerSpawnDelay - 1);
                        smallSaucerTimeBetweenShots = max(0.25, smallSaucerTimeBetweenShots - 0.025);
                        smallSaucerAngleDeviation = max(0, smallSaucerAngleDeviation - 5);
                    }

                    break;
                }
            }
            
            if (!bulletErased && bullets[i].origin != 's') {
                for (int j = 0; j < saucers.size(); j++) {
                    Entity& saucerEntity = saucers[j].entity;
                    if (saucerEntity.isCollidingWith(bullet, testPoints)) {
                        playSound(explosionSound, explosionSoundChannel);

                        Entity newExplosion(bullet.getX() + bullet.getW() / 2 - 32 / 2, bullet.getY() + bullet.getH() / 2 - 32 / 2, 32, 32, 32, 32, asteroidExplosion);
                        Particle explosionObj = {newExplosion, 0, 5, 0, 0.05};
                        particles.push_back(explosionObj);

                        playerScore += smallSaucerPoints;
                        saucers.erase(saucers.begin() + j);
                        lastSaucerSpawn = utils::hireTimeInSeconds();

                        bulletErased = true;
                        bullets.erase(bullets.begin() + i);

                        break;
                    }
                }
            }

            if (bulletErased) {
                continue;
            }

            if (!playerIsRespawning && bullets[i].origin == 's' &&  player.isCollidingWith(bullet, testPoints)) {
                playerLives--;

                Entity newExplosion(player.getX() + player.getW() / 2 - 64 / 2, player.getY() + player.getH() / 2 - 64 / 2, 64, 64, 64, 64, playerExplosion);
                Particle explosionObj = {newExplosion, 0, 10, 0, 0.05};
                particles.push_back(explosionObj);

                if (playerLives == 0) {
                    gameRunning = false;
                    continue;
                }

                player.setX(screenWidth / 2 + player.getW() / 2);
                player.setY(screenHeight / 2 + player.getH() / 2);
                playerXVel = 0, playerYVel = 0, playerDeg = 90;

                playerIsRespawning = true;
                playerInvincibleStart = utils::hireTimeInSeconds();

                hearts.erase(hearts.begin() + playerLives);
                continue;
            }

            window.render(bullet);
        }

        for (int i = 0; i < asteroids.size(); i++) {
            Entity& asteroid = asteroids[i].entity;

            // check if player is colliding with asteroid
            if (!playerIsRespawning && player.isCollidingWith(asteroid, testPoints)) {
                Entity newExplosion(player.getX() + player.getW() / 2 - 64 / 2, player.getY() + player.getH() / 2 - 64 / 2, 64, 64, 64, 64, playerExplosion);
                Particle explosionObj = {newExplosion, 0, 10, 0, 0.05};
                particles.push_back(explosionObj);

                player.setX(screenWidth / 2 + player.getW() / 2);
                player.setY(screenHeight / 2 + player.getH() / 2);
                playerXVel = 0, playerYVel = 0, playerDeg = 90;

                playerIsRespawning = true;
                playerInvincibleStart = utils::hireTimeInSeconds();

                // float asteroidSpeed;
                // if (asteroids[i].size == asteroidLarge) asteroidSpeed = asteroidSpeedMedium;
                // else if (asteroids[i].size == asteroidMedium) asteroidSpeed = asteroidSpeedSmall;
                // else if (asteroids[i].size == asteroidSmall) asteroidSpeed = asteroidSpeedLarge;
                float asteroidSpeed;
                vector<vector<vector<float>>> asteroidVertices;

                if (asteroids[i].size == asteroidLarge) {
                    asteroidSpeed = asteroidSpeedMedium;
                    asteroidVertices = asteroidMediumVerticesAll;
                }
                else if (asteroids[i].size == asteroidMedium) {
                    asteroidSpeed = asteroidSpeedSmall;
                    asteroidVertices = asteroidSmallVerticesAll;
                }
                else if (asteroids[i].size == asteroidSmall) {
                    asteroidSpeed = asteroidSpeedLarge;
                    asteroidVertices = asteroidLargeVerticesAll;
                }

                int oldPlayerScore = playerScore;
                breakAsteroid(
                    asteroids[i],
                    asteroids,
                    asteroidSpeed,
                    asteroidLarge,
                    asteroidMedium,
                    asteroidSmall,
                    asteroidSpritesMedium,
                    asteroidSpritesSmall,
                    asteroidScoring,
                    explosionSound,
                    explosionSoundChannel,
                    asteroidVertices
                );

                // if (floor(oldPlayerScore / extraLifeScore) < floor(playerScore / extraLifeScore)) {
                //     playerLives++;
                //     Entity newHeart(5 + (hearts.size() * (32 + 5)), 5, 32, 32, 32, 32, heartEmpty);
                //     hearts.push_back(newHeart);
                // }

                asteroids.erase(asteroids.begin() + i);

                if (asteroids.size() == 0) {
                    currentLevel++;
                    asteroidsSpawned = 0;
                    lastAsteroidSpawn = utils::hireTimeInSeconds() + levelDelay;
                    levelAsteroidCount++;
                    saucerSpawnDelay = max(2.0f, saucerSpawnDelay - 1);
                    smallSaucerTimeBetweenShots = max(0.25, smallSaucerTimeBetweenShots - 0.025);
                    smallSaucerAngleDeviation = max(0, smallSaucerAngleDeviation - 5);
                }

                playerLives--;

                if (playerLives == 0) {
                    gameRunning = false;
                    continue;
                }

                hearts.erase(hearts.begin() + playerLives);
                continue;
            }

            bool asteroidHitSaucer = false;

            for (int j = 0; j < saucers.size(); j++) {
                Entity& saucerEntity = saucers[j].entity;
                if (saucerEntity.isCollidingWith(asteroid, testPoints)) {
                    asteroidHitSaucer = true;

                    Entity newExplosion(saucerEntity.getX() + saucerEntity.getW() / 2 - 32 / 2, saucerEntity.getY() + saucerEntity.getH() / 2 - 32 / 2, 32, 32, 32, 32, asteroidExplosion);
                    Particle explosionObj = {newExplosion, 0, 5, 0, 0.05};
                    particles.push_back(explosionObj);

                    // float asteroidSpeed;
                    // if (asteroids[i].size == asteroidLarge) asteroidSpeed = asteroidSpeedMedium;
                    // else if (asteroids[i].size == asteroidMedium) asteroidSpeed = asteroidSpeedSmall;
                    // else if (asteroids[i].size == asteroidSmall) asteroidSpeed = asteroidSpeedLarge;

                    float asteroidSpeed;
                    vector<vector<vector<float>>> asteroidVertices;

                    if (asteroids[i].size == asteroidLarge) {
                        asteroidSpeed = asteroidSpeedMedium;
                        asteroidVertices = asteroidMediumVerticesAll;
                    }
                    else if (asteroids[i].size == asteroidMedium) {
                        asteroidSpeed = asteroidSpeedSmall;
                        asteroidVertices = asteroidSmallVerticesAll;
                    }
                    else if (asteroids[i].size == asteroidSmall) {
                        asteroidSpeed = asteroidSpeedLarge;
                        asteroidVertices = asteroidLargeVerticesAll;
                    }

                    breakAsteroid(
                        asteroids[i],
                        asteroids,
                        asteroidSpeed,
                        asteroidLarge,
                        asteroidMedium,
                        asteroidSmall,
                        asteroidSpritesMedium,
                        asteroidSpritesSmall,
                        asteroidScoring,
                        explosionSound,
                        explosionSoundChannel,
                        asteroidVertices
                    );

                    asteroids.erase(asteroids.begin() + i);
                    saucers.erase(saucers.begin() + j);

                    if (asteroids.size() == 0) {
                        currentLevel++;
                        asteroidsSpawned = 0;
                        lastAsteroidSpawn = utils::hireTimeInSeconds() + levelDelay;
                        levelAsteroidCount++;
                        saucerSpawnDelay = max(2.0f, saucerSpawnDelay - 1);
                        smallSaucerTimeBetweenShots = max(0.25, smallSaucerTimeBetweenShots - 0.025);
                        smallSaucerAngleDeviation = max(0, smallSaucerAngleDeviation - 5);
                    }

                    playerScore += smallSaucerPoints;

                    lastSaucerSpawn = utils::hireTimeInSeconds();
                    break;
                }
            }

            if (asteroidHitSaucer) {
                continue;
            }

            asteroid.setAngle((asteroid.getAngle() + asteroids[i].angleChange) % 360);
            asteroid.setX(asteroid.getX() + asteroids[i].movement[0]);
            asteroid.setY(asteroid.getY() + asteroids[i].movement[1]);

            if (asteroid.isOffScreen(screenWidth, screenHeight)) {
                asteroid.wrap(screenWidth, screenHeight);
            }

            window.render(asteroid, -asteroid.getAngle());

            if (window.getDebug()) {
                window.drawLine(
                    asteroid.getX() + asteroid.getCenter()[0],
                    asteroid.getY() + asteroid.getCenter()[1],
                    asteroid.getX() + asteroid.getCenter()[0] + asteroids[i].movement[0] * asteroids[i].size / 2,
                    asteroid.getY() + asteroid.getCenter()[1] + asteroids[i].movement[1] * asteroids[i].size / 2, 
                    {0, 255, 0},
                    255
                );
            }
        }

        for (int i = 0; i < particles.size(); i++) {
            Particle& particle = particles[i];
            Entity& explosionEntity = particle.entity;

            if (utils::hireTimeInSeconds() - particle.lastFrameUpdate >= particle.timeBetweenFrames) {
                particle.frame++;

                if (particle.frame > particle.totalFrames) {
                    particles.erase(particles.begin() + i);
                    continue;
                }

                particle.entity.setCurrentFrame(
                    particle.frame * explosionEntity.getCurrentFrame().w, 
                    0,
                    explosionEntity.getCurrentFrame().w,
                    explosionEntity.getCurrentFrame().h 
                );

                particle.lastFrameUpdate = utils::hireTimeInSeconds();
            }
            window.render(particle.entity);
        }
        
        for (int i = 0; i < saucers.size(); i++) {
            SmallSaucer& saucer = saucers[i];
            Entity& saucerEntity = saucer.entity;

            if (!playerIsRespawning && player.isCollidingWith(saucerEntity, testPoints)) {
                playSound(explosionSound, explosionSoundChannel);

                Entity newExplosion(player.getX() + player.getW() / 2 - 64 / 2, player.getY() + player.getH() / 2 - 64 / 2, 64, 64, 64, 64, playerExplosion);
                Particle explosionObj = {newExplosion, 0, 10, 0, 0.05};
                particles.push_back(explosionObj);

                player.setX(screenWidth / 2 + player.getW() / 2);
                player.setY(screenHeight / 2 + player.getH() / 2);
                playerXVel = 0, playerYVel = 0, playerDeg = 90;

                playerIsRespawning = true;
                playerInvincibleStart = utils::hireTimeInSeconds();
                lastSaucerSpawn = utils::hireTimeInSeconds();

                playerLives--;

                if (playerLives == 0) {
                    gameRunning = false;
                    continue;
                }

                hearts.erase(hearts.begin() + playerLives);

                saucers.erase(saucers.begin() + i);

                playerScore += smallSaucerPoints;
                continue;
            }

            saucer.entity.setX(saucer.entity.getX() + saucer.xMovement);
            if (saucer.entity.isOffScreen(screenWidth, screenHeight)) {
                saucer.entity.wrap(screenWidth, screenHeight);
            }

            if (utils::hireTimeInSeconds() - saucer.timeBetweenShots >= saucer.lastShot) {
                playSound(fireSound, fireSoundChannel);

                saucer.lastShot = utils::hireTimeInSeconds();
                Entity bulletEntity(
                    saucerEntity.getX() + saucerEntity.getW()/2 - bulletWidth/2,
                    saucerEntity.getY() + saucerEntity.getH()/2 - bulletHeight/2,
                    bulletWidth,
                    bulletHeight,
                    16,
                    16,
                    bulletSprite
                );

                int angleToPlayer = atan2(
                    (player.getY() + player.getH() / 2) - (bulletEntity.getY() + bulletEntity.getH() / 2), 
                    (player.getX() + player.getW() / 2) - (bulletEntity.getX() + bulletEntity.getW() / 2)
                ) * (180/M_PI);

                // add angle deviation to bullet trajectory to make saucers less dangerous
                angleToPlayer += utils::randNum(-saucer.angleDeviation, saucer.angleDeviation);

                vector<float> bulletMovement = {
                    cos(angleToPlayer * (M_PI/180)) * bulletSpeed,
                    sin(angleToPlayer * (M_PI/180)) * bulletSpeed
                };

                Bullet newBullet = {bulletEntity, bulletMovement, 's'};
                bullets.push_back(newBullet);
            }

            if (!saucer.yJumping && utils::hireTimeInSeconds() - saucer.lastYJump >= smallSaucerTimeBetweenJumps) {
                saucer.yJumping = true;
                saucer.yJumpStart = utils::hireTimeInSeconds();
                saucer.yJumpDistance = utils::randChoice(vector<float>({-smallSaucerJumpDistance, smallSaucerJumpDistance}));
            }

            if (saucer.yJumping && utils::hireTimeInSeconds() - saucer.yJumpStart >= saucer.yJumpDuration) {
                saucer.yJumping = false;
                saucer.lastYJump = utils::hireTimeInSeconds();
            }

            if (saucer.yJumping) {
                saucerEntity.setY(saucerEntity.getY() + saucer.yJumpDistance);
            }

            window.render(saucerEntity);
        }

        if (utils::hireTimeInSeconds() - playerInvincibleStart > playerInvincibleDuration) {
            playerIsRespawning = false;
            playerRender = true;
        }

        if (playerIsRespawning) {
            if (utils::hireTimeInSeconds() - lastRenderSwitch >= 0.2) {
                lastRenderSwitch = utils::hireTimeInSeconds();
                playerRender = !playerRender;
            }
        }

        if (playerRender) {
            window.render(player, -(playerDeg - 90));
        }

        // RENDERING UI ELEMENTS: --------------------------------------

        if (scoreText.getText() != to_string(playerScore)) {
            scoreText.setText(to_string(playerScore));
            scoreText.setX(screenWidth - scoreText.getW() - 5);
        }

        for (int i = 0; i < allText.size(); i++) {
            allText[i]->render(window);
        }

        bool playerOverHearts = false;
        for (int i = 0; i < hearts.size(); i++) {
            if (hearts[i].isCollidingWith(player, testPoints)) {
                playerOverHearts = true;
                break;
            }
        }
        if (!playerOverHearts) {
            for (int i = 0; i < hearts.size(); i++) {
                window.render(hearts[i]);
            }
        }

        for (int i = 0; i < buttons.size(); i++) {
            Entity buttonEntity = buttons[i].getEntity();
            if (!(clickedX == -1 && clickedY == -1)) {
                if (buttons[i].isClicked(clickedX, clickedY)) {
                    buttons[i].click(clickedX, clickedY);
                }
            }
            window.render(buttonEntity);
        }

        // for (int i = 0; i < testPoints.size(); i++) {
        //     vector<vector<float>>& current = testPoints[i];
        //     if (current.size() == 1) {
        //         window.drawPoint(current[0][0], current[0][1], {0, 255, 255}, 255, 4, 4);
        //     } else {
        //         window.drawLine(current[0][0], screenHeight, current[1][0], 0, {255, 255, 0}, 255);
        //     }
        // }
        for (int i = 0; i < testPoints.size(); i++) {
            vector<vector<float>>& current = testPoints[i];
            vector<int> color;
            if (i % 3 == 0) {
                color = {255, 0, 0};
            } else if (i % 3 == 1) {
                color = {255, 255, 0};
            } else {
                color = {255, 255, 255};
            }
            window.drawPoint(current[0][0], current[0][1], color, 255, 5, 5);
            window.drawPoint(current[1][0], current[1][1], color, 255, 5, 5);
            window.drawLine(current[0][0], current[0][1], current[1][0], current[1][1], color, 255);
        }

        window.display();

        // if (testPoints.size() != 0) {
        //     SDL_Delay(1000);
        // }
        // endFrameTicks = SDL_GetTicks();

        delayRefreshRate(SDL_GetTicks() - startTicks, refreshRate);
    }
    cleanupAudio(allSounds);
}

int highScore(RenderWindow window, SDL_Event event, int refreshRate, SDL_Texture* monospaceFont) {
    Text title(0, 0, 64, "Highscores", monospaceFont);
    title.center(screenWidth, screenHeight);
    title.setY(20);

    SDL_Texture* backButtonTexture = window.loadTexture("./res/back_button.png");

    Entity backButtonEntity(17, 27, 50, 50, 64, 64, backButtonTexture);
    Button backButton(&backButtonEntity, &startButtonClick);

    vector<pair<string, string>> highscores; // {INITIALS, SCORE}
    string currentLine;

    ifstream highscoreFile("./HIGHSCORE.txt");
    
    while (getline(highscoreFile, currentLine)) {
        string name;
        string score;
        name = currentLine.substr(0, currentLine.find(":"));
        score = currentLine.substr(currentLine.find(":") + 1);
        highscores.push_back({name, score});
    }
    highscoreFile.close();

    // float textPadding = ((screenHeight - 74) / highscores.size()) - 32;
    float textPadding = ((screenHeight - 50 - 74) / highscores.size()) - 32;

    vector<Text> textScores;
    for (int i = 0; i < highscores.size(); i++) {
        string text1;        
        string text2;        
        if (highscores[i] == pair<string, string>({"NULL", "0"})) {
            // text = to_string(i+1) + "  None";
            text1 = "None";
            text2 = "None";
        } else {
            // text = to_string(i+1) + highscores[i].first + "  " + highscores[i].second;
            // text = highscores[i].first + " " + highscores[i].second;
            text1 = highscores[i].first;
            text2 = highscores[i].second;
        }
        Text newScore(0, 0, 32, text1, monospaceFont);
        newScore.setY(74 + textPadding + (i * (32 + textPadding)));
        newScore.setX(2 * screenWidth / 7);
        textScores.push_back(newScore);

        Text newScoreNum(0, 0, 32, text2, monospaceFont);
        newScoreNum.setY(74 + textPadding + (i * (32 + textPadding)));
        newScoreNum.setX(2 * screenWidth / 3);
        textScores.push_back(newScoreNum);

        Text newNum(0, 0, 32, to_string(i+1), monospaceFont);
        newNum.setY(74 + textPadding + (i * (32 + textPadding)));
        newNum.setX(title.getX());
        textScores.push_back(newNum);
    }

    bool highScoreRunning = true;
    while (highScoreRunning) {
        int startTicks = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                highScoreRunning = false;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (backButton.isClicked(event.button.x, event.button.y)) {
                        return 1;
                    }
                }
            }
        }

        window.clear();

        title.render(window);

        for (int i = 0; i < textScores.size(); i++) {
            textScores[i].render(window);
        }

        Entity buttonEntity = backButton.getEntity();
        window.render(buttonEntity);

        window.display();

        delayRefreshRate(SDL_GetTicks() - startTicks, refreshRate);
    }

    return 0;    
}

int gameOver(RenderWindow window, SDL_Event event, int refreshRate, SDL_Texture* monospaceFont) {
    // float endTime = utils::hireTimeInSeconds();
    bool gameOverScreen = true;

    string currentLine;
    vector<pair<string, int>> highscores;
    bool newHighScore = false;
    int newScorePos;

    ifstream highscoreFile("./HIGHSCORE.txt");
    while (getline(highscoreFile, currentLine)) {
        string name = currentLine.substr(0, currentLine.find(":"));
        string scoreStr = currentLine.substr(currentLine.find(":") + 1);
        int score = stoi(scoreStr);
        highscores.push_back({name, score});
    }
    highscoreFile.close();

    for (int i = 0; i < highscores.size(); i++) {
        if (playerScore > highscores[i].second) {
            newHighScore = true;
            newScorePos = i;
            break;   
        }
    }

    // if (newHighScore) {
    bool submitted = false;
    string nameInputText = newHighScore ? "Enter Name For highscore" : "";
    string enterTextStr = newHighScore ? "press enter to submit" : "";
    Text nameInput(0, 0, 32, nameInputText, monospaceFont);
    nameInput.center(screenWidth, screenHeight);
    Text enterText(0, 0, 20, enterTextStr, monospaceFont);
    enterText.center(screenWidth, screenHeight);
    enterText.setY(nameInput.getY() + 32 + 20);
    string inputText = "";
    // }

    if (newHighScore) {
        SDL_StartTextInput();
    }

    float lastTextRender = utils::hireTimeInSeconds();
    bool textRender = true;

    Text endGame(0, 0, 50, "Game Over", monospaceFont);
    endGame.setX(screenWidth / 2 - endGame.getW() / 2);
    endGame.setY(screenHeight / 3 - endGame.getH() / 2);

    SDL_Texture* buttonSprite = window.loadTexture("./res/start_game_button.png");
    Entity playAgainButtonEntity(screenWidth / 2 - 500 / 2, screenHeight / 1.5 - 70 / 2, 500, 70, 500, 70, buttonSprite);
    Button playAgain(&playAgainButtonEntity, &startButtonClick);

    Text playAgainText(0, 0, 40, "Play Again", monospaceFont);
    playAgainText.center(screenWidth, screenHeight);
    playAgainText.setY(playAgainText.getY() + screenHeight / 6);

    Entity mainMenuButtonEntity(screenWidth / 2 - 500 / 2, screenHeight / 1.25 - 70 / 2, 500, 70, 500, 70, buttonSprite);
    Button mainMenu(&mainMenuButtonEntity, &startButtonClick);

    Text mainMenuText(0, 0, 40, "Main Menu", monospaceFont);
    mainMenuText.center(screenWidth, screenHeight);
    mainMenuText.setY(mainMenuText.getY() + screenHeight / 3.3333);

    while (gameOverScreen) {
        int startTicks = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                gameOverScreen = false;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (playAgain.isClicked(event.button.x, event.button.y)) {
                        return 1;
                    }
                    if (mainMenu.isClicked(event.button.x, event.button.y)) {
                        return 2;
                    }
                }
            }

            if (newHighScore && !submitted) {
                if (event.type == SDL_TEXTINPUT && inputText.size() < 10) {
                    string eventText(event.text.text);
                    if (!(inputText.size() == 0 && eventText[eventText.size() - 1] == ' ')) {
                        inputText += eventText[eventText.size() - 1];
                        nameInput.setText(inputText);
                    }
                }
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_BACKSPACE && inputText.size() != 0) {
                        inputText.pop_back();
                        nameInput.setText(inputText);
                    }
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        submitted = true;
                        enterText.setText("Highscore Submitted");
                        enterText.setX(screenWidth / 2 - enterText.getW() / 2);
                        // highscores[newScorePos] = {inputText, playerScore};
                        highscores.insert(highscores.begin() + newScorePos, {inputText, playerScore});
                        highscores.pop_back();
                        ofstream highscoreFileOutput("./HIGHSCORE.txt");
                        for (int i = 0; i < highscores.size(); i++) {
                            string nextLine = highscores[i].first + ":" + to_string(highscores[i].second);
                            if (i != highscores.size() - 1) {
                                nextLine += '\n';
                            }
                            highscoreFileOutput << nextLine;
                        }
                        highscoreFileOutput.close();
                    }
                }
            }
        }

        window.clear();

        if (textRender) {
            playAgainText.render(window);
        }
        endGame.render(window);
        mainMenuText.render(window);

        if (utils::hireTimeInSeconds() - lastTextRender > 0.5) {
            lastTextRender = utils::hireTimeInSeconds();
            textRender = !textRender;
        }

        Entity buttonEntity = playAgain.getEntity();
        window.render(buttonEntity);

        Entity buttonEntity2 = mainMenu.getEntity();
        window.render(buttonEntity2);

        nameInput.center(screenWidth, screenHeight);
        nameInput.render(window);
        enterText.render(window);

        window.display();
        delayRefreshRate(SDL_GetTicks() - startTicks, refreshRate);
    }
    return 0;
}

int main(int argc, char* args[]) {

    srand((unsigned) time(0));

    // initialize sdl video and audio

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) > 0) {
        cout << "SDL INIT FAILED. SDL_ERROR: " << SDL_GetError() << "\n";
    }
    if (!IMG_Init(IMG_INIT_PNG)) {
        cout << "SDL IMAGE INIT FAILED. SDL_ERROR: " << SDL_GetError() << "\n";
    }
    if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << '\n';
    }
    
    // Mix_Chunk* explosion = Mix_LoadWAV("./res/audio/bangLarge.wav");
    // Mix_PlayChannel(-1, explosion, 0);

    RenderWindow window("Asteroids", screenWidth, screenHeight);

    SDL_Surface* icon = IMG_Load("./res/asteroid_icon.png");
    SDL_SetWindowIcon(window.getWindow(), icon);

    bool gameRunning = true;

    SDL_Texture* monospaceFont = window.loadTexture("./res/fonts/font1.png");

    SDL_Event event;
    const int refreshRate = 60; // set to 60 to make sure speed is the same on all displays

    int nextFunc = 0; // 0 = mainMenu, 1 = mainGame, 2 = gameOver, 3 = highScore

    while (true) {
        int result;
        if (nextFunc == 0) {
            result = startMenu(window, event, refreshRate, monospaceFont);
            if (result == 0) {
                break;
            } else if (result == 2) {
                nextFunc = 3;
            } else {
                nextFunc = 1;
            }
            // window.clear();
            // SDL_Delay(500);
        }

        if (nextFunc == 1) {
            result = mainGame(window, event, refreshRate, monospaceFont);
            if (result == 0) {
                break;
            }
            SDL_Delay(500);
            nextFunc = 2;
        }

        if (nextFunc == 2) {
            result = gameOver(window, event, refreshRate, monospaceFont);
            if (result == 0) {
                break;
            } else if (result == 1) {
                nextFunc = 1;
            } else if (result == 2) {
                nextFunc = 0;
            }
            playerScore = 0;
            // SDL_Delay(500);
        }

        if (nextFunc == 3) {
            result = highScore(window, event, refreshRate, monospaceFont);
            if (result == 0) {
                break;
            } else if (result == 1) {
                nextFunc = 0;
            }
            // SDL_Delay(500);
        }
    }

    window.cleanUp();
    // Mix_FreeChunk(explosion);
    // explosion = NULL;
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}