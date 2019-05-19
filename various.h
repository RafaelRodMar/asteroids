
void Text(sf::RenderWindow &app, const std::string &pstr, float px, float py,sf::Color pcolor, int psize, sf::Font &pfont)
{
    sf::Text str;
    str.setString(pstr);
    str.setFont(pfont);
    str.setCharacterSize(psize);
    str.setPosition(px, py);
    str.setFillColor(pcolor);
    app.draw(str);
}

void UpdateHiScores(std::vector<int> &v, int newscore)
{
    //new score to the end
    v.push_back(newscore);
    //sort
    sort(v.rbegin(), v.rend());
    //remove the last
    v.pop_back();
}

void ReadHiScores(std::vector<int> &v)
{
    std::ifstream in("hiscores.dat");
    if(in.good())
    {
        std::string str;
        getline(in,str);
        std::stringstream ss(str);
        int n;
        for(int i=0;i<5;i++)
        {
            ss >> n;
            v.push_back(n);
        }
        in.close();
    }
    else
    {
        //if file does not exist fill with 5 scores
        for(int i=0;i<5;i++)
        {
            v.push_back(0);
        }
    }
}

void WriteHiScores(std::vector<int> &v)
{
    std::ofstream out("hiscores.dat");
    for(int i=0;i<5;i++)
    {
        out << v[i] << " ";
    }
    out.close();
}
