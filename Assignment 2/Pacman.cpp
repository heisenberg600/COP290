#include "Button.cpp"
#include "Server.cpp"
#include "Client.cpp"




class Pacman: public Game, public TextureSet{
    private:
    Player *Thanos = new Player(&player1Tex);
    Player *ThanosPast = new Player(&player2Tex);
    bool type=true;

    Enemy *Blinky = new Enemy(&blinkyTex, "blinky");
    Enemy *Pinky = new Enemy(&pinkyTex, "pinky");
    Enemy *Inky  = new Enemy(&inkyTex, "inky");
    Enemy *Clyde = new Enemy(&clydeTex, "clyde");

    Map *map = NULL; 

    PlayMode mode = Single;

    SDL_Rect life1 = {0,0,20,20};
    SDL_Rect life2 = {20,0,20,20};
    SDL_Rect optionsRect[3] = {{0,0,50,50},{50,0,50,50},{100,0,50,50}};
    SDL_Rect backRect = {0,0,WIN_WIDTH,WIN_HEIGHT};

    bool selected = false;
    bool paused = false;

    bool escDown = false;

    Uint32 pauseStart = 0;
    int pauseTime = 0;

    string balStr(int n) {
        string out = to_string(n);

        while (out.size() != 4) {
            out = "0" + out;
        } 
        return out;
    }
    Server s;
    Client c;

    Button *pauseBtn[3];

    public:

    SDL_Event event;


    void init() {

        selected = false;
        paused = false;

        escDown = false;

        const char* temp = WIN_NAME.c_str();
        Game::init(temp,SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,WIN_WIDTH,WIN_HEIGHT);
        if (isRunning) {

            loadTex(IMAGES_PATH);
            map = new Map(MAPS_PATH + "/map2.txt");
            map->genrateMap();

            Thanos->setBounds(map->getHeight(),map->getWidth());
            Thanos->setInitTile(map->getPlayerInit());


            Blinky->init(map);
            Pinky->init(map);
            Inky->init(map);
            Clyde->init(map);

            mainMenu();
        }

    }

    void initMode() {

        if (mode == Doffline) {
            ThanosPast->setBounds(map->getHeight(),map->getWidth());
            ThanosPast->setInitTile(map->getPlayerInit());

            Inky->setID("blinky");
            Clyde->setID("pinky");
        }
        if (mode == Donline){
            if (type){
                s.innit();
            }
            else {
                c.innit();
            }
            ThanosPast->setBounds(map->getHeight(),map->getWidth());
            ThanosPast->setInitTile(map->getPlayerInit());

            Inky->setID("blinky");
            Clyde->setID("pinky");
        }

        Blinky->setOffset(SDL_GetTicks()/1000);
        Inky->setOffset(SDL_GetTicks()/1000);
        Pinky->setOffset(SDL_GetTicks()/1000);
        Clyde->setOffset(SDL_GetTicks()/1000);
    }


    void render(){

        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderClear(renderer);

        Uint32 start = SDL_GetTicks();

        renderBack();

        Blinky->render(); 
        Pinky->render();      
        Inky->render();
        Clyde->render();
        Thanos->render();

        if (mode == Donline || mode == Doffline) {
            ThanosPast->render();
        }

        if (paused) {
            renderPause();
        }


        SDL_RenderPresent(renderer);

        int spend = SDL_GetTicks() - start;

        while (spend < SPEED*FRAME_DELAY/(60*35)) {
            spend = SDL_GetTicks() - start;

            eventManager(&event);
        }
    }

    void renderPause() {
        pauseTex.renderWM(100,300);

        for (int i=0; i<3; i++) {
            pauseBtn[i]->handleEventWT(&event,&optionsRect[i]);
        }

    }

    void renderBack() {

        int x,y;
        for (int j=0; j < map->getHeight(); j++) {

            for (int i=0; i< map->getWidth(); i++) {
                Tile *currTile = map->getTile(i,j);
                if ((currTile != NULL)) {
                    x = TILE_WIDTH*i;
                    y = TILE_HEIGHT*j;

                    if (currTile->getBrick()) {
                        brickTex.render(x,y);
                    }

                    else if (currTile->isCoin) {
                        coinTex.render(x,y);
                    }
                }
            }
        }
        scoreBackTex.renderWM(5,10);
        renderScore2(Thanos->getScore(), 85,60);

        for (int i=0; i< Thanos->getLifeLeft(); i++) {
            playerShow.renderWM(40*i +200,45,&life1);
        }

        if (mode ==Doffline || mode == Donline) {
            scoreBackTex.renderWM(WIN_WIDTH - (scoreBackTex.getWidth() +5) ,10, NULL, SDL_FLIP_HORIZONTAL);
            renderScore2(ThanosPast->getScore(), WIN_WIDTH - 110,60);

            for (int i=0; i< Thanos->getLifeLeft(); i++) {
                playerShow.renderWM(WIN_WIDTH- (40*i +200),47,&life2);
            }
        }
    }

