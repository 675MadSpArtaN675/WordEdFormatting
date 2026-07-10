#ifndef FORMATTER
#define FORMATTER


#include <string>
#include <map>
#include <memory>
#include <concepts>

#include <boost/bimap.hpp>
#include <boost/regex.hpp>

#include "exporter.hpp"

template<typename T>
concept StringConvertable = std::convertible_to<T, std::string>
    || requires(T type) { type.str(); } 
    || requires(T type) { type.to_str(); };

class EXPORT Formatter {
public:
    Formatter();
    Formatter(std::string file_pattern_name);
    Formatter(const Formatter& other);
    Formatter(Formatter&& other);

    void setFilePattern(std::string file_name);

    template<StringConvertable T>
    void bindArg(T _arg_value);

    template<StringConvertable T>
    void bindArg(int arg_num, T _arg_value);

    template<StringConvertable T>
    void bindArg(std::string arg_name, T _arg_value);

    void removeArg(int arg_num);
    void removeArg(std::string arg_name);

    bool isArgEmpty(int arg_num);
    bool isArgEmpty(std::string arg_num);

    int getArgNum(std::string arg_name);
    std::string getArgName(unsigned int arg_num);

    std::string getArgValue(unsigned int arg_num);
    std::string getArgValue(std::string name);

    unsigned int count();
    void clearArgs();

    std::string toStr();
    void saveTo(std::string filename);

    template<StringConvertable T>
    Formatter operator%(T value);

    template<StringConvertable T>
    Formatter& operator%=(T value);

    Formatter& operator=(const Formatter& other) = default;
    Formatter& operator=(Formatter&& other) = default;

protected:
    void init_formatter();
    unsigned int get_free_index();

    boost::bimap<std::string, unsigned int> _aliases;
    std::map<unsigned int, std::shared_ptr<std::string>> _args;
    
    std::string _name;
    std::unique_ptr<duckx::Document> _document;
};

#endif