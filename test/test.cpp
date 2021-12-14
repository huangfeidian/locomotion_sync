#include "locomotion.h"
#include <iostream>
#include <vector>
#include <array>
#include "point.h"
#include <random>
using namespace spiritsaway::locomotion_sync;
using namespace spiritsaway::geometry;


void test_line(float max_accelerate, float max_speed, std::vector<point> corridor,  const float radius)
{
	const float position_scale = 0.005 * max_speed;
	const float speed_scale = 0.01 * max_accelerate;
	vec3_sync_data pos_sync_data;
	vec3_sync_data speed_sync_data;

	std::uint32_t pos_sync_data_sz = 0;
	std::uint32_t sync_raw_sz = 0;
	std::uint32_t speed_sync_data_sz = 0;
	point pos = corridor[0];
	point pre_speed, new_speed;

	point sync_pos;
	point sync_speed;
	
	std::uint32_t dest_idx = 1;
	pos_sync_data.full(pos.data);
	pos_sync_data.replay(sync_pos.data, position_scale);
	pos_sync_data_sz += pos_sync_data.data_sz();
	speed_sync_data.full(pre_speed.data);
	speed_sync_data.replay(sync_speed.data, speed_scale);
	speed_sync_data_sz += speed_sync_data.data_sz();
	float dt = 0.1f;
	sync_raw_sz = sizeof(float) * 3;
	int i = 0;
	while (true)
	{
		i++;
		auto cur_dest = corridor[dest_idx];
		auto desire_speed = cur_dest - pos;
		desire_speed.normalize();
		desire_speed *= max_speed;
		auto speed_diff = desire_speed - pre_speed;
		if (speed_diff.length() > dt * max_accelerate)
		{
			speed_diff.normalize();
			new_speed += speed_diff * dt * max_accelerate;
		}
		else
		{
			new_speed = desire_speed;
		}
		point new_pos = pos + new_speed * dt;

		speed_sync_data.diff(new_speed.data, speed_scale);
		speed_sync_data.replay(sync_speed.data, speed_scale);
		speed_sync_data_sz += speed_sync_data.data_sz();
		pos_sync_data.diff(new_pos.data, position_scale);
		pos_sync_data_sz += pos_sync_data.data_sz();
		pos_sync_data.replay(sync_pos.data, position_scale);
		sync_raw_sz += sizeof(float) * 3;
		
		auto speed_dis = sync_speed.distance(new_speed);
		auto pos_dis = sync_pos.distance(new_pos);
		
		if (speed_dis > 2 * speed_scale || pos_dis > 2 * position_scale)
		{
			std::cout << "sync fail" << std::endl;
			exit(1);
		}
		pre_speed = new_speed;
		pos = new_pos;
		if (pos.distance(cur_dest) < radius)
		{
			dest_idx += 1;
			std::cout <<"iter "<< i<< " sync_raw_sz " << sync_raw_sz << " pos_sync_data_sz " << pos_sync_data_sz << " speed sync sz " << speed_sync_data_sz << std::endl;
			if (dest_idx == corridor.size())
			{
				break;
			}
			i = 0;
			sync_raw_sz = 0;
			pos_sync_data_sz = 0;
			speed_sync_data_sz = 0;
		}
	}
	

}

int main()
{
	std::vector<point> corridor;
	std::array<point, 8> cube;
	cube[0] = point(1, -1, -1);
	cube[1] = point(1, 1, -1);
	cube[2] = point(-1, 1, -1);
	cube[3] = point(-1, -1, -1);
	cube[4] = point(1, -1, 1);
	cube[5] = point(1, 1, 1);
	cube[6] = point(-1, 1, 1);
	cube[7] = point(-1, -1, 1);
	for (int i = 0; i < 8; i++)
	{
		cube[i] *= 100;
	}
	for (int j = 0; j < 13; j++)
	{
		corridor.push_back(cube[(j * 47) % 8]);
	}
	float max_speed = 4;
	float max_accelerate = 2;
	float radius = 2;
	test_line(max_accelerate, max_speed, corridor, radius);
}