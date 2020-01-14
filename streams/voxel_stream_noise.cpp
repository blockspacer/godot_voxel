#include "voxel_stream_noise.h"

namespace {

static bool is_ranges_intersect(int start1, int end1, int start2, int end2) {
	int max_start = std::max(start1, start2);
	int min_end = std::min(end1, end2);
	return (min_end >= max_start);
}

} // namespace


void VoxelStreamNoise::set_channel(VoxelBuffer::ChannelId channel) {
	ERR_FAIL_INDEX(channel, VoxelBuffer::MAX_CHANNELS);
	_channel = channel;
}

VoxelBuffer::ChannelId VoxelStreamNoise::get_channel() const {
	return _channel;
}

void VoxelStreamNoise::set_noise(Ref<OpenSimplexNoise> noise) {
	_noise = noise;
}

Ref<OpenSimplexNoise> VoxelStreamNoise::get_noise() const {
	return _noise;
}

void VoxelStreamNoise::set_height_start(real_t y) {
	_height_start = y;
}

real_t VoxelStreamNoise::get_height_start() const {
	return _height_start;
}

void VoxelStreamNoise::set_height_range(real_t hrange) {
	_height_range = hrange;
}

real_t VoxelStreamNoise::get_height_range() const {
	return _height_range;
}

void VoxelStreamNoise::emerge_block(Ref<VoxelBuffer> out_buffer, Vector3i origin_in_voxels, int lod) {

	ERR_FAIL_COND(out_buffer.is_null());
	ERR_FAIL_COND(_noise.is_null());

	OpenSimplexNoise &noise = **_noise;
	VoxelBuffer &buffer = **out_buffer;
	if (_channel == VoxelBuffer::CHANNEL_SDF)
	{
		buffer.clear_channel_f(_channel, 100.0);
	}
	else if (_channel == VoxelBuffer::CHANNEL_TYPE) {
		buffer.clear_channel(_channel, 0);
	}

	/// \note iso_scale affects `blockyness` of terrain
	const float iso_scale = noise.get_period() * 0.1;
	//print_line(String("iso_scale = {0}").format(varray(iso_scale)));

	float zethaX = origin_in_voxels.x + (buffer.get_size().x << lod);
	float zethaY = origin_in_voxels.y + (buffer.get_size().y << lod);
	float zethaZ = origin_in_voxels.z + (buffer.get_size().z << lod);

	/// \note can`t just use height_in_range
	const bool height_in_range = (buffer.get_size().y - 1) >= 0 && is_ranges_intersect(
		_height_start,
		_height_start + _height_range,
		origin_in_voxels.y, /// \note can not add (0 << lod) because it == 0
		origin_in_voxels.y + ((buffer.get_size().y - 1) << lod)
	);

	if (!height_in_range) {
		return;
	}

	for (int z = 0; z < buffer.get_size().z; ++z) {
		for (int x = 0; x < buffer.get_size().x; ++x) {
			for (int y = 0; y < buffer.get_size().y; ++y) {
				float lx = origin_in_voxels.x + (x << lod);
				float ly = origin_in_voxels.y + (y << lod);
				float lz = origin_in_voxels.z + (z << lod);

				/// \note can`t just use height_in_range
				const bool voxel_in_range = ly < _height_start + _height_range && ly > _height_start;
				if (!voxel_in_range) {
					continue;
				}

				const float n = noise.get_noise_3d(lx, ly, lz); // ranges from [-1.0 to 1.0]
				//print_line(String("at {0} n = {1}").format(varray(Vector3(lx, ly, lz), n)));

				/// \note voxel is air if its SDF value is >= 0
				const float sdf_value = n * iso_scale;

				if (_channel == VoxelBuffer::CHANNEL_SDF)
				{
					//print_line(String("at {0} sdf_value = {1}").format(varray(Vector3(lx, ly, lz), sdf_value)));
					buffer.set_voxel_f(sdf_value, x, y, z, _channel);
				}
				else if (_channel == VoxelBuffer::CHANNEL_TYPE) {
					buffer.set_voxel_f(1, x, y, z, _channel);
				}
			}
		}
	}
}

void VoxelStreamNoise::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_noise", "noise"), &VoxelStreamNoise::set_noise);
	ClassDB::bind_method(D_METHOD("get_noise"), &VoxelStreamNoise::get_noise);

	ClassDB::bind_method(D_METHOD("set_height_start", "hstart"), &VoxelStreamNoise::set_height_start);
	ClassDB::bind_method(D_METHOD("get_height_start"), &VoxelStreamNoise::get_height_start);

	ClassDB::bind_method(D_METHOD("set_height_range", "hrange"), &VoxelStreamNoise::set_height_range);
	ClassDB::bind_method(D_METHOD("get_height_range"), &VoxelStreamNoise::get_height_range);

	ClassDB::bind_method(D_METHOD("set_channel", "channel"), &VoxelStreamNoise::set_channel);
	ClassDB::bind_method(D_METHOD("get_channel"), &VoxelStreamNoise::get_channel);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "noise", PROPERTY_HINT_RESOURCE_TYPE, "OpenSimplexNoise"), "set_noise", "get_noise");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "height_start"), "set_height_start", "get_height_start");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "height_range"), "set_height_range", "get_height_range");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "channel", PROPERTY_HINT_ENUM, VoxelBuffer::CHANNEL_ID_HINT_STRING), "set_channel", "get_channel");
}
