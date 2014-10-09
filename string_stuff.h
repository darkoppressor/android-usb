#ifndef string_stuff_h
#define string_stuff_h

#include <string>
#include <sstream>

class String_Stuff{
public:

    std::string first_letter_capital(std::string str_input);

    std::string capitalize_all_words(std::string str_input);

    std::string upper_case(std::string str_input);

    std::string lower_case(std::string str_input);

    std::string underscore_to_space(std::string str_input);

    //Prepares newlines (\n) in a string to be written to disk.
    std::string add_newlines(std::string str_input);

    //Process newlines (\n) in a string that has been read from disk.
    std::string process_newlines(std::string str_input);

    //Returns the number of newlines in the passed string.
    int newline_count(std::string str_input);

    //Returns the length of the last line in the passed string.
    //Simply returns the length of the string if it has only one line.
    int length_of_last_line(std::string str_input);

    //Returns the length of the longest line in the passed string.
    int longest_line(std::string str_input);

    std::string erase_first_line(std::string str_input);

    bool is_number(std::string str_input);

    //Returns a string with the time represented by seconds.
    //If highest is true, only shows the highest unit of time possible.
    std::string time_string(int seconds,bool highest=false);

    bool string_to_bool(std::string get_string);
    std::string bool_to_string(bool get_bool);

    long string_to_long(std::string get_string);
    unsigned long string_to_unsigned_long(std::string get_string);
    double string_to_double(std::string get_string);

    template<class Number_Type>
    inline std::string num_to_string(Number_Type number,int precision=10,bool show_point=false,std::string notation="default"){
        std::stringstream strstream("");

        strstream.precision(precision);

        if(show_point){
            strstream.setf(std::ios::showpoint);
        }

        if(notation=="fixed"){
            strstream<<std::fixed;
        }
        else if(notation=="scientific"){
            strstream<<std::scientific;
        }

        strstream<<number;

        return strstream.str();
    }

    template<class Number_Type>
    inline std::string num_to_roman_numeral(Number_Type number){
        std::string message="";

        int num=(int)number;

        if(num>=4000){
            int x=(num-num%4000)/1000;
            message="("+num_to_roman_numeral(x)+")";
            num%=4000;
        }

        const std::string roman[13]={"M","CM","D","CD","C","XC","L","XL","X","IX","V","IV","I"};
        const int decimal[13]={1000,900,500,400,100,90,50,40,10,9,5,4,1};

        for(int i=0;i<13;i++){
            while(num>=decimal[i]){
                num-=decimal[i];
                message+=roman[i];
            }
        }

        return message;
    }
};

#endif
