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
Player::Player(sf::RenderWindow *App,MapTile **map):
ImgAnim::ImgAnim(GameConfig::g_imgManag["player"].img,GameConfig::g_imgManag["player"].nbrCollum,GameConfig::g_imgManag["player"].nbrLine)
,m_hp(GameConfig::g_config["starthp"])
,m_app(App)
,m_map(map)
,m_velx(0),m_vely(0),
m_jumpLock(false),m_colBot(false),m_direction(false),m_lookUp(false),m_moving(false)
{
   pause();
}
void Player::MovePlayer(){
    float movHor=0;
    float movVer=0;
    int limitVer=0;
    int limitHor=0;
    float movHorTest=GetVelx()*m_app->GetFrameTime()/1000.f;
    float movVerTest=GetVely()*m_app->GetFrameTime()/1000.f;
    bool bas=false, haut=false, kill=false,block=false;
//    sf::Vector2f bestResult(0,0);
//    if(CollisionGeneral(GetMovedPlayerRect(movHorTest,movVerTest),kill)){
//        if(movHorTest<0)movHor=-1*movHorTest;
//        else movHor=movHorTest;
//
//        if(movVerTest<0)movVer=-1*movVerTest;
//        else movVer=movVerTest;
//        for(float xp=movHor;xp>=0;xp-=0.1){
//            if(movHorTest<0)xp*=-1;
//            for(float yp=movHor;yp>=0;yp-=0.1){
//                if(movVerTest<0)yp*=-1;
//                if(!CollisionGeneral(GetMovedPlayerRect( xp, yp),kill)){
//                    Move(xp,yp);
//                    return;
//                    if( fabs (xp)+ fabs (yp)>fabs(bestResult.x)+ fabs (bestResult.y))
//                        bestResult=sf::Vector2f(xp,yp);
//                }
//                cout<<xp<<" "<<yp<<" - "<<bestResult.x<<" "<<bestResult.y<<endl;
//                if(movVerTest<0)yp*=-1;
//            }
//
//            if(movHorTest<0)xp*=-1;
//        }
//        Move(bestResult.x,bestResult.y);
//        if(bestResult.y<=0.2 && bestResult.y>=0){
//            //SetVely(0);
//            UnlockJump();
//            SetBottomCollision(true);
//        }
//        //exit(0);
//    }
//    else{
//        Move(movHorTest,movVerTest);
//        Gravity();
//    }
    //! --------------------------
    //! On vérifie les collisions horizontals
    if(!CollisionGeneral(GetMovedPlayerRect(movHorTest,0),kill)){//! Pas de collision
        movHor=movHorTest;
    }
    else{//! Sinon on reposition le joueur
        SetVelx(0);
        if(movHorTest>0){
            for(float xp=movHorTest;xp>=0 && !block;xp-=0.01){
                if(!CollisionGeneral(GetMovedPlayerRect(xp,movVerTest),kill)){
                    block=true;
                    movHor=xp;
                }
            }
        }
        else{
            for(float xp=movHorTest;xp<=0 && !block;xp+=0.01){
                if(!CollisionGeneral(GetMovedPlayerRect(xp,movVerTest),kill)){
                    block=true;
                    movHor=xp;
                }
            }
        }
    }
    block=false;
    //! On vérifie les collisions vertical
    if(!CollisionVertical(GetMovedPlayerRect(0,movVerTest),haut,bas)){//! Pas de collision
        Gravity();
        movVer=movVerTest;
    }
    else{//! Sinon on reposition le joueur
        if(haut){//! Si l'on touche le haut
        }
        if(bas){//! Si l'on touche le sol
            if(!GetBottomCollision()){
                if(movVerTest>0){
                    for(float yp=movVerTest;yp>=0 && !block;yp-=0.01){
                        if(!CollisionGeneral(GetMovedPlayerRect(movHor,yp),kill)){
                            block=true;
                            movVer=yp;
                        }
                    }
                }
                else{
                    for(float yp=movVerTest;yp<=0 && !block;yp+=0.01){
                        if(!CollisionGeneral(GetMovedPlayerRect(movHor,yp),kill)){
                            block=true;
                            movVer=yp;
                        }
                    }
                }
            }
            kill=false;
            UnlockJump();
            SetBottomCollision(true);
        }
        SetVely(0);
    }

    //! On vérifie si le mouvement envisagé cause une collision
    if(!CollisionGeneral(GetMovedPlayerRect(movHor,movVer),kill)){
        if(movHor<GameConfig::g_config["tileheight"]&&movVer<GameConfig::g_config["tilewidth"])
                Move(movHor,movVer);
        else SetVely(GameConfig::g_config["tileheight"]/2);
    }
    else{
        cout<<"FFFFFFFFFFFFFFFFUUUUUUUUUUUUUUUU-"<<endl;
       // exit(0);
    }

    //! Ouch!
    if(kill)Degat(200);

}

sf::FloatRect Player::GetPlayerRect(){
    return sf::FloatRect(GetPosition().x,GetPosition().y,GameConfig::g_config["playercollwidth"],GameConfig::g_config["playercollheight"]);
}

sf::FloatRect Player::GetMovedPlayerRect(const float moveX,const float moveY){
  return sf::FloatRect(GetPosition().x+moveX,GetPosition().y+moveY,GameConfig::g_config["playercollwidth"],GameConfig::g_config["playercollheight"]);
}

void Player::Gravity(){
        m_vely+=GameConfig::g_config["gravity"]/1000.f*m_app->GetFrameTime();
}

void Player::Jump(){
    if(!m_jumpLock){
        m_jumpLock=true;
        m_vely+=GameConfig::g_config["jump"];
        SetBottomCollision(false);
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
        m_velx=-50;
    }
    else if(!left&&right){
        m_moving=true;
        m_direction=DROITE;
        m_velx=50;
    }
    else{
        m_moving=false;
        m_velx*=0.8;
    }
    FlipX(m_direction);
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
                    //sf::FloatRect  theTile(x*GameConfig::g_config["tilewidth"],y*GameConfig::g_config["tileheight"],GameConfig::g_config["tilewidth"],GameConfig::g_config["tileheight"]);
                    //if(playerRect.Intersects(theTile)||theTile.Intersects(playerRect))
                    return true;
                }
            }
        }
    }
    return false;
 }
 bool Player::CollisionVertical(const sf::FloatRect playerRect, bool &haut, bool &bas){
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
                            if((*m_map)->Tile(x,y).fall)(*m_map)->m_tileSet.at(x).at(y).touch=true;
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
        if(m_lookUp==HAUT ){
            if(m_moving==BOUGE){
                vely=-162;
                velx=162;
                if(m_direction==GAUCHE){
                    velx=-162;
                }
            }
            else{
                vely=-100;
            }
        }
        else{
            velx=-100;
            if(m_direction==DROITE)velx=100;
        }

        m_listObject->push_back(new GameBullet(GameConfig::GameConfig::g_imgManag["bullet"].img,GameConfig::GameConfig::g_imgManag["bullet"].nbrCollum,GameConfig::GameConfig::g_imgManag["bullet"].nbrLine,10,this,velx,vely,true));
        m_listObject->back()->SetPosition(GetPosition());
        m_listObject->back()->setDelay(0.1);
        m_listObject->back()->SetColor(sf::Color::Red);
        m_lastShot.Reset();
    }
}

void Player::Drawing(){

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
