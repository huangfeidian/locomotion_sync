#pragma once
#include <cstdint>
#include <array>

namespace spiritsaway::locomotion_sync
{
	enum class vec3_sync_flags : std::uint8_t
	{
		x_partial = 0,
		x_full,
		y_partial,
		y_full,
		z_partial,
		z_full,
		max
	};
	struct vec3_sync_data
	{
		std::uint8_t flags = 0;
		std::array<std::int8_t, 12> data;
		const std::uint8_t data_sz()const
		{
			std::uint8_t test_flag = 1;
			std::uint8_t result = 0;
			for (std::uint8_t i = std::uint8_t(vec3_sync_flags::x_partial); i < std::uint8_t(vec3_sync_flags::max); i++)
			{
				if (flags & test_flag)
				{
					if (i % 2 == 0)
					{
						result += 1;
					}
					else
					{
						result += 4;
					}
				}
			}
			return result;
		}
		void to_big_endian(std::int8_t* data) const
		{

		}
		void from_big_endian(std::int8_t* data) const
		{

		}
		void diff(const float* old_data, const float* new_data, float scale)
		{
			std::uint8_t data_fill_next = 0;

			for (int i = 0; i < 3; i++)
			{
				float old_v = old_data[i];
				float new_v = new_data[i];
				int diff_v = (new_v - old_v) / scale;
				if (diff_v > 254 || diff_v < -255)
				{
					flags |= (0x2 << (i * 2));
					std::int8_t* new_v_ptr = reinterpret_cast<std::int8_t*>(&new_v);
					std::copy(new_v_ptr, new_v_ptr + 4, data.begin() + data_fill_next);
					data_fill_next += 4;
				}
				else if (diff_v == 0)
				{
				}
				else
				{
					flags |= (0x2 << (i * 2));
					data[data_fill_next] = std::int8_t(diff_v);

				}
			}
		}
		void full(const float* new_data)
		{
			for (int i = 0; i < 3; i++)
			{
				float new_v = new_data[2 - i];

				flags <<= 2;
				flags |= 0x2;
				std::int8_t* new_v_ptr = reinterpret_cast<std::int8_t*>(&new_v);
				std::copy(new_v_ptr, new_v_ptr + 4, data.begin() + i * 2);
			}
		}
		void replay(float* dest, float scale) const
		{
			std::uint8_t next_data_idx = 0;
			for (int i = 0; i < 3; i++)
			{
				auto cur_flag = (flags >> (i * 2)) & 0x3;
				if (cur_flag == 0x1)
				{
					float cur_diff = data[next_data_idx] * scale;
					dest[i] += cur_diff;
					next_data_idx++;
				}
				else if (cur_flag == 0x2)
				{
					std::int8_t* new_v_ptr = reinterpret_cast<std::int8_t*>(dest + i);
					std::copy(data.data() + next_data_idx, data.data() + next_data_idx + 4, new_v_ptr);
					from_big_endian(new_v_ptr);
					next_data_idx += 4;
				}
			}
		}
		std::uint8_t encode(std::int8_t* buffer) const
		{
			buffer[0] = *reinterpret_cast<const std::int8_t*>(&flags);
			auto cur_data_sz = data_sz();
			std::copy(data.begin(), data.begin() + data_sz(), buffer + 1);
			return cur_data_sz + 1;
		}
		bool decode(const std::int8_t* buffer, std::uint32_t remain_sz)
		{
			if (remain_sz == 0)
			{
				return false;
			}
			flags = *reinterpret_cast<const std::uint8_t*>(buffer);
			auto cur_data_sz = data_sz();
			if (remain_sz < cur_data_sz + 1)
			{
				return false;
			}
			std::copy(buffer + 1, buffer + 1 + cur_data_sz, data.begin());
			return true;
		}
	};
}