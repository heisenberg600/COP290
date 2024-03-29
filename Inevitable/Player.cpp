#include "Gamer.cpp"


class Player : public Gamer{

private:

    int score = 0;
    int maxLife = MAX_LIFE;
    int lifeLeft = MAX_LIFE;

    bool alive = true;
    Stone stone = NONE;
    Move moveDir = STATIC;

public: 


    Player(Texture *playerTex){
        texture = playerTex;
        sprite = new Sprites(playerTex);
        score = 0;
        lifeLeft = MAX_LIFE;
        xRel=0;
        yRel=0;
    }

    ~Player() {
        free();
    }

    void init(int height, int width, Tile* initialTile) {
        Gamer::setBounds(height,width);
        Gamer::setInitTile(initialTile);
        score = 0;
        lifeLeft = MAX_LIFE;
        moveDir = STATIC;
    }

    void setStone(Stone s){stone =s;}
    Stone getStone(){return stone;}
    void setMaxLife(int max) {maxLife = max;}
    int getScore() {return score;}
    int getLifeLeft() {return lifeLeft;}
    bool isAlive() {return alive;}
    int getCurr() {
        if (nextUD == 0) {
            return nextLR;
        }

        else {
            return nextUD;
        }
    }
    Tile* getcurrTile(){
        return currTile;
    }

    void updateScore(int a){
        score+=a;
    }

    void movement(Map *map) {
        int currX = currTile->getX();
        int currY = currTile->getY();

        int nextX = currX;
        int nextY = currY;

        switch (moveDir){
        case MOVE_UP:

            nextY -= 1; 
            nextUD = UP;
            handleBound(nextY, yBound);                                   
            break;

        case MOVE_DOWN:
            
            nextY += 1;
            nextUD = DOWN;
            handleBound(nextY, yBound);                    
            break;

        case MOVE_RIGHT:
            
            nextX += 1;
            nextLR = RIGHT;
            nextUD = FRONT;
            handleBound(nextX, xBound);                   
            break;

        case MOVE_LEFT:
            
            nextX -= 1;
            nextLR = LEFT;
            nextUD = FRONT;
            handleBound(nextX, xBound);                    
            break;
        }
        nextTile = map->getTile(nextX,nextY);

        if (nextTile != NULL) {

            if (nextTile->isCoin) {
                score+= 1;
                COINS_LEFT -= 1;
                sound->playCoin();
                nextTile->isCoin = false;

                initRel();
                currTile = nextTile;
            }

            else if (nextTile->isMind) {
                sound->playMind();
                nextTile->isMind = false;
                stoneTime=SDL_GetTicks()/1000;
                setStone(MIND);

                initRel();
                currTile = nextTile;
            }
            
            else if (nextTile->getBrick() == false) {
                initRel();
                currTile = nextTile;
            }

            nextTile = NULL;
            animating = true;
            moveDir = STATIC;
        }
        
    }

    void checkStone(){
        if (stone==MIND && SDL_GetTicks()/1000-stoneTime>POWER_TIME){
            setStone(NONE);
        }
    }
    void update(Move dir) {

        moveDir = dir;
    }

    void render() {
        if (visible) {
            checkStone();
            Gamer::render();
        }
    }

    void updateM(Map *map) {
        if (moveDir != STATIC && visible && !animating) {
            movement( map);
        }
    }

    void die() {

        if (lifeLeft < 0 ) {
            alive = false;
            visible = false;
        }
        else {
            lifeLeft -=1;
        }
        sound->playDie();
        currTile = initTile;
        currUD = FRONT;
        currLR = LEFT;
        animating  = false;
    }


};