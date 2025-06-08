#ifndef UG_SORT_VECTOR_HPP
#define UG_SORT_VECTOR_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>
#include <functional>

namespace ug_sort {

    /// @param Compare(v, v_cmp) return 0 means equal, > 0 means v is larger
    template <typename T, typename Compare>
    class sort_vector final {
        struct inner_e_t {
            T v;
            size_t prev_idx;
            size_t next_idx;
            size_t cnt;
        };

    public:
        using view_func_t  = std::function<void(const T&)>;

        sort_vector(Compare comp = Compare()): m_comp(comp) {}
        ~sort_vector() {}

        void push(T v)
        {
            if (!m_v.empty())
            {
                auto idx = head;
                while (m_comp(m_v[idx].v, v) < 0)
                {
                    if (idx == m_v[idx].next_idx)
                    {
                        break;
                    }
                    idx = m_v[idx].next_idx;
                }

                int cmpRet = m_comp(m_v[idx].v, v);
                if (cmpRet == 0)
                {
                    m_v[idx].cnt++;
                    return;
                }

                size_t cur_idx = m_v.size();
                m_v.emplace_back(v, cur_idx,cur_idx, 1);

                if (cmpRet < 0)
                {
                    tail = cur_idx;
                    m_v[idx].next_idx = cur_idx;
                    m_v[cur_idx].prev_idx = idx;
                    return;
                }

                if (idx == m_v[idx].prev_idx)
                {
                    head = cur_idx;
                    m_v[idx].prev_idx = cur_idx;
                    m_v[cur_idx].next_idx = idx;
                    return;
                }

                m_v[m_v[idx].prev_idx].next_idx = cur_idx;
                m_v[cur_idx].prev_idx = m_v[idx].prev_idx;
                m_v[cur_idx].next_idx = idx;
                m_v[idx].prev_idx = cur_idx;
            }
            else
            {
                m_v.emplace_back(v, 0, 0, 1);
                head = 0;
                tail = 0;
            }
        }

        void pop(T& out)
        {
            if (m_v.empty())
            {
                throw std::out_of_range("empty");
            }

            if (m_v.size() == 1)
            {
                out = m_v[0].v;
                m_v.clear();
                head = 0;
                tail = 0;
                return;
            }

            size_t last_idx = m_v.size() - 1;
            if (last_idx != tail)
            {
                if (m_v[last_idx].prev_idx != last_idx)
                {
                    m_v[m_v[last_idx].prev_idx].next_idx = tail;
                    m_v[m_v[last_idx].next_idx].prev_idx = tail;
                    m_v[m_v[tail].prev_idx].next_idx = last_idx;

                    std::swap(m_v[last_idx], m_v[tail]);
                    m_v[last_idx].next_idx = last_idx;
                    tail = last_idx;
                }
                else
                {
                    m_v[m_v[last_idx].next_idx].prev_idx = tail;
                    m_v[m_v[tail].prev_idx].next_idx = last_idx;

                    std::swap(m_v[last_idx], m_v[tail]);

                    m_v[last_idx].next_idx = last_idx;
                    m_v[tail].prev_idx = tail;

                    head = tail;
                    tail = last_idx;
                }
            }

            out = m_v[tail].v;
            if (m_v[tail].cnt == 1)
            {
                m_v[m_v[tail].prev_idx].next_idx = m_v[tail].prev_idx;
                m_v.erase(m_v.begin() + tail);
                tail = m_v[tail].prev_idx;
            }
            else
            {
                m_v[tail].cnt--;
            }
        }

        void view_iter(view_func_t func)
        {
            if (m_v.empty())
            {
                return;
            }

            size_t idx = head;
            while (idx != tail)
            {
                func(m_v[idx].v);
                idx = m_v[idx].next_idx;
            }
            func(m_v[idx].v);
        }

    private:
        std::vector<inner_e_t> m_v;
        size_t head;
        size_t tail;
        Compare m_comp;
    };
};

#endif // UG_SORT_VECTOR_HPP