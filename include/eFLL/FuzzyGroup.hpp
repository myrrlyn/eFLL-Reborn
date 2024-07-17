#pragma once

#include <algorithm>
#include <cstdio>
#include <stdexcept>
#include <type_traits>

namespace efll
{
    /**
     * @brief A group of four values representing a constrained region of fuzzy
     * logic space.
     *
     * @details
     */
    template <typename T>
    class FuzzyGroup
    {
        static_assert(std::is_floating_point<T>::value);

      public:
        FuzzyGroup() = default;

        /**
         * @brief Constructs a group from four demarcation points.
         *
         * @note These points must be strictly monotonically increasing.
         *
         * This is the only exception-`throw`ing site in the codebase.
         */
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

        T compute_pertinence(T crispness)
        {
            fprintf(stderr, "2: computing pertinence with %f\n", crispness);
            if (crispness < a)
            {
                fprintf(stderr, "2: x < A\n");
                this->y = static_cast<T>(all_lesser());
            }
            else if (crispness < b)
            {
                fprintf(stderr, "2: A < x < B\n");
                T slope = 1.0 / (b - a);
                this->y = slope * (crispness - b) + 1.0;
            }
            // The original library switches from exclusive to inclusive here.
            else if (crispness <= c)
            {
                fprintf(stderr, "2: B < x < C\n");
                this->y = 1.0;
            }
            else if (crispness <= d)
            {
                fprintf(stderr, "2: C < x < D\n");
                T slope = 1.0 / (c - d);
                this->y = slope * (crispness - c) + 1.0;
            }
            else
            {
                fprintf(stderr, "2: D < x\n");
                this->y = static_cast<T>(all_greater());
            }

            fprintf(stderr, "2: y: %f\n", this->y);
            return this->y;
        }

        /// @brief Raise the pertinence value of this group.
        /// @details Only applies the new value if it is greater than the current.
        /// @param pertinence The new pertinence value.
        void set_pertinence(T pertinence)
        {
            fprintf(stderr, "2: changing pert to %f\n", std::max(this->y, pertinence));
            this->y = std::max(this->y, pertinence);
        }

        /// @brief Get the group's pertinence value.
        T get_pertinence() const
        {
            fprintf(stderr, "2: getting pertinence %f\n", this->y);
            return this->y;
        }

        /// @brief Set the group pertinence back to zero.
        void reset_pertinence()
        {
            fprintf(stderr, "2: resetting\n");
            this->y = 0.0;
        }

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
