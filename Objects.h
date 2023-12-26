//IDS Objetos
enum IDS{PLAYER, GROUND};

//Avatar do jogador
struct Character
{
    int ID;
    int x;
    int y;
    int lives;
    int speed;
    int bx;
    int by;
    int score;
};

//Chão
struct Ground
{
    int ID;
    int x;
    int y;
    int bx;
    int by;
    int speed;
    bool live;
    int cor;
};
