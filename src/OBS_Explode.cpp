#include "OBS.h"

#include "sounds/Sounds.h"
#include "utils/Utils.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;

float Game::getRandomFloat(int8_t min, int8_t max) {

    return min + ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * (max - min));

}

void Game::explode(int16_t x, int16_t y, ExplosionSize explosionSize, ExplosionColor explosionColor) {

    //uint8_t particleIdx = this->particles.getInactiveParticle();


    // One non-moving particle to hide the object being destroyed ..

    // if (particleIdx != Constants::Particle_None) {

    //     Part &particle = this->particles.particles[particleIdx];

    //     particle.setX(expX);
    //     particle.setY(expY);
    //     particle.setDeltaX(0);
    //     particle.setDeltaY(0);
    //     particle.setAge(0);
    //     particle.setSize(10);
    //     particle.maxAge=0;
    //     particle.isBlue=isBlue;
    //     particle.setActive(true);

    //     isBlue = !isBlue;

    // }

    /* 
    for (uint8_t i = 0; i < 30; i++) {

        uint8_t particleIdx = this->particles.getInactiveParticle();

        if (particleIdx != Constants::Particle_None) {

            Part &particle = this->particles.particles[particleIdx];

            particle.setX(expX);
            particle.setY(expY);
            particle.setDeltaX(this->getRandomFloat(-3, 3));
            particle.setDeltaY(this->getRandomFloat(-3, 3));
            particle.setAge(random(0, 2));
            particle.setMaxAge(10 + random(0, 10));
            particle.setSize(this->getRandomFloat(1, 5));
            particle.isBlue=isBlue;
            particle.setActive(true);

            isBlue = !isBlue;

        }

    }
      
    for (uint8_t i = 0; i < 15; i++) {

        uint8_t particleIdx = this->particles.getInactiveParticle();

        if (particleIdx != Constants::Particle_None) {

            Part &particle = this->particles.particles[particleIdx];

            particle.setX(expX);
            particle.setY(expY);            
            particle.setDeltaX(this->getRandomFloat(-8, 8));
            particle.setDeltaY(this->getRandomFloat(-8, 8));
            particle.setAge(random(0, 2));
            particle.setMaxAge(10 + random(0, 10));
            particle.setSize(this->getRandomFloat(1, 5));
            particle.isBlue=isBlue;
            particle.setActive(true);

            isBlue = !isBlue;

        }

    }
    */

    bool isBlue = false;

    const uint8_t sizeMax[] = { 3, 4, 8 };
    const uint8_t deltaMax[] = { 4, 6, 8 };
    const uint8_t ageMax[] = { 10, 7, 12 };
    const uint8_t particlesToLaunch[] = { 20, 30, 60 };
    const uint8_t shockwaveRadius[] = { 20, 25, 35 };

    for (uint8_t i = 0; i < particlesToLaunch[static_cast<uint8_t>(explosionSize)]; i++) {

        uint8_t particleIdx = this->particles.getInactiveParticle();

        if (particleIdx != Constants::Particle_None) {

            Part &particle = this->particles.particles[particleIdx];

            particle.setX(x);
            particle.setY(y);
            particle.setDeltaX(this->getRandomFloat(-deltaMax[static_cast<uint8_t>(explosionSize)], deltaMax[static_cast<uint8_t>(explosionSize)]));
            particle.setDeltaY(this->getRandomFloat(-deltaMax[static_cast<uint8_t>(explosionSize)], deltaMax[static_cast<uint8_t>(explosionSize)]));
            particle.setAge(random(0, 2));
            particle.setMaxAge(ageMax[static_cast<uint8_t>(explosionSize)] + random(0, ageMax[static_cast<uint8_t>(explosionSize)]));
            particle.setColor(explosionColor);
            particle.setActive(true);

            if (i > particlesToLaunch[static_cast<uint8_t>(explosionSize)] /2) {

                particle.setSize(1);

            }
            else {

                particle.setSize(this->getRandomFloat(1, sizeMax[static_cast<uint8_t>(explosionSize)]));

            }

        }

    }
      
    // for (uint8_t i = 0; i < 15; i++) {

    //     uint8_t particleIdx = this->particles.getInactiveParticle();

    //     if (particleIdx != Constants::Particle_None) {

    //         Part &particle = this->particles.particles[particleIdx];

    //         particle.setX(expX);
    //         particle.setY(expY);            
    //         particle.setDeltaX(this->getRandomFloat(-8, 8));
    //         particle.setDeltaY(this->getRandomFloat(-8, 8));
    //         particle.setAge(random(0, 2));
    //         particle.setMaxAge(10 + random(0, 10));
    //         particle.setSize(this->getRandomFloat(1, 5));
    //         particle.isBlue=isBlue;
    //         particle.setActive(true);

    //         isBlue = !isBlue;

    //     }

    // }

    createShockwave(x, y, shockwaveRadius[static_cast<uint8_t>(explosionSize)]);
    
}