    void update() {

        if (!paused) {

            setMovement();
            Blinky->update(map, Thanos,NULL);
            Pinky->update(map, Thanos,Blinky);

            if (mode ==Single) {
                Inky->update(map, Thanos,Blinky);
                Clyde->update(map, Thanos,Blinky);
            }
            else if (mode == Doffline|| mode == Donline) {
                Inky->update(map, ThanosPast,NULL);
                Clyde->update(map, ThanosPast,Inky);
            }

            // this we've to look upon.....pausing 1 tile before!!(maybe have to review when to call p->die())

            if (Thanos->justDied) {
                Thanos->justDied = false;

                Uint32 start = SDL_GetTicks();

                int spend = SDL_GetTicks() - start;

                while (spend < 3500) {
                    spend = SDL_GetTicks() - start;

                    eventManager(&event);

                    if (!isRunning){break;}
                }
            }

        }

        else {
            pauseTime = SDL_GetTicks() - pauseStart;


            if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE && escDown) {

                for (int i=0; i<3; i++) {
                    pauseBtn[i]->~Button();
                }

                paused = false;
                escDown = false;
                
            }

            else if (event.type == SDL_KEYDOWN && type && event.key.keysym.sym == SDLK_ESCAPE) {
                escDown = true;
            }

            if (pauseBtn[0]->isSelected()) {
                for (int i=0; i<3; i++) {
                    pauseBtn[i]->~Button();
                }

                paused = false;
                escDown = false;
                pauseBtn[0]->deselect();
            }

            else if (pauseBtn[1]->isSelected()){
                //restart
                pauseBtn[1]->deselect();
            }

            else if (pauseBtn[2]->isSelected()) {

                pauseBtn[2]->deselect();

                for (int i=0; i<3; i++) {
                    pauseBtn[i]->~Button();
                }

                isRunning = false;
            }

        }
        
    }

    void clean() {
        freeTex();

        Thanos->~Player();
        ThanosPast->~Player();

        map->~Map();

        Blinky->~Enemy();
        Pinky->~Enemy();
        Inky->~Enemy();
        Clyde->~Enemy();

        Game::clean();
    }


    void setMovement() {
        if (event.type == SDL_KEYUP && type && event.key.keysym.sym == SDLK_ESCAPE && escDown) {
            paused = true;
            pauseStart = SDL_GetTicks();
            pauseTime = 0;
            escDown = false;

            for (int i=0; i< 3; i++) {
                pauseBtn[i] = new Button(100*i + 300,500,&optionTex);
                pauseBtn[i]->setDimen(50,50);
            }
        }

        if (event.type == SDL_KEYDOWN && type) {

            switch (event.key.keysym.sym){
            
            case SDLK_ESCAPE:
                escDown = true;
                break;

            case SDLK_UP:
                s.sendMessage("up");
                Thanos->update(MOVE_UP,map);           
                break;

            case SDLK_DOWN:
                s.sendMessage("down");
                Thanos->update(MOVE_DOWN,map);                
                break;

            case SDLK_RIGHT:
                s.sendMessage("right");
                Thanos->update(MOVE_RIGHT,map);
                break;

            case SDLK_LEFT:
                s.sendMessage("left");
                Thanos->update(MOVE_LEFT,map);              
                break;

            case SDLK_w:
                if (mode ==Doffline) {
                    ThanosPast->update(MOVE_UP,map);
                }           
                break;

            case SDLK_s:
                if (mode ==Doffline) {
                    ThanosPast->update(MOVE_DOWN,map);
                }                
                break;

            case SDLK_d:
                if (mode ==Doffline) {
                    ThanosPast->update(MOVE_RIGHT,map);
                }
                break;

            case SDLK_a:
                if (mode ==Doffline) {
                    ThanosPast->update(MOVE_LEFT,map);
                }              
                break;
            }
            if (c.recieveMessage()=="up"){
                ThanosPast->update(MOVE_UP,map);                 
            }
            else if (c.recieveMessage()=="down"){
                ThanosPast->update(MOVE_DOWN,map);                 
            }
            else if (c.recieveMessage()=="right"){
                ThanosPast->update(MOVE_RIGHT,map);                 
            }
            else if (c.recieveMessage()=="left"){
                ThanosPast->update(MOVE_LEFT,map);                 
            }
        }
        else if (event.type == SDL_KEYDOWN && !type) {

            switch (event.key.keysym.sym){
            case SDLK_UP:
                c.sendMessage("up");
                ThanosPast->update(MOVE_UP,map);           
                break;

            case SDLK_DOWN:
                c.sendMessage("down");
                ThanosPast->update(MOVE_DOWN,map);                
                break;

            case SDLK_RIGHT:
                c.sendMessage("right");
                ThanosPast->update(MOVE_RIGHT,map);
                break;

            case SDLK_LEFT:
                c.sendMessage("left");
                ThanosPast->update(MOVE_LEFT,map);              
                break;
            }
            if (s.recieveMessage()=="up"){
                Thanos->update(MOVE_UP,map);                 
            }
            else if (c.recieveMessage()=="down"){
                Thanos->update(MOVE_DOWN,map);                 
            }
            else if (c.recieveMessage()=="right"){
                Thanos->update(MOVE_RIGHT,map);                 
            }
            else if (c.recieveMessage()=="left"){
                Thanos->update(MOVE_LEFT,map);                 
            }
        }
    }
    
    void mainMenu() {
        
        Uint32 start;
        int spend;

        int backCurr = 0;
        int sparkCurr = 225;
        int sciCurr = 0;
        int base = movableBG.getWidth();

        Button *menuText[3];
        string names[3] = {"Single Player [s]", "Two Player [d]", "Online [o]"};

        for (int i=0; i< 3; i++) {
            menuText[i] = new Button(150,75*i + 325,&modeTex);
            loadWord(menuText[i]->getFont(), names[i]);
        }
        

        while (isRunning && !selected) {

            start = SDL_GetTicks();

            eventManager(&event);

            SDL_SetRenderDrawColor(renderer, 0,0,0,255);
            SDL_RenderClear(renderer);



            movableBG.renderWM(0,0,&backRect);
            backCurr += 1;
            if (backCurr == base) {
                backCurr = 0;
            }
            
            backRect.x = backCurr;

            logoTex.renderWM(220,100);
            gameTex.renderWM((WIN_WIDTH-gameTex.getWidth())/2, WIN_HEIGHT-gameTex.getHeight());

            sciTex.Load(IMAGES_PATH + "/sci/" + balStr(sciCurr) + ".png");
            sciTex.renderWM(500,300);

            sparkTex.Load(IMAGES_PATH + "/spark/" + balStr(sparkCurr) + ".png");
            sparkTex.renderWM(275,595);

            if (backCurr%10 ==0 ){

                sciCurr = (sciCurr + 1)%71;
                sparkCurr = sparkCurr%51 + 225;
            }

            for (int i=0; i<3; i++) {
                menuText[i]->handleEvent(&event);
            }

            SDL_RenderPresent(renderer);


            selected = selecMenu(menuText);

            while (FRAME_DELAY > spend) {
                
                eventManager(&event);

                spend = SDL_GetTicks() - start;
            }
        }

        for (int i=0; i<3; i++) {
            menuText[i]->~Button();
        }
        initMode();
    }

    bool selecMenu(Button** menuText) {

        if (menuText[0]->isSelected()){
            mode = Single;
            menuText[0]->deselect();
            return true;
        }

        else if (menuText[1]->isSelected()){
            mode = Doffline;
            menuText[1]->deselect();
            return true;
        }

        else if (menuText[2]->isSelected()){
            mode = Donline;
            menuText[2]->deselect();
            return true;
        }

        if (event.type == SDL_KEYDOWN) {

            switch (event.key.keysym.sym){
            case SDLK_s:
                mode = Single;           
                return true;

            case SDLK_d:
                mode = Doffline;           
                return true;

            case SDLK_o:
                mode = Donline;           
                return true;

            case SDLK_c:
                mode=Donline;
                type=false;
                return true;
            }
        }

        return false;
    }

};