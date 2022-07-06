class PlayableCharacter : public Entity {
public:
    float SPEED = 48;
    float size = 80;

    bool isMoving = false;

    std::string facingDirection = "r";
};