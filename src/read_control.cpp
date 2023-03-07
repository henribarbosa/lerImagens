#include "../include/read_control.h"

control::control(const std::string& path, const std::string& fieldName)
{
    Param.name = fieldName;

    std::ifstream infile(path);
    std::string line;

    std::string large_key, key, value;

    while (std::getline(infile, line))
    {
        if(line.find('#')+1) // ignora comentarios
        {
            int pos = line.find('#');
            //std::cout << pos << std::endl;
            line = line.substr(0,pos);
            //std::cout << line << std::endl;
        }

        if (line.empty())  // pula linhas vazias
        {
            //std::cout << "Passed" << std::endl;
            continue;
        }

        if (line.find('{')+1) // inicio de declaracao de classe
        {
            large_key = line.substr(0,line.find('{')) + ".";
            continue;
            //std::string value;
            
        }

        if (line.find('}')+1) // fim de declaracao de class
        {
            large_key = "";
            continue;
        }
        
        // encontra os valores chaves no texto
        key = line.substr(0,line.find(':'));
        key.erase(std::remove(key.begin(), key.end(), ' '), key.end());
        key = large_key + key;
        value = line.substr(line.find(':')+1,line.size());

        if (key == Param.name)
            Param.threshold = std::stof(value);

    }
}

control::~control()
{}

float control::returnThreshold()
{
    return Param.threshold;
}
