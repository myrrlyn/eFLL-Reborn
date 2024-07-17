#pragma once

#include <cstddef>
#include <cstdlib>
#include <type_traits>
#include <vector>

#include "eFLL/FuzzyGroup.hpp"

namespace efll
{
    template <typename T>
    class FuzzyIo
    {
      public:
        static_assert(std::is_floating_point<T>::value);

        FuzzyIo() = default;

        FuzzyIo(std::size_t index) :
            m_index{index}
        {
        }

        virtual ~FuzzyIo() = default;

        std::size_t get_index() const { return m_index; }

        void set_crispness(T crispness) { m_crispness = crispness; }
        T get_crispness() const { return m_crispness; }

        void push_group(FuzzyGroup<T> group) { m_groups.push_back(group); }

        template <class... Args>
        void emplace_group(Args&&... args)
        {
            m_groups.emplace_back(std::move(args)...);
        }

        void reset_all_groups()
        {
            for (FuzzyGroup<T>& grp : m_groups)
            {
                grp.reset_pertinence();
            }
        }

      protected:
        /// @brief An index within a larger collection.
        std::size_t m_index = 0;

        /// @brief A crispness value used to compute the pertinence for various
        /// groups.
        T m_crispness = 0.0;

        /// @brief A collection of fuzzy regions.
        std::vector<FuzzyGroup<T>> m_groups{};

        void clear_groups() { m_groups.clear(); }
    };
}  // namespace efll
