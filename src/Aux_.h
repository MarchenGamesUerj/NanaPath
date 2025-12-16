#include "Global.h"

int AUX_WaitEventTimeoutCount(SDL_Event* evt, Uint32* ms) {
    Uint32 antes = SDL_GetTicks();
    int isevt = SDL_WaitEventTimeout(evt, *ms);

    if (isevt) {
        Uint32 espera = SDL_GetTicks() - antes;
        if (espera >= *ms) *ms = 0;
        else *ms -= espera;
    }
    else *ms = TIMEOUT;

    return isevt;
}