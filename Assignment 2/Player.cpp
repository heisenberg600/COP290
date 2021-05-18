#include "Gamer.cpp"



class Player : public Gamer{

private:

    int score = 0;
    int maxLife = MAX_LIFE;
    int lifeLeft = MAX_LIFE;

    bool alive = true;
    Stone stone = NONE;

public: 

    bool justDied = false;


    Player(Texture *playerTex){
        texture = playerTex;
        sprite = new Sprites(playerTex);
        score = 0;
        lifeLeft = MAX_LIFE;
    }

    ~Player() {
        free();
    }

    void init(int height, int width, Tile* initialTile) {
        Gamer::setBounds(height,width);
        Gamer::setInitTile(initialTile);
        score = 0;
        lifeLeft = MAX_LIFE;
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

    void movement(Move dir,Map *map) {
        int currX = currTile->getX();
        int currY = currTile->getY();

        int nextX = currX;
        int nextY = currY;

        switch (dir){
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
            
            else if (nextTile->getBrick() == false) {
                initRel();
                currTile = nextTile;
            }

            nextTile = NULL;
            animating = true;
        }
        
    }

    void update(Move dir, Map *map) {

        if (visible && !animating) {
            movement(dir, map);
        }
    }

    void render() {
        if (visible) {
            Gamer::render();
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

        justDied = true;
    }


};