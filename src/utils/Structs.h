#pragma once

struct Point {

    int16_t x = 0;
    int16_t y = 0;
    
};

struct SplashScreenVars {

    uint8_t counter = 0;

};

struct TitleScreenVars {

    uint8_t counter = 0;
    TitleMode mode = TitleMode::OBS;
    uint8_t panel = 0;
    uint8_t index = 0;
    bool skipTypewriter = false;

    void reset() {

        this->mode = TitleMode::OBS;
        this->counter = 0;
        this->panel = 0;
        this->index = 0;
        this->skipTypewriter = false;

    }

};

struct GameScreenVars {

    int8_t xOffset = 0;
    int8_t yOffset = 0;
    uint8_t offsetCount = 0;
    uint8_t scoreIndex = 0;
    uint8_t clearScores = 0;
    uint8_t highScoreCounter = 0;

    void reset() {

        this->xOffset = 0;
        this->yOffset = 0;
        this->offsetCount = 0;
        this->scoreIndex = 0;
        this->clearScores = 0;
        this->highScoreCounter = 0;

    }

};

struct Asteroid : public Point {

    uint8_t type = 0;
    bool active = false;
    
};

struct Bullet : public Point {

    uint8_t hitCount = 0;
    uint8_t muzzleIndex = 0;
    bool active = true;
    Direction direction = Direction::Down;
    
    HitObject hitObject = HitObject::None;
    
    void reset() {
        this->x = -1;
        this->hitCount = 0;
        this->hitObject = HitObject::None;
        this->active = false;
    }

};

struct Bullets {

    Bullet bullets[Constants::Bullet_Count];
    
    void reset() {

        for (Bullet &bullet : this->bullets) {

            bullet.reset();

        }

    }

    uint8_t getInactiveBullet() {

        for (uint8_t i = 0; i < Constants::Bullet_Count; i++) {

            Bullet &bullet = this->bullets[i];

            if (!bullet.active) {

                return i;

            }

        }

        return Constants::Bullet_None;
    }

};


struct BossBullets {

    Bullet bullets[Constants::Boss_Bullet_Count];
    
    void reset() {

        for (Bullet &bullet : this->bullets) {

            bullet.reset();

        }

    }

    uint8_t getInactiveBullet() {

        for (uint8_t i = 0; i < Constants::Boss_Bullet_Count; i++) {

            Bullet &bullet = this->bullets[i];

            if (!bullet.active) {

                return i;

            }

        }

        return Constants::Bullet_None;

    }

};

struct Enemy : public Point {

    bool active = false;
    Path path = Path::Small;
    Motion motion = Motion::Slow;

    uint8_t pathCount = 0; 
    uint8_t yOffset = 0;         
    uint8_t explodeCounter = 0;
        
    bool updateExplosion() {

        if (this->explodeCounter > 0) {

            this->explodeCounter--;

            if (this->explodeCounter == 0) {

                this->explodeCounter = 0;
                return true;

            }

        }

        return false;

    }

    bool getActive() {

        return this->active;

    }

};

struct Boss : public Point {

    bool active = false;
    uint8_t y = 0;         
    uint8_t x = 0;         
    uint8_t pathCounter = 0;         
    uint8_t explodeCounter = 0;

    uint8_t topHealth = 3;
    uint8_t bottomHealth = 3;
    bool fireFromTop = false;
        
    bool updateExplosion() {

        if (this->explodeCounter > 0) {

            this->explodeCounter--;

            if (this->explodeCounter == 0) {

                this->explodeCounter = 0;
                return true;

            }

        }

        return false;

    }

    bool getActive() {

        return this->active;

    }

};

struct Player {

    uint8_t y = 26;
    uint16_t score = 0;

    uint16_t health = (17 * Constants::Health_Factor) - 1;
    uint8_t explodeCounter = 0;

    Direction direction = Direction::None;

    bool updateExplosion() {

        if (this->explodeCounter > 0) {

            this->explodeCounter--;

            if (this->explodeCounter == 0) {

                this->explodeCounter = 0;
                return true;

            }

        }

        return false;

    }

    void reset() {

        this->y = 26;
        this->score = 0;
        this->direction = Direction::None;
        this->health = (17 * Constants::Health_Factor) - 1;
        this->explodeCounter = 0;

    }

};

struct Rect {

   int16_t x;      
   int16_t y;      
   uint8_t width;  
   uint8_t height; 

   Rect() = default;
  
   constexpr Rect(int16_t x, int16_t y, uint8_t width, uint8_t height)
     : x(x), y(y), width(width), height(height)
   {   }

 };