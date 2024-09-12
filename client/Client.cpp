#include <cstring>
#include <ios>
#include <iostream>
#include <mutex>
#include <thread>
#include "Client.hpp"
#include "Network.hpp"
#include "SDL_events.h"
#include "SDL_render.h"
#include "StreamReceiver.hpp"
#include "Decoder.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

Client::Client() {
    network = new Network(this, 8080);
    receiver = new StreamReceiver(network);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        std::cerr << "Couldn't init SDL: " << SDL_GetError() << std::endl;
        exit(1);
	}

    if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0) {
        std::cerr << "SDL_mixer init error: " << Mix_GetError() << std::endl;
        exit(1);
    }

	window = SDL_CreateWindow("Quack Recv", 10, 40, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_HIDDEN);
	if (!window) {
        std::cerr << "Couldn't create window: " << SDL_GetError() << std::endl;
        exit(1);
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
        std::cerr << "Couldn't create renderer: " << SDL_GetError() << std::endl;
        exit(1);
	}

    recreate_texture(WINDOW_WIDTH, WINDOW_HEIGHT);

    event_sound = SDL_RegisterEvents(2);
    if (event_sound == static_cast<uint32_t>(-1)) {
        std::cerr << "Could not register custom events" << std::endl;
        exit(1);
    }
    event_bg = event_sound + 1;

    m_sounds.resize(SOUND_COUNT);
    m_music.resize(MUSIC_COUNT);
    

    m_sounds[SOUND_LOST_LIFE]= Mix_LoadWAV("media/sounds/elevator.wav");
    if(m_sounds[SOUND_LOST_LIFE] == nullptr) {
        std::cout << "Failed to load sound: " << Mix_GetError() << std::endl;
        exit(1);
    }
    
    m_sounds[SOUND_GAME_LOST] = Mix_LoadWAV("media/sounds/lost_quack.wav");
    if(m_sounds[SOUND_GAME_LOST] == nullptr) {
        std::cout << "Failed to load sound: " << Mix_GetError() << std::endl;
        exit(1);
    }

    m_sounds[SOUND_LEVEL_COMPLETED] = Mix_LoadWAV("media/sounds/next_level.wav");
    if(m_sounds[SOUND_LEVEL_COMPLETED] == nullptr) {
        std::cout << "Failed to load sound: " << Mix_GetError() << std::endl;
        exit(1);
    }

    m_sounds[SOUND_GAME_WON]= Mix_LoadWAV("media/sounds/quackie_og.wav");
    if(m_sounds[SOUND_GAME_WON] == nullptr) {
        std::cout << "Failed to load sound: " << Mix_GetError() << std::endl;
        exit(1);
    }

    m_sounds[SOUND_PICKUP_KEY]= Mix_LoadWAV("media/sounds/key.wav");
    if(m_sounds[SOUND_PICKUP_KEY] == nullptr) {
        std::cout << "Failed to load sound: " << Mix_GetError() << std::endl;
        exit(1);
    }

    m_sounds[SOUND_PICKUP_BLOCK]= Mix_LoadWAV("media/sounds/suck.wav");
    if(m_sounds[SOUND_PICKUP_BLOCK] == nullptr) {
        std::cout << "Failed to load sound: " << Mix_GetError() << std::endl;
        exit(1);
    }

    m_sounds[SOUND_DROP_BLOCK]= Mix_LoadWAV("media/sounds/poop.wav");
    if(m_sounds[SOUND_DROP_BLOCK] == nullptr) {
        std::cout << "Failed to load sound: " << Mix_GetError() << std::endl;
        exit(1);
    }

    m_music[MUSIC_BACKGROUND]= Mix_LoadMUS("media/sounds/background.wav");
    if(m_music[MUSIC_BACKGROUND] == nullptr) {
        std::cout << "Failed to load sound: " << Mix_GetError() << std::endl;
        exit(1);
    }

    // Check if we have a controller added
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            controller = SDL_GameControllerOpen(i);
            break;
        }
    }
}

Client::~Client() {
    delete receiver;
    delete network;

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();

}

void Client::recreate_texture(int width, int height) {
    if (texture && (texture_width != width || texture_height != height)) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    if (!texture) {
        texture_width = width;
        texture_height = height;

        texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_YV12,
            SDL_TEXTUREACCESS_STREAMING,
            texture_width,
            texture_height 
        ); 

        if (!texture) {
            std::cerr
                << "Couldn't create texture: "
                << SDL_GetError()
                << std::endl;
            exit(1);
        }
    }
}

void Client::push_sound_event(Sound sound) {
    SDL_Event event;
    SDL_zero(event);
    event.type = event_sound;
    event.user.code = sound;
    SDL_PushEvent(&event);
}

void Client::push_music_event(Music music) {
    SDL_Event event;
    SDL_zero(event);
    event.type = event_bg;
    event.user.code = music;
    SDL_PushEvent(&event);
}

