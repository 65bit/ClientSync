#pragma once

#include <fstream>
#include <string>
#include <document.h>
#include <Logger.hpp>

class JsonProcessor
{
public:
    virtual ~JsonProcessor() = default;
    
    bool read(const std::string _path)
    {
        std::ifstream file(_path);
        
        if(!file.is_open())
        {
            LOG_WARNING("Unnable to open config file:" + _path);
            return false;
        }
        
        const std::string content(std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{});
        
        rapidjson::Document doc;
        doc.Parse(content.c_str());
        
        if(doc.HasParseError())
        {
            LOG_WARNING("Unnable to read config:" + _path);
            return false;
        }
        
        return processJson(doc);
    }
    
protected:
    virtual bool processJson(rapidjson::Document& _doc) = 0;
};
