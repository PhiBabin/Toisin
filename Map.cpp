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

#include "Map.hpp"
MapTile::MapTile():m_app(NULL),m_playerOne(NULL){}
MapTile::MapTile(sf::RenderWindow *App, Player *playerOne):
m_width(0),m_height(0),m_app(App),m_playerOne(playerOne){

    LoadMap();
}

 bool MapTile::CollisionTile(float x, float y){
    return m_tileSet.at(x).at(y).solid;
 }

Type MapTile::Tile(float x, float y){
    return m_tileSet.at(x).at(y);
 }

 vector<GameEntity*> * MapTile::GetMapEntity(){
    return &m_mapEntity;
 }
 sf::Vector2i MapTile::GetPlateau(){
        return m_currentPlateau;
 }

 bool MapTile::CollisionGeneral(const sf::FloatRect playerRect){
    int maxHeight, minHeight, maxWidth, minWidth;
    minHeight=playerRect.Top/GameConfig::g_config["tileheight"];
    minWidth=playerRect.Left/GameConfig::g_config["tilewidth"];
    maxHeight=(playerRect.Top+playerRect.Height-1)/GameConfig::g_config["tileheight"];
    maxWidth=(playerRect.Left+playerRect.Width-1)/GameConfig::g_config["tilewidth"];

    if(minHeight<0)minHeight=0;
    if(maxHeight>m_height)maxHeight=m_height;
    if(minWidth<0)minWidth=0;
    if(maxWidth>m_width)maxWidth=m_width;
    for(int y=minHeight;y<=maxHeight;y++){
        for(int x=minWidth;x<=maxWidth;x++){
            if(!(x>=m_width or y>=m_height)){
                if(m_tileSet[x][y].solid){
                    sf::FloatRect  theTile(x*GameConfig::g_config["tilewidth"],y*GameConfig::g_config["tileheight"],GameConfig::g_config["tilewidth"],GameConfig::g_config["tileheight"]);
                    if(playerRect.Intersects(theTile)||theTile.Intersects(playerRect)) return true;
                }
            }
        }
    }
    return false;
 }

void MapTile::Draw(){
    cout<<m_currentPlateau.x<<" "<<m_currentPlateau.y<</*"FPS="<<1.f/(m_app->GetFrameTime())*1000<<*/"Joueur 1 x="<<m_playerOne->GetPosition().x<<" y="<<m_playerOne->GetPosition().y<<" vely="<<m_playerOne->GetVely()<<" velx="<<m_playerOne->GetVelx()<<endl;

    m_currentPlateau= sf::Vector2i(m_playerOne->GetPosition().x/(GameConfig::g_config["platwidth"]*GameConfig::g_config["tilewidth"]),
                        m_playerOne->GetPosition().y/(GameConfig::g_config["platheight"]*GameConfig::g_config["tilewidth"]));
    //! On affiche les tiles du background
    m_app->Draw(sf::Sprite(m_background.GetTexture()));
    //! On affiche la map
    m_app->Draw(sf::Sprite(m_map.GetTexture()));

    //! On affiche le personnage et ces éléments
    m_app->Draw(*m_playerOne);
    m_playerOne->Drawing(m_app);
    //! On affiche les objets de la carte
    for(unsigned int i=0;i<m_mapEntity.size();i++){
        if((m_mapEntity.at(i))->isDelete()){
            delete m_mapEntity.at(i);
            m_mapEntity.erase( m_mapEntity.begin() + i );
        }
        else{
            m_app->Draw(*(m_mapEntity.at(i)));
        }
    }
}
vector<Type> & MapTile::operator [] (int X){
    return m_tileSet.at(X);
}
 unsigned char MapTile::FindType(sf::Color Pix){
        for(unsigned char it=0;it<m_typeList.size();it++){
            if(m_typeList[it].colorPix==Pix) return it;
        }
        cerr<<"[Fatal Error]Impossible de trouver un type correspondant."<<endl;
        exit(0);
        return 0;
 }
