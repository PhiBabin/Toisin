#include "GameConfig.hpp"

/**
    On déclare les attributs statics
**/
map<string,int> GameConfig::g_config;

map<int,mobType> GameConfig::g_mob;

map<string,sf::SoundBuffer> GameConfig::g_soundManag;

map<string,imgAnim> GameConfig::g_imgManag;

/**
    Constructeur
**/
GameConfig::GameConfig()
{
}
/**
    Chargement de la configuration
**/
void GameConfig::LoadConfig(){
    TiXmlDocument doc("config/config.xml");
    doc.LoadFile();

    TiXmlHandle hDoc(&doc);
    TiXmlHandle hRoot(0);
    TiXmlElement* pElem;

    pElem=hDoc.FirstChild("img").FirstChild().Element();
    for(; pElem; pElem=pElem->NextSiblingElement()){
        imgAnim newAnim;
        sf::Texture newImg;
        if(!newImg.LoadFromFile(pElem->Attribute("path"))){
            cerr<<"[FATAL ERROR]Image not found: "<<pElem->Attribute("path")<<endl;
            exit(0);
        }
        newAnim.img=newImg;
        newAnim.nbrCollum=atoi(pElem->Attribute("nbrCollums"));
        newAnim.nbrLine=atoi(pElem->Attribute("nbrLines"));
        g_imgManag[pElem->Attribute("name")]=newAnim;
    }
    pElem=hDoc.FirstChild("enemies").FirstChild().Element();
    for(; pElem; pElem=pElem->NextSiblingElement()){
        g_mob[atoi(pElem->Attribute("type"))].path=atoi(pElem->Attribute("path"));
        g_mob[atoi(pElem->Attribute("type"))].x=atoi(pElem->Attribute("x"));
        g_mob[atoi(pElem->Attribute("type"))].y=atoi(pElem->Attribute("y"));
        g_mob[atoi(pElem->Attribute("type"))].width=atoi(pElem->Attribute("width"));
        g_mob[atoi(pElem->Attribute("type"))].height=atoi(pElem->Attribute("height"));
    }
    pElem=hDoc.FirstChild("sound").FirstChild().Element();
    for(; pElem; pElem=pElem->NextSiblingElement()){
        sf::SoundBuffer newSound;
        newSound.LoadFromFile(pElem->Attribute("path"));
        g_soundManag[pElem->Attribute("name")]=newSound;
    }
    pElem=hDoc.FirstChild("config").FirstChild().Element();
    for(; pElem; pElem=pElem->NextSiblingElement()){
        GameConfig::g_config[pElem->Attribute("name")]=atoi(pElem->Attribute("value"));
    }
}

sf::Color GameConfig::NbrToColor(int nbr){
    if(nbr==1)return sf::Color::Red;

}

int GameConfig::ColorToNbr(sf::Color myColor){
    if(myColor==sf::Color(205,45,45))return 1;
}

GameConfig::~GameConfig()
{
}
