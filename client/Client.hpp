#pragma once
#include <cstdint>
#include <vector>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_gamecontroller.h>
#include "NetworkDefines.hpp"

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;
class StreamReceiver;
class Network;

class Client {
private:
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
    SDL_GameController* controller = nullptr;
    int texture_width{};
    int texture_height{};
    uint32_t event_sound;
    uint32_t event_bg;

    Network* network;
    StreamReceiver* receiver;
    std::vector<Mix_Music*> m_music;
    std::vector<Mix_Chunk*> m_sounds;

    void recreate_texture(int width, int height);

public:
    Client();
    ~Client();

    void start();

    void push_sound_event(Sound sound);
    void push_music_event(Music music);
};
