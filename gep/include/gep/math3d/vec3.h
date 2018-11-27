#pragma once

#include <xmmintrin.h>
#include "gep/math3d/constants.h"
#include "gep/math3d/vec2.h"
#include <algorithm>


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
			data[0] = 0;
			data[1] = 0;
			data[2] = 0;
        }

        /// \brief constructor for a unitialized instance
        inline vec3_t(DoNotInitialize) {}

        /// \brief constructor
        inline vec3_t(T x, T y, T z)
        {
			data[0] = x;
			data[1] = y;
			data[2] = z;
        }

        /// \brief constructor
        inline explicit vec3_t(T data[3])
        {
			this->data[0] = data[0];
			this->data[1] = data[1];
			this->data[2] = data[2];
        }

        /// \brief constructor
        inline explicit vec3_t(const T xyz)
        {
			data[0] = xyz;
			data[1] = xyz;
			data[2] = xyz;
        }

        /// \brief constructor
        inline vec3_t(vec2_t<T> xy, T z)
        {
			data[0] = xy.x;
			data[1] = xy.y;
			data[2] = z;
        }

        /// \brief + operator for adding another 3 component vector
        inline const vec3_t<T> operator + (const vec3_t<T>& rh) const
        {
            return vec3_t<T>(data[0] + rh.x, data[1] + rh.y, data[2] + rh.z);
        }

        /// \brief - operator for subtracting another 3 component vector
        inline const vec3_t<T> operator - (const vec3_t<T>& rh) const
        {
			return vec3_t<T>(data[0] - rh.x, data[1] - rh.y, data[2] - rh.z);
        }

        /// \brief * operator for multiplying with a scalar
        inline const vec3_t<T> operator * (const T rh) const
        {
			return vec3_t<T>(data[0] * rh, data[1] * rh, data[2] * rh);
        }

        /// \brief * operator for multipying with another vector (component vise)
        inline const vec3_t<T> operator * (const vec3_t<T>& rh) const
        {
			return vec3_t<T>(data[0] * rh.x, data[1] * rh.y, data[2] * rh.z);
        }

        /// \brief / operator for dividing by a scalar
        inline const vec3_t<T> operator / (const T rh) const
        {
			return vec3_t<T>(data[0] / rh, data[1] / rh, data[2] / rh);
        }

        /// \brief / operator for dividing witha nother vector (component vise)
        inline const vec3_t<T> operator / (const vec3_t<T>& rh) const
        {
			return vec3_t<T>(data[0] / rh.x, data[1] / rh.y, data[2] / rh.z);
        }

        /// \brief += operator
        inline vec3_t<T>& operator += (const vec3_t<T>& rh)
        {
			data[0] = data[0] + rh.x;
			data[1] = data[1] + rh.y;
			data[2] = data[2] + rh.z;
            return *this;
        }

        /// \brief -= operator
        inline vec3_t<T>& operator -= (const vec3_t<T>& rh)
        {
			data[0] = data[0] - rh.x;
			data[1] = data[1] - rh.y;
			data[2] = data[2] - rh.z;
			return *this;
        }

        /// \brief *= operator (scalar)
        inline vec3_t<T>& operator *= (const T rh)
        {
			data[0] = data[0] * rh;
			data[1] = data[1] * rh;
			data[2] = data[2] * rh;
			return *this;
        }

        /// \brief *= operator (component vise)
        inline vec3_t<T>& operator *= (const vec3_t<T>& rh)
        {
			data[0] = data[0] * rh.x;
			data[1] = data[1] * rh.y;
			data[2] = data[2] * rh.z;
			return *this;
        }

        /// \brief /= operator (scalar)
        inline vec3_t<T>& operator /= (const T rh)
        {
			data[0] = data[0] / rh;
			data[1] = data[1] / rh;
			data[2] = data[2] / rh;
			return *this;
        }

        /// \brief /= operator (component vise)
        inline vec3_t<T>& operator /= (const vec3_t<T>& rh)
        {
			data[0] = data[0] / rh.x;
			data[1] = data[1] / rh.y;
			data[2] = data[2] / rh.z;
			return *this;
        }

        /// \brief unary - operator
        inline const vec3_t<T> operator - () const
        {
			return vec3_t<T>(-data[0], -data[1], -data[2]);
        }

        /// \brief Sets all components to the specified value
        inline void set(T x, T y, T z)
        {
			data[0] = x;
			data[1] = y;
			data[2] = z;
        }

        /// \brief comparing two instances with an epsilon
        inline bool epsilonCompare (const vec3_t<T>& rh, const T e = GetEpsilon<T>::value()) const
        {
			vec3_t<T> temp = vec3_t<T>(abs(data[0] - rh.x), abs(data[1] - rh.y), abs(data[2] - rh.z));
			if(temp.x < e || temp.y < e || temp.z < e)
				return true;
			return false;
        }

        /// \brief returns the length of the vector
        inline typename CalcSqrt<T>::result_t length() const
        {
            return sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
        }

        /// \brief returns the squared length of the vector
        inline T squaredLength() const
        {
            return data[0] * data[0] + data[1] * data[1] + data[2] * data[2];
        }

        /// \brief computes the dot product of this and another 3 component vector
        inline T dot(const vec3_t<T>& rh) const
        {
			return data[0] * rh.x + data[1] * rh.y + data[2] * rh.z;
        }

        /// \brief computes the cross product of this and another 3 component vector
        inline const vec3_t<T> cross(const vec3_t<T>& rh) const
        {
			T temp0 = data[1] * rh.z - data[2] * rh.y;
			T temp1 = data[2] * rh.x - data[0] * rh.z;
			T temp2 = data[0] * rh.y - data[1] * rh.x;
            return vec3_t<T>(temp0, temp1, temp2);
        }

        /// \brief returns a normalized version of this vector
        inline const vec3_t<T> normalized() const
        {
			T length = this->squaredLength();
			length = Q_rsqrt(length);
            return *this * length;
			
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
