/**
Copyright (C) 2011 babin philippe

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.*/

#include "Player.hpp"
Player::Player(sf::Image &img, MapTile **map):
ImgAnim::ImgAnim(img,3,4)
,m_hp(GameConfig::g_config["starthp"])
,m_velx(0),m_vely(0),
m_jumpLock(false),m_colBot(false),m_direction(false),m_lookUp(false),m_moving(false)
{
    setDelay(0.2);
}

sf::FloatRect Player::GetPlayerRect(){
    return sf::FloatRect(GetPosition().x,GetPosition().y,GameConfig::g_config["playercollwidth"],GameConfig::g_config["playercollheight"]);
}
sf::FloatRect Player::GetMovedPlayerRect(const float moveX,const float moveY){
  return sf::FloatRect(GetPosition().x+moveX,GetPosition().y+moveY,GameConfig::g_config["playercollwidth"],GameConfig::g_config["playercollheight"]);
}
sf::FloatRect Player::GetViewRect(){
   return sf::FloatRect(GetPosition().x-GameConfig::g_config["screenwidth"]/8,GetPosition().y-GameConfig::g_config["screenheight"]/8,GetPosition().x+GameConfig::g_config["screenheight"]/8,GetPosition().y+GameConfig::g_config["screenheight"]/8);
}

void Player::Gravity(sf::RenderWindow &app){
        m_vely+=GameConfig::g_config["gravity"]/1000.f*app.GetFrameTime();
}
void Player::Jump(){
    if(!m_jumpLock){
        m_jumpLock=true;
        m_vely+=GameConfig::g_config["jump"];
        SetBottomCollision(false);
        cout<<"jump"<<endl;
    }
}
void Player::TurnUp(bool up){
    if(up==HAUT)m_lookUp=true;
    else m_lookUp=false;
}
void Player::Turn(bool left, bool right){

    if(left&&!right){
        m_moving=true;
        m_direction=GAUCHE;
        if(m_colBot){
            setAnimRow(1);
        }
        else {
            setAnimRow(3);
        }
        play();
        m_velx=-150;
    }
    else if(!left&&right){
        m_moving=true;
        m_direction=DROITE;
        if(m_colBot)setAnimRow(0);
        else setAnimRow(2);
        play();
        m_velx=150;
    }
    else{
        m_moving=false;
        if(m_colBot&& m_direction==GAUCHE)setAnimRow(1);
        else if(m_colBot&& m_direction==DROITE)setAnimRow(0);
        else if(animRow()<2) setAnimRow(animRow()+2);
        else setAnimRow(animRow());
        stop();
        m_velx*=0.8;
    }
}
 bool Player::CollisionGeneral(const sf::FloatRect playerRect,bool &kill){
    int maxHeight, minHeight, maxWidth, minWidth;
    minHeight=playerRect.Top/GameConfig::g_config["tileheight"];
    minWidth=playerRect.Left/GameConfig::g_config["tilewidth"];
    maxHeight=(playerRect.Top+playerRect.Height-1)/GameConfig::g_config["tileheight"];
    maxWidth=(playerRect.Left+playerRect.Width-1)/GameConfig::g_config["tilewidth"];

    if(minHeight<0)minHeight=0;
    if(maxHeight>(*m_map)->m_height)maxHeight=(*m_map)->m_height;
    if(minWidth<0)minWidth=0;
    if(maxWidth>(*m_map)->m_width)maxWidth=(*m_map)->m_width;
    for(int y=minHeight;y<=maxHeight;y++){
        for(int x=minWidth;x<=maxWidth;x++){
            if(!(x>=(*m_map)->m_width or y>=(*m_map)->m_height)){
                if((*m_map)->Tile(x,y).kill)kill=true;
                if((*m_map)->Tile(x,y).solid){
                    sf::FloatRect  theTile(x*GameConfig::g_config["tilewidth"],y*GameConfig::g_config["tileheight"],GameConfig::g_config["tilewidth"],GameConfig::g_config["tileheight"]);
                    if(playerRect.Intersects(theTile)||theTile.Intersects(playerRect)) return true;
                }
            }
        }
    }
    return false;
 }
 bool Player::CollisionVertical(const sf::FloatRect playerRect, bool &haut, bool &bas,int &solidLimit){
    int maxHeight, minHeight, maxWidth, minWidth;
    bool CollisionVertical=false;
    minHeight=playerRect.Top/GameConfig::g_config["tileheight"];
    minWidth=playerRect.Left/GameConfig::g_config["tilewidth"];
    maxHeight=(playerRect.Top+playerRect.Height-1)/GameConfig::g_config["tileheight"];
    maxWidth=(playerRect.Left+playerRect.Width-1)/GameConfig::g_config["tilewidth"];

    if(minHeight<0)minHeight=0;
    if(maxHeight>(*m_map)->m_height)maxHeight=(*m_map)->m_height;
    if(minWidth<0)minWidth=0;
    if(maxWidth>(*m_map)->m_width)maxWidth=(*m_map)->m_width;
    for(int y=minHeight;y<=maxHeight;y++){
        for(int x=minWidth;x<=maxWidth;x++){
            if(!(x>=(*m_map)->m_width or y>=(*m_map)->m_height)){
                if((*m_map)->Tile(x,y).solid){
                    sf::FloatRect  theTile(x*GameConfig::g_config["tilewidth"],y*GameConfig::g_config["tileheight"],GameConfig::g_config["tilewidth"],GameConfig::g_config["tileheight"]);
                    if(playerRect.Intersects(theTile)||theTile.Intersects(playerRect)){
                        CollisionVertical=true;
                        if(y*GameConfig::g_config["tileheight"]<=playerRect.Top+playerRect.Height&&y*GameConfig::g_config["tileheight"]>=playerRect.Top){
                            bas=true;
                            solidLimit=y;
                        }
                        if((y+1)*GameConfig::g_config["tileheight"]>=playerRect.Top&&(y+1)*GameConfig::g_config["tileheight"]<=playerRect.Top+playerRect.Height){
                            haut=true;
                        }
                    }
                }
            }
        }
    }
    return CollisionVertical;
 }
 bool Player::CollisionHorizontal(const sf::FloatRect playerRect, bool &gauche, bool &droite,int &solidLimit){
    int maxHeight, minHeight, maxWidth, minWidth;
    bool CollisionHorizontal=false;
    minHeight=playerRect.Top/GameConfig::g_config["tileheight"];
    minWidth=playerRect.Left/GameConfig::g_config["tilewidth"];
    maxHeight=(playerRect.Top+playerRect.Height-1)/GameConfig::g_config["tileheight"];
    maxWidth=(playerRect.Left+playerRect.Width-1)/GameConfig::g_config["tilewidth"];

    if(minHeight<0)minHeight=0;
    if(maxHeight>(*m_map)->m_height)maxHeight=(*m_map)->m_height;
    if(minWidth<0)minWidth=0;
    if(maxWidth>(*m_map)->m_width)maxWidth=(*m_map)->m_width;
    for(int y=minHeight;y<=maxHeight;y++){
        for(int x=minWidth;x<=maxWidth;x++){
            if(!(x>=(*m_map)->m_width or y>=(*m_map)->m_height)&&(*m_map)->Tile(x,y).solid){
                sf::FloatRect  theTile(x*GameConfig::g_config["tilewidth"],y*GameConfig::g_config["tileheight"],GameConfig::g_config["tilewidth"],GameConfig::g_config["tileheight"]);
                if(playerRect.Intersects(theTile)||theTile.Intersects(playerRect)){
                    CollisionHorizontal= true;
                    if(x*GameConfig::g_config["tilewidth"]>=playerRect.Left&&x*GameConfig::g_config["tilewidth"]<=playerRect.Left+playerRect.Width){
                        droite=true;
                        solidLimit=x;
                    }
                    if((x+1)*GameConfig::g_config["tilewidth"]<=playerRect.Left+playerRect.Width&&(x+1)*GameConfig::g_config["tilewidth"]>=playerRect.Left){
                        gauche=true;
                        solidLimit=x;
                    }
                }
            }
        }
    }
    return CollisionHorizontal;
 }
