#pragma once

#include <algorithm>
#include <cmath>
#include <type_traits>

#include "eFLL/FuzzyComposition.hpp"
#include "eFLL/FuzzyIo.hpp"

namespace efll
{
    template <typename T>
    class FuzzyOutput : public FuzzyIo<T>
    {
      public:
        FuzzyOutput() :
            FuzzyIo<T>()
        {
        }

        FuzzyOutput(std::size_t index) :
            FuzzyIo<T>(index)
        {
        }

        virtual ~FuzzyOutput() = default;

        void truncate()
        {
            m_composition.clear();
            for (auto& grp : this->m_groups)
            {
                if (grp.get_pertinence() <= 0.0)
                {
                    continue;
                }
                // Not a trapezoid
                if (grp.get_A() != grp.get_B())
                {
                    m_composition.add_point(grp.get_A(), 0.0);
                }
                // Triangle (B==C, A!=D)
                if (grp.get_B() == grp.get_C() && grp.get_A() != grp.get_D())
                {
                    if (grp.get_pertinence() >= 1.0)
                    {
                        m_composition.add_point(grp.get_B(), grp.get_pertinence());
                        m_composition.add_point(grp.get_D(), 0.0);
                    }
                    else
                    {
                        T new_xB = grp.get_B();
                        T new_yB = grp.get_pertinence();
                        rebuild({grp.get_A(), 0.0}, {grp.get_B(), 1.0}, {grp.get_A(), grp.get_pertinence()},
                                {grp.get_D(), grp.get_pertinence()}, new_xB, new_yB);
                        m_composition.add_point(new_xB, new_yB);

                        T new_xC = grp.get_C();
                        T new_yC = grp.get_pertinence();
                        rebuild({grp.get_C(), 1.0}, {grp.get_D(), 0.0}, {grp.get_A(), grp.get_pertinence()},
                                {grp.get_D(), grp.get_pertinence()}, new_xC, new_yC);
                        m_composition.add_point(new_xC, new_yC);
                    }
                }
                // Trapezoid
                else if (grp.get_B() != grp.get_C())
                {
                    if (grp.get_pertinence() >= 1.0)
                    {
                        m_composition.add_point(grp.get_B(), grp.get_pertinence());
                        m_composition.add_point(grp.get_C(), grp.get_pertinence());
                    }
                    else
                    {
                        // TODO(myrrlyn): deduplicate with above
                        T new_xB = grp.get_B();
                        T new_yB = grp.get_pertinence();
                        rebuild({grp.get_A(), 0.0}, {grp.get_B(), 1.0}, {grp.get_A(), grp.get_pertinence()},
                                {grp.get_D(), grp.get_pertinence()}, new_xB, new_yB);
                        m_composition.add_point(new_xB, new_yB);

                        T new_xC = grp.get_C();
                        T new_yC = grp.get_pertinence();
                        rebuild({grp.get_C(), 1.0}, {grp.get_D(), 0.0}, {grp.get_A(), grp.get_pertinence()},
                                {grp.get_D(), grp.get_pertinence()}, new_xC, new_yC);
                        m_composition.add_point(new_xC, new_yC);
                    }
                }
                // Not a triangle, not a trapezoid, singleton.
                else
                {
                    m_composition.add_point(grp.get_B(), 0.0);
                    m_composition.add_point(grp.get_B(), grp.get_pertinence());
                    m_composition.add_point(grp.get_B(), 0.0);
                }

                if (grp.get_C() != grp.get_D())
                {
                    m_composition.add_point(grp.get_D(), 0.0);
                }
            }
            m_composition.build();
        }

        T get_crispness_output() { return m_composition.compute(); }

        /**
         * @brief Sort the groups in ascending order by their A point.
         *
         * @details The original code indicates that this is a "simple bubble sort". myrrlyn is uncertain if he rebuilt
         * it correctly.
         */
        void sort()
        {
            auto end = this->m_groups.end();
            for (auto const& one : this->m_groups)
            {
                for (auto two = this->m_groups.begin(); two != this->m_groups.end(); two = std::next(two))
                {
                    auto three = std::next(two);
                    if (three == end)
                    {
                        break;
                    }
                    if (two->get_A() > three->get_A())
                    {
                        std::swap(*two, *three);
                    }
                }
            }
        }

        FuzzyComposition<T>& get_composition() { return m_composition; }

      private:
        FuzzyComposition<T> m_composition{};

        bool rebuild(Point<T> a, Point<T> b, Point<T> c, Point<T> d, T& point, T& pertinence)
        {
            T denominator = std::fabs((d.y - c.y) * (b.x - a.x) - (d.x - c.x) * (b.y - a.y));
            T numerator_a = std::fabs((d.x - c.x) * (a.y - c.y) - (d.y - c.y) * (a.x - c.x));
            T numerator_b = std::fabs((b.x - a.x) * (a.y - c.y) - (b.y - a.y) * (a.x - c.x));

            if (denominator < FuzzyComposition<T>::EPSILON)
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

            point      = a.x + slope_a * (b.x - a.x);
            pertinence = a.y + slope_a * (b.y - a.y);
            return true;
        }
    };
}  // namespace efll
