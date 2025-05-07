#include "header/basic/string_int_map.h"

StringIntMap::StringIntMap()
{
    clear_all_data();
}

void StringIntMap::add_new_string(const std::string& str)
{
    if (get_index_of_string(str) == INDEX_NOT_EXIST)
    {
        size_t n = get_map_size();
        int2str_vector.emplace_back(str);
        str2int_map.emplace(str, n);
    }
}

void StringIntMap::clear_all_data() noexcept
{
    int2str_vector.clear();
    str2int_map.clear();
    add_new_string("");
}

size_t StringIntMap::operator[](const std::string& str) const noexcept
{
    return get_index_of_string(str);
}

size_t StringIntMap::get_index_of_string(const std::string& str) const noexcept
{
    auto iter = str2int_map.find(str);
    return (iter != str2int_map.end()) ? iter->second : INDEX_NOT_EXIST;
}

std::string StringIntMap::operator[](size_t index) const noexcept
{
    return get_string_of_index(index);
}

std::string StringIntMap::get_string_of_index(size_t index) const noexcept
{
    if (index < get_map_size())
        return int2str_vector[index];
    else
        return "INVALID STRING FROM StringIntMap";
}

size_t StringIntMap::get_map_size() const noexcept
{
    return int2str_vector.size();
}

