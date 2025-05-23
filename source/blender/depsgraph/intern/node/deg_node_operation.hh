/* SPDX-FileCopyrightText: 2013 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup depsgraph
 */

#pragma once

#include <functional>
#include <string>

#include "intern/node/deg_node.hh"

#include "intern/depsgraph_type.hh"

struct Depsgraph;

namespace blender::deg {

struct ComponentNode;

/* Evaluation Operation for atomic operation */
/* XXX: move this to another header that can be exposed? */
using DepsEvalOperationCb = std::function<void(::Depsgraph *)>;

/* Identifiers for common operations (as an enum). */
enum class OperationCode {
  /* Generic Operations. -------------------------------------------------- */

  /* Placeholder for operations which don't need special mention */
  OPERATION = 0,

  /* Generic parameters evaluation. --------------------------------------- */
  ID_PROPERTY,
  PARAMETERS_ENTRY,
  PARAMETERS_EVAL,
  PARAMETERS_EXIT,
  VISIBILITY,

  /* Hierarchy. ----------------------------------------------------------- */
  HIERARCHY,

  /* Animation, Drivers, etc. --------------------------------------------- */
  /* NLA + Action */
  ANIMATION_ENTRY,
  ANIMATION_EVAL,
  ANIMATION_EXIT,
  /* Driver */
  DRIVER,
  /* Writes to RNA properties to ensure implicitly-shared data is un-shared. */
  DRIVER_UNSHARE,

  /* Scene related. ------------------------------------------------------- */
  SCENE_EVAL,
  AUDIO_ENTRY,
  AUDIO_VOLUME,

  /* Object related. ------------------------------------------------------ */
  OBJECT_FROM_LAYER_ENTRY,
  OBJECT_BASE_FLAGS,
  OBJECT_FROM_LAYER_EXIT,
  DIMENSIONS,

  /* Transform. ----------------------------------------------------------- */
  /* Transform entry point. */
  TRANSFORM_INIT,
  /* Local transforms only */
  TRANSFORM_LOCAL,
  /* Parenting */
  TRANSFORM_PARENT,
  /* Constraints */
  TRANSFORM_CONSTRAINTS,
  /* Handle object-level updates, mainly proxies hacks and recalc flags. */
  TRANSFORM_EVAL,
  /* Initializes transformation for simulation.
   * For example, ensures point cache is properly reset before doing rigid
   * body simulation. */
  TRANSFORM_SIMULATION_INIT,
  /* Transform exit point */
  TRANSFORM_FINAL,

  /* Rigid body. ---------------------------------------------------------- */
  /* Perform Simulation */
  RIGIDBODY_REBUILD,
  RIGIDBODY_SIM,
  /* Copy results to object */
  RIGIDBODY_TRANSFORM_COPY,

  /* Geometry. ------------------------------------------------------------ */

  /* Initialize evaluation of the geometry. Is an entry operation of geometry
   * component. */
  GEOMETRY_EVAL_INIT,
  /* Modifier. */
  MODIFIER,
  /* Evaluate the whole geometry, including modifiers. */
  GEOMETRY_EVAL,
  /* Evaluation of geometry is completely done. */
  GEOMETRY_EVAL_DONE,
  /* Evaluation of a shape key.
   * NOTE: Currently only for object data data-blocks. */
  GEOMETRY_SHAPEKEY,

  /* Object data. --------------------------------------------------------- */
  LIGHT_PROBE_EVAL,
  SPEAKER_EVAL,
  SOUND_EVAL,
  ARMATURE_EVAL,

  /* Pose. ---------------------------------------------------------------- */
  /* Init pose, clear flags, etc. */
  POSE_INIT,
  /* Initialize IK solver related pose stuff. */
  POSE_INIT_IK,
  /* Pose is evaluated, and runtime data can be freed. */
  POSE_CLEANUP,
  /* Pose has been fully evaluated and ready to be used by others. */
  POSE_DONE,
  /* IK/Spline Solvers */
  POSE_IK_SOLVER,
  POSE_SPLINE_IK_SOLVER,

  /* Bone. ---------------------------------------------------------------- */
  /* Bone local transforms - entry point */
  BONE_LOCAL,
  /* Pose-space conversion (includes parent + rest-pose. */
  BONE_POSE_PARENT,
  /* Constraints */
  BONE_CONSTRAINTS,
  /* Bone transforms are ready
   *
   * - "READY"  This (internal, noop is used to signal that all pre-IK
   *            operations are done. Its role is to help mediate situations
   *            where cyclic relations may otherwise form (i.e. one bone in
   *            chain targeting another in same chain,
   *
   * - "DONE"   This noop is used to signal that the bone's final pose
   *            transform can be read by others. */
  /* TODO: deform mats could get calculated in the final_transform ops... */
  BONE_READY,
  BONE_DONE,
  /* B-Bone segment shape computation (after DONE) */
  BONE_SEGMENTS,

