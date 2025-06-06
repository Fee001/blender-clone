/* SPDX-FileCopyrightText: 2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#include "gpu_shader_compositor_texture_utilities.glsl"

void main()
{
  int2 texel = int2(gl_GlobalInvocationID.xy);

  float4 first_color = texture_load(first_tx, texel);
  float4 second_color = texture_load(second_tx, texel);
  float mask_value = texture_load(mask_tx, texel).x;

  /* Choose the closer pixel as the foreground, that is, the masked pixel with the lower z value.
   * If Use Alpha is disabled, return the foreground, otherwise, mix between the foreground and
   * background using the alpha of the foreground. */
  float4 foreground_color = mix(second_color, first_color, mask_value);
  float4 background_color = mix(first_color, second_color, mask_value);
  float mix_factor = use_alpha ? foreground_color.a : 1.0f;
  float4 combined_color = mix(background_color, foreground_color, mix_factor);

  /* Use the more opaque alpha from the two images. */
  combined_color.a = use_alpha ? max(second_color.a, first_color.a) : combined_color.a;

  imageStore(combined_img, texel, combined_color);
}
