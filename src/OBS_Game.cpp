#include "OBS.h"
#include "sounds/Sounds.h"
#include "utils/Utils.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;

const int8_t xOffsets[] = { -2, 1, 2, 0 };
const int8_t yOffsets[] = { 0, -2, 1, 2 };

void Game::game_Init() {

    for (Asteroid &smallAsteroid : smallAsteroids) {

        smallAsteroid.x = random(110, 256);
        smallAsteroid.y = random(0, 80);
        smallAsteroid.active = true;
        
    }

    for (Asteroid &largeAsteroid : largeAsteroids) {

        launchLargeAsteroid(largeAsteroid);
        
    }

    for (Enemy &enemy : enemies) {

        launchEnemy(enemy);
        
    }

    bullets.reset();
    bossBullets.reset();

    player.reset();
    gameState = GameState::Game;
    gameScreenVars.reset();
    PC::setFrameRate(50);

}   

void Game::game() {


    // Move player ..

    switch (gameState) {
    
        case GameState::Game ... GameState::Game_BossLeaving:

            {
                if (PC::buttons.pressed(BTN_C) || PC::buttons.repeat(BTN_C, 1)) {

                    gameState = GameState::Game_EnemyLeaving;

                    for (Enemy &enemy : this->enemies) {

                        if (enemy.x > 110) {

                            enemy.active = false;

                        }
                        
                    }

                    for (Asteroid &largeAsteroid : largeAsteroids) {

                        if (largeAsteroid.x > 110) {

                            largeAsteroid.active = false;
                                
                        }
                        
                    }

                }


                // Increase score based on distance travelled ..

                if (PC::frameCount % Constants::ScoreDistance == 0) {

                    player.score++;

                }

                player.y = player.y + static_cast<int8_t>(player.direction);

                if (player.direction == Direction::Up && player.y == 0) {
                    player.direction = Direction::None;
                }

                if (player.direction == Direction::Down && player.y == 78) {
                    player.direction = Direction::None;
                }

                if (PC::buttons.pressed(BTN_A) || PC::buttons.pressed(BTN_B) || PC::buttons.pressed(BTN_LEFT) || PC::buttons.pressed(BTN_RIGHT) || PC::buttons.pressed(BTN_UP) || PC::buttons.pressed(BTN_DOWN)) {
                    
                    uint8_t bulletIdx = bullets.getInactiveBullet();

                    if (bulletIdx != Constants::Bullet_None) {

                        Bullet &bullet = bullets.bullets[bulletIdx];
                        bullet.active = true;

                        switch (player.direction) {

                            case Direction::Up:
                                player.direction = Direction::Down;
                                break;

                            case Direction::Down:
                                player.direction = Direction::Up;
                                break;

                            default:

                                switch (player.y) {

                                    case 0:
                                        player.direction = Direction::Down;
                                        break;

                                    case 78:
                                        player.direction = Direction::Up;
                                        break;

                                    default:
                                        if (random(0,2) == 0) {
                                            player.direction = Direction::Up;
                                        }
                                        else {
                                            player.direction = Direction::Down;
                                        }
                                        break;

                                }

                                break;
                                
                        }

                        bullet.x = 24;
                        bullet.y = player.y + 2;
                        bullet.muzzleIndex = 8;

                        #ifdef SOUNDS
                            playSoundEffect(SoundEffect::Laser);
                        #endif
                        bullet.hitObject = HitObject::None;

                    }

                }


                // Player bullets ..

                for (Bullet &bullet : bullets.bullets) {
                        
                    if (bullet.hitCount > 0) {

                        bullet.hitCount++;

                        if (bullet.hitCount > 3) {
                            bullet.reset();
                        }

                    }

                    if (bullet.x > 0 && bullet.hitCount == 0) {

                        if (bullet.muzzleIndex > 0) {

                            bullet.muzzleIndex--;

                        }
                        else {

                            bullet.x = bullet.x + 4;

                            if (bullet.x >= 110) {

                                bullet.active = false;

                            }

                        }

                        if (bullet.active) checkBulletCollision(bullet);

                    }

                    if (bullet.hitObject == HitObject::LargeAsteroid) {

                        bullet.x--;

                    }

                }


                // Boss bullets ..

                for (Bullet &bullet : bossBullets.bullets) {
// printf("%i,%i(%i) ",bullet.x,bullet.y, bullet.active);             

                    if (bullet.active) {
                            
                        if (bullet.hitCount > 0) {

                            bullet.hitCount++;

                            if (bullet.hitCount > 3) {
                                bullet.reset();
                            }

                        }

                        if (bullet.x > -4 && bullet.hitCount == 0) {

                            switch (bullet.direction) {

                                case Direction::UpLeft:
                                    bullet.x = bullet.x - 2;
                                    bullet.y = bullet.y - 1;
                                    break;

                                case Direction::Left:
                                    bullet.x = bullet.x - 2;
                                    break;

                                case Direction::DownLeft:
                                    bullet.x = bullet.x - 2;
                                    bullet.y = bullet.y + 1;
                                    break;

                            }

                            if (bullet.x <= -4 || bullet.y < -6 || bullet.y > 88) {

                                bullet.active = false;

                            }

                        }

                        if (bullet.active) checkBossBulletCollision(bullet);

                    }

                }
// printf("\n");                        



                // Has the player hit a asteroid?

                bool collision = false;
                Rect playerRect = { 10, player.y + 1, 12, 8 };

                for (Asteroid &largeAsteroid : largeAsteroids) {

                    if (largeAsteroid.active) {
                            
                        Rect asteroidRect = { largeAsteroid.x + 1, largeAsteroid.y + 1, 17, 17 };

                        if (collide(playerRect, asteroidRect)) {

                            if (player.health > 0)  {

                                player.health--;

                                if (player.health > 0)  {

                                    player.health--;

                                }

                                #ifdef SOUNDS
                                    playSoundEffect(SoundEffect::Player_Hit);
                                #endif                            
                                
                                if (player.health == 0) {

                                    player.explodeCounter = 21;

                                    #ifdef SOUNDS
                                        playSoundEffect(SoundEffect::Mini_Explosion);
                                    #endif         
                                }

                            }

                            collision = true;
                            break;

                        }

                    }

                }

                if (collision) {

                    gameScreenVars.offsetCount++;

                    if (gameScreenVars.offsetCount > 4) {

                        gameScreenVars.offsetCount = 1;

                    }

                    gameScreenVars.xOffset = xOffsets[gameScreenVars.offsetCount - 1];
                    gameScreenVars.yOffset = yOffsets[gameScreenVars.offsetCount - 1];
                    
                    // arduboy.invert(offsetCount % 2);

                }
                else {


                    // Has the player hit an enemy ?

                    for (Enemy &enemy : enemies) {

                        Rect enemyRect = { enemy.x + 1, enemy.y + 1, 10, 11 };

                        if (collide(playerRect, enemyRect)) {

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
                                
                            collision = true;
                            break;

                        }

                    }

                    if (collision) {

                        gameScreenVars.offsetCount++;

                        if (gameScreenVars.offsetCount > 4) {

                            gameScreenVars.offsetCount = 1;

                        }

                        gameScreenVars.xOffset = xOffsets[gameScreenVars.offsetCount - 1];
                        gameScreenVars.yOffset = yOffsets[gameScreenVars.offsetCount - 1];
                        
                        // arduboy.invert(offsetCount % 2);

                    }
                    else {

                        gameScreenVars.offsetCount = 0;
                        gameScreenVars.xOffset = 0;
                        gameScreenVars.yOffset = 0;
                        // arduboy.invert(false);

                    }

                }

            }

            break;

        case GameState::Score:

            if (gameScreenVars.highScoreCounter == 0) {

                if (PC::buttons.pressed(BTN_A) || PC::buttons.pressed(BTN_B) || PC::buttons.pressed(BTN_LEFT) || PC::buttons.pressed(BTN_RIGHT) || PC::buttons.pressed(BTN_UP) || PC::buttons.pressed(BTN_DOWN)) {

                    gameState = GameState::Title_Init;

                }

            }
            else {

                gameScreenVars.highScoreCounter--;

            }


            // Clear scores ..

            if (PC::buttons.pressed(BTN_C) || PC::buttons.repeat(BTN_C, 1)) {

                gameScreenVars.clearScores++;

                switch (gameScreenVars.clearScores) {

                    case 21 ... 60:
                        //arduboy.setRGBled(128 - (clearScores * 2), 0, 0);
                        break;

                    case 61:
                        gameScreenVars.clearScores = 0;
                        gameScreenVars.scoreIndex = 255;
                        player.score = 0;
                        //arduboy.setRGBled(0, 0, 0);
                        cookie->reset();

                        #ifdef SOUNDS
                            playSoundEffect(SoundEffect::KeyPress);
                        #endif
                        
                        return;

                }

            }
            else {

                if (gameScreenVars.clearScores > 0) {
                
                    //arduboy.setRGBled(0, 0, 0);
                    gameScreenVars.clearScores = 0;

                }
            
            }

            break;

        default:

            break;

    }
  

    // Move and render starfield and asteroids ..

    moveRenderStarfield();
    moveRenderSmallAsteroids(false);
    moveRenderLargeAsteroids(false);
    moveBoss();
    moveEnemies();


    switch (gameState) {

        case GameState::Game ... GameState::Game_BossLeaving:
            {

                // Render player ..

                if (player.health > 0 || player.explodeCounter > 16) {

                    PD::drawBitmap(9 + gameScreenVars.xOffset, player.y + gameScreenVars.yOffset, Images::PlayerShip);
                    PD::drawBitmap(gameScreenVars.xOffset, player.y + 3 + gameScreenVars.yOffset, Images::ShipParticle[PC::frameCount % 8 < 4]);

                }
                
                if (player.explodeCounter > 0) {

                    PD::drawBitmap(6, player.y + gameScreenVars.yOffset, Images::Puffs[(21 - player.explodeCounter) / 3]);

                }
                
                if (player.updateExplosion()) {
                
                    gameState = GameState::Score;
                    gameScreenVars.highScoreCounter = 64;
                    gameScreenVars.scoreIndex = cookie->saveScore(player.score);

                }


                // Render player bullets ..
                
                for (Bullet &bullet : bullets.bullets) {
                                            
                    if (bullet.x > 0) {
                            
                        if (bullet.muzzleIndex > 1) {

                            PD::drawBitmap(bullet.x + gameScreenVars.xOffset, bullet.y + gameScreenVars.yOffset, Images::Muzzle[3 - (bullet.muzzleIndex / 2)]);

                        }
                        else {

                            switch (bullet.hitCount) {

                                case 0:
                                    PD::drawBitmap(bullet.x + gameScreenVars.xOffset, bullet.y + gameScreenVars.yOffset, Images::Bullet);
                                    break;

                                default:
                                    PD::drawBitmap(bullet.x + gameScreenVars.xOffset, bullet.y - 5 + gameScreenVars.yOffset, Images::Hit[bullet.hitCount - 1]);
                                    break;

                            }

                        }

                    }

                }


//                 // Render Boss ..

//                 switch (this->gameState) {

//                     case GameState::Game_BossEntering ... GameState::Game_BossLeaving:

//                         PD::drawBitmap(this->boss.x, this->boss.y, Images::Bosses[0]);
// PD::drawRect(boss.x - 1, boss.y + 6, 4, 5 );
// PD::drawRect(boss.x - 1, boss.y + 29, 4, 5 );
// PD::drawRect(boss.x + 5, boss.y + 1, 16, 39 );
//                         break;


//                     default:
//                         break;

//                 }


                // Render boss bullets ..
                
                for (Bullet &bullet : bossBullets.bullets) {
                                            
                    if (bullet.x > 0) {

                        switch (bullet.hitCount) {

                            case 0:
                                PD::drawBitmap(bullet.x + gameScreenVars.xOffset, bullet.y + gameScreenVars.yOffset, Images::BossBullet);
                                break;

                            default:
                                PD::drawBitmap(bullet.x + gameScreenVars.xOffset, bullet.y - 5 + gameScreenVars.yOffset, Images::Hit[bullet.hitCount - 1 + 3]);
                                break;

                        }

                    }

                }


                // Render Boss ..

                switch (this->gameState) {

                    case GameState::Game_BossEntering ... GameState::Game_BossLeaving:

                        PD::drawBitmap(this->boss.x, this->boss.y, Images::Bosses[(PC::frameCount % 32) / 8]);

                        for (uint8_t i = 0; i < 3; i++) {
                            PD::drawBitmap(this->boss.x + (i * 3) + 3, this->boss.y + 7, Images::Boss_Health);
                            PD::drawBitmap(this->boss.x + (i * 3) + 3, this->boss.y + 30, Images::Boss_Health);
                        }

PD::drawRect(boss.x, boss.y + 7, 4, 3 );
PD::drawRect(boss.x, boss.y + 30, 4, 3 );
PD::drawRect(boss.x + 8, boss.y + 1, 18, 38 );
PD::drawRect(boss.x + 2, boss.y + 4, 16, 9 );
PD::drawRect(boss.x + 2, boss.y + 27, 16, 9 );

                        break;


                    default:
                        break;

                }



                // Render the HUD ..

                PD::setColor(0);
                PD::fillRect(88, 81, 22, 7);
                PD::setColor(6);

                uint8_t health_Bar = player.health / Constants::Health_Factor;
                uint8_t digits[5] = {};
                extractDigits(digits, player.score);
                uint8_t location = 106;

                for (uint8_t j = 0; j < 5; ++j, location -= 4) {

                    PD::drawBitmap(location, 82, Images::Numbers[digits[j]]);

                }

                PD::drawBitmap(66, 0, Images::Shield);
                PD::setColor(6);
                PD::drawLine(91, 2, 91 + health_Bar, 2);

            }

            break;

        case GameState::Score:
            {
                PD::setColor(11);
                PD::drawBitmap(3, 16, Images::HighScore);
                PD::setCursor(15, 25);
                PD::print("Your Score  ");

                if (PC::frameCount % 48 < 24 || player.score == 0) {
                    printScore(88, 25, player.score);
                }

                PD::setCursor(15, 38);
                PD::print("Top Scores");

                if (gameScreenVars.scoreIndex != 0 || PC::frameCount % 48 < 24) {
                    this->printScore(88, 38, this->cookie->score[0]);
                }

                if (gameScreenVars.scoreIndex != 1 || PC::frameCount % 48 < 24) {
                    this->printScore(88, 47, this->cookie->score[1]);
                }

                if (gameScreenVars.scoreIndex != 2 || PC::frameCount % 48 < 24) {
                    this->printScore(88, 56, this->cookie->score[2]);
                }

            }

            break;

        default:

            break;

    }




    // Switch states if all enemies / asteroids have left ..

    bool active = false;

    switch (this->gameState) {

        case GameState::Game_EnemyLeaving:

            for (Enemy &enemy : this->enemies) {

                if (enemy.x > Constants::Enemy_OffScreen) {

                    active = true;

                }
                
            }

            if (!active) { this->gameState = GameState::Game_AsteroidLeaving; }

            break;

        case GameState::Game_AsteroidLeaving:

            for (Asteroid &largeAsteroid : largeAsteroids) {

                if (largeAsteroid.active) {

                    switch (largeAsteroid.x) {

                        case Constants::LargeAsteroid_OffScreen + 1 ... 60:
                            active = true;
                            break;

                        case 110 ... 1000:
                            largeAsteroid.active = false;
                            break;
                            
                    }

                }
                
            }

            if (!active) { 

                this->gameState = GameState::Game_BossEntering; 
                this->boss.active = true;
                this->boss.pathCounter = random(0, 20);
                this->boss.x = 110;
                this->boss.y = 10 + Constants::Enemy_Path_Large[boss.pathCounter];
                
            }

            break;

    }

}
