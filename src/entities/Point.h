#pragma once

class Point {

    private:

        int16_t x = 0;
        int16_t y = 0;

    
    public:

        int16_t getX()                          { return this->x; }
        int16_t getY()                          { return this->y; }

        void setX(int16_t val)                  { this->x = val; }
        void setY(int16_t val)                  { this->y = val; }

    
    public:

        void decX()                             { this->x--;}
        void incX()                             { this->x++;}
        void decY()                             { this->y--;}
        void incY()                             { this->y++;}

};
