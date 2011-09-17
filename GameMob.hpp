#ifndef GAMEMOB_HPP_INCLUDED
#define GAMEMOB_HPP_INCLUDED


class GameMob: public GameEntity{
    public:
        GameMob(int type,sf::Vector2f position);
        virtual bool isDraw();
        virtual bool isDelete()const;
        virtual bool isCollision()const;
        virtual bool collisionEffect(Player &player);
        virtual ~GameMob();
    private:
        int m_type;
};


#endif // GAMEMOB_HPP_INCLUDED
