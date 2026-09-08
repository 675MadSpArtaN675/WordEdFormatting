#include <optional>
#include <algorithm>

#include <boost/filesystem/operations.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/ftxui.hpp>

#include <boost/filesystem.hpp>
#include <boost/lambda2.hpp>

class AccessDirectories
{
    boost::filesystem::path directory;
    unsigned int depth;
    std::vector<std::string> ignored_file_names;

public:
    AccessDirectories(std::string _directory, unsigned int _depth, std::vector<std::string> _ignored_files_by_patterns) {
        setDirectory(_directory);

        depth = _depth;
        ignored_file_names = _ignored_files_by_patterns;
    }

    AccessDirectories(AccessDirectories& other) : directory(other.directory), depth(other.depth), ignored_file_names(other.ignored_file_names) {}

    AccessDirectories(AccessDirectories&& other) : directory(other.directory), depth(other.depth), ignored_file_names(other.ignored_file_names) {
        other.directory = boost::filesystem::path();
        other.depth = 0;
        other.ignored_file_names = std::vector<std::string>();
    }

    void setDirectory(std::string path_str) {
        boost::filesystem::path path;

        if (path_str.empty()) {
            path = boost::filesystem::current_path();
        }
        else {
            path = boost::filesystem::path(path_str);
        }

        if (path.is_relative()){
            path = boost::filesystem::absolute(path);
        }

        directory = path;
    }

    void setDirectory(boost::filesystem::path path){
        if (path.is_relative()) {
            path = boost::filesystem::absolute(path);
        }

        directory = path;
    }

    boost::filesystem::path getDirectory() {
        return directory;
    }

    void setDepth(const unsigned int& _depth) {
        depth = _depth;
    }

    unsigned int getDepth() {
        return depth;
    }

    void addIgnoredPattern(std::string value) {
        if (!value.empty()) {
            ignored_file_names.push_back(value);
        }
    }

    void removeIgnoredPattern(unsigned int _index) {
        if (_index < ignored_file_names.size()) {
            ignored_file_names.erase(ignored_file_names.begin() + _index);
        }
    }

    void removeIgnorePatternByName(std::string value) {
        if (!value.empty()) {
            std::erase_if(ignored_file_names, boost::lambda2::_1 == value);
        }
    }

    void clear() {
        ignored_file_names.clear();
    }

    std::optional<std::string> getPattern(unsigned int _index){
        if (_index < ignored_file_names.size())
        {
            return ignored_file_names[_index];
        }

        return std::nullopt;
    }

    std::optional<std::string> operator[](unsigned int index){
        return getPattern(index);
    }

    AccessDirectories& operator=(const AccessDirectories& other) = default;
    AccessDirectories& operator=(AccessDirectories&& other) = default;
};

int main(int argc, char** argv) {
    ftxui::Component button = ftxui::Button("Get file", []() {});

    ftxui::Element _base_element = ftxui::window(
        ftxui::text("Formatter"),
        ftxui::border(
            ftxui::gridbox({
                {button->Render()}
            })
        )
    );

    ftxui::App output = ftxui::App::TerminalOutput();
    ftxui::Component _container = ftxui::Container::Vertical({button});

    output.Loop(ftxui::Renderer(_container, [&] {
        return ftxui::Element(_base_element);
    }));
}