#include "../include/SDLTexture/SDLTexture.h"

SDLTexture::~SDLTexture()
{ if (this->texture) SDL_DestroyTexture(this->texture); }
