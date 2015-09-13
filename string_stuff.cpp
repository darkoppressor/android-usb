/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "string_stuff.h"
#include "world.h"

#include <vector>

#include <boost/algorithm/string.hpp>

using namespace std;

string String_Stuff::first_letter_capital(string str_input){
    if(str_input.length()>0){
        string first_letter="";
        first_letter+=str_input[0];

        boost::algorithm::to_upper(first_letter);

        str_input.erase(str_input.begin());
        str_input.insert(0,first_letter);
    }

    return str_input;
}

string String_Stuff::capitalize_all_words(string str_input){
    string message="";

    vector<string> words;
    boost::algorithm::split(words,str_input,boost::algorithm::is_any_of(" "));

    for(int i=0;i<words.size();i++){
        words[i]=first_letter_capital(words[i]);

        message+=words[i];

        if(i<words.size()-1){
            message+=" ";
        }
    }

    return message;
}

string String_Stuff::upper_case(string str_input){
    boost::algorithm::to_upper(str_input);

    return str_input;
}

string String_Stuff::lower_case(string str_input){
    boost::algorithm::to_lower(str_input);

    return str_input;
}

string String_Stuff::underscore_to_space(string str_input){
    return boost::algorithm::replace_all_copy(str_input,"_"," ");
}

string String_Stuff::add_newlines(string str_input){
    string newline="\\";
    newline+="n";

    boost::algorithm::replace_all(str_input,"\xA",newline);

    return str_input;
}

string String_Stuff::process_newlines(string str_input){
    string newline="\\";
    newline+="n";

    boost::algorithm::replace_all(str_input,newline,"\xA");

    return str_input;
}

int String_Stuff::newline_count(string str_input){
    int newlines=0;
    string newline="\xA";

    for(int i=0;i<str_input.length();i++){
        if(str_input[i]==newline[0]){
            newlines++;
        }
    }

    return newlines;
}

int String_Stuff::length_of_last_line(string str_input){
    vector<string> lines;
    boost::algorithm::split(lines,str_input,boost::algorithm::is_any_of("\xA"));

    return lines[lines.size()-1].length();
}

int String_Stuff::longest_line(string str_input){
    vector<string> lines;
    boost::algorithm::split(lines,str_input,boost::algorithm::is_any_of("\xA"));

    int longest_line_length=0;
    for(int i=0;i<lines.size();i++){
        if(lines[i].length()>longest_line_length){
            longest_line_length=lines[i].length();
        }
    }

    return longest_line_length;
}

string String_Stuff::erase_first_line(string str_input){
    string newline="\xA";

    for(int i=0;i<str_input.length();i++){
        if(str_input[i]==newline[0]){
            str_input.erase(0,1);

            break;
        }
        else{
            str_input.erase(0,1);

            i--;
        }
    }

    return str_input;
}

bool String_Stuff::is_number(string str_input){
    if(str_input.length()==0 || (str_input.length()==1 && str_input[0]=='-')){
        return false;
    }

    for(int i=0;i<str_input.length();i++){
        if(!isdigit(str_input[i]) && (i!=0 || (i==0 && str_input[i]!='-'))){
            return false;
        }
    }

    return true;
}

string String_Stuff::time_string(int seconds,bool highest){
    string text="";

    int minutes=0;
    int hours=0;
    int days=0;
    int weeks=0;
    int months=0;
    int years=0;

    for(int i=seconds;i>=60;){
        minutes++;
        i-=60;
        if(i<60){
            seconds=i;
        }
    }
    for(int i=minutes;i>=60;){
        hours++;
        i-=60;
        if(i<60){
            minutes=i;
        }
    }
    for(int i=hours;i>=24;){
        days++;
        i-=24;
        if(i<24){
            hours=i;
        }
    }
    for(int i=days;i>=7;){
        weeks++;
        i-=7;
        if(i<7){
            days=i;
        }
    }
    for(int i=weeks;i>=4;){
        months++;
        i-=4;
        if(i<4){
            weeks=i;
        }
    }
    for(int i=months;i>=12;){
        years++;
        i-=12;
        if(i<12){
            months=i;
        }
    }

    if(years>0){
        text+=num_to_string(years)+" year";
        if(years!=1){
            text+="s";
        }
        if(!highest){
            text+=", ";
        }
    }
    if((!highest || years<=0) && months>0){
        text+=num_to_string(months)+" month";
        if(months!=1){
            text+="s";
        }
        if(!highest){
            text+=", ";
        }
    }
    if((!highest || (years<=0 && months<=0)) && weeks>0){
        text+=num_to_string(weeks)+" week";
        if(weeks!=1){
            text+="s";
        }
        if(!highest){
            text+=", ";
        }
    }
    if((!highest || (years<=0 && months<=0 && weeks<=0)) && days>0){
        text+=num_to_string(days)+" day";
        if(days!=1){
            text+="s";
        }
        if(!highest){
            text+=", ";
        }
    }
    if((!highest || (years<=0 && months<=0 && weeks<=0 && days<=0)) && hours>0){
        text+=num_to_string(hours)+" hour";
        if(hours!=1){
            text+="s";
        }
        if(!highest){
            text+=", ";
        }
    }
    if((!highest || (years<=0 && months<=0 && weeks<=0 && days<=0 && hours<=0)) && minutes>0){
        text+=num_to_string(minutes)+" minute";
        if(minutes!=1){
            text+="s";
        }
        if(!highest){
            text+=", ";
        }
    }
    if(!highest || (years<=0 && months<=0 && weeks<=0 && days<=0 && hours<=0 && minutes<=0)){
        text+=num_to_string(seconds)+" second";
        if(seconds!=1){
            text+="s";
        }
    }

    return text;
}

bool String_Stuff::string_to_bool(string get_string){
    if(get_string=="true" || get_string=="1"){
        return true;
    }
    else if(get_string=="false" || get_string=="0"){
        return false;
    }
    else{
        message_log.add_error("Invalid value for string_to_bool: '"+get_string+"'");

        return false;
    }
}

string String_Stuff::bool_to_string(bool get_bool){
    if(get_bool){
        return "true";
    }
    else{
        return "false";
    }
}

long String_Stuff::string_to_long(string get_string){
    return strtol(get_string.c_str(),NULL,0);
}

unsigned long String_Stuff::string_to_unsigned_long(string get_string){
    return strtoul(get_string.c_str(),NULL,0);
}

double String_Stuff::string_to_double(string get_string){
    return strtod(get_string.c_str(),NULL);
}
