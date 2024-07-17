#pragma once

#include <algorithm>
#include <stdexcept>
#include <type_traits>

namespace efll
{
    template <typename T>
    class FuzzyGroup
    {
        static_assert(std::is_floating_point<T>::value);

      public:
        FuzzyGroup() = default;
        FuzzyGroup(T a_, T b_, T c_, T d_) :
            a{a_},
            b{b_},
            c{c_},
            d{d_}
        {
            if (a > b || b > c || c > d)
            {
                throw std::invalid_argument("fuzzy control points must be monotonically increasing");
            }
        }

        T get_A() const { return a; }
        T get_B() const { return b; }
        T get_C() const { return c; }
        T get_D() const { return d; }

        void compute_pertinence(T crispness)
        {
            if (crispness < a)
            {
                y = static_cast<T>(all_lesser());
                return;
            }

            if (crispness < b)
            {
                T slope = 1.0 / (b - a);
                y       = slope * (crispness - b) + 1.0;
                return;
            }

            // The original library switches from exclusive to inclusive here.
            if (crispness <= c)
            {
                y = 1.0;
                return;
            }

            if (crispness <= d)
            {
                T slope = 1.0 / (c - d);
                y       = slope * (crispness - c) + 1.0;
                return;
            }

            y = static_cast<T>(all_greater());
        }

        /// @brief Raise the pertinence value of this group.
        /// @details Only applies the new value if it is greater than the current.
        /// @param pertinence The new pertinence value.
        void set_pertinence(T pertinence) { y = std::max(y, pertinence); }

        /// @brief Get the group's pertinence value.
        T get_pertinence() const { return y; }

        /// @brief Set the group pertinence back to zero.
        void reset_pertinence() { y = 0.0; }

        /// @brief Check if this group represents "everything smaller is true".
        bool all_lesser() const { return a == b && b != c && c != d; }

        /// @brief Check if this group represents "everything larger is true".
        bool all_greater() const { return a != b && b != c && c == d; }

      private:
        T a = 0.0;
        T b = 0.0;
        T c = 0.0;
        T d = 0.0;

        /// @brief Pertinence value.
        T y = 0.0;
    };
}  // namespace efll
