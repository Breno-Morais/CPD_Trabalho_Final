#include "./parser.hpp"
#include "estruturas.hpp"

using namespace std;
using namespace aria::csv;

hashtable<int, Player> players_table(100000);
hashtable<int, vector<Avaliacao>> user_ratings_table(500000);
hashtable<string, vector<int>> position_table(21);
hashtable<string, vector<int>> tags_table(937);

vector<string> str_tokenizer(string s, char del);
vector<int> removeDuplicates(const vector<int>& in_vector);
void printPlayer(int id);
void quickSort(int *arr, int low, int high);
void quickSortAv(Avaliacao *arr, int low, int high);
void user_search(int n);
void position_search(int n, std::string pos);
void tags_search(std::vector<string> tags_to_be_searched);

int main(int argc, char** argv) 
{
    auto start = chrono::steady_clock::now();

    cout << "Reading names and inserting in trie tree..." << endl;
    Trie *TriePlayers = new Trie();
    std::ifstream fPlayer("players.csv");
    CsvParser parser(fPlayer);

    bool ignore_first = true;
    for (auto& row : parser) 
    {
        if(ignore_first)
        {
            ignore_first = false;
            continue;
        }
        Player temp;
        temp.fifa_id = std::atoi(row[0].c_str());
        temp.name = row[1];
        temp.positions = row[2];
        temp.rating = 0.0f;
        temp.rcount = 0;

        players_table.insere(temp.fifa_id,temp);
        TriePlayers->InsertWord(temp.name, temp.fifa_id);

        vector<string> player_positions = str_tokenizer(temp.positions, ',');
        for(string pos: player_positions)
            position_table.insere_array<int>(pos, temp.fifa_id);
    }

    cout << "Reading ratings and inserting in hash table..." << endl;
    std::ifstream fRating("rating.csv");
    CsvParser parser1(fRating);

    ignore_first = true;
    for (auto& row : parser1) 
    {
        if(ignore_first)
        {
            ignore_first = false;
            continue;
        }
        unsigned int user_id = std::atoi(row[0].c_str());
        unsigned int player_id = std::atoi(row[1].c_str());
        float rating = std::atof(row[2].c_str());;
        Player* temp = &(players_table[player_id]->value);

        int i1 = (temp->rcount) + 1;

        if(temp->rcount == 0)
            temp->rating = rating;
        else
            temp->rating = temp->rating + (1.0f/i1)*(rating - temp->rating);

        temp->rcount += 1;
        user_ratings_table.insere_array<Avaliacao>(user_id, {(int)player_id, rating});
    }

    cout << "Reading tags and inserting in hash table..." << endl;
    ifstream fTags("tags.csv");
    CsvParser parser2(fTags);

    ignore_first = true;
    for (auto& row : parser2)
    {
        if (ignore_first)
        {
            ignore_first = false;
            continue;
        }

        unsigned int player_id = std::atoi(row[1].c_str());
        bool alreadyInserted = false;
        string player_tag = row[2];

        Player* temp = &(players_table[player_id]->value);

        for (string inserted_tag : temp->tags) // não adiciona tags repetidas
        {
            if (player_tag == inserted_tag)
                alreadyInserted = true;
        }
        if (!alreadyInserted)
            temp->tags.push_back(player_tag);

        tags_table.insere_array<int>(player_tag, temp->fifa_id);
    }

    cout << "Done" << endl;
    auto end = chrono::steady_clock::now();
    cout << "Loading time: " << chrono::duration_cast<chrono::milliseconds>(end-start).count() << " milliseconds" << endl;

    string token;
    while(token != "exit")
    {
        cout << "& ";
        string comando;
        getline(std::cin, comando);
        string delimiter = " ";
        size_t pos = comando.find(delimiter);
        token = comando.substr(0, pos);
        comando.erase(0, pos + delimiter.length());

        if(token == "user")
        {
            user_search(std::atoi(comando.c_str()));
        }
        else if(token == "player")
        {
            TriePlayers->possibleSufixos(comando);
        }
        else if(token.substr(0,3) == "top")
        {
            token.erase(0,3);
            int n = std::atoi(token.c_str());
            if((comando.at(0) == '\'') && (comando.at(comando.length()-1) == '\''))
                position_search(n, comando.substr(1, comando.length()-2));
        }
        else if (token == "tags")
        {
            // vetor de strings contendo tags
            vector<string> tags = str_tokenizer(comando, '\'');

            // filtra espaços entre tags do vetor de strings para obter apenas as tags
            for (auto str = tags.begin(); str != tags.end(); str++)
            {
                if (*str == " ")
                    tags.erase(str--);
            }

            tags_search(tags);
        }
        else if(token == "exit")
            cout << "Exiting..." << endl;
        else
            cout << "Unrecognized command" << endl;
    }

    return 0;
}

vector<string> str_tokenizer(string s, char del)
{
    vector<string> vector_s;
    stringstream ss(s);
    string word;
    while (!ss.eof())
    {
        getline(ss, word, del);
        if (!word.empty())
            vector_s.push_back(word);
    }

    return vector_s;
}

void printPlayer(int id)
{
    Player temp = players_table[id]->value;
    std::cout << id << ", " << temp.name << ", [" << temp.positions << "] ," << temp.rating << ", " << temp.rcount << std::endl;
}

