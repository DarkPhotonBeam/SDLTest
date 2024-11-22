#include <iostream>
#include <SDL2/SDL.h>
#include "GameInput.h"
#include <Eigen/Dense>

bool init();

bool loadMedia();

void close();

int pollInput(void *data);

constexpr int SCREEN_WIDTH = 1920;
constexpr int SCREEN_HEIGHT = 1080;

SDL_Window *gWindow = nullptr;
SDL_Renderer *gRenderer = nullptr;

SDL_Surface *gScreenSurface = nullptr;

SDL_Surface *gHelloWorld = nullptr;

Game_Input gInput{};

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                               SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == nullptr) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    //gScreenSurface = SDL_GetWindowSurface( gWindow );

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

    if (gRenderer == nullptr) {
        printf("SDL could not create renderer! SDL_Error: %s\n", SDL_GetError());
    }

    return true;
}

#define IMG_SRC "../hello_world.bmp"

bool loadMedia() {
    gHelloWorld = SDL_LoadBMP(IMG_SRC);
    if (gHelloWorld == nullptr) {
        printf("Unable to load image %s! SDL Error: %s\n", IMG_SRC, SDL_GetError());
        return false;
    }
    return true;
}

void close() {
    SDL_FreeSurface(gHelloWorld);
    gHelloWorld = nullptr;

    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;

    SDL_DestroyRenderer(gRenderer);

    SDL_Quit();
}

int pollInput(void *data) {
    SDL_Event e;
    while (!gInput.quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                printf("SDL QUIT\n");
                gInput.quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                //printf("SDL KEYDOWN\n");
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        gInput.up = true;
                        gInput.yAxis = -1.0;
                        break;
                    case SDLK_DOWN:
                        gInput.down = true;
                        gInput.yAxis = 1.0;
                        break;
                    case SDLK_LEFT:
                        gInput.left = true;
                        gInput.xAxis = -1.0;
                        break;
                    case SDLK_RIGHT:
                        gInput.right = true;
                        gInput.xAxis = 1.0;
                        break;
                    case SDLK_SPACE:
                        gInput.space = true;
                        break;
                    case SDLK_r:
                        gInput.r = true;
                        break;
                    default:
                        printf("Other key\n");
                        break;
                }
            } else if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        gInput.up = false;
                        if (!gInput.down) gInput.yAxis = 0.0;
                        else gInput.yAxis = 1.0;
                        break;
                    case SDLK_DOWN:
                        gInput.down = false;
                        if (!gInput.up) gInput.yAxis = 0.0;
                        else gInput.yAxis = -1.0;
                        break;
                    case SDLK_LEFT:
                        gInput.left = false;
                        if (!gInput.right) gInput.xAxis = 0.0;
                        else gInput.xAxis = 1.0;
                        break;
                    case SDLK_RIGHT:
                        gInput.right = false;
                        if (!gInput.left) gInput.xAxis = 0.0;
                        else gInput.xAxis = -1.0;
                        break;
                    case SDLK_SPACE:
                        gInput.space = false;
                        break;
                    case SDLK_r:
                        gInput.r = false;
                        break;
                    default:
                        break;
                }
            }
        }
    }
    return 0;
}

Uint64 getAvg(std::vector<Uint64> &vec) {
    Uint64 sum = 0;
    for (const unsigned long & it : vec) {
        sum += it;
    }
    const Uint64 avg = sum / vec.size();
    return avg;
}

int main() {
    if (!init()) {
        printf("Failed to initialize!\n");
        return 1;
    }
    if (!loadMedia()) {
        printf("Failed to load media!\n");
        return 1;
    }


    SDL_Thread *inpThread = SDL_CreateThread(&pollInput, "Input Thread", nullptr);
    double xC = SCREEN_WIDTH / 2;
    double yC = SCREEN_HEIGHT / 2;
    SDL_Rect rect = {.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2, .w = 40, .h = 40};
    Uint64 lastTime = SDL_GetTicks64();

    Eigen::Vector2d dir(0, 0);

    bool spaceHit = false;
    bool firstHit = true;
    Uint64 lastHit = SDL_GetTicks64();
    std::vector<Uint64> deltas{};

    while (!gInput.quit) {
        const Uint64 now = SDL_GetTicks64();
        double deltaTime = now - lastTime;
        lastTime = now;

        if (gInput.yAxis != 0 || gInput.xAxis != 0) {
            //printf("DOWN WITH THE SICKNESS\n");
            dir(0) = gInput.xAxis;
            dir(1) = gInput.yAxis;

            dir = dir.normalized();

            yC += dir(1) * deltaTime * 0.6;
            xC += dir(0) * deltaTime * 0.6;
        }

        if (gInput.space && !spaceHit) {
            spaceHit = true;
            Uint64 nows = SDL_GetTicks64();
            Uint64 delta = nows - lastHit;
            lastHit = nows;
            if (firstHit || gInput.r) {
                firstHit = false;
                deltas.clear();
                printf("BPM RESET\n");
            } else {
                deltas.push_back(delta);
                Uint64 avg = getAvg(deltas);
                Uint64 bpm = 60000 / avg;
                printf("BPM: %lu\n", bpm);
            }
        }

        if (spaceHit && !gInput.space) {
            spaceHit = false;
        }

        rect.y = static_cast<int>(yC);
        rect.x = static_cast<int>(xC);

        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
        SDL_RenderClear(gRenderer);

        //SDL_BlitSurface(gHelloWorld, nullptr, gScreenSurface, nullptr);

        SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 255);
        int success = SDL_RenderFillRect(gRenderer, &rect);
        if (!success) {
            //printf("SDL_RenderFillRect failed: %s\n", SDL_GetError());
        }

        SDL_RenderPresent(gRenderer);
        //SDL_UpdateWindowSurface(gWindow);
    }

    SDL_WaitThread(inpThread, nullptr);
    close();
    return 0;
}
