#include "OBS.h"
#include "sounds/Sounds.h"
#include "utils/Utils.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;

const int8_t xOffsets[] = { -2, 1, 2, 0 };
const int8_t yOffsets[] = { 0, -2, 1, 2 };

void Game::game_Init() {

    for (Asteroid &smallAsteroid : smallAsteroids) {

        smallAsteroid.setX(random(110, 256));
        smallAsteroid.setY(random(0, 80));
        smallAsteroid.setActive(true);
        
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
                // Launch a boss ?

                if (gameState == GameState::Game && this->gameScreenVars.distance > 0 && (this->gameScreenVars.distance % 100 == 0) ||  PC::buttons.pressed(BTN_C) || PC::buttons.repeat(BTN_C, 1)) {

                    gameState = GameState::Game_EnemyLeaving;
                    this->playTheme(Theme::Boss);

                    for (Enemy &enemy : this->enemies) {

                        if (enemy.getX() > 110) {

                            enemy.setActive(false);
                            enemy.setX(Constants::Enemy_OffScreen);

                        }
                        
                    }

                    for (Asteroid &largeAsteroid : largeAsteroids) {

                        if (largeAsteroid.getX() > 110) {

                            largeAsteroid.setActive(false);
                                
                        }
                        
                    }

                    this->boss.reset();

                }


                // Increase score based on distance travelled ..

                if (PC::frameCount % Constants::ScoreDistance == 0) {

                    this->gameScreenVars.score++;
                    this->gameScreenVars.distance++;

                }

                player.setY(player.getY() + static_cast<int8_t>(player.getDirection()));

                if (player.getDirection() == Direction::Up && player.getY() == 0) {
                    player.setDirection(Direction::None);
                }

                if (player.getDirection() == Direction::Down && player.getY() == 78) {
                    player.setDirection(Direction::None);
                }

                if (PC::buttons.pressed(BTN_A) || PC::buttons.pressed(BTN_B) || PC::buttons.pressed(BTN_LEFT) || PC::buttons.pressed(BTN_RIGHT) || PC::buttons.pressed(BTN_UP) || PC::buttons.pressed(BTN_DOWN)) {
                    
                    uint8_t bulletIdx = bullets.getInactiveBullet();

                    if (bulletIdx != Constants::Bullet_None) {

                        Bullet &bullet = bullets.bullets[bulletIdx];
                        bullet.setActive(true);

                        switch (player.getDirection()) {

                            case Direction::Up:
                                player.setDirection(Direction::Down);
                                break;

                            case Direction::Down:
                                player.setDirection(Direction::Up);
                                break;

                            default:

                                switch (player.getY()) {

                                    case 0:
                                        player.setDirection(Direction::Down);
                                        break;

                                    case 78:
                                        player.setDirection(Direction::Up);
                                        break;

                                    default:
                                        if (random(0,2) == 0) {
                                            player.setDirection(Direction::Up);
                                        }
                                        else {
                                            player.setDirection(Direction::Down);
                                        }
                                        break;

                                }

                                break;
                                
                        }

                        bullet.setX(24);
                        bullet.setY(player.getY() + 2);
                        bullet.setMuzzleIndex(8);
                        bullet.setHitObject(HitObject::None);

                        #ifdef SOUNDS
                            playSoundEffect(SoundEffect::Laser);
                        #endif

                    }

                }


                // Player bullets ..

                for (Bullet &bullet : bullets.bullets) {
                        
                    if (bullet.getHitCount() > 0) {

                        bullet.incHitCount();

                        switch (bullet.getHitObject()) {

                            case HitObject::BossBullet:

                                if (bullet.getHitCount() > 7) {
                                    bullet.reset();
                                }

                                break;

                            default:

                                if (bullet.getHitCount() > 3) {
                                    bullet.reset();
                                }

                                break;

                        }

                    }

                    if (bullet.getX() > 0 && bullet.getHitCount() == 0) {

                        if (bullet.getMuzzleIndex() > 0) {

                            bullet.decMuzzleIndex();

                        }
                        else {

                            bullet.setX(bullet.getX() + 4);

                            if (bullet.getX() >= 110) {

                                bullet.setActive(false);

                            }

                        }

                        if (bullet.getActive()) checkBulletCollision(bullet);

                    }

                    if (bullet.getHitObject() == HitObject::LargeAsteroid) {

                        bullet.setX(bullet.getX() - 1);

                    }

                }


                // Boss bullets ..

                for (Bullet &bullet : bossBullets.bullets) {
// printf("%i,%i(%i) ",bullet.x,bullet.y, bullet.active);             

                    if (bullet.getActive()) {
                            
                        if (bullet.getHitCount() > 0) {

                            bullet.incHitCount();

                            if (bullet.getHitCount() > 3) {
                                bullet.reset();
                            }

                        }

                        if (bullet.getX() > -4 && bullet.getHitCount() == 0) {

                            switch (bullet.getDirection()) {

                                case Direction::UpLeft:
                                    bullet.setX(bullet.getX() - 2);
                                    bullet.setY(bullet.getY() - 1);
                                    break;

                                case Direction::Left:
                                    bullet.setX(bullet.getX() - 2);
                                    break;

                                case Direction::DownLeft:
                                    bullet.setX(bullet.getX() - 2);
                                    bullet.setY(bullet.getY() + 1);
                                    break;

                            }

                            if (bullet.getX() <= -4 || bullet.getY() < -6 || bullet.getY() > 88) {

                                bullet.setActive(false);

                            }

                        }

                        if (bullet.getActive()) checkBossBulletCollision(bullet);

                    }

                }
// printf("\n");                        



                // Has the player hit a asteroid?

                bool collision = false;
                Rect playerRect = player.getRect();

                for (Asteroid &largeAsteroid : largeAsteroids) {

                    if (largeAsteroid.getActive()) {
                            
                        Rect asteroidRect = largeAsteroid.getLargeAsteroidRect();

                        if (collide(playerRect, asteroidRect)) {

                            if (player.getHealth() > 0)  {

                                player.decHealth(2);

                                #ifdef SOUNDS
                                    playSoundEffect(SoundEffect::Player_Hit);
                                #endif                            
                                
                                if (player.getHealth() == 0) {

                                    player.setExplodeCounter(21);

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

                        Rect enemyRect = enemy.getRect();

                        if (collide(playerRect, enemyRect)) {

                            if (player.getHealth() > 0)  {

                                player.decHealth(1);

                                #ifdef SOUNDS
                                    playSoundEffect(SoundEffect::Mini_Explosion);
                                #endif
                            
                                if (player.getHealth() == 0) {
                                    player.setExplodeCounter(21);

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
                        this->gameScreenVars.score = 0;
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

// printf("%i\n", this->gameState);
    switch (gameState) {

        case GameState::Game ... GameState::Game_BossLeaving:
            {

                // Render player ..

                if (player.getHealth() > 0 || player.getExplodeCounter() > 16) {

                    PD::drawBitmap(9 + gameScreenVars.xOffset, player.getY() + gameScreenVars.yOffset, Images::PlayerShip);
                    PD::drawBitmap(gameScreenVars.xOffset, player.getY() + 3 + gameScreenVars.yOffset, Images::ShipParticle[PC::frameCount % 8 < 4]);

                }
                
                if (player.getExplodeCounter() > 0) {

                    PD::drawBitmap(6, player.getY() + gameScreenVars.yOffset, Images::Puffs[(21 - player.getExplodeCounter()) / 3]);

                }
                
                if (player.updateExplosion()) {
                
                    gameState = GameState::Score;
                    gameScreenVars.highScoreCounter = 64;
                    gameScreenVars.scoreIndex = cookie->saveScore(this->gameScreenVars.score);
                    this->muteTheme();

                }


                // Render player bullets ..
                
                for (Bullet &bullet : bullets.bullets) {
                                            
                    if (bullet.getX() > 0) {
                            
                        if (bullet.getMuzzleIndex() > 1) {

                            PD::drawBitmap(bullet.getX() + gameScreenVars.xOffset, bullet.getY() + gameScreenVars.yOffset, Images::Muzzle[3 - (bullet.getMuzzleIndex() / 2)]);

                        }
                        else {

                            switch (bullet.getHitCount()) {

                                case 0:
                                    PD::drawBitmap(bullet.getX() + gameScreenVars.xOffset, bullet.getY() + gameScreenVars.yOffset, Images::Bullet);
                                    break;

                                default:

                                    switch (bullet.getHitObject()) {

                                        case HitObject::BossBullet:
printf("Hit bossbullet %i %i - %i\n",bullet.getX() + gameScreenVars.xOffset, bullet.getY() - 5 + gameScreenVars.yOffset,bullet.getHitCount());
                                            PD::drawBitmap(bullet.getX() + gameScreenVars.xOffset, bullet.getY() - 5 + gameScreenVars.yOffset, Images::Hit360[bullet.getHitCount() - 1]);
                                            break;

                                        default:
                                            PD::drawBitmap(bullet.getX() + gameScreenVars.xOffset, bullet.getY() - 5 + gameScreenVars.yOffset, Images::Hit[bullet.getHitCount() - 1]);
                                            break;

                                    }

                                    break;

                            }

                        }

                    }

                }


                // Render boss bullets ..
                
                for (Bullet &bullet : bossBullets.bullets) {
                                            
                    if (bullet.getX() > 0) {

                        switch (bullet.getHitCount()) {

                            case 0:
                                PD::drawBitmap(bullet.getX() + gameScreenVars.xOffset, bullet.getY() + gameScreenVars.yOffset, Images::BossBullet);
                                break;

                            default:
                                PD::drawBitmap(bullet.getX() + gameScreenVars.xOffset, bullet.getY() - 5 + gameScreenVars.yOffset, Images::Hit[bullet.getHitCount() - 1 + 3]);
                                break;

                        }

                    }

                }


                // Render Boss ..

                bool renderInRed = false;

                if (this->boss.getExplodeCounter(ExplodeType::Body) == 3 || this->boss.getExplodeCounter(ExplodeType::Body) == 4)                           renderInRed = true;
                if (this->boss.getExplodeCounter(ExplodeType::TopHand) > 0 && (this->boss.getExplodeCounter(ExplodeType::TopHand) / 3) % 2 == 0)            renderInRed = true;
                if (this->boss.getExplodeCounter(ExplodeType::BottomHand) > 0 && (this->boss.getExplodeCounter(ExplodeType::BottomHand) / 3) % 2 == 0)      renderInRed = true;

                switch (this->gameState) {

                    case GameState::Game_BossEntering ... GameState::Game_BossLeaving:

                        if (!renderInRed) {
                            PD::drawBitmap(this->boss.getX(), this->boss.getY(), Images::Bosses[(PC::frameCount % 32) / 8]);
                        }
                        else {
                            PD::drawBitmap(this->boss.getX(), this->boss.getY(), Images::Boss_04_Red);
                        }

                        for (uint8_t i = 1; i < 4; i++) {

                            if (this->boss.getTopHealth() >= i) {
                                PD::drawBitmap(this->boss.getX() - (i * 3) + 12, this->boss.getY() + 7, Images::Boss_Health);
                            }

                            if (this->boss.getBottomHealth() >= i) {
                                PD::drawBitmap(this->boss.getX() - (i * 3) + 12, this->boss.getY() + 30, Images::Boss_Health);
                            }

                        }

                        if (this->boss.getExplodeCounter(ExplodeType::TopHand) > 0) {

                            PD::drawBitmap(this->boss.getX() + gameScreenVars.xOffset - 2, this->boss.getY() + gameScreenVars.yOffset + 2, Images::Puffs[(21 - this->boss.getExplodeCounter(ExplodeType::TopHand)) / 3]);
                            this->boss.decExplodeCounter(ExplodeType::TopHand);

                        }

                        if (this->boss.getExplodeCounter(ExplodeType::BottomHand) > 0) {

                            PD::drawBitmap(this->boss.getX() + gameScreenVars.xOffset - 2, this->boss.getY() + gameScreenVars.yOffset + 25, Images::Puffs[(21 - this->boss.getExplodeCounter(ExplodeType::BottomHand)) / 3]);
                            this->boss.decExplodeCounter(ExplodeType::BottomHand);

                        }

                        if (this->boss.getExplodeCounter(ExplodeType::Body) > 0) {

                            PD::drawBitmap(this->boss.getX() + this->boss.getExplodePoint().getX() + gameScreenVars.xOffset, 
                                           this->boss.getY() + this->boss.getExplodePoint().getY() - 5 + gameScreenVars.yOffset, 
                                           Images::Hit[(this->boss.getExplodeCounter(ExplodeType::Body) - 1) / 3]);
// printf("hit %i %i\n",this->boss.getExplodeCounter(ExplodeType::Body), (this->boss.getExplodeCounter(ExplodeType::Body) - 1) / 3 );
                            this->boss.decExplodeCounter(ExplodeType::Body);
                        
                        }

// PD::drawRect(this->boss.x, this->boss.y + 7, 4, 3 );
// PD::drawRect(this->boss.x, this->boss.y + 30, 4, 3 );
// PD::drawRect(this->boss.x + 8, this->boss.y + 1, 18, 38 );
// PD::drawRect(this->boss.x + 2, this->boss.y + 4, 16, 9 );
// PD::drawRect(this->boss.x + 2, this->boss.y + 27, 16, 9 );

                        break;


                    default:
                        break;

                }



                // Render the HUD ..

                PD::setColor(0);
                PD::fillRect(88, 81, 22, 7);
                PD::setColor(6);

                uint8_t health_Bar = player.getHealth() / Constants::Health_Factor;
                uint8_t digits[5] = {};
                extractDigits(digits, this->gameScreenVars.score);
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

                if (PC::frameCount % 48 < 24 || this->gameScreenVars.score == 0) {
                    printScore(88, 25, this->gameScreenVars.score);
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

                if (enemy.getActive()) {

                    active = true;

                }
                
            }

            if (PC::frameCount % 64 <= 32) { PD::drawBitmap(23, 37, Images::BossWarning); }

            if (!active) { this->gameState = GameState::Game_AsteroidLeaving; }

            break;

        case GameState::Game_AsteroidLeaving:

            for (Asteroid &largeAsteroid : largeAsteroids) {

                if (largeAsteroid.getActive()) {

                    switch (largeAsteroid.getX()) {

                        case Constants::LargeAsteroid_OffScreen + 1 ... 19:
                            break;

                        case 20 ... 109:
                            active = true;
                            break;

                        case 110 ... 1000:
                            largeAsteroid.setActive(false);
                            largeAsteroid.setX(Constants::LargeAsteroid_OffScreen);
                            break;
                            
                    }

                }
                
            }

            if (!active) { 

                this->gameState = GameState::Game_BossEntering; 
                this->boss.setActive(true);
                this->boss.setPathCounter(random(0, 20));
                this->boss.setX(110);
                this->boss.setY(10 + Constants::Enemy_Path_Large[this->boss.getPathCounter()]);
                
            }

            if (PC::frameCount % 64 <= 32) { PD::drawBitmap(23, 37, Images::BossWarning); }

            break;

        case GameState::Game_BossEntering:

            if (this->boss.getActive() > 90 && PC::frameCount % 64 <= 32) { PD::drawBitmap(23, 37, Images::BossWarning); }

            break;

        case GameState::Game_BossLeaving:

            if (this->boss.getX() == 110) {

                this->gameState = GameState::Game;
                this->muteTheme();
                this->boss.setActive(false);

            }

            break;

    }

}
