#include "OBS.h"
#include "sounds/Sounds.h"
#include "utils/Utils.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;


void Game::playTheme(Theme theme) {

    #ifdef SOUNDS

    constexpr char themes[2][17] = { "music/OBS_00.raw", "music/OBS_01.raw", };

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
                                           
                case SoundEffect::KeyPress:
                    Audio::play<1>(Sounds::sfx_Typewriter);    
                    break;
                
                case SoundEffect::Player_Hit:
                    Audio::play<1>(Sounds::sfx_Player_Hit);    
                    break;
                
                case SoundEffect::Game_Over:
                    Audio::play<1>(Sounds::sfx_Game_Over);    
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

        asteroid.setX(110 + random(0, 96));
        asteroid.setY(random(0, 70));
        asteroid.setType(random(0, 2));
        asteroid.setActive(true);

        Rect asteroid1_Rect = asteroid.getLargeAsteroidRect();

        bool collision = false;

        for (Asteroid &largeAsteroid : largeAsteroids) {

            if (asteroid.getX() != largeAsteroid.getX() || asteroid.getY() != largeAsteroid.getY()) {

                Rect asteroid2_Rect = largeAsteroid.getLargeAsteroidRect();

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

    const uint8_t colour[] = { 7, 6, 5 };

    for (uint8_t i = 0; i < Constants::StarField_Size; i++) {

        Point &star = this->starfield[i];

        uint8_t frameMultiple = 6 + ((i % 3) * 2);

        if (PC::frameCount % frameMultiple == 0) {

            star.setX(star.getX() - 1);

            if (star.getX() == -1) {
                star.setX(110);
                star.setY(random(0, 88));
            }

        }

        PD::setColor(colour[i % 3]);
        PD::drawPixel(star.getX() + gameScreenVars.xOffset, star.getY() + gameScreenVars.yOffset);
        
    }

}

void Game::moveRenderSmallAsteroids(bool alternate) {

    // bool launch = true;

    // switch (this->gameState) {

    //     case GameState::Game_Init:
    //     case GameState::Game:
    //     case GameState::Game_EnemyLeaving:
    //     case GameState::Game_BossLeaving:
    //     case GameState::Score:
    //         break;

    //     default:
    //         launch = false;
    //         break;

    // }


    // Move and render small asteroids ..

    for (uint8_t i = 0; i < Constants::SmallAsteroid_Size; i++) {

        Asteroid &smallAsteroid = smallAsteroids[i];

        if (PC::frameCount % 3 == 0) {

            if (smallAsteroid.getX() > Constants::SmallAsteroid_OffScreen) smallAsteroid.setX(smallAsteroid.getX() - 1);
            
            if (smallAsteroid.getX() == Constants::SmallAsteroid_OffScreen) {
                // if (launch) {
                    smallAsteroid.setX(110 + random(0, 96));
                    smallAsteroid.setY(random(0, 80));
                    smallAsteroid.setActive(true);
                // }
                // else {
                //     smallAsteroid.active = false;
                // }

            }

        }

        if (!alternate || (i % 2 == 0)) {

            if (smallAsteroid.getActive()) {
                PD::drawBitmap(smallAsteroid.getX() + gameScreenVars.xOffset, smallAsteroid.getY() + gameScreenVars.yOffset, Images::SmallAsteroid);
            }

        }
        
    }

}

void Game::moveRenderLargeAsteroids(bool alternate) {

    bool launch = true;

    switch (this->gameState) {

        case GameState::Game_Init:
        case GameState::Game:
        case GameState::Game_EnemyLeaving:
        case GameState::Game_BossLeaving:
        case GameState::Score:
            break;

        default:
            launch = false;
            break;

    }

    for (uint8_t i = 0; i < Constants::LargeAsteroid_Size; i++) {
    
        Asteroid &largeAsteroid = largeAsteroids[i];

        if (PC::frameCount % 2 == 0) {

            if (largeAsteroid.getX() > Constants::LargeAsteroid_OffScreen) largeAsteroid.setX(largeAsteroid.getX() - 1);

            if (largeAsteroid.getX() == Constants::LargeAsteroid_OffScreen) {

                if (launch) {
                    launchLargeAsteroid(largeAsteroid);
                }
                else {
                    largeAsteroid.setActive(false);
                }

            }

        }

        if (!alternate || (i % 2 == 0)) {

            if (largeAsteroid.getActive()) {

                PD::drawBitmap(largeAsteroid.getX() + gameScreenVars.xOffset, largeAsteroid.getY() + gameScreenVars.yOffset, Images::BigAsteroid[largeAsteroid.getType()]);

            }

        }
        
    }

}

void Game::launchEnemy(Enemy &enemy) {

    switch (this->gameState) {

        case GameState::Game_Init:
        case GameState::Game:
        case GameState::Game_BossLeaving:
        case GameState::Score:
            break;

        default:
            return;

    }

    while (true) {

        enemy.setX(110 + random(0, 96));
        enemy.setY(random(0, 46));
        enemy.setPathCount(random(6, 12));
        enemy.setMotion(static_cast<Motion>(random(0, 2)));
        enemy.setPath(static_cast<Path>(random(1, 3)));
        enemy.setActive(true);

        switch (enemy.getPath()) {

            case Path::Large:
                enemy.setYOffset(random(0, 24));
                break;

            case Path::Medium:
                enemy.setYOffset(random(0, 44));
                break;

            case Path::Small:
                enemy.setYOffset(random(0, 64));
                break;
            
        }


        Rect enemy_Rect = enemy.getRect();

        bool collision = false;


        // Check for overlap with large asteroids ..
        
        for (Asteroid &largeAsteroid : largeAsteroids) {

            Rect asteroid_Rect = { largeAsteroid.getX() - 4, largeAsteroid.getY() - 4, 27, 27 };

            if (collide(enemy_Rect, asteroid_Rect)) {

                collision = true;
                break;

            }

        }


        // Check for overlap with other enemies ..
        
        if (!collision) {
            
            for (Enemy &enemy2 : enemies) {

                if (enemy.getX() != enemy2.getX() || enemy.getY() != enemy2.getY()) {

                    Rect enemy_Rect2 = enemy2.getRect();

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


void Game::checkBulletCollision(Bullet &bullet) {


    // Has the bullet hit a large asteroid?

    Rect bulletRect = bullet.getRect(BulletType::PlayerBullet);

    for (Asteroid &largeAsteroid : largeAsteroids) {

        if (largeAsteroid.getActive()) {
                
            Rect asteroidRect = largeAsteroid.getLargeAsteroidRect();

            if (collide(bulletRect, asteroidRect)) {

                bullet.setHitObject(HitObject::LargeAsteroid);
                bullet.setHitCount(1);
                bullet.setMuzzleIndex(0);
                bullet.setX(largeAsteroid.getX() - 4);
                return;

            }

        }

    }


    // Has the bullet hit an enemy?

    for (Enemy &enemy : enemies) {

        Rect enemyRect = enemy.getRect();

        if (enemy.getActive() && collide(bulletRect, enemyRect)) {

            bullet.setHitObject(HitObject::Enemy);
            bullet.setHitCount(1);
            bullet.setMuzzleIndex(0);
            bullet.setX(enemy.getX() - 4);

            enemy.setExplodeCounter(21);
            enemy.setActive(false);
            this->gameScreenVars.score = this->gameScreenVars.score + 5;

            PC::setFrameRate(50 + (this->gameScreenVars.score / 24));

            #ifdef SOUNDS
                playSoundEffect(SoundEffect::Mini_Explosion);
            #endif            

            return;

        }

    }


    // Has the bullet hit a boss ?

    if (this->boss.getActive()) {


        // Top hand ..

        if (this->boss.getTopHealth() > 0 ) {

            Rect enemyRect = { this->boss.getX(), this->boss.getY() + 7, 4, 3 };

            if (collide(bulletRect, enemyRect)) {

                if (this->boss.getTopHealth() > 0) {

                    this->boss.setTopHealth(this->boss.getTopHealth() - 1);
                    this->boss.setExplodeCounter(ExplodeType::TopHand, 21);

                    bullet.setHitObject(HitObject::Boss);
                    bullet.setMuzzleIndex(0);
                    bullet.setX(-10);
                    bullet.setActive(false);

                    #ifdef SOUNDS
                        playSoundEffect(SoundEffect::Mini_Explosion);
                    #endif   
                                    
                    return;

                }

            }

        }


        // Lower hand ..

        if (this->boss.getBottomHealth() > 0) {

            Rect enemyRect = { this->boss.getX(), this->boss.getY() + 30, 4, 3 };

            if (collide(bulletRect, enemyRect)) {

                if (this->boss.getBottomHealth() > 0) {

                    this->boss.setBottomHealth(this->boss.getBottomHealth() - 1);
                    this->boss.setExplodeCounter(ExplodeType::BottomHand, 21);

                    bullet.setHitObject(HitObject::Boss);
                    bullet.setMuzzleIndex(0);
                    bullet.setX(-10);
                    bullet.setActive(false);
                    
                    #ifdef SOUNDS
                        playSoundEffect(SoundEffect::Mini_Explosion);
                    #endif   

                    return;

                }

            }

        }


        // Upper arm ..

        Rect enemyRect = { this->boss.getX() + 2, this->boss.getY() + 4, 16, 9 };

        if (collide(bulletRect, enemyRect)) {

            this->boss.setExplodeCounter(ExplodeType::Body, 6);
            Point &point = this->boss.getExplodePoint();

            point.setX(enemyRect.x - this->boss.getX());
            point.setY(bullet.getY() - 2 - this->boss.getY());
            bullet.setX(-10);
            bullet.setActive(false);
            return;

        }


        // Lower arm ..

        enemyRect = { this->boss.getX() + 2, this->boss.getY() + 27, 16, 9 };

        if (collide(bulletRect, enemyRect)) {

            this->boss.setExplodeCounter(ExplodeType::Body, 6);
            Point &point = this->boss.getExplodePoint();
            point.setX(enemyRect.x - this->boss.getX());
            point.setY(bullet.getY() - 2 - this->boss.getY());
            bullet.setX(-10);
            bullet.setActive(false);
// printf("Bottom arm\n");   
            return;

        }


        // Body ..

        enemyRect = { this->boss.getX() + 8, this->boss.getY() + 1, 18, 38 };

        if (collide(bulletRect, enemyRect)) {

            this->boss.setExplodeCounter(ExplodeType::Body, 6);
            Point &point = this->boss.getExplodePoint();
            point.setX(enemyRect.x - this->boss.getX());
            point.setY(bullet.getY() - 2 - this->boss.getY());
            bullet.setX(-10);
            bullet.setActive(false);
// printf("Body\n");                
            return;

        }

// PD::drawRect(this->boss.x, this->boss.y + 7, 4, 3 );
// PD::drawRect(this->boss.x, this->boss.y + 30, 4, 3 );
// PD::drawRect(this->boss.x + 8, this->boss.y + 1, 18, 38 );
// PD::drawRect(this->boss.x + 2, this->boss.y + 4, 16, 9 );
// PD::drawRect(this->boss.x + 2, this->boss.y + 27, 16, 9 );



        if (this->boss.getTopHealth() == 0 && this->boss.getBottomHealth() == 0) {

            this->gameState = GameState::Game_BossLeaving;
            this->gameScreenVars.score = this->gameScreenVars.score + 1;

        }


        // Has the bullet hit a boss bullet?

        for (Bullet &bossBullet : this->bossBullets.bullets) {

            if (bossBullet.getActive()) {

                Rect bossBulletRect = bossBullet.getRect(BulletType::BossBullet);

                if (collide(bulletRect, bossBulletRect)) {

                    bullet.setHitObject(HitObject::BossBullet);
                    bullet.setHitCount(1);
                    // bullet.setX(-10);
                    bossBullet.setX(-10);
                    bossBullet.setActive(false);
// printf("Hit\n")                    ;
                    return;

                }

            }

        }

    }

}

void Game::printScore(uint8_t x, uint8_t y, uint16_t score) {

    uint8_t digits[5] = {};
    extractDigits(digits, score);

    for (uint8_t j = 0; j < 5; ++j) {

        PD::setCursor(x - (j * 5), y);
        PD::print(static_cast<char>(48 + digits[j]));

    }

}


void Game::lauchHealth() {

    this->health.setX(110);
    this->health.setY(random(0, 74));
    this->health.setActive(true);

}


void Game::moveHealth() {

    this->health.decX(1);

    if (this->health.getX() <= -14) {
        this->health.setActive(false);
    }
    
}
