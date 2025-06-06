/* SPDX-FileCopyrightText: 2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#include "gpu_shader_compositor_texture_utilities.glsl"
#include "gpu_shader_smaa_lib.glsl"

void main()
{
  int2 texel = int2(gl_GlobalInvocationID.xy);

  /* Add 0.5 to evaluate the input sampler at the center of the pixel and divide by the image size
   * to get the coordinates into the sampler's expected [0, 1] range. */
  float2 coordinates = (float2(texel) + float2(0.5f)) / float2(texture_size(edges_tx));

  float4 offset[3];
  float2 pixel_coordinates;
  SMAABlendingWeightCalculationVS(coordinates, pixel_coordinates, offset);

  float4 weights = SMAABlendingWeightCalculationPS(
      coordinates, pixel_coordinates, offset, edges_tx, area_tx, search_tx, float4(0.0f));
  imageStore(weights_img, texel, weights);
}
