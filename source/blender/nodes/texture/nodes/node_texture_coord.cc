/* SPDX-FileCopyrightText: 2005 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup texnodes
 */

#include "BLI_math_vector.h"
#include "node_texture_util.hh"

static blender::bke::bNodeSocketTemplate outputs[] = {
    {SOCK_VECTOR, N_("Coordinates")},
    {-1, ""},
};

static void vectorfn(
    float *out, TexParams *p, bNode * /*node*/, bNodeStack ** /*in*/, short /*thread*/)
{
  copy_v3_v3(out, p->co);
}

static void exec(void *data,
                 int /*thread*/,
                 bNode *node,
                 bNodeExecData *execdata,
                 bNodeStack **in,
                 bNodeStack **out)
{
  tex_output(node, execdata, in, out[0], &vectorfn, static_cast<TexCallData *>(data));
}

void register_node_type_tex_coord()
{
  static blender::bke::bNodeType ntype;

  tex_node_type_base(&ntype, "TextureNodeCoordinates", TEX_NODE_COORD);
  ntype.ui_name = "Coordinates";
  ntype.enum_name_legacy = "COORD";
  ntype.nclass = NODE_CLASS_INPUT;
  blender::bke::node_type_socket_templates(&ntype, nullptr, outputs);
  ntype.exec_fn = exec;

  blender::bke::node_register_type(ntype);
}
