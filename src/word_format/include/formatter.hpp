#ifndef FORMATTER
#define FORMATTER


#include <string>
#include <map>
#include <memory>

#include <boost/filesystem.hpp>

#include <boost/bimap.hpp>
#include <boost/regex.hpp>

#include "exporter.hpp"

class EXPORT Formatter {
public:
    using aliases_collection_type = boost::bimap<std::string, unsigned int>;
    using arguments_collection_type = std::map<unsigned int, std::shared_ptr<std::string>>;

    Formatter();
    Formatter(std::string file_pattern_name);
    Formatter(const Formatter& other);
    Formatter(Formatter&& other);

    void setFilePattern(std::string file_name);

    template<StringConvertable T>
    void bindArg(int arg_num, T _arg_value);

    template<StringConvertable T>
    void bindArg(std::string arg_name, T _arg_value);

    template<StringConvertable T>
    void bindArg(T _arg_value);

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

    void saveTo(std::string filename);

    template<StringConvertable T>
    Formatter operator%(T value);

    template<StringConvertable T>
    Formatter& operator%=(T value);

    Formatter& operator=(const Formatter& other) = default;
    Formatter& operator=(Formatter&& other) = default;

protected:
    void init_formatter();
    void add_arg(const boost::smatch& _match);

    void get_places_for_values(const std::vector<duckx::Run>& _runs, std::string _text, boost::regex& value_place_pattern, int& run_counter);
    void join_partial_text(const std::vector<duckx::Run>& _runs, std::string& full_partial_arg, boost::regex& value_place_pattern, int& run_counter);

    void getting_places(duckx::Run& selected_run, std::string& text_of_run, unsigned int& _index_of_run);
    void saving_partial_place(duckx::Run& selected_run, std::string& text_of_run, boost::smatch& _match, const unsigned int& _index_of_run);

    unsigned int get_free_index();

    aliases_collection_type _aliases;
    arguments_collection_type _args;

    std::string _name;
    std::unique_ptr<duckx::Document> _document;

    boost::regex value_place_pattern{"[\\[\\{](?<name>[\\w_]*)(?<parameters>\\:[\\w#_-\\d]*)?[\\]\\}]"};
};

#include "formatter_impl.hpp"

#endif