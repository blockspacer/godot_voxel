#ifndef HEADER_VOXEL_STREAM_IMAGE
#define HEADER_VOXEL_STREAM_IMAGE

#include "voxel_stream.h"
#include <core/image.h>
#include <modules/opensimplex/open_simplex_noise.h>

// Provides infinite tiling heightmap based on an image
class VoxelStreamImage : public VoxelStream {
	GDCLASS(VoxelStreamImage, VoxelStream)
public:
	VoxelStreamImage();

	enum SdfMode {
		SDF_VERTICAL = 0, // Lowest quality, fastest
		SDF_VERTICAL_AVERAGE,
		SDF_SEGMENT,
		SDF_MODE_COUNT
	};

	static const char *SDF_MODE_HINT_STRING;

	void set_image(Ref<Image> im);
	Ref<Image> get_image() const;

	void set_channel(VoxelBuffer::ChannelId channel);
	VoxelBuffer::ChannelId get_channel() const;

	void set_sdf_mode(SdfMode mode);
	SdfMode get_sdf_mode() const;

	void emerge_block(Ref<VoxelBuffer> p_out_buffer, Vector3i origin_in_voxels, int lod);

private:
	static void _bind_methods();

private:
	Ref<Image> _image;
	VoxelBuffer::ChannelId _channel = VoxelBuffer::CHANNEL_TYPE;
	SdfMode _sdf_mode = SDF_VERTICAL_AVERAGE;
};

VARIANT_ENUM_CAST(VoxelStreamImage::SdfMode)

#endif // HEADER_VOXEL_STREAM_IMAGE
