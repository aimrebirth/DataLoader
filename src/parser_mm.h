#pragma once

#include <vector>

class ParserMemoryManager
{
public:
    ~ParserMemoryManager()
    {
        for (auto &v : m)
            delete v;
    }

    template <class T, class ...Args>
    T *create(Args&&... args)
    {
        auto p = new T(std::forward<Args>(args)...);
        m.push_back(p);
        return p;
    }

private:
    std::vector<void*> m;
};
