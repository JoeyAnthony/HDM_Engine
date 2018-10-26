#pragma once

#include "gep/math3d/constants.h"
#include "gep/math3d/vec2.h"


namespace gep
{
    template <typename T>
    struct vec3_t {
        union {
            struct {
                T x,y,z;
            };
            T data[3];
        };

        typedef T component_t;
        static const size_t dimension = 3;

        /// \brief default constructor
        inline vec3_t()
        {
            //
            // TODO implement me!
            //
        }

        /// \brief constructor for a unitialized instance
        inline vec3_t(DoNotInitialize) {}

        /// \brief constructor
        inline vec3_t(T x, T y, T z)
        {
            //
            // TODO implement me!
            //
        }

        /// \brief constructor
        inline explicit vec3_t(T data[3])
        {
            //
            // TODO implement me!
            //
        }

        /// \brief constructor
        inline explicit vec3_t(const T xyz)
        {
            //
            // TODO implement me!
            //
        }

        /// \brief constructor
        inline vec3_t(vec2_t<T> xy, T z)
        {
            //
            // TODO implement me!
            //
        }

        /// \brief + operator for adding another 3 component vector
        inline const vec3_t<T> operator + (const vec3_t<T>& rh) const
        {
            //
            // TODO implement me!
            //
            return vec3_t<T>();
        }

        /// \brief - operator for subtracting another 3 component vector
        inline const vec3_t<T> operator - (const vec3_t<T>& rh) const
        {
            //
            // TODO implement me!
            //
            return vec3_t<T>();
        }

        /// \brief * operator for multiplying with a scalar
        inline const vec3_t<T> operator * (const T rh) const
        {
            //
            // TODO implement me!
            //
            return vec3_t<T>();
        }

        /// \brief * operator for multipying with another vector (component vise)
        inline const vec3_t<T> operator * (const vec3_t<T>& rh) const
        {
            //
            // TODO implement me!
            //
            return vec3_t<T>();
        }

        /// \brief / operator for dividing by a scalar
        inline const vec3_t<T> operator / (const T rh) const
        {
            //
            // TODO implement me!
            //
            return vec3_t<T>();
        }

        /// \brief / operator for dividing witha nother vector (component vise)
        inline const vec3_t<T> operator / (const vec3_t<T>& rh) const
        {
            //
            // TODO implement me!
            //
            return vec3_t<T>();
        }

        /// \brief += operator
        inline vec3_t<T>& operator += (const vec3_t<T>& rh)
        {
            //
            // TODO implement me!
            //
            return *this;
        }

        /// \brief -= operator
        inline vec3_t<T>& operator -= (const vec3_t<T>& rh)
        {
            //
            // TODO implement me!
            //
            return *this;
        }

        /// \brief *= operator (scalar)
        inline vec3_t<T>& operator *= (const T rh)
        {
            //
            // TODO implement me!
            //
            return *this;
        }

        /// \brief *= operator (component vise)
        inline vec3_t<T>& operator *= (const vec3_t<T>& rh)
        {
            //
            // TODO implement me!
            //
            return *this;
        }

        /// \brief /= operator (scalar)
        inline vec3_t<T>& operator /= (const T rh)
        {
            //
            // TODO implement me!
            //
            return *this;
        }

        /// \brief /= operator (component vise)
        inline vec3_t<T>& operator /= (const vec3_t<T>& rh)
        {
            //
            // TODO implement me!
            //
            return *this;
        }

        /// \brief unary - operator
        inline const vec3_t<T> operator - () const
        {
            //
            // TODO implement me!
            //
            return vec3_t<T>();
        }

        /// \brief Sets all components to the specified value
        inline void set(T x, T y, T z)
        {
            //
            // TODO implement me!
            //
        }

        /// \brief comparing two instances with an epsilon
        inline bool epsilonCompare (const vec3_t<T>& rh, const T e = GetEpsilon<T>::value()) const
        {
            //
            // TODO implement me!
            //
            return false;
        }

        /// \brief returns the length of the vector
        inline typename CalcSqrt<T>::result_t length() const
        {
            //
            // TODO implement me!
            //
            return CalcSqrt<T>::result_t();
        }

        /// \brief returns the squared length of the vector
        inline T squaredLength() const
        {
            //
            // TODO implement me!
            //
            return T();
        }

        /// \brief computes the dot product of this and another 3 component vector
        inline T dot(const vec3_t<T>& rh) const
        {
            //
            // TODO implement me!
            //
            return T();
        }

        /// \brief computes the cross product of this and another 3 component vector
        inline const vec3_t<T> cross(const vec3_t<T>& rh) const
        {
            vec3_t<T> res;
            //
            // TODO implement me!
            //
            return res;
        }

        /// \brief returns a normalized version of this vector
        inline const vec3_t<T> normalized() const
        {
            //
            // TODO implement me!
            //
            return vec3_t<T>();
        }


    private:
        const vec3_t<T> negated(const vec3_t<T>& rh) const { return -*this; }
        const vec3_t<T> addFromScript(const vec3_t<T>& rh) const { return *this + rh; }
        const vec3_t<T> subFromScript(const vec3_t<T>& rh) const { return *this - rh; }
        const vec3_t<T> mulFromScript(const vec3_t<T>& rh) const { return *this * rh; }
        const vec3_t<T> mulScalarFromScript(T rh) const { return *this * rh; }
        const vec3_t<T> divFromScript(const vec3_t<T>& rh) const { return *this / rh; }
        const vec3_t<T> divScalarFromScript(T rh) const { return *this / rh; }
    };

    typedef vec3_t<float> vec3;
    typedef vec3_t<int> ivec3;
};
