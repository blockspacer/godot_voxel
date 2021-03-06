#ifndef VOXEL_STREAM_H
#define VOXEL_STREAM_H

#include "../voxel_buffer.h"
#include <core/resource.h>

// Provides access to a source of paged voxel data.
// Must be implemented in a multi-thread-safe way.
class VoxelStream : public Resource {
	GDCLASS(VoxelStream, Resource)
public:
	virtual void emerge_block(Ref<VoxelBuffer> out_buffer, Vector3i origin_in_voxels, int lod);
	virtual void immerge_block(Ref<VoxelBuffer> buffer, Vector3i origin_in_voxels, int lod);

protected:
	static void _bind_methods();

	void _emerge_block(Ref<VoxelBuffer> out_buffer, Vector3 origin_in_voxels, int lod);
	void _immerge_block(Ref<VoxelBuffer> buffer, Vector3 origin_in_voxels, int lod);
};

#endif // VOXEL_STREAM_H
