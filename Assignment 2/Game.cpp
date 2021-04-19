//Using SDL and standard IO
#include <algorithm>
#include <SDL2/SDL.h>
#include <iostream>

using namespace std;

class Game
{
protected:
    bool isRunning; 
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

public:
    Game(){
    }
    ~Game(){
    }

    void init(const char* title, int xpos, int ypos, int width, int height){
        if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
            cout<< "Game Subsystem could not initialize! Error: "<<  SDL_GetError() << endl;
            isRunning = false;
        }

        else{
            cout<< "Game Subsystem initialize!"<< endl;

            //Create window
            window = SDL_CreateWindow( title, xpos, ypos, width, height, SDL_WINDOW_SHOWN );

            if( window == NULL ){
                cout << "Game Window could not be created! Error: "<< SDL_GetError() << endl;
                isRunning = false;
            }

            else{
                cout<< "Game Window created!"<< endl;

                renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

                if (renderer == NULL) {
                    cout << "Renderer could not be created! Error: "<< SDL_GetError() << endl;
                    isRunning = false;
                }
            

                else {
                    cout << "Renderer created!" << endl;
                    isRunning = true;
                }
            }
        }
    }


    void clean(){
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        cout << "Game closed" << endl;
    }

    void eventManager(SDL_Event *event){
        SDL_PollEvent(event);

        switch(event->type) {
            case SDL_QUIT :
                isRunning = false;
                break;
            default:
                break;
        }
    }

    bool isOn(){
        return isRunning;
    }

};