  /* Particle System. ----------------------------------------------------- */
  PARTICLE_SYSTEM_INIT,
  PARTICLE_SYSTEM_EVAL,
  PARTICLE_SYSTEM_DONE,

  /* Particle Settings. --------------------------------------------------- */
  PARTICLE_SETTINGS_INIT,
  PARTICLE_SETTINGS_EVAL,
  PARTICLE_SETTINGS_RESET,

  /* Point Cache. --------------------------------------------------------- */
  POINT_CACHE_RESET,

  /* File cache. ---------------------------------------------------------- */
  FILE_CACHE_UPDATE,

  /* Collections. --------------------------------------------------------- */
  VIEW_LAYER_EVAL,

  /* Copy on Write. ------------------------------------------------------- */
  COPY_ON_EVAL,

  /* Shading. ------------------------------------------------------------- */
  SHADING,
  SHADING_DONE,
  MATERIAL_UPDATE,
  LIGHT_UPDATE,
  WORLD_UPDATE,

  /* Light linking. ------------------------------------------------------- */
  LIGHT_LINKING_UPDATE,

  /* Node Tree. ----------------------------------------------------------- */
  NTREE_OUTPUT,
  NTREE_GEOMETRY_PREPROCESS,

  /* Batch caches. -------------------------------------------------------- */
  GEOMETRY_SELECT_UPDATE,

  /* Masks. --------------------------------------------------------------- */
  MASK_ANIMATION,
  MASK_EVAL,

  /* Movie clips. --------------------------------------------------------- */
  MOVIECLIP_EVAL,

  /* Images. -------------------------------------------------------------- */
  IMAGE_ANIMATION,

  /* Synchronization. ----------------------------------------------------- */
  SYNCHRONIZE_TO_ORIGINAL,

  /* Generic data-block --------------------------------------------------- */
  GENERIC_DATABLOCK_UPDATE,

  /* Sequencer. ----------------------------------------------------------- */

  SEQUENCES_EVAL,

  /* instancing system. --------------------------------------------------- */

  /* Operation on an instancer object. Relations from instanced objects go here. */
  INSTANCER,

  /* Operation on an object which is being instanced. */
  INSTANCE,
  INSTANCE_GEOMETRY,
};
const char *operationCodeAsString(OperationCode opcode);

/* Flags for Depsgraph Nodes.
 * NOTE: IS a bit shifts to allow usage as an accumulated. bitmask.
 */
enum OperationFlag {
  /* Node needs to be updated. */
  DEPSOP_FLAG_NEEDS_UPDATE = (1 << 0),

  /* Node was directly modified, causing need for update. */
  DEPSOP_FLAG_DIRECTLY_MODIFIED = (1 << 1),

  /* Node was updated due to user input. */
  DEPSOP_FLAG_USER_MODIFIED = (1 << 2),

  /* Node may not be removed, even when it has no evaluation callback and no outgoing relations.
   * This is for NO-OP nodes that are purely used to indicate a relation between components/IDs,
   * and not for connecting to an operation. */
  DEPSOP_FLAG_PINNED = (1 << 3),

  /* The operation directly or indirectly affects ID node visibility. */
  DEPSOP_FLAG_AFFECTS_VISIBILITY = (1 << 4),

  /* Evaluation of the node is temporarily disabled. */
  DEPSOP_FLAG_MUTE = (1 << 5),

  /* Set of flags which gets flushed along the relations. */
  DEPSOP_FLAG_FLUSH = (DEPSOP_FLAG_USER_MODIFIED),

  /* Set of flags which get cleared upon evaluation. */
  DEPSOP_FLAG_CLEAR_ON_EVAL = (DEPSOP_FLAG_DIRECTLY_MODIFIED | DEPSOP_FLAG_NEEDS_UPDATE |
                               DEPSOP_FLAG_USER_MODIFIED),
};

/* Atomic Operation - Base type for all operations */
struct OperationNode : public Node {
  OperationNode();

  std::string identifier() const override;
  /**
   * Full node identifier, including owner name.
   * used for logging and debug prints.
   */
  std::string full_identifier() const;

  void tag_update(Depsgraph *graph, eUpdateSource source) override;

  bool is_noop() const
  {
    return (bool)evaluate == false;
  }

  OperationNode *get_entry_operation() override
  {
    return this;
  }
  OperationNode *get_exit_operation() override
  {
    return this;
  }

  /* Set this operation as component's entry/exit operation. */
  void set_as_entry();
  void set_as_exit();

  /* Component that contains the operation. */
  ComponentNode *owner;

  /* Callback for operation. */
  DepsEvalOperationCb evaluate;

  /* How many inlinks are we still waiting on before we can be evaluated. */
  uint32_t num_links_pending;
  bool scheduled;

  /* Identifier for the operation being performed. */
  OperationCode opcode;
  int name_tag;

  /* (OperationFlag) extra settings affecting evaluation. */
  int flag;

  DEG_DEPSNODE_DECLARE;
};

void deg_register_operation_depsnodes();

}  // namespace blender::deg
