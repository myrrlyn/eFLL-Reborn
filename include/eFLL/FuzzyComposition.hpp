#pragma once

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iterator>
#include <list>
#include <type_traits>

namespace efll
{

    template <typename T>
    struct Point
    {
        static_assert(std::is_floating_point<T>::value);

        /// @brief Originally `point`.
        T x;

        /// @brief Originally `pertinence`.
        T y;

        bool operator==(Point<T> const& rhs) const { return x == rhs.x && y == rhs.y; }
    };

    template <typename T>
    class FuzzyComposition
    {
      public:
        static_assert(std::is_floating_point<T>::value);

        FuzzyComposition()  = default;
        ~FuzzyComposition() = default;

        void add_point(T value, T pertinence) { m_points.emplace_back(value, pertinence); }

        bool contains_point(T value, T pertinence) const
        {
            // TODO(myrrlyn): Replace with std::find or std::any_of?
            for (Point<T> const& pt : m_points)
            {
                if (pt.x == value && pt.y == pertinence)
                {
                    return true;
                }
            }
            return false;
        }

        void build()
        {
            auto head   = m_points.begin();
            auto cursor = m_points.begin();
            // Find the local maximum
            while (cursor != m_points.end())
            {
                // This is what it's called in the original.
                auto tmp = cursor;
                while (tmp != head)
                {
                    if (cursor->x < tmp->x)
                    {
                        break;
                    }
                    tmp = std::prev(tmp);
                }

                while (tmp != head)
                {
                    if (rebuild(cursor, std::next(cursor), tmp, std::prev(tmp)))
                    {
                        cursor = m_points.begin();
                        break;
                    }
                    tmp = std::prev(tmp);
                }
                cursor = std::next(cursor);
            }
        }

        /// @brief Computes the centroid of the composition's area.
        T compute()
        {
            T numerator   = 0.0;
            T denominator = 0.0;

            if (m_points.empty() || denominator == 0.0)
            {
                return 0.0;
            }

            // The original code does not include the final element in the loop.
            auto tail = std::prev(m_points.end());
            auto head = m_points.begin();
            while (head != tail)
            {
                T area    = 0.0;
                T middle  = 0.0;
                auto next = std::next(head);
                T dx      = next->x - head->x;

                // Singlet
                if (head->y != next->y && head->x == next->x)
                {
                    if (head->y > 0.0)
                    {
                        area   = head->y;
                        middle = head->x;
                    }
                }
                // Triangle
                else if (head->y == 0.0 || next->y == 0.0)
                {
                    T pertinence;
                    if (head->y > 0.0)
                    {
                        pertinence = head->y;
                    }
                    else
                    {
                        pertinence = next->y;
                    }
                    area   = (dx * pertinence) / 2.0;
                    middle = dx / 1.5;
                    if (head->y >= next->y)
                    {
                        middle /= 2.0;
                    }
                    middle += head->x;
                }
                else if (head->y > 0.0 && next->y > 0.0)
                {
                    // Square
                    if (head->y == next->y)
                    {
                        area = dx * head->y;
                    }
                    // Trapezoid
                    else
                    {
                        area = ((head->y + next->y) / 2.0) * dx;
                    }
                    middle = (dx / 2.0) + head->x;
                }

                numerator += middle * area;
                denominator += area;
                head = next;
            }

            return numerator / denominator;
        }

        void clear() { m_points.clear(); }

        std::size_t size() const { return m_points.size(); }

        static constexpr T EPSILON = 1.0e-3;

      private:
        using points = std::list<Point<T>>;
        std::list<Point<T>> m_points;

        bool rebuild(::std::list<Point<T>>::iterator a0,
                     ::std::list<Point<T>>::iterator a1,
                     ::std::list<Point<T>>::iterator b0,
                     ::std::list<Point<T>>::iterator b1)
        {
            T denominator = std::fabs((b1->y - b0->y) * (a1->x - a0->x) - (b1->x - b0->x) * (a1->y - a0->y));
            T numerator_a = std::fabs((b1->x - b0->x) * (a0->y - b0->y) - (b1->y - b0->y) * (a0->x - b0->x));
            T numerator_b = std::fabs((a1->x - a0->x) * (a0->y - b0->y) - (a1->y - a0->y) * (a0->x - b0->x));

            // If the denominator is nearly zero, there is no intersection.
            if (denominator < EPSILON)
            {
                return false;
            }

            T slope_a = numerator_a / denominator;
            T slope_b = numerator_b / denominator;

            // If either slope is outside 0-45deg, there is no intersection.
            if (slope_a <= 0.0 || slope_a >= 1.0 || slope_b <= 0.0 || slope_b >= 1.0)
            {
                return false;
            }

            Point<T> pt;
            pt.x = a0.x + slope_a * (a1.x - a0.x);
            pt.y = a0.y + slope_a * (a1.y - a0.y);
            // In the original code, the new point is dropped in between the two
            // A entries,
            m_points.insert(a1, pt);
            // Then the initial B point is re-assigned to be _after_ the new
            // point, and the trailing B point indicates marks the new point as
            // _its_ successor. However, this apparent cycle is actually only
            // used to delete the B region from the original sequence.
            m_points.erase(b0, b1);

            return true;
        }
    };

}  // namespace efll
