/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "sorting.h"

#include <cmath>

using namespace std;

vector<GUI_Object> quick_sort(vector<GUI_Object> objects,bool sort_by_y){
    if(objects.size()<=1){
        return objects;
    }

    vector<GUI_Object> less_than;
    vector<GUI_Object> greater_than;
    vector<GUI_Object> result;

    int pivot=(int)floor((double)(objects.size()-1)/2.0);

    for(int i=0;i<objects.size();i++){
        if(i!=pivot){
            if((sort_by_y && objects[i].y<=objects[pivot].y) || (!sort_by_y && objects[i].x<=objects[pivot].x)){
                less_than.push_back(objects[i]);
            }
            else{
                greater_than.push_back(objects[i]);
            }
        }
    }

    vector<GUI_Object> sorted_less=quick_sort(less_than,sort_by_y);
    vector<GUI_Object> sorted_greater=quick_sort(greater_than,sort_by_y);

    result.insert(result.end(),sorted_less.begin(),sorted_less.end());
    result.push_back(objects[pivot]);
    result.insert(result.end(),sorted_greater.begin(),sorted_greater.end());

    return result;
}
