#ifndef SOILCODEPARSER_H
#define SOILCODEPARSER_H

#include <soilstring.h>

class SoilCodeParser : public SoilObject
{
public:

    SoilString orig_filename;
    SoilString orig_code;
    SoilString pure_code;
    SoilString logger_code;

    //the index is mapped to the chars in pure_code, the value in the position in orig_code
    int * pure_pos_in_orig;
    bool loaded;

    SoilCodeParser();
    bool load_code_from_file(const SoilString& file);
    void parse_pure_code();//pure_code doesn't include strings or comments
    void load_cpp_function_logger(const SoilString& orig_filename);
    void unload();
    void append_log_to_logger_code(int log_n, const SoilString& short_filename, const SoilString& func_name, bool with_end);
};


#endif//SOILCODEPARSER_H