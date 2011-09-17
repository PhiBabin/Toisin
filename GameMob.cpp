GameMob::GameMob(int type,sf::Vector2f position):
    GameEntity(GameConfig::g_imgManag["mobs"].img,1,1,true)
    ,m_type(type){

    m_colHeight=GameConfig::g_mob[type].height;
    m_colWidth=GameConfig::g_mob[type].width;
    SetPosition(position);
    m_isLoop=false;
}
bool GameMob::isDraw(){
    return true;
}
bool GameMob::isDelete()const{
    return false;
}
bool GameMob::isCollision()const{
    return m_col;
}
bool GameMob::collisionEffect(Player &player){
    return false;
}

GameMob::~GameMob()
{
}
