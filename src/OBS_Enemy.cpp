#include "OBS.h"
#include "sounds/Sounds.h"
#include "utils/Utils.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;


void Game::moveEnemies() {


    // Move and render enemies ..

    for (Enemy &enemy : enemies) {

        switch (enemy.motion) {

            case Motion::Slow:

                if (PC::frameCount % 2 == 0) {

                    switch (enemy.path) {

                        case Path::Small:

                            enemy.pathCount++;
                            if (enemy.pathCount == 36) enemy.pathCount = 0;

                            enemy.x--;                    
                            enemy.y = Constants::Enemy_Path_Small[enemy.pathCount];
                            break;

                        case Path::Medium:

                            enemy.pathCount++;
                            if (enemy.pathCount == 70) enemy.pathCount = 0;

                            enemy.x--;                    
                            enemy.y = Constants::Enemy_Path_Medium[enemy.pathCount];
                            break;

                        case Path::Large:

                            enemy.pathCount++;
                            if (enemy.pathCount == 91) enemy.pathCount = 0;

                            enemy.x--;                    
                            enemy.y = Constants::Enemy_Path_Large[enemy.pathCount] + enemy.yOffset;
                            break;

                        default:
                            break;

                    }

                }

                break;


            case Motion::Fast:

                if (PC::frameCount % 3 < 2) {

                    switch (enemy.path) {

                        case Path::Small:

                            enemy.pathCount++;
                            if (enemy.pathCount == 36) enemy.pathCount = 0;

                            enemy.x--;                    
                            enemy.y = Constants::Enemy_Path_Small[enemy.pathCount] + enemy.yOffset;
                            break;

                        case Path::Medium:

                            enemy.pathCount++;
                            if (enemy.pathCount == 70) enemy.pathCount = 0;

                            enemy.x--;                    
                            enemy.y = Constants::Enemy_Path_Medium[enemy.pathCount] + enemy.yOffset;
                            break;

                        case Path::Large:

                            enemy.pathCount++;
                            if (enemy.pathCount == 91) enemy.pathCount = 0;

                            enemy.x--;                    
                            enemy.y = Constants::Enemy_Path_Large[enemy.pathCount];
                            break;

                        default:
                            break;

                    }

                }

                break;

        }


        if (enemy.x == Constants::Enemy_OffScreen) {

            launchEnemy(enemy);

        }

        if (enemy.getActive() || enemy.explodeCounter > 16) {

            PD::drawBitmap(enemy.x + gameScreenVars.xOffset, enemy.y + gameScreenVars.yOffset, Images::Enemy);

        }

        if (enemy.explodeCounter > 0) {

            PD::drawBitmap(enemy.x +gameScreenVars. xOffset - 3, enemy.y + gameScreenVars.yOffset, Images::Puffs[(21 - enemy.explodeCounter) / 3]);

        }
        
        if (enemy.updateExplosion()) {
        
            launchEnemy(enemy);

        }

    }

}