// void Game::smol_shwave(int16_t shx, int16_t shy) {

//     uint8_t shwaveIdx = this->shwaves.getInactiveShwave();

//     if (shwaveIdx != Constants::Shwave_None) {

//         Shwave &shwave = this->shwaves.shwaves[shwaveIdx];

//         shwave.setX(shx);
//         shwave.setY(shy);
//         shwave.r=3;
//         shwave.tr=6;
//         shwave.col=9;
//         shwave.speed=1;
//         shwave.setActive(true);
        
//     }

// }

void Game::createShockwave(int16_t x, int16_t y, uint8_t maxRadius) {

    uint8_t shockwaveIdx = this->shockwaves.getInactiveShwave();

    if (shockwaveIdx != Constants::Shockwave_None) {

        Shockwave &shockwave = this->shockwaves.shockwaves[shockwaveIdx];

        shockwave.setX(x);
        shockwave.setY(y);
        shockwave.setRadius(3);
        shockwave.setMaxRadius(maxRadius);
        shockwave.setSpeed(3);
        shockwave.setActive(true);
        
    }

}

// void Game::smol_spark(int16_t sx, int16_t sy) {

//     uint8_t particleIdx = this->particles.getInactiveParticle();

//     if (particleIdx != Constants::Particle_None) {

//         Part &particle = this->particles.particles[particleIdx];

//         particle.setX(sx);
//         particle.setY(sy);
//         particle.setDeltaX(random(0,17) - 8);
//         particle.setDeltaY(-random(0, 3));
//         particle.setAge(random(0, 2));
//         particle.setMaxAge(10 + random(0, 10));
//         particle.setSize(this->getRandomFloat(1, 5));
//         particle.isBlue=false;
//         particle.spark=true;
//         particle.setActive(true);
 
//     }

// }

void Game::renderShockwave() {

    for (Shockwave &shockwave : this->shockwaves.shockwaves) {

        if (shockwave.getActive()) {

            uint8_t color = 7 - ((shockwave.getRadius() + 1) / (shockwave.getMaxRadius() / 3));
            if (color == 4) color = 5;

            PD::setColor(color);
            PD::drawCircle(shockwave.getX(), shockwave.getY(), shockwave.getRadius());        

            shockwave.setRadius(shockwave.getRadius() + shockwave.getSpeed());

            if (shockwave.getRadius() > shockwave.getMaxRadius()) {

                shockwave.setActive(false);

            }

        }

    }
 
}

void Game::renderParticles() {

    for (Part &particle : this->particles.particles) {

        if (particle.getActive()) {

            uint8_t color = 7;

            switch (particle.getColor()) {

                case ExplosionColor::Blue:
                    color = this->getBlueShade(particle.getAge());
                    break;

                case ExplosionColor::Red:
                    color = this->getRedShade(particle.getAge());
                    break;

            }
            
            if (static_cast<uint8_t>(particle.getSize()) <= 1) {

                PD::setColor(color);
                PD::drawPixel(particle.getX(), particle.getY());

            }
            else {

                PD::setColor(color);
                PD::fillCircle(particle.getX(), particle.getY(), static_cast<uint8_t>(particle.getSize()));

            }


            particle.setX(particle.getX() + particle.getDeltaX());
            particle.setY(particle.getY() + particle.getDeltaY());

            particle.setDeltaX(particle.getDeltaX() * 0.65);
            particle.setDeltaY(particle.getDeltaY() * 0.65);
            particle.incAge();

            if (particle.getAge() > particle.getMaxAge()) {

                particle.setSize(particle.getSize() - 0.5);

            }

            if (particle.getSize() < 0) {

                particle.setActive(false);

            }
  
        }

    }  

}

uint8_t Game::getRedShade(uint8_t page) {

    switch (page) {

        case 6 ... 7:
            return 10;

        case 8 ... 10:
            return 9;

        case 11 ... 12:
            return 8;

        case 13 ... 15:
            return 2;

        case 16 ... 255:
            return 5;

        default:
            return 7;
    }

}

uint8_t Game::getBlueShade(uint8_t page) {

    switch (page) {

        case 6 ... 7:
            return 6;

        case 8 ... 10:
            return 12;

        case 11 ... 12:
            return 13;

        case 13 ... 15:
            return 1;

        case 16 ... 255:
            return 1;

        default:
            return 7;

    }

}
