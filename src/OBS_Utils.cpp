#include "OBS.h"
#include "sounds/Sounds.h"
#include "utils/Utils.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;


void Game::playTheme(Theme theme) {

    #ifdef SOUNDS

    constexpr char themes[2][19] = { "music/1nvad_00.raw", "music/1nvad_01.raw" };

    switch (this->cookie->sfx) {

        case SoundEffects::Music:
        case SoundEffects::Both:

            if (this->mainThemeFile.openRO(themes[static_cast<uint8_t>(theme)])) {
                auto& music = Audio::play<0>(this->mainThemeFile);
                music.setLoop(true);
            }

            break;

        case SoundEffects::SFX:
        case SoundEffects::None:

            this->muteTheme();

    }

    #endif

}

void Game::muteTheme() {

    #ifdef SOUNDS
    Audio::stop<0>();
    #endif

}


void Game::playSoundEffect(SoundEffect soundEffect) {

    #ifdef SOUNDS

    constexpr char themes[1][19] = { "music/1nvad_00.raw" };

    switch (this->cookie->sfx) {

        case SoundEffects::SFX:
        case SoundEffects::Both:
                
            switch (soundEffect) {
                
                case SoundEffect::Laser:
                    Audio::play<1>(Sounds::sfx_Laser);    
                    break;
                
                case SoundEffect::Mini_Explosion:
                    Audio::play<1>(Sounds::sfx_Mini_Explosion);    
                    break;
                                
                case SoundEffect::Level_Cleared:
                    Audio::play<1>(Sounds::sfx_Level_Cleared);    
                    break;
                
                case SoundEffect::Bomb_HitGround:
                    Audio::play<1>(Sounds::sfx_Bomb_HitGround);    
                    break;
                
                case SoundEffect::Drop_Bomb:
                    Audio::play<1>(Sounds::sfx_Drop_Bomb);    
                    break;
                
                case SoundEffect::Player_Hit:
                    Audio::play<1>(Sounds::sfx_Player_Hit);    
                    break;

            }

            break;

        default: break;
        
    }  

    #endif

}    


bool Game::collide(Rect rect1, Rect rect2) {

   return !(rect2.x                >= rect1.x + rect1.width  ||
            rect2.x + rect2.width  <= rect1.x                ||
            rect2.y                >= rect1.y + rect1.height ||
            rect2.y + rect2.height <= rect1.y);

 }


// Launch a large Asteroid making sure there is no overlap ..

void Game::launchLargeAsteroid(Asteroid &asteroid) {

    while (true) {

        asteroid.x = 110 + random(0, 96);
        asteroid.y = random(0, 70);
        asteroid.type = random(0, 2);

        Rect asteroid1_Rect = { asteroid.x, asteroid.y, 19, 19 };

        bool collision = false;

        for (Asteroid &largeAsteroid : largeAsteroids) {

            if (asteroid.x != largeAsteroid.x || asteroid.y != largeAsteroid.y) {

                Rect asteroid2_Rect = { largeAsteroid.x - 4, largeAsteroid.y - 4, 27, 27 };

                if (collide(asteroid1_Rect, asteroid2_Rect)) {

                    collision = true;
                    break;

                }

            }

        }

        if (!collision) break;                    

    }

}


void Game::moveRenderStarfield() {

    for (Point &star : starfield) {

        if (PC::frameCount % 6 == 0) {

            star.x--;

            if (star.x == -1) {
                star.x = 110;
                star.y = random(0, 88);
            }

        }

        PD::setColor(6 + random(0, 2));
        PD::drawPixel(star.x + xOffset, star.y + yOffset);
        
    }

}

void Game::moveRenderSmallAsteroids(bool alternate) {


    // Move and render small asteroids ..

    for (uint8_t i = 0; i < Constants::SmallAsteroid_Size; i++) {

        Asteroid &smallAsteroid = smallAsteroids[i];

        if (PC::frameCount % 3 == 0) {

            smallAsteroid.x--;

            if (smallAsteroid.x == -9) {
                smallAsteroid.x = 110 + random(0, 96);
                smallAsteroid.y = random(0, 80);
            }

        }

        if (!alternate || (i % 2 == 0)) {
            PD::drawBitmap(smallAsteroid.x + xOffset, smallAsteroid.y + yOffset, Images::SmallAsteroid);
        }
        
    }

}

void Game::moveRenderLargeAsteroids(bool alternate) {

    for (uint8_t i = 0; i < Constants::LargeAsteroid_Size; i++) {
    
        Asteroid &largeAsteroid = largeAsteroids[i];

        if (PC::frameCount % 2 == 0) {

            largeAsteroid.x--;

            if (largeAsteroid.x == -19) {

                launchLargeAsteroid(largeAsteroid);

            }

        }


        if (!alternate || (i % 2 == 0)) {
            PD::drawBitmap(largeAsteroid.x + xOffset, largeAsteroid.y + yOffset, Images::BigAsteroid[largeAsteroid.type]);
        }
        
    }

}

void Game::launchEnemy(Enemy &enemy) {

    while (true) {

        enemy.x = 110 + random(0, 96);
        enemy.y = random(0, 46);
        enemy.pathCount = random(6, 12);
        enemy.motion = static_cast<Motion>(random(0, 2));
        enemy.path = static_cast<Path>(random(1, 3));
        enemy.active = true;

        switch (enemy.path) {

            case Path::Large:
                enemy.yOffset = random(0, 24);
                break;

            case Path::Medium:
                enemy.yOffset = random(0, 44);
                break;

            case Path::Small:
                enemy.yOffset = random(0, 64);
                break;
            
        }


        Rect enemy_Rect = { enemy.x, enemy.y, 6, 9 };

        bool collision = false;


        // Check for overlap with large asteroids ..
        
        for (Asteroid &largeAsteroid : largeAsteroids) {

            Rect asteroid_Rect = { largeAsteroid.x - 4, largeAsteroid.y - 4, 27, 27 };

            if (collide(enemy_Rect, asteroid_Rect)) {

                collision = true;
                break;

            }

        }


        // Check for overlap with other enemies ..
        
        if (!collision) {
            
            for (Enemy &enemy2 : enemies) {

                if (enemy.x != enemy2.x || enemy.y != enemy2.y) {

                    Rect enemy_Rect2 = { enemy2.x - 2, enemy2.y - 2, 8, 11 };

                    if (collide(enemy_Rect, enemy_Rect2)) {

                        collision = true;
                        break;

                    }

                }

            }

        }

        if (!collision) break;                    

    }

}