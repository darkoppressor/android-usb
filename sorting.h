/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef sorting_h
#define sorting_h

#include "engine_interface.h"

#include <vector>

std::vector<GUI_Object> quick_sort(std::vector<GUI_Object> objects,bool sort_by_y);

#endif
