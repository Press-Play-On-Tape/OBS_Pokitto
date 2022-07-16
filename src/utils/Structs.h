#pragma once

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
    uint16_t score = 0;

    void reset() {

        this->xOffset = 0;
        this->yOffset = 0;
        this->offsetCount = 0;
        this->scoreIndex = 0;
        this->clearScores = 0;
        this->score = 0;
        this->highScoreCounter = 0;

    }

};
