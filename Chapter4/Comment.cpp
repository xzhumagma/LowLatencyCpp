#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>

template <typename Func, typename... Args>
auto callFunction(Func &&func, Args &&...args)
{
    return std::forward<Func>(func)(std::forward<Args>(args)...);
}

inline auto add(int a, int b)
{
    return a + b;
}

inline auto sort(std::vector<int> &a)
{
    std::sort(a.begin(), a.end());
    return a;
}

int main()
{
    auto result = callFunction(add, 1, 2);
    std::cout << "Result: " << result << std::endl;
    std::vector<int> vec{3, 2, 1};
    auto result2 = callFunction(sort, vec);
    for (auto &i : result2)
    {
        std::cout << i << " ";
    }
    return 0;
}