void quickSort(int *arr, int low, int high)
{
    int i = low;
    int j = high;
    float pivot = players_table[arr[(i + j) / 2]]->value.rating;
    int temp;

    while (i <= j)
    {
        while (players_table[arr[i]]->value.rating < pivot)
            i++;
        while (players_table[arr[j]]->value.rating > pivot)
            j--;
        if (i <= j)
        {
            temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
            i++;
            j--;
        }
    }
    if (j > low)
        quickSort(arr, low, j);
    if (i < high)
        quickSort(arr, i, high);
}

void quickSortAv(Avaliacao *arr, int low, int high)
{
    int i = low;
    int j = high;
    float pivot = arr[(i + j) / 2].nota;
    Avaliacao temp;

    while (i <= j)
    {
        while (arr[i].nota < pivot)
            i++;
        while (arr[j].nota > pivot)
            j--;
        if (i <= j)
        {
            temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
            i++;
            j--;
        }
    }
    if (j > low)
        quickSortAv(arr, low, j);
    if (i < high)
        quickSortAv(arr, i, high);
}

void user_search(int n)
{
    if(user_ratings_table[n] == NULL)
    {
        std::cout << "USUARIO NAO ENCONTRADO"  << std::endl;
        return;
    }
    unsigned int vec_size = user_ratings_table[n]->value.size();
    unsigned int true_size;
    if(vec_size > 20)
        true_size = 20;
    else
        true_size = vec_size;

    Avaliacao top20[true_size];

    for(unsigned int i = 0; i < true_size; i++)
    {
        top20[i] = user_ratings_table[n]->value[i];
    }
    quickSortAv(top20, 0, true_size-1);

    for(unsigned int i = true_size; i < vec_size; i++)
    {
        float temp = user_ratings_table[n]->value[i].nota;
        float last_rating = top20[0].nota;
        if(temp > last_rating)
        {
            top20[0] = user_ratings_table[n]->value[i];
            quickSortAv(top20, 0, true_size-1);
        }
    }

    std::cout << "sofifa_id, name, positions, global_rating, count, rating" << std::endl;
    for(int i = true_size - 1; i >= 0; i--)
    {
        Player temp = players_table[top20[i].id_player]->value;
        std::cout << temp.fifa_id << ", " << temp.name << ", [" << temp.positions << "] ," << temp.rating << ", " << temp.rcount << ", " << top20[i].nota <<std::endl;
    }
}

void position_search(int n, std::string pos)
{
    if(position_table[pos] == NULL)
    {
        std::cout << "POSICAO NAO ENCONTRADA"  << std::endl;
        return;
    }
    int vec_size = position_table[pos]->value.size();
    int true_size;
    if(vec_size > n)
        true_size = n;
    else
        true_size = vec_size;

    int topN[true_size];
    int i = 0;
    int inc = 0;
    int valor_de_corte = 1000;
    while((i < true_size) && (i + inc < vec_size))
    {
        if(players_table[position_table[pos]->value[i + inc]]->value.rcount >= valor_de_corte)
        {
            topN[i] = players_table[position_table[pos]->value[i + inc]]->value.fifa_id;
            i++;
        }
        else
            inc++;
    }
    if(i == 0) // Se ele n�o achou ninguem com mais de 1000 avalia��es, sai fora
        return;

    true_size = i - 1;
    quickSort(topN, 0, true_size);

    i = true_size;
    while(i < vec_size)
    {
        if(players_table[position_table[pos]->value[i]]->value.rcount >= valor_de_corte)
        {
            float temp = players_table[position_table[pos]->value[i]]->value.rating;
            float last_rating = players_table[topN[0]]->value.rating;
            if(temp > last_rating)
            {
                topN[0] = players_table[position_table[pos]->value[i]]->value.fifa_id;
                quickSort(topN, 0, true_size);
            }
        }
        i++;
    }

    std::cout << "sofifa_id, name, positions, global_rating, count" << std::endl;
    for(int i = true_size; i >= 0; i--)
        printPlayer(topN[i]);
}

void tags_search(std::vector<string> tags_to_be_searched)
{
    for (string tag_requested : tags_to_be_searched) // verifica se alguma tag passsada não possui jogadores na hash table
    {
        if (tags_table[tag_requested] == NULL)
        {
            cout << "Invalid tag: " << tag_requested << endl;
            return;
        }
    }

    vector<int> players_with_tag = tags_table[tags_to_be_searched[0]]->value;
    for (int i = 1; i < tags_to_be_searched.size(); i++)
    {
        for (auto playerid = players_with_tag.begin(); playerid != players_with_tag.end(); playerid++)
        {
            vector<string> player_tags = players_table[*playerid]->value.tags;
            bool has_tag = false;
            for (string player_tag : player_tags)
            {
                if (player_tag == tags_to_be_searched[i])
                    has_tag = true;
            }

            if (!has_tag)
                players_with_tag.erase(playerid--);
        }
    }

    vector<int> filtered_players_with_tag = removeDuplicates(players_with_tag);
    for (int filtered_player_id : filtered_players_with_tag)
        printPlayer(filtered_player_id);
}

vector<int> removeDuplicates(const vector<int>& in_vector)
{
    vector<int> ids_filtered;
    for (int id : in_vector) // remove duplicados
    {
        bool inserted = false;
        for (int id_filtered : ids_filtered)
        {
            if (id == id_filtered)
                inserted = true;
        }
        if (!inserted)
            ids_filtered.push_back(id);
    }
    return ids_filtered;
}