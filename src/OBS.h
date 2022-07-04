#pragma once

#include "Pokitto.h"
#include <LibAudio>
#include <LibSchedule>
#include "images/Images.h"
#include "utils/Constants.h"
#include "utils/Enums.h"
#include "utils/Structs.h"
#include "utils/GameCookie.h"

using PC=Pokitto::Core;
using PD=Pokitto::Display;
using PB=Pokitto::Buttons;


class Game {

    private:

        Point starfield[Constants::StarField_Size];
        Asteroid smallAsteroids[Constants::SmallAsteroid_Size];
        Asteroid largeAsteroids[Constants::LargeAsteroid_Size];
        Enemy enemies[Constants::Enemy_Size];
        Player player;

        Bullets bullets;

        File mainThemeFile;
        
        int8_t xOffset = 0;
        int8_t yOffset = 0;
        uint8_t offsetCount = 0;
        uint8_t scoreIndex = 0;
        uint8_t clearScores = 0;

        SplashScreenVars splashScreenVariables;
        TitleScreenVars titleScreenVars;
        GameCookie *cookie;

        GameState gameState = GameState::Splash_Init;


    public:

        void setup(GameCookie *cookie);
        void loop();

    private:

        void splashScreen_Init();
        void splashScreen();
        void game_Init();
        void game();
        void title_Init();
        void title();

        void launchLargeAsteroid(Asteroid &asteroid);
        void moveRenderStarfield();
        void moveRenderSmallAsteroids(bool alternate);
        void moveRenderLargeAsteroids(bool alternate);
        void introText(int8_t y);
        void launchEnemy(Enemy &enemy);
        void checkBulletCollision(Bullet &bullet);

        void playTheme(Theme theme);
        void muteTheme();
        void playSoundEffect(SoundEffect soundEffect);
        bool collide(Rect rect1, Rect rect2);
        void printScore(uint8_t x, uint8_t y, uint16_t score);

};

