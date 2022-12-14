#include "soilcodeparser.h"


SoilCodeParser::SoilCodeParser()
{
    pure_pos_in_orig = NULL;
    loaded = false;
}
//-------------------------------------------------------------- LOAD CODE
bool SoilCodeParser::load_code_from_file(const SoilString& filename)
{
    bool res = orig_code.load_from_file(filename);
    if(res < 0){return false;}
    pure_code = "";
    if(loaded && pure_pos_in_orig != NULL){delete[] pure_pos_in_orig;}
    pure_pos_in_orig = new int[orig_code.get_length()];

    parse_pure_code();
    
    return true;
}
//-------------------------------------------------------------- PARSE PURE CODE
//pure_code doesn't include strings or comments or precompiler # lines
void SoilCodeParser::parse_pure_code()
{
    if(orig_code.get_length() == 0){return;}
    int i = 0;
    SOIL_CHAR cur_sch = SoilString::c_char_to_soil_char(' ');
    SOIL_CHAR next_sch;

    SOIL_CHAR hash_sch = SoilString::c_char_to_soil_char('#');
    SOIL_CHAR fslash_sch = SoilString::c_char_to_soil_char('/');
    SOIL_CHAR ast_sch = SoilString::c_char_to_soil_char('*');
    SOIL_CHAR squote_sch = SoilString::c_char_to_soil_char('\'');
    SOIL_CHAR dquote_sch = SoilString::c_char_to_soil_char('"');
    SOIL_CHAR bslash_sch = SoilString::c_char_to_soil_char('\\');
    SOIL_CHAR br_sch = SoilString::c_char_to_soil_char('\n');
    SOIL_CHAR open_brace_sch = SoilString::c_char_to_soil_char('{');
    SOIL_CHAR close_brace_sch = SoilString::c_char_to_soil_char('}');

    bool in_pure_code = true;
    bool in_prep = false;
    bool in_line_com = false;
    bool in_block_com = false;
    bool in_single_quote = false;
    bool in_double_quote = false;
    bool escape = false;

    int p = 0;
    int b = 0;
    int l = 0;

    int end = orig_code.get_length();
    next_sch = orig_code.at(i);

    while(i < end)
    {
        
        cur_sch = orig_code.at(i);
        if(i < end - 1)
        {
            next_sch = orig_code.at(i + 1);
        }else{
            next_sch = SoilString::c_char_to_soil_char(' ');
        }
        if(in_pure_code)
        {
            if(SoilString::soil_char_are_equal(cur_sch,hash_sch)){in_prep = true;in_pure_code = false;}
            if(SoilString::soil_char_are_equal(cur_sch,fslash_sch) && SoilString::soil_char_are_equal(next_sch,fslash_sch)){in_line_com = true;in_pure_code = false;}
            if(SoilString::soil_char_are_equal(cur_sch,fslash_sch) && SoilString::soil_char_are_equal(next_sch,ast_sch)){in_block_com = true;in_pure_code = false;}
            if(SoilString::soil_char_are_equal(cur_sch,dquote_sch)){in_double_quote = true;in_pure_code = false;}
            if(SoilString::soil_char_are_equal(cur_sch,squote_sch)){in_single_quote = true;in_pure_code = false;}

            if(!in_line_com && !in_block_com && !in_prep)
            {
                pure_code.append(cur_sch);
                pure_pos_in_orig[p] = i;
                p++;
            }
            i++;

            continue;
        }

        if(in_prep)
        {
            if(SoilString::soil_char_are_equal(cur_sch,br_sch)){in_pure_code = true;}
        }
        if(in_line_com)
        {
            if(SoilString::soil_char_are_equal(cur_sch,br_sch)){in_pure_code = true;}
        }
        if(in_block_com)
        {
            if(SoilString::soil_char_are_equal(cur_sch,ast_sch) && SoilString::soil_char_are_equal(next_sch,fslash_sch)){in_pure_code = true;i++;}
        }
        if(in_single_quote)
        {
            if(SoilString::soil_char_are_equal(cur_sch,bslash_sch)){escape = !escape;}else{escape = false;}
            if(!escape && SoilString::soil_char_are_equal(cur_sch,squote_sch)){in_pure_code = true;}
        }
        if(in_double_quote)
        {
            if(SoilString::soil_char_are_equal(cur_sch,bslash_sch)){escape = !escape;}else{escape = false;}
            if(!escape && SoilString::soil_char_are_equal(cur_sch,dquote_sch)){in_pure_code = true;}
        }

        //RIGHT AFTER LEAVING OTHER TEXT CONTEXTS
        if(in_pure_code)
        {
            //block_com is the only context that doesn't need to add its ending char
            if(!in_block_com)
            {
                pure_code.append(cur_sch);
                pure_pos_in_orig[p] = i;
                p++;
            }
            in_prep = false;
            in_line_com = false;
            in_block_com = false;
            in_single_quote = false;
            in_double_quote = false;
        }
        i++;
    }

}
//-------------------------------------------------------------- LOAD CPP FUNCTION LOGS
void SoilCodeParser::load_cpp_function_logger(const SoilString& orig_filename)
{
    //orig_filename here is not to access it, but just to write it to the logs

    SoilString short_orig_filename;
    SoilString fslash("/");

    short_orig_filename = orig_filename.section(fslash,-2,-1);

    logger_code = "";
    SoilString line_str;
    int line_pos = 0;
    int line_len = 0;
    bool in_func = false;

    int search_brace_res;
    int search_res;
    int search_pos = 0;
    bool getting_braces = true;
    bool before_braces = true;

    SoilString open_brace_str("{");
    SoilString close_brace_str("}");
    int line_open_brace_n = 0;
    int line_close_brace_n = 0;

    bool got_start = false;

    int return_pos = 0;
    bool got_return = false;

    SoilString func_name="";

    int func_start_pos = 0;
    int func_end_pos = 0;

    int nest_i = 0;
    bool place_end_logger = true;

    int copy_orig_start = 0;
    int copy_orig_end = 0;

    int copy_dest_start = 0;
    int copy_dest_end = 0;

    int log_n = 0;
    int i = 0;
    while(i < 100000)
    {
        
        //LOOP THROUGH LINES
        line_len = line_str.load_from_str_line(pure_code, line_pos);
        

        if(line_len <= 0){break;}

        bool removing_br = true;
        while(removing_br)
        {
            SOIL_CHAR last_line_ch = line_str.at(-1);
            if(last_line_ch.N == 1)
            {
                if(last_line_ch.B[0] < 32)
                {
                    line_str.remove(-1,-1);
                }else{
                    removing_br = false;
                }
            }else{
                removing_br = false;
            }
            if(line_str.get_length() <= 0){removing_br = false;}
        }
        if(line_len <= 0){break;}

        int c = 0;
        //CHECKING FOR FUNCTION TITLE
        if(line_len > 6 && nest_i == 0)
        {
            if(line_str.count_sub_str("(", false, false) > 0 && line_str.count_sub_str(")", false, false) > 0 && line_str.count_sub_str(";", false, false) == 0)
            {
                in_func=true;
                got_start = false;
                func_name = line_str;
                place_end_logger = true;
                //GET FUNC NAME
                
            }
        }

        //CHECKING CURLY BRACES IN LINE
        line_open_brace_n = line_str.count_sub_str("{",false,false);
        line_close_brace_n = line_str.count_sub_str("}",false,false);
        nest_i += line_open_brace_n;
        nest_i -= line_close_brace_n;

        if(in_func)
        {
            if(!got_start)
            {
                if(line_open_brace_n > 0)
                {
                    func_start_pos = line_str.search_str("{",0,false,false);
                    func_start_pos++;
                    got_start = true;
                    
                    //INSERT LOGGER HERE
                    copy_orig_end = pure_pos_in_orig[line_pos + func_start_pos];
                    
                    logger_code.append(orig_code.section(copy_orig_start, (copy_orig_end - copy_orig_start)));

                    append_log_to_logger_code(log_n, short_orig_filename, func_name, false);
                    log_n++;

                    copy_orig_start = copy_orig_end;
                }
            }

            if(got_start)
            {
                if(nest_i == 0 && place_end_logger)
                {
                    func_end_pos = line_str.search_str("}",-1,false,false,false);

                    //INSERT LOGGER HERE
                    copy_orig_end = pure_pos_in_orig[line_pos + func_end_pos];
                    logger_code.append(orig_code.section(copy_orig_start, (copy_orig_end - copy_orig_start)));
                   
                    append_log_to_logger_code(log_n, short_orig_filename, func_name, true);
                    log_n++;

                    copy_orig_start = copy_orig_end;
                    got_start = false;
                    in_func = false;
                    
                }
            }
            if((return_pos = line_str.search_str("return", 0, true, true)) >= 0)
            {
                got_return = true;
                SOIL_CHAR after_ch = line_str.at(return_pos + 6);
                if(return_pos > 0)
                {
                    SOIL_CHAR before_ch = line_str.at(return_pos - 1);
    
                    if(SoilString::soil_char_is_alphanum_or_(before_ch) || after_ch.N != 1)
                    {
                        got_return = false;
                    }
                    
                }
                if(after_ch.B[0] != ' ' && after_ch.B[0] != ';'){got_return = false;}
                if(got_return)
                {
                    int return_semicolon_pos = line_str.search_str(";", return_pos, true, true);
                    if(return_semicolon_pos > 5)
                    {

                        int after_return_bracket_n = line_str.count_sub_str("}", return_pos,line_str.get_length(), false,false);
                        if((nest_i + after_return_bracket_n) <= 1){place_end_logger = false;}
                        //INSERT LOGGER HERE
                        copy_orig_end = pure_pos_in_orig[line_pos + return_pos];
                        logger_code.append(orig_code.section(copy_orig_start, (copy_orig_end - copy_orig_start)));
                        
                        logger_code.append("\n{");
                        append_log_to_logger_code(log_n, short_orig_filename, func_name, true);
                        copy_orig_start = copy_orig_end;
                        copy_orig_end = pure_pos_in_orig[line_pos + return_semicolon_pos + 1];
                        logger_code.append(orig_code.section(copy_orig_start, (copy_orig_end - copy_orig_start)));
                        logger_code.append("\n}\n");

                        log_n++;

                        copy_orig_start = copy_orig_end;
                    }
                }//if got_return
            }//search_str return
        }//end in_func

        line_pos += line_len;

        i++;
    }
    copy_orig_end = orig_code.get_length();
    logger_code.append(orig_code.section(copy_orig_start, (copy_orig_end - copy_orig_start)));

}
//-------------------------------------------------------------- UNLOAD CODE
void SoilCodeParser::unload()
{
    orig_code = "";
    pure_code = "";
    logger_code = "";
    if(loaded && pure_pos_in_orig != NULL)
    {
        delete[] pure_pos_in_orig;
        pure_pos_in_orig = NULL;
    }
}
//-------------------------------------------------------------- APPEND LOG TO LOGGER CODE
void SoilCodeParser::append_log_to_logger_code(int log_n, const SoilString& short_filename, const SoilString& func_name, bool with_end)
{

    SoilString log_file_var("LOGGER_FILE_");
    log_file_var.append(log_n);

    logger_code.append("\nFILE * ");
    logger_code.append(log_file_var);
    logger_code.append(" = fopen(\"LOGGER_FILE.txt\", \"a\");\nif(");
    logger_code.append(log_file_var);
    logger_code.append(" != NULL)\n{\n \tfprintf(");
    logger_code.append(log_file_var);
    logger_code.append(",\"\\n");
    logger_code.append(short_filename);
    logger_code.append(" : ");
    logger_code.append(func_name);

    if(with_end)
    {
        logger_code.append(" : END");
    }

    logger_code.append("\");\n}\nif(");
    logger_code.append(log_file_var);
    logger_code.append(" != NULL){fclose(");
    logger_code.append(log_file_var);
    logger_code.append(");}\n");


}