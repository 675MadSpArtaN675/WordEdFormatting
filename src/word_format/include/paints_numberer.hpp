#ifndef PAINTS_NUMBERER
#define PAINTS_NUMBERER

#define BOOST_ATOMIC_WINDOWS_DISABLE_WAIT_ON_ADDRESS
#define BOOST_ATOMIC_NO_ATOMIC_WAIT

#ifndef DISABLE_STANDART_PATTERNS
    #define STANDARD_PAINT_PATTERN "Рисунок\\s*\\{.*\\}\\s*[‒–—−―]?[\\sа-яА-Я\\w]*"
    #define NORMAL_CONDITION !
#endif

#include <map>
#include <list>
#include <vector>
#include <queue>
#include <string>
#include <memory>
#include <iterator>
#include <iostream>
#include <unordered_set>

#include <boost/log/common.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/format.hpp>

#include <exprtk.hpp>

#include "exporter.hpp"
#include "constants_elements.hpp"
#include "pattern_title.hpp"


struct EXPORT PaintPoint
{
    unsigned int paragraph_num;
    unsigned int run_num;
    unsigned int paint_num;

    PaintPoint(unsigned int _par_num, unsigned int _run_num, unsigned int _paint_num) : paragraph_num(_par_num), run_num(_run_num), paint_num(_paint_num)
    {}
};

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

    bool is_paints_setted();
    bool empty();

    void clear();

    void numerate_in_text();
    void numerate();

    PaintsNumberer& operator=(const PaintsNumberer& other) = default;
    PaintsNumberer& operator=(PaintsNumberer&& other) = default;

protected:
    unsigned long init_paint_num();

    void performing_runs(std::vector<duckx::Run>& _runs, unsigned long& paint_num, const unsigned long& par_num);
    void perform_full_text(std::string& run_text, boost::sregex_iterator& _start, unsigned long& paint_num, const unsigned long& par_num, const int& run_num);

    std::string process_line(const std::string& input_line, unsigned long& paint_num, bool& is_end);

    void align_runs(std::vector<duckx::Run>& _prepared_runs, duckx::Run& runs_old);
    void add_paint_num_to_table(const unsigned int& paragraph_num, const unsigned int& run_num,const unsigned int& paint_num);

    long get_paint_num(std::string bracket_expression, const unsigned int& paragraph_num, const int& run_num, std::vector<unsigned int>& old_num);
    std::string create_paint_num_title(unsigned int num);

    std::string get_correct_num_of_intext_paint(std::string bracket_expression, const unsigned int& paragraph_num, const int& run_num, std::vector<unsigned int>& old_nums);

    bool is_pattern_has(std::wstring _text);
    bool is_has_intext_pattern(std::wstring _text);

    std::vector<duckx::Run> get_runs(duckx::Paragraph& _par);
    std::wstring get_wstring_paragraph_text(duckx::Paragraph& _par);
    std::wstring get_wstring_paragraph_text(duckx::Paragraph& _par, unsigned int& count);
    bool PaintsNumberer::is_paints_setted();

    std::unique_ptr<duckx::Document> _document;
    std::unordered_set<PatternTitle> _paint_patterns;
    std::map<unsigned int, std::vector<PaintPoint>> _setted_paints;

    unsigned int _paints_count, _max_numeration, _min_numeration;

    PATTERNS patterns_of_elements;
};

#endif