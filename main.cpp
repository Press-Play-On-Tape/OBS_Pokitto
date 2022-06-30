#include "Pokitto.h"
#include <LibAudio>
#include "src/utils/GameCookie.h"
#include "src/utils/Enums.h"
#include "src/OBS.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;

Game game;
GameCookie cookie;

Audio::Sink<2, PROJ_AUD_FREQ> audio;

int main(){
    
    cookie.begin("OBS", sizeof(cookie), (char*)&cookie);

    PC::begin();
    PD::loadRGBPalette(palettePico);  
    PD::persistence = true;
    PD::setColor(5);
    PD::setInvisibleColor(14);
    PC::setFrameRate(60);
    PD::setFont(fontTiny);
    PD::textWrap = false;


    // Has the cookie been initialised?

    if (cookie.initialised != COOKIE_INITIALISED) {

        cookie.initialise();

    }

    game.setup(&cookie);
    
    while( PC::isRunning() ){

        if (!PC::update()) continue;
        game.loop();

    }
    
    return 0;
}
