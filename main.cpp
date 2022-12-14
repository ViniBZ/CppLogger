#include <iostream>

#include <time.h>
#include <math.h>
#include <cstdio>
#include <ctime>
#include <stdio.h>

#include <filesystem>
#include "soilcodeparser.h"

namespace FS = std::filesystem;
//---------------------------------------------------------------- ITERATE IN DIR
int iterate_in_dir(FS::path fs_orig_dir, FS::path fs_dest_dir)
{
    //orig_dir
    //orig_file
    //dest_dir
    //dest_file


    int result = 0;
    int tmp_res = 0;
    if(FS::is_directory(fs_orig_dir))
    {
        FS::directory_iterator it(fs_orig_dir);
        FS::path entry;
        SoilString orig_filename;
        SoilString dest_dir = fs_dest_dir.c_str();
        SoilString short_filename;
        SoilString dest_filename;
        SoilString fslash('/');
        SoilString dot('.');
        SoilString ext;

        SoilCodeParser parser;


        int loop_n = 1;
        int loop_i = 0;
        // This is for avoiding conflict when renaming the files to UNDO the changes
    
        it = FS::directory_iterator(fs_orig_dir);
        for(auto& entry: it)
        {
            if(!FS::is_directory(entry))
            {
                
                orig_filename = entry.path().c_str();
                short_filename = orig_filename.section(fslash,-1,-1,false,false,false,false);
                
                ext = short_filename.section(dot, -1,-1);
                if(ext == "cpp")
                {
                    dest_filename = dest_dir;
                    dest_filename.append('/');
                    dest_filename.append(short_filename);

                    parser.load_code_from_file(orig_filename);
                    parser.load_cpp_function_logger(orig_filename);
                    parser.logger_code.save_to_file(dest_filename);

                    std::cout << "\nLOGGER PLACED IN:\n" << std::endl;
                    short_filename.printf_str(1,1);
                }
            }//not directory

        }//end for


    }//is directory

    return result;
}
//---------------------------------------------------------------- MAIN
int main(int argc, char * argv[])
{

    if(argc != 3)
    {
        std::cout << "\n\nUSAGE: SOIL_REVERSE ORIG_DIR DEST_DIR" << std::endl;
        std::cout << "\n" << std::endl;
        return 1;
    }

    
    FS::path orig_dir(argv[1]);
    FS::path dest_dir(argv[2]);
    FS::path entry;


    bool valid_folder = true;

    if(!FS::is_directory(orig_dir))
    {
        std::cout << "\nINVALID ORIG_DIR!" << std::endl;    
        valid_folder = false;
    }

    if(!FS::is_directory(dest_dir))
    {
        std::cout << "\nINVALID DEST_DIR!" << std::endl;
        valid_folder = false;
    }

    if(!valid_folder)
    {
        return 2;
    }

    if(are_strings_equal(argv[1],argv[2]))
    {
        std::cout << "\nUSE DIFFERENT DIRECTORIES!" << std::endl;
        return 4;
    }

    FS::directory_iterator it(orig_dir);    
    iterate_in_dir(orig_dir, dest_dir);

    std::cout << "\n" << std::endl;
    return 0;
}