void MapTile::LoadMap(){
    map<string,string> levelConfig;
	int typeSpawn1;
	const int tilewidth =GameConfig::g_config["tilewidth"];
	const int tileheight =GameConfig::g_config["tileheight"];

    //! On charge la configuration du niveau
    stringstream ss;
    ss<<GameConfig::g_config["level"];
    string pathConfig="map/level"+ ss.str()+".tmx";


    //! Initiation des textures temporaire
    sf::Texture m_ImgTypeTile;
    sf::Texture image_schemaImg;
    m_ImgTypeTile.LoadFromFile("map/level"+ ss.str()+"/tileset.png");

    TiXmlDocument doc(pathConfig.c_str());
    doc.LoadFile();

    TiXmlHandle hRoot(&doc);
    TiXmlHandle hDoc(0);
    TiXmlElement* pElem;
    TiXmlElement* elemProp;
    hDoc=TiXmlHandle(hRoot.FirstChildElement().Element());

    //! On change les dimensions de la map
    m_width=atoi(hRoot.FirstChild("map").Element()->Attribute("width"));
    m_height=atoi(hRoot.FirstChild("map").Element()->Attribute("height"));

    //! On crée une liste de tile aux bonnes dimensions
    m_typeList.empty();
    m_typeList.resize((m_ImgTypeTile.GetWidth())/ tilewidth*(m_ImgTypeTile.GetHeight()/ tileheight));

    //! On charge le tileset
    pElem=hDoc.FirstChild("tileset").FirstChild().Element()->NextSiblingElement();
    for(; pElem; pElem=pElem->NextSiblingElement()){
        int id=atoi(pElem->Attribute("id"));
        int x=id%(m_ImgTypeTile.GetWidth()/ tilewidth);
        int y=(id-id%(m_ImgTypeTile.GetWidth()/tilewidth))/(m_ImgTypeTile.GetWidth()/tilewidth);

        Type newTile;
        newTile.visible=false;newTile.kill=false;newTile.solid=true;
        newTile.tile.SetTexture(m_ImgTypeTile);
        newTile.zoneRect=sf::IntRect(x*tilewidth, y*tileheight, tilewidth, tileheight);
        cout<<id<<" - "<<x<<" , "<<y<<" - "<<newTile.zoneRect.Left<<" "<<newTile.zoneRect.Top<<endl;
        elemProp=TiXmlHandle(pElem).FirstChild("properties").FirstChild().Element();
        for(; elemProp; elemProp=elemProp->NextSiblingElement()){
            if(string(elemProp->Attribute("name"))=="visible"&& atoi(elemProp->Attribute("value"))==1){
                newTile.visible=true;
            }

            if(string(elemProp->Attribute("name"))=="kill"&& atoi(elemProp->Attribute("value"))==1){
                newTile.kill=true;
            }

            if(string(elemProp->Attribute("name"))=="solid"&& atoi(elemProp->Attribute("value"))==0){
                newTile.solid=false;
            }
        }
        m_typeList[id]=newTile;
    }
//    tilesetImg.LoadFromFile(levelConfig["mappath"]);
//    backImg.LoadFromFile(levelConfig["backpath"]);
/////    backbackImg.LoadFromFile(levelConfig["backbackpath"]);
/////    sf::Sprite backback(backbackImg);
/////    backback.SetPosition(backbackCoor);
//    //! On supprime les vectors
//	m_typeList.erase(m_typeList.begin(),m_typeList.end());
//	m_tileSet.erase(m_tileSet.begin(),m_tileSet.end());
//    image_schemaImg.LoadFromFile(levelConfig["corrpath"]);
//    //! Initiation des images des tiles
//    m_ImgTypeTile.LoadFromFile(levelConfig["tilepath"]);
//    m_width=tilesetImg.GetWidth();
//    m_height=tilesetImg.GetHeight();

//    //! On charge les items
//    pElem=hDoc.FirstChild("items").FirstChild().Element();
//    for(; pElem; pElem=pElem->NextSiblingElement()){
//        int itemX =atoi(pElem->Attribute("x1"));
//        int itemY =atoi(pElem->Attribute("y1"));
//        int itemX2 =atoi(pElem->Attribute("x2"));
//        int itemY2 =atoi(pElem->Attribute("y2"));
//        int itemX3 =atoi(pElem->Attribute("x3"));
//        int itemY3 =atoi(pElem->Attribute("y3"));
//    }

    //! On liste les objets
    pElem=hDoc.FirstChild("objectgroup").FirstChild().Element();
    for(int i=0; pElem; pElem=pElem->NextSiblingElement()){
        if(string(pElem->Attribute("name"))=="spawn"){
             sf::Vector2f spawnLocationOne(atoi(pElem->Attribute("x")),atoi(pElem->Attribute("y"))-GameConfig::g_config["playercollheight"]);
             m_spawnLocationOne=spawnLocationOne;
             m_playerOne->SetPosition(m_spawnLocationOne);
             m_currentPlateau= sf::Vector2i(m_spawnLocationOne.x/GameConfig::g_config["platwidth"],m_spawnLocationOne.y/GameConfig::g_config["platheight"]);
        }
    }

	//! Charge le background
    m_background.Create(m_width*tilewidth,m_height*tileheight);
    pElem=hDoc.FirstChild("layer").FirstChild().FirstChild().Element();
    for(int i=0; pElem; pElem=pElem->NextSiblingElement()){
        int x=i%(m_width);
        int y=(i-i%(m_width))/m_width;
        int id =atoi(pElem->Attribute("gid"));
        if(id !=0)id--;

        Type theNewTile = m_typeList[id];
        theNewTile.tile.SetTexture(m_ImgTypeTile);
        theNewTile.tile.SetPosition(x*tilewidth,y*tileheight);
        theNewTile.tile.SetSubRect(m_typeList[id].zoneRect);

        if(theNewTile.visible)m_background.Draw(theNewTile.tile);
        i++;
    }
    m_background.Display();

    //! On vide la map
    vector<Type> tileListSub;
    tileListSub.resize(m_height);
    m_tileSet.empty();
    m_tileSet.resize(m_width,tileListSub);

	//! Charge le niveau
    m_map.Create(m_width*tilewidth,m_height*tileheight);
    pElem=TiXmlHandle(hDoc.FirstChild("layer").Element()->NextSiblingElement()).FirstChild().FirstChild().Element();
    for(int i=0; pElem; pElem=pElem->NextSiblingElement()){
        int x=i%(m_width);
        int y=(i-i%(m_width))/m_width;
        int id =atoi(pElem->Attribute("gid"));
        if(id !=0)id--;

        m_tileSet[x][y]= m_typeList[id];
        m_tileSet[x][y].tile.SetTexture(m_ImgTypeTile);
        m_tileSet[x][y].tile.SetPosition(x*tilewidth,y*tileheight);
        m_tileSet[x][y].tile.SetSubRect(m_typeList[id].zoneRect);

        if(m_tileSet[x][y].visible)m_map.Draw(m_tileSet[x][y].tile);
        i++;
    }
    m_map.Display();
//    //exit(0);
//    //! Chargement du background
//    m_background.Create(m_width*GameConfig::g_config["tilewidth"],m_height*GameConfig::g_config["tileheight"]);
////!    m_background.Draw(backback);
//    for(int it=0;it<m_width;it++){
//        for(int it2=0;it2<m_height;it2++){
//            theTile=FindType(backImg.CopyToImage().GetPixel(it, it2));
//            Type theNewTile= m_typeList[theTile];
//            theNewTile.tile.SetPosition(it*GameConfig::g_config["tilewidth"],it2*GameConfig::g_config["tileheight"]);
//            //theNewTile.tile.SetTexture(m_ImgTypeTile);
//            theNewTile.tile.SetSubRect(m_typeList[theTile].zoneRect);
//            if(theNewTile.visible)m_background.Draw(theNewTile.tile);
//        }
//    }
//    m_background.Display();
//    backImg.Create(0,0);
}
MapTile::~MapTile(){
    for(unsigned int i=0;i<m_mapEntity.size();i++){
        delete m_mapEntity.at(i);
    }
}
