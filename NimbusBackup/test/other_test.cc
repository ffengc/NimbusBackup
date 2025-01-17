/*
 * Write by Yufc
 * See https://github.com/ffengc/NimbusBackup
 * please cite my project link: https://github.com/ffengc/NimbusBackup when you use this code
 */

// for other test

#include <iostream>
#include <ostream>
#include <unordered_map>
#include <vector>

template <class t1, class t2>
std::ostream& operator<<(std::ostream& out, std::pair<t1, t2> p) {
    out << p.first << " " << p.second << std::endl;
    return out;
}
template <class iter_type>
void print(iter_type begin, iter_type end) {
    auto it = begin;
    for (; it != end; ++it)
        std::cout << *it << " ";
    std::cout << std::endl;
}
template <class container_type>
void print(container_type container) {
    print(container.begin(), container.end());
}

void test_vec_constructor() {
    std::unordered_map<int, int> hash_map = { { 1, 2 }, { 2, 3 }, { 3, 4 } };
    std::vector<std::pair<int, int>> res(hash_map.begin(), hash_map.end());
    print(res.begin(), res.end());
}

int main() {
    test_vec_constructor();
    return 0;
}