#include <iostream>
#include <locale>
#include <optional>
#include <unordered_set>
#include <algorithm>

#include <ftxui/ftxui.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/component.hpp>

#include <boost/lambda2.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

class AccessDirectory
{
    boost::filesystem::path directory;
    unsigned int depth;
    std::vector<std::string> ignored_file_names;

public:
    AccessDirectory(std::string _directory, unsigned int _depth, std::vector<std::string> _ignored_files_by_patterns) {
        setDirectory(_directory);

        depth = _depth;
        ignored_file_names = _ignored_files_by_patterns;
    }

    AccessDirectory(const AccessDirectory& other) : directory(other.directory), depth(other.depth), ignored_file_names(other.ignored_file_names) {}

    AccessDirectory(AccessDirectory&& other) : directory(other.directory), depth(other.depth), ignored_file_names(other.ignored_file_names) {
        other.directory = boost::filesystem::path();
        other.depth = 0;
        other.ignored_file_names = std::vector<std::string>();
    }

    bool isExists() {
        return boost::filesystem::exists(directory);
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

    boost::filesystem::path getDirectory() const {
        return directory;
    }

    void setDepth(const unsigned int& _depth) {
        depth = _depth;
    }

    unsigned int getDepth() const {
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

    std::optional<std::string> getPattern(unsigned int _index) const {
        if (_index < ignored_file_names.size())
        {
            return ignored_file_names[_index];
        }

        return std::nullopt;
    }

    std::optional<std::string> operator[](unsigned int index) const {
        return getPattern(index);
    }

    AccessDirectory& operator=(const AccessDirectory& other) = default;
    AccessDirectory& operator=(AccessDirectory&& other) = default;

    bool operator==(const AccessDirectory& other) const
    {
        return directory == other.directory && depth == other.depth && ignored_file_names == other.ignored_file_names;
    }
};

template<>
struct std::hash<AccessDirectory>
{
    std::size_t operator()(const AccessDirectory& _directory) const noexcept {
        std::size_t _path = std::hash<std::string>{}(_directory.getDirectory().string());
        std::size_t _depth = std::hash<unsigned int>{}(_directory.getDepth());

        return _path ^ _depth;
    }
};

struct DirectoryButton {
    int id;
    ftxui::Component component;
};

int main(int argc, char** argv) {
    int index = 0;
    int depth = 1;
    int index_of_selected_file = 0;
    std::vector<DirectoryButton> path_s_buttons;
    std::unordered_set<AccessDirectory> _directories_to_checkup;

    ftxui::App output = ftxui::App::TerminalOutput();
    output.Fullscreen();

    std::string _input_path;
    std::string path_depth_input;
    std::vector<std::string> _entries = {"None"};
    ftxui::InputOption path_options = ftxui::InputOption::Default();
    path_options.multiline = false;

    ftxui::Component path_input = ftxui::Input(&_input_path, path_options) | ftxui::center;
    ftxui::Component path_depth = ftxui::Input(&path_depth_input, path_options) | ftxui::center | ftxui::CatchEvent([](ftxui::Event _event) {
        return _event.is_character() && std::isdigit(_event.character()[0]);
    }) ;

    ftxui::Component get_file_button = ftxui::Button("Get file", output.ExitLoopClosure());
    ftxui::Component files_dropdown = ftxui::Dropdown(_entries, &index_of_selected_file);

    ftxui::Component paths_container = ftxui::Container::Vertical({});

    ftxui::Component check_path_add_button = ftxui::Button("Add path", [&]() {
        AccessDirectory new_path(_input_path, std::stoi(path_depth_input), std::vector<std::string>());

        if (new_path.isExists()) {
            ftxui::Component _new_field = ftxui::Button(new_path.getDirectory().string(),
            [&path_s_buttons, &paths_container, index]() {
                std::vector<DirectoryButton>::const_iterator found_button = std::find_if(path_s_buttons.cbegin(), path_s_buttons.cend(), [&index](DirectoryButton db_){return db_.id == index;});

                if (found_button != path_s_buttons.end()) {
                    int index_in_container = found_button->component->Index();

                    paths_container->ChildAt(index_in_container)->Detach();
                    path_s_buttons.erase(found_button);
                }
            });

            path_s_buttons.push_back(DirectoryButton(index++, _new_field));
            paths_container->Add(_new_field);
        }

        _input_path.clear();
    });
    ftxui::Component _container = ftxui::Container::Vertical({
        ftxui::Container::Horizontal(
            {path_input, path_depth, check_path_add_button, paths_container}
        ),
        files_dropdown,
        get_file_button
    });

    ftxui::Component render_component = ftxui::Renderer(_container, [&] {
        return ftxui::window(
        ftxui::text("Formatter"),
        ftxui::border(
            ftxui::gridbox(
                {
                    {ftxui::vbox(
                        ftxui::hbox(
                            ftxui::text("Path to add:") | ftxui::center,
                            path_input->Render(),
                            ftxui::text(" Search depth:") | ftxui::center,
                            path_depth->Render(),
                            ftxui::separator(),
                            check_path_add_button->Render(),
                            ftxui::text(" ")
                        ) | ftxui::border | ftxui::flex,
                        ftxui::separator(),
                        files_dropdown->Render(),
                        ftxui::separator(),
                        get_file_button->Render()
                    ),
                    ftxui::separator(),
                    ftxui::vbox(paths_container->Render()) | ftxui::flex | ftxui::border
                },
            })
        )
    );
    });

    output.Loop(render_component);
}