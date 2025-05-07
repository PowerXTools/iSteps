#include "header/model/model_var_table.h"
#include "header/basic/utility.h"
#include <iostream>

using namespace std;

MODEL_VAR_TABLE::MODEL_VAR_TABLE()
{
    clear();
}

MODEL_VAR_TABLE::~MODEL_VAR_TABLE()
{
    ;
}

void MODEL_VAR_TABLE::clear()
{
    var_index2name_map.clear();
    var_name2index_map.clear();
}

void MODEL_VAR_TABLE::add_variable_name_index_pair(string var_name, size_t var_index)
{
    if(var_name!="")
    {
        var_name = string2upper(var_name);
        map<string, size_t>::const_iterator iter_name = var_name2index_map.begin();
        map<size_t, string>::const_iterator iter_index = var_index2name_map.begin();
        iter_name = var_name2index_map.find(var_name);
        iter_index = var_index2name_map.find(var_index);
        if(iter_name==var_name2index_map.end() and iter_index==var_index2name_map.end())
        {// var_name and var_index not exist
            var_name2index_map.insert(pair<string, size_t>(var_name, var_index));
            var_index2name_map.insert(pair<size_t, string>(var_index, var_name));
        }
        else
        {// var_name or var_index already exists
            return;
        }
    }
}

size_t MODEL_VAR_TABLE::size() const
{
    return var_index2name_map.size();
}

size_t MODEL_VAR_TABLE::operator[](const string var_name) const
{
    return get_variable_index_from_variable_name(var_name);
}

string MODEL_VAR_TABLE::operator[](const size_t var_index) const
{
    return get_variable_name_from_variable_index(var_index);
}

size_t MODEL_VAR_TABLE::get_variable_index_from_variable_name(string var_name) const
{
    var_name = string2upper(var_name);
    map<string, size_t>::const_iterator iter = var_name2index_map.begin();

    iter = var_name2index_map.find(var_name);
    if(iter!=var_name2index_map.end())
        return iter->second;
    else
        return INDEX_NOT_EXIST;
}

string MODEL_VAR_TABLE::get_variable_name_from_variable_index(size_t var_index) const
{
    map<size_t, string>::const_iterator iter = var_index2name_map.begin();
    iter = var_index2name_map.find(var_index);
    if(iter!=var_index2name_map.end())
        return iter->second;
    else
        return "";
}