void Player::SetMapObject(vector<GameEntity*> *listObject){
    m_listObject=listObject;
}

void Player::Degat(int degats){
    m_hp-=degats;
}
int Player::GetHp(){
    return m_hp;
}
void Player::SetHp(int nhp){
    m_hp=nhp;
}
bool Player::IsDead(){
    if(m_hp<=0){
        m_hp=GameConfig::g_config["starthp"];
        return true;
    }
    else return false;
}
float Player::GetVelx(){
    return m_velx;
}
float Player::GetVely(){
    return m_vely;
}
void Player::SetVelx(float nx){
    m_velx=nx;
}
void Player::SetVely(float ny){
    m_vely=ny;
}
void Player::SetBottomCollision(bool is){
    m_colBot=is;
}
bool Player::GetBottomCollision() const{
    return m_colBot;
}
void Player::UnlockJump(){
    m_jumpLock=false;
    m_vely=0;
}
void Player::Shoot(){
    if(m_lastShot.GetElapsedTime()/1000.f>0.4){
        float velx=0,vely=0;
        int rotation=0;
        if(m_lookUp==HAUT ){
            if(m_moving==BOUGE){
                rotation=-45;
                vely=-162;
                velx=162;
                if(m_direction==GAUCHE){
                    rotation=45;
                    velx=-162;
                }
            }
            else{
                rotation=-90;
                vely=-250;
            }
        }
        else{
            velx=-250;
            if(m_direction==DROITE)velx=250;
        }

        m_listObject->push_back(new GameBullet(GameConfig::GameConfig::g_imgManag["fire"].img,GameConfig::GameConfig::g_imgManag["fire"].nbrCollum,GameConfig::GameConfig::g_imgManag["fire"].nbrLine,10,true,this,velx,vely));
        m_listObject->back()->SetPosition(GetPosition());
        m_listObject->back()->setDelay(0.1);
        if(!(m_lookUp==HAUT && m_moving==IMMOBILE))m_listObject->back()->FlipX(m_direction);
        else m_listObject->back()->FlipX(false);
        m_listObject->back()->SetRotation(rotation);
        m_lastShot.Reset();
    }
}

void Player::Drawing(sf::RenderWindow* app){

}
void Player::Pause(){
    m_lastShot.Pause();
    m_hurt.Pause();
}
void Player::Resume(){
    m_lastShot.Play();
    m_hurt.Play();
}
Player::~Player(){
}
