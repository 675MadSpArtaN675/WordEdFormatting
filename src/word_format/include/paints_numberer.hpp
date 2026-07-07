#ifndef PAINTS_NUMBERER
#define PAINTS_NUMBERER

#define BOOST_ATOMIC_WINDOWS_DISABLE_WAIT_ON_ADDRESS
#define BOOST_ATOMIC_NO_ATOMIC_WAIT

#define STANDARD_PAINT_PATTERN "Рисунок\\s*\\{.*\\}\\s*[‒–—−―]?[\\sа-яА-Я\\w]*"

#ifdef DEBUG_LOG
    #define LOG(message) std::cout << message << std::endl
#else
    #define LOG(message)
#endif

#include <list>
#include <vector>
#include <string>
#include <memory>
#include <iterator>

#include <boost/log/common.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/format.hpp>

#include "exporter.hpp"
#include "constants_elements.hpp"
#include "pattern_title.hpp"

class EXPORT PaintsNumberer {
public:
    PaintsNumberer();
    PaintsNumberer(std::string filename, unsigned int min_numeration = 0, unsigned int max_numeration = 0);
    PaintsNumberer(const PaintsNumberer& other);
    PaintsNumberer(PaintsNumberer&& other);

    void setFile(const std::string& file_path);
    void setFile(duckx::Document* file_doc);

    duckx::Document* releaseFile();

    void addPaintPattern(std::string pattern, bool is_title_clear = false);
    void changePaintPattern(unsigned int index, std::string pattern, bool is_title_clear = false);
    void removePaintPattern(unsigned int index);
    std::string getPattern(unsigned int index);

    std::list<std::string> patterns();
    
    void set_min_numeration(unsigned int index);
    void set_max_numeration(unsigned int index);

    unsigned int get_min_numeration();
    unsigned int get_max_numeration();

    unsigned int paints_count();
    bool empty();
    void clear();
    
    void numerate_in_text();
    void numerate();

    PaintsNumberer& operator=(const PaintsNumberer& other) = default;
    PaintsNumberer& operator=(PaintsNumberer&& other) = default;

protected:
    unsigned long init_paint_num();

    void performing_runs(std::vector<duckx::Run>& _runs, unsigned long& paint_num);
    void perform_full_text(std::string& run_text, boost::sregex_iterator& _start, unsigned long& paint_num);
    
    std::string process_line(const std::string& input_line, unsigned long& paint_num, bool& is_end);
    void align_runs(std::vector<duckx::Run>& _prepared_runs, duckx::Run& runs_old);
    
    std::vector<duckx::Run> get_runs(duckx::Paragraph& _par);
    std::wstring get_wstring_paragraph_text(duckx::Paragraph& _par);


    std::unique_ptr<duckx::Document> _document;
    std::list<PatternTitle> _paint_patterns;

    unsigned int _paints_count, _max_numeration, _min_numeration;

    PATTERNS patterns_of_elements;
};

#endif