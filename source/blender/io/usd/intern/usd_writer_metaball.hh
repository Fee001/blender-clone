/* SPDX-FileCopyrightText: 2020 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once

#include "usd_writer_mesh.hh"

namespace blender::io::usd {

class USDMetaballWriter : public USDGenericMeshWriter {
 public:
  USDMetaballWriter(const USDExporterContext &ctx);

 protected:
  Mesh *get_export_mesh(Object *object_eval, bool &r_needsfree) override;
  void free_export_mesh(Mesh *mesh) override;
  bool is_supported(const HierarchyContext *context) const override;
  bool check_is_animated(const HierarchyContext &context) const override;

 private:
  bool is_basis_ball(Scene *scene, Object *ob) const;
};

}  // namespace blender::io::usd
