#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <forward_list>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <iterator>
#include <algorithm>

using namespace std;

#define ALPHABET_SIZE 31

struct Player
{
    int fifa_id; // Id do Jogador
    string name; // Nome do Jogador
    string positions; // Posições do Jogador
    float rating; // Média de Avaliação
    int rcount; // Número de Avaliações
};

struct Avaliacao
{
    int id_player; // O id do jogador avaliado
    float nota; // nota dado para o jogador
};

void printPlayer(int id);

template <class Tkey, class Tvalue>
struct Node
{
    Tkey key;
    Tvalue value;
};

template <class Tkey, class Tvalue>
class hashtable
{
    private:
        forward_list<Node<Tkey, Tvalue>> *table;
        unsigned int hashfunction(int id)
        {
            return id % M;
        }
        unsigned int hashfunction(string id)
        {
            int hashI = 0;
            for(int i = 0; i < (int)id.length(); i++)
                hashI = (31 * hashI + id.at(i)) % M;

            return hashI;
        }
    public:
        int M; // Tamanho

        hashtable(int T_size = 1)
        {
            M = T_size;
            table = new forward_list<Node<Tkey, Tvalue>>[M];
        }

        ~hashtable()
        {
            delete[] table;
        }

        void insere(Tkey key, Tvalue value, bool Novo = false)
        {
            unsigned int hashI = hashfunction(key);

            if(!Novo) // Se eu tenho certeza que esse elemento é novo, eu não preciso verificar
            {
                for(auto x = table[hashI].begin(); x != table[hashI].end(); ++x){
                    if(key == x->key) // Se o elemento já existe
                        return;   // Não faz nada
                }
            }
            Node<Tkey,Tvalue> myNode = {.key = key, .value = value};
            table[hashI].push_front(myNode);
        }

        template<typename Tvec>
        void insere_array(Tkey key, Tvec value)
        {
            unsigned int hashI = hashfunction(key);

            for(auto x = table[hashI].begin(); x != table[hashI].end(); ++x)
            {
                if(key == x->key) // Se o elemento já existe
                {
                    x->value.push_back(value);
                    return;
                }
            }
            Node<Tkey,Tvalue> myNode = {.key = key, .value = std::vector<Tvec>{value}};
            table[hashI].push_front(myNode);
        }

        Node<Tkey, Tvalue>* operator[](Tkey key)
        {
            int hashI = hashfunction(key);

            for(auto x = table[hashI].begin(); x != table[hashI].end(); ++x)
            {
                if(key == x->key)
                {
                    return &(*x);
                }
            }

            return NULL;
        }
};

template <typename T>
struct range_t
{
    T b, e;
    range_t(T x, T y) : b(x), e(y) {}
    T begin()
    {
        return b;
    }
    T end()
    {
        return e;
    }
};

template <typename T>
range_t<T> range(T b, T e)
{
    return range_t<T>(b, e);
}

vector<string> split(string str, string token){
    vector<string>result;
    while(str.size()){
        unsigned int index = str.find(token);
        if(str[0] == ' ') str.erase(0,1);
        if(index!=string::npos){
            result.push_back(str.substr(0,index));
            str = str.substr(index+token.size());
            if(str.size()==0)result.push_back(str);
        }else{
            result.push_back(str);
            break;
        }
    }
    return result;
}

template<typename T>
void printVector(const T& t) {
    std::copy(t.cbegin(), t.cend(), std::ostream_iterator<typename T::value_type>(std::cout, ", "));
}

int charAt(string str, int d)
{
    if (int(str.size()) <= d)
        return -1;
    else
        return (str.at(d))-65;
}

class Trie
{
private:
        class Trie_Node
        {
            public:
                Trie_Node *children[ALPHABET_SIZE];

                bool isWord;
                int id;

                Trie_Node()
                {
                    for(int i = 0; i < ALPHABET_SIZE; i++)
                    {
                        children[i] = NULL;
                    }

                    isWord = false;
                    id = -1;
                }

                ~Trie_Node()
                {
                    for(int i = 0; i < ALPHABET_SIZE; i++)
                        if(children[i])
                        {
                            delete children[i];
                            children[i] = NULL;
                        }

                    delete this;
                }
        };

        bool isLastNode(Trie_Node *current)
        {
            for(int i = 0; i < ALPHABET_SIZE; i++)
                if(current->children[i])
                    return false;
            return true;
        }

       int ItoC(char c)
        {
            switch(c)
            {
                case ' ':
                    return 26;
                    break;

                case '\'':
                    return 27;
                    break;

                case '-':
                    return 28;
                    break;

                case '.':
                    return 29;
                    break;

                case '"':
                    return 30;
                    break;

                default:
                    return static_cast<int>(std::tolower(c) - 'a');
            }
        }

        char CtoI(int i)
        {
            switch(i)
            {
                case 26:
                    return ' ';
                    break;

                case 27:
                    return '\'';
                    break;

                case 28:
                    return '-';
                    break;

                case 29:
                    return '.';
                    break;

                case 30:
                    return '"';
                    break;

                default:
                    return static_cast<int>('a' + i);
            }
        }

    public:
        Trie_Node *root;
        int Total_Words;

        Trie()
        {
            root = new Trie_Node();
            Total_Words = 0;
        }

        void InsertWord(string& str, int id)
        {
            Trie_Node *current = root;
            for(unsigned int i = 0; i < str.size(); i++)
            {
                int c = ItoC(str[i]);

                if(current->children[c] == NULL)
                {
                    current->children[c] = new Trie_Node();
                }

                current = (current->children[c]);
            }

            current->isWord = true;
            current->id = id;
        }

        bool SearchWord(string& str)
        {
            Trie_Node *current = root;

            for(unsigned int i = 0; i < str.size(); i++)
            {
                if(current->children[ItoC(str[i])] == NULL)
                    return false;

                current = (current->children[ItoC(str[i])]);
            }

            if(current->isWord == true)
            {
                std::cout << "\nID: " << current->id << std::endl;
                return true;
            }

            return false;
        }

        void possibleSufixos(std::string& prefixo)
        {
            Trie_Node *current = root;
            int index;

            for(unsigned int i = 0; i < prefixo.length(); i++)
            {
                index = ItoC(prefixo.at(i));
                if(current->children[index] == NULL) return;
                current = current->children[index];
            }

            caminha(prefixo, current);

            return;
        }

        void caminha(std::string prefixo, Trie_Node* current)
        {
            if(current->isWord)
                printPlayer(current->id);

            for(int i = 0; i < ALPHABET_SIZE; ++i)
            {
                if(current->children[i])
                    caminha(prefixo + CtoI(i), current->children[i]);
            }
        }
};
