#include <iostream>
#include "random.h"
using namespace std;
class Monster{
    public:
        enum Type{
            Dragon,
            Goblin,
            Ogre,
            Skeleton,
            Orc,
            Troll,
            Vampire,
            Zombie,
            MaxMonstersTypes,
        };
    private:
        Type m_type;
        string m_name;
        string m_roar;
        int m_hp;
    public:
        Monster(Type type,string name,string roar,int hp)
        :m_type{type},m_name{name},m_roar{roar},m_hp(hp)
        {}
        string getTypeString(Type m_type) const{
            switch(m_type){
                case Dragon: return "Dragon";
                case Goblin: return "Goblin";
                case Ogre: return "Ogre";
                case Skeleton: return "Skeleton";
                case Orc: return "Orc";
                case Troll: return "Troll";
                case Vampire: return "Vampire";
                case Zombie: return "Zombie";
                default: return "???";
            }
        }
        void print() const{
            cout<<m_name<<" the "<<getTypeString(m_type);
            if(m_hp<1)cout<<" is dead";
            else cout<<" has "<<m_hp<<" hit points and says "<<m_roar;
            cout<<endl;
        }
};

namespace MonsterGenerator
{
    Monster::Type getType (int n){
        switch(n){
                case 0: return Monster::Dragon;
                case 1: return Monster::Goblin;
                case 2: return Monster::Ogre;
                case 3: return Monster::Skeleton;
                case 4: return Monster::Orc;
                case 5: return Monster::Troll;
                case 6: return Monster::Vampire;
                case 7: return Monster::Zombie;
                default: return Monster::MaxMonstersTypes;
            }
    }
    string getName(int n)
	{
        switch (n)
        {
            case 0:  return "Blarg";
            case 1:  return "Moog";
            case 2:  return "Pksh";
            case 3:  return "Tyrn";
            case 4:  return "Mort";
            case 5:  return "Hans";
            default: return "LESTER THE MOLESTER";
        }
    }

    string getRoar(int n)
	{
        switch (n)
        {
            case 0:  return "*ROAR*";
            case 1:  return "*peep*";
            case 2:  return "*squeal*";
            case 3:  return "*whine*";
            case 4:  return "*growl*";
            case 5:  return "*burp*";
            default: return "Meaow";
        }
    }

	Monster generate()
	{
		return Monster{ getType(Random::get(0,6)), getName(Random::get(0,6)), getRoar(Random::get(0,6)), Random::get(1,100) };
	}

};

int main()
{
	Monster m{ MonsterGenerator::generate() };
	m.print();

	return 0;
}