void Client::start() {
    SDL_ShowWindow(window);
    SDL_Event event;
    bool running = true;
    int joy_xdir = 0;
    std::mutex texture_mutex{};
    DecodedFrame decoded_frame{};
    
	std::thread recv_thread([&]() {
		while (running) {
			receiver->receive_stream(decoded_frame, texture_mutex);
		}
	});


    while (running) {
        // Check if we are receiving packets at all
        network->check_heartbeat();

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.repeat == 0)
                    network->send_keydown(event.key.keysym.sym);
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    network->send_keydown(256 /* glfw escape */);
                    running = false;
                }
                break;
            case SDL_KEYUP:
                network->send_keyup(event.key.keysym.sym);
                break;
            // Following this blogpost:
            // https://blog.rubenwardy.com/2023/01/24/using_sdl_gamecontroller/
            case SDL_CONTROLLERDEVICEADDED:
                if (!controller)
                    controller = SDL_GameControllerOpen(event.cdevice.which);
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                if (controller) {
                    auto j = SDL_GameControllerGetJoystick(controller);
                    auto j_id = SDL_JoystickInstanceID(j);
                    if (event.cdevice.which == j_id) {
                        SDL_GameControllerClose(controller);
                        controller = nullptr;

                        for (int i = 0; i < SDL_NumJoysticks(); i++) {
                            if (SDL_IsGameController(i)) {
                                controller = SDL_GameControllerOpen(i);
                                break;
                            }
                        }
                    }
                }
                break;
            case SDL_CONTROLLERBUTTONDOWN:
                switch (event.cbutton.button) {
                case SDL_CONTROLLER_BUTTON_A:
                    network->send_keydown('w');
                    break;
                case SDL_CONTROLLER_BUTTON_X:
                    network->send_keydown('e');
                    break;
                case SDL_CONTROLLER_BUTTON_B:
                    network->send_keydown(' ');
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                    network->send_keydown('a');
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                    network->send_keydown('d');
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_UP:
                    network->send_keydown('w');
                    break;
                case SDL_CONTROLLER_BUTTON_START:
                    network->send_keydown('r');
                    break;
                case SDL_CONTROLLER_BUTTON_BACK:
                    network->send_keydown(256 /* glfw escape */);
                    running = false;
                    break;
                }
                break;
            case SDL_CONTROLLERBUTTONUP:
                switch (event.cbutton.button) {
                case SDL_CONTROLLER_BUTTON_A:
                    network->send_keyup('w');
                    break;
                case SDL_CONTROLLER_BUTTON_X:
                    network->send_keyup('e');
                    break;
                case SDL_CONTROLLER_BUTTON_B:
                    network->send_keyup(' ');
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                    network->send_keyup('a');
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                    network->send_keyup('d');
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_UP:
                    network->send_keyup('w');
                    break;
                case SDL_CONTROLLER_BUTTON_START:
                    network->send_keyup('r');
                    break;
                }
                break;
            case SDL_CONTROLLERAXISMOTION:
                if (event.caxis.axis == 0) {
                    if (event.caxis.value < -8000) {
                        if (joy_xdir == 1) {
                            network->send_keyup('d');
                            joy_xdir = 0;
                        }

                        if (joy_xdir == 0) {
                            network->send_keydown('a');
                            joy_xdir = -1;
                        }
                    } else if (event.caxis.value > 8000) {
                        if (joy_xdir == -1) {
                            network->send_keyup('a');
                            joy_xdir = 0;
                        }

                        if (joy_xdir == 0) {
                            network->send_keydown('d');
                            joy_xdir = 1;
                        }
                    } else {
                        if (joy_xdir == -1)
                            network->send_keyup('a');
                        else if (joy_xdir == 1)
                            network->send_keyup('d');

                        joy_xdir = 0;
                    }
                }
                break;
            case SDL_USEREVENT: /* sounds */
                Mix_PlayChannel(
                    -1 /* pick nearest available channel */,
                    m_sounds[event.user.code],
                    0 /* don't loop */
                );
                break;
            case SDL_USEREVENT+1: /* music */
                Mix_FadeInMusic(
                    m_music[event.user.code],
                    -1 /* number of loops, as long as possible */,
                    2000
                );
                break;
            default:
                break;
            }
        }

        SDL_RenderClear(renderer);

        {
            std::scoped_lock lock{texture_mutex};
            if (decoded_frame.Yplane != nullptr) {
                recreate_texture(
                    decoded_frame.width,
                    decoded_frame.height
                );

                SDL_UpdateYUVTexture(
                    texture,
                    nullptr, 
                    decoded_frame.Yplane,
                    decoded_frame.Ysize,
                    decoded_frame.Uplane,
                    decoded_frame.Usize,
                    decoded_frame.Vplane,
                    decoded_frame.Vsize
                );
                decoded_frame = {};
            }
        }

        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    // Just exit here so that the thread is killed (we might be waiting
    // for network traffic, so just setting running to false might not
    // really terminate the thread).
    std::exit(0);
}

int main() {
    Client* client = new Client();
    client->start();

    return 0;
}

