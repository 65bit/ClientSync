#pragma once

#include <string>
#include <document.h>
#include <Logger.hpp>

class ConfigReader
{
public:
    virtual ~ConfigReader() = default;
    
    bool read(const std::string _path)
    {
        std::string content;
        
        //#TODO: Read content
        
        //Processing
        
        rapidjson::Document doc;
        doc.Parse(content.c_str());
        
        if(doc.HasParseError())
        {
            LOG_WARNING("Unnable to read config:" + _path);
            return false;
        }
        
        return process(doc);
    }
    
protected:
    virtual bool process(rapidjson::Document& _doc) = 0;
};
