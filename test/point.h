#pragma once

#include <array>
#include <vector>
#include <utility>


namespace spiritsaway::geometry
{
	template<typename T>
	struct basic_point
	{
		union
		{
			struct
			{
				T x;
				T y;
				T z;
			};
			T data[3];
		};
		
		basic_point() :x(0), y(0), z(0)
		{

		}
		basic_point(T in_x, T in_y, T in_z) :x(in_x), y(in_y), z(in_z)
		{

		}
		basic_point(const basic_point& other)
			: x(other.x)
			, y(other.y)
			, z(other.z)
		{

		}
		basic_point& operator=(const basic_point& other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
			return *this;
		}
		friend basic_point operator*(const basic_point& p, double s)
		{
			basic_point new_pos;
			new_pos.x = static_cast<T>(p.x*s);
			new_pos.y = static_cast<T>(p.y*s);
			new_pos.z = static_cast<T>(p.z * s);
			return new_pos;
		}
		friend basic_point operator/(const basic_point& p, double s)
		{
			basic_point new_pos;
			new_pos.x = p.x/s;
			new_pos.y = p.y/s;
			new_pos.z = p.z / s;
			return new_pos;
		}
		friend basic_point operator*(double s,const basic_point& p)
		{
			basic_point new_pos;
			new_pos.x = p.x*s;
			new_pos.y = p.y*s;
			new_pos.z = p.z * s;
			return new_pos;
		}
		
		friend basic_point operator+(const basic_point& first, const basic_point& second)
		{
			basic_point new_pos;
			new_pos.x = first.x + second.x;
			new_pos.y = first.y + second.y;
			new_pos.z = first.z + second.z;
			return new_pos;
		}
		friend basic_point operator-(const basic_point& first, const basic_point& second)
		{
			basic_point new_pos;
			new_pos.x = first.x - second.x;
			new_pos.y = first.y - second.y;
			new_pos.z = first.z - second.z;
			return new_pos;
		}
		bool operator==(const basic_point& second)
		{
			return x == second.x&&y == second.y && z == second.z;
		}
		basic_point& operator+=(const basic_point& second)
		{
			
			this->x = this->x + second.x;
			this->y = this->y + second.y;
			this->z = this->z + second.z;
			return *this;
		}
		basic_point& operator-=(const basic_point& second)
		{
			
			this->x = this->x - second.x;
			this->y = this->y - second.y;
			this->z = this->z - second.z;
			return *this;
		}
		basic_point& operator*=(const T& scale)
		{
			x *= scale;
			y *= scale;
			z *= scale;
			return *this;
		}
		basic_point& operator/=(const T& scale)
		{
			x /= scale;
			y /= scale;
			z /= scale;
			return *this;
		}
		static basic_point radius_point(T radius, float in_angle, basic_point center = basic_point())
		{
			basic_point result;
			result.x = static_cast<T>(radius * std::cosf(in_angle));
			result.z = static_cast<T>(radius * std::sinf(in_angle));
			result.y = 0;
			return result + center;
		}
		T operator*(const basic_point& other_point) const
		{
			return x * other_point.x + y * other_point.y + z * other_point.z;
		}

		T dot2d(const basic_point& other_point) const
		{
			return x * other_point.x  + z * other_point.z;
		}
		T& operator[](int index)
		{
			return data[index];
		}
		const T& operator[](int index) const
		{
			return data[index];
		}
		T distance(const basic_point& other) const
		{
			auto diff_x = x - other.x;
			auto diff_y = y - other.y;
			auto diff_z = z - other.z;
			return std::sqrt(diff_x * diff_x + diff_y * diff_y + diff_z * diff_z);
		}
		T distance_2d(const basic_point& other) const
		{
			auto diff_x = x - other.x;
			auto diff_z = z - other.z;
			return std::sqrt(diff_x * diff_x +  diff_z * diff_z);
		}
		bool in_range(const basic_point& other, T radius) const
		{
			auto diff_x = x - other.x;
			auto diff_y = y - other.y;
			auto diff_z = z - other.z;
			auto temp_dis = diff_x * diff_x + diff_y * diff_y + diff_z * diff_z;
			return temp_dis < (radius * radius);
		}
		bool in_range_2d(const basic_point& other, T radius) const
		{
			auto diff_x = x - other.x;
			auto diff_z = z - other.z;
			auto temp_dis = diff_x * diff_x  + diff_z * diff_z;
			return temp_dis < (radius* radius);
		}
		void normalize()
		{
			auto temp_length = std::sqrt(x * x + y * y + z * z);
			if (temp_length < 0.0001)
			{
				x = 1;
				y = 0;
				z = 0;
				return;
			}
			x /= temp_length;
			y /= temp_length;
			z /= temp_length;
		}
		T length() const
		{
			return std::sqrt(x * x + y * y + z * z);
		}
		T len_square() const
		{
			return x * x + y * y + z * z;
		}

		void rotate(float angle)
		{
			float c = std::cosf(angle);
			float s = std::sinf(angle);
			T new_x, new_z;
			new_x = x * c - z * s;
			new_z = x * s + z * c;
			x = new_x;
			z = new_z;
		}
		void rotate_90()
		{
			auto pre_x = x;
			auto pre_z = z;
			x = -1 * pre_z;
			z = pre_x;
		}
		static basic_point cross(const basic_point& a, const basic_point& b)
		{
			basic_point result;
			result.x = a.y * b.z - a.z * b.y;
			result.y = a.z * b.x - a.x * b.z;
			result.z = a.x * b.y - a.y * b.x;
			return result;
		}
		static T cross2d(const basic_point& a, const basic_point& b)
		{
			return a.z * b.x - a.x * b.z;
		}
		const T* get_data() const
		{
			return data;
		}
		void clear()
		{
			x = 0;
			y = 0;
			z = 0;
		}
		

	};
	using point = basic_point<float>;
	template<typename T1, typename T2>
	basic_point<T1> cast_point(const basic_point<T2>& in_point)
	{
		basic_point<T1> result;
		result.x = static_cast<T1>(in_point.x);
		result.y = static_cast<T1>(in_point.y);
		result.z = static_cast<T1>(in_point.z);
		return result;
	}
	
	
}
