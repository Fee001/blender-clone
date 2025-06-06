/* SPDX-FileCopyrightText: 2022 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

/** \file
 * \ingroup draw
 *
 * A unique identifier for each object component.
 * It is used to access each component data such as matrices and object attributes.
 * It is valid only for the current draw, it is not persistent.
 *
 * The most significant bit is used to encode if the object needs to invert the front face winding
 * because of its object matrix handedness. This is handy because this means sorting inside
 * #DrawGroup command will put all inverted commands last.
 *
 * Default value of 0 points toward an non-cull-able object with unit bounding box centered at
 * the origin.
 */

#include "draw_shader_shared.hh"

struct Object;
struct DupliObject;
struct DEGObjectIterData;

namespace blender::draw {

struct ResourceHandle {
  /* Index for getting a specific resource in the resource arrays (e.g. object matrices).
   * Last bit contains handedness. */
  uint32_t raw;

  ResourceHandle() = default;
  ResourceHandle(uint raw_) : raw(raw_){};
  ResourceHandle(uint index, bool inverted_handedness)
  {
    raw = index;
    SET_FLAG_FROM_TEST(raw, inverted_handedness, 0x80000000u);
  }

  bool has_inverted_handedness() const
  {
    return (raw & 0x80000000u) != 0;
  }

  uint resource_index() const
  {
    return (raw & 0x7FFFFFFFu);
  }
};

/* Refers to a range of contiguous handles in the resource arrays.
 * Typically used to render instances of an object, but can represent a single instance too.
 * The associated objects will all share handedness and state and can be rendered together. */
struct ResourceHandleRange {
  /* First handle in the range. */
  ResourceHandle handle_first;
  /* Number of handle in the range. */
  uint32_t count;

  ResourceHandleRange() = default;
  ResourceHandleRange(ResourceHandle handle) : handle_first(handle), count(1) {}
  ResourceHandleRange(ResourceHandle handle, uint len) : handle_first(handle), count(len) {}

  IndexRange index_range() const
  {
    return {handle_first.raw, count};
  }

  /* TODO(fclem): Temporary workaround to keep existing code to work. Should be removed once we
   * complete the instance optimization project. */
  operator ResourceHandle() const
  {
    return handle_first;
  }
};

/* TODO(fclem): Move to somewhere more appropriated after cleaning up the header dependencies. */
struct ObjectRef {
  Object *object;
  /** Duplicated object that corresponds to the current object. */
  DupliObject *dupli_object;
  /** Object that created the dupli-list the current object is part of. */
  Object *dupli_parent;
  /** Unique handle per object ref. */
  ResourceHandleRange handle;

  ObjectRef() = default;
  ObjectRef(DEGObjectIterData &iter_data, Object *ob);
  ObjectRef(Object *ob);

  /* Is the object coming from a Dupli system. */
  bool is_dupli() const
  {
    return dupli_object != nullptr;
  }
};

};  // namespace blender::draw
