#include "OBS.h"
#include "sounds/Sounds.h"
#include "utils/Utils.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;


void Game::moveBoss() {


    // Move Boss ..

    if (PC::frameCount % 2 == 0) {

        switch (this->gameState) {

            case GameState::Game_BossEntering:

                boss.x--;
                boss.pathCounter++;
                if (boss.pathCounter == 70) boss.pathCounter = 0;
                boss.y = 10 + Constants::Enemy_Path_Large[boss.pathCounter];

                if (boss.x == 82) {
                    this->gameState = GameState::Game_Boss;
                }

                break;

            case GameState::Game_Boss:

                boss.pathCounter++;
                if (boss.pathCounter == 70) boss.pathCounter = 0;
                boss.y = 10 + Constants::Enemy_Path_Medium[boss.pathCounter];

                break;

        }

    }


    // Boss fires bullet?

    switch (this->gameState) {

        case GameState::Game_BossEntering:
        case GameState::Game_Boss:
            {

                if ((boss.topHealth > 0 || boss.bottomHealth > 0) && (random(0, Constants::Boss_Bullet_Random) == 0)) {

                    uint8_t bulletIdx = bossBullets.getInactiveBullet();

                    if (bulletIdx != Constants::Bullet_None) {
    // printf("fired bullet ");
                        Bullet &bullet = bossBullets.bullets[bulletIdx];

                        bullet.y = boss.y + (boss.topHealth > 0 && boss.bottomHealth > 0 ? (bulletIdx % 2 == 0 ? 8 : 31) : boss.topHealth > 0 ? 8 : 31);
                        bullet.x = 82;
                        bullet.direction = static_cast<Direction>(random(static_cast<uint8_t>(Direction::UpLeft), static_cast<uint8_t>(Direction::DownLeft) + 1));
                        bullet.active = true;

    // printf(", driection %i -- %i\n", (uint8_t)bullet.direction, static_cast<uint8_t>(Direction::UpLeft) );

                    }
                    else {
    // printf("no fired bullet ");
                    }

                }

            }
            break;

    }

}



void Game::checkBossBulletCollision(Bullet &bullet) {


    // Has the bullet hit a large asteroid?

    Rect bulletRect = { bullet.x + 1, bullet.y + 1, 7, 5 };

    for (Asteroid &largeAsteroid : largeAsteroids) {

        Rect asteroidRect = { largeAsteroid.x + 1, largeAsteroid.y + 1, 17, 17 };

        if (collide(bulletRect, asteroidRect)) {
            bullet.hitObject = HitObject::LargeAsteroid;
            bullet.hitCount = 1;
            bullet.muzzleIndex = 0;
            bullet.x = largeAsteroid.x + 17;
        }

    }


    // Has the bullet hit the player?

    Rect playerRect = { 10, player.y + 1, 12, 8 };

    if (collide(bulletRect, playerRect)) {

        bullet.hitObject = HitObject::Player;
        bullet.hitCount = 1;
        bullet.muzzleIndex = 0;
        bullet.x = 22;

        if (player.health > 0)  {

            player.health--;

            #ifdef SOUNDS
                playSoundEffect(SoundEffect::Mini_Explosion);
            #endif
        
            if (player.health == 0) {

                player.explodeCounter = 21;

                #ifdef SOUNDS
//                                        tunes.playScore(Sounds::PlayerDies);
                #endif  
            }

        }


        #ifdef SOUNDS
            playSoundEffect(SoundEffect::Mini_Explosion);
        #endif            

    }

}