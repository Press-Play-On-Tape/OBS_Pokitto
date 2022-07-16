#pragma once

#include "Point.h"

class Asteroid : public Point {

    private:

        uint8_t type = 0;
        bool active = false;
    
    public:

        uint8_t getType()                       { return this->type; }
        bool getActive()                        { return this->active; }

        void setType(uint8_t val)               { this->type = val; }
        void setActive(bool val)                { this->active = val; }

        Rect getLargeAsteroidRect() {

            Rect rect = { this->getX() + 1, this->getY() + 1, 17, 17 };
            return rect;

        }

        Rect getSmallAsteroidRect() {

            Rect rect = { this->getX() + 1, this->getY() + 1, 17, 17 };
            return rect;

        }

};