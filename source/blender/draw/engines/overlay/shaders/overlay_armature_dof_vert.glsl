/* SPDX-FileCopyrightText: 2018-2022 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#include "infos/overlay_armature_info.hh"

VERTEX_SHADER_CREATE_INFO(overlay_armature_dof)

#include "draw_view_clipping_lib.glsl"
#include "draw_view_lib.glsl"

vec3 sphere_project(float ax, float az)
{
  float sine = 1.0 - ax * ax - az * az;
  float q3 = sqrt(max(0.0, sine));

  return vec3(-az * q3, 0.5 - sine, ax * q3) * 2.0;
}

void main()
{
  mat4 inst_obmat = data_buf[gl_InstanceID].object_to_world;
  mat4 model_mat = inst_obmat;
  model_mat[0][3] = model_mat[1][3] = model_mat[2][3] = 0.0;
  model_mat[3][3] = 1.0;

  vec2 amin = vec2(inst_obmat[0][3], inst_obmat[1][3]);
  vec2 amax = vec2(inst_obmat[2][3], inst_obmat[3][3]);

  vec3 final_pos = sphere_project(pos.x * abs((pos.x > 0.0) ? amax.x : amin.x),
                                  pos.y * abs((pos.y > 0.0) ? amax.y : amin.y));

  vec3 world_pos = (model_mat * vec4(final_pos, 1.0)).xyz;
  gl_Position = drw_point_world_to_homogenous(world_pos);
  finalColor = data_buf[gl_InstanceID].color_;

  edgeStart = edgePos = ((gl_Position.xy / gl_Position.w) * 0.5 + 0.5) * sizeViewport;

  view_clipping_distances(world_pos);
}
