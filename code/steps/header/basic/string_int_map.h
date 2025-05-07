#ifndef STRING_INT_MAP_H
#define STRING_INT_MAP_H

#include <vector>
#include <string>
#include <unordered_map>
#include <cstddef> // for size_t

class StringIntMap
{
public:
    StringIntMap();
    ~StringIntMap() = default;

    void clear_all_data() noexcept;
    void add_new_string(const std::string& str);

    size_t operator[](const std::string& str) const noexcept;
    std::string operator[](size_t index) const noexcept;
    size_t get_map_size() const noexcept;

private:
    size_t get_index_of_string(const std::string& str) const noexcept;
    std::string get_string_of_index(size_t index) const noexcept;

    std::unordered_map<std::string, size_t> str2int_map;
    std::vector<std::string> int2str_vector;

    static constexpr size_t INDEX_NOT_EXIST = static_cast<size_t>(-1);
};

#endif // STRING_INT_MAP_H
