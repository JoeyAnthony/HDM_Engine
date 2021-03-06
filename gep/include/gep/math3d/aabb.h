#pragma once

#include "gep/math3d/vec3.h"

namespace gep
{
    template<size_t dim>
    struct numVerticesHelper
    {
        static const size_t value = numVerticesHelper<dim-1>::value * 2;
    };

    template <>
    struct numVerticesHelper<1>
    {
        static const size_t value = 2;
    };

    /// \brief a axis aligned box
    template <typename T>
    struct AxisAlignedBox_t
    {
    private:
        T m_min,m_max;

    public:

        /// \brief default constructor
        inline AxisAlignedBox_t() {}

        /// \brief no initialization constructor
        inline AxisAlignedBox_t(DoNotInitialize arg) : m_min(DO_NOT_INITIALIZE), m_max(DO_NOT_INITIALIZE) {}

        /// \brief constructor
        ///  \param m_min
        ///    the m_minimum bounds of the box
        ///  \param m_max
        ///    the m_maximum bounds of the box
        inline AxisAlignedBox_t(const T& m_min, const T& m_max)
        {
            this->m_min = m_min;
            this->m_max = m_max;
            GEP_ASSERT(isValid(), "invalid bounding box");
        }

        /// \brief returns the m_minimum bounds
        inline const T getMin() const { return m_min; }

        /// \brief returns the m_maximum bounds
        inline const T getMax() const { return m_max; }

        /// \brief returns the size of the box
        const T size() const
        {
            return (m_max - m_min);
        }

        /// \brief computes all corner vertices of the box
        /// \param vertices
        ///   the resulting vertices (out)
        void getVertices(T (&vertices)[numVerticesHelper<T::dimension>::value]) const
        {
            //
            // TODO implement me!
            //
        }

        /// \brief returns a bounding box moved by a given amount
        const AxisAlignedBox_t<T> move(const T& amount) const
        {
			return AxisAlignedBox_t<T>(m_min + amount, m_max + amount);
        }

        /// \brief checks if a point is within the box
        bool contains(const T& rh) const
        {
            
            return true;
        }

		bool insidePlane(vec2 p, vec2 min, vec2 max)
        {
	        
        }

		float areaTriangle(vec2 p, vec2 t1, vec2 t2)
        {
			float lengthB = cos(Q_rsqrt(t1.x*t1.x + t1.y*t1.y) / Q_rsqrt(t2.x*t2.x + t2.y*t2.y));
			float lengthA = cos(Q_rsqrt(t1.x*t1.x + t1.y*t1.y) / Q_rsqrt(t2.x*t2.x + t2.y*t2.y));
			
        }

        /// \brief checks if a another bounding is completely inside
        bool contains(const AxisAlignedBox_t<T>& rh) const
        {
            //
            // TODO implement me!
            //
            return true;
        }

        /// \brief checks if this bounding box intersects another one
        bool intersects(const AxisAlignedBox_t<T>& rh) const
        {
            return true;
        }

        /// \brief helper method to check if the bounding box is valid
        bool isValid() const
        {
            for(size_t i=0; i < GEP_ARRAY_SIZE(m_min.data); i++)
                if(!(m_min.data[i] < m_max.data[i]))
                    return false;
            return true;
        }
    };

    typedef AxisAlignedBox_t<vec3> AABB;
}
