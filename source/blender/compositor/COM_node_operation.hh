/* SPDX-FileCopyrightText: 2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#include "BLI_string_ref.hh"

#include "DNA_node_types.h"

#include "NOD_derived_node_tree.hh"

#include "COM_context.hh"
#include "COM_operation.hh"
#include "COM_result.hh"
#include "COM_scheduler.hh"

namespace blender::compositor {

using namespace nodes::derived_node_tree_types;

/* ------------------------------------------------------------------------------------------------
 * Node Operation
 *
 * A node operation is a subclass of operation that nodes should implement and instantiate in the
 * get_compositor_operation function of bNodeType, passing the inputs given to that function to the
 * constructor. This class essentially just implements a default constructor that populates output
 * results for all outputs of the node as well as input descriptors for all inputs of the nodes
 * based on their socket declaration. The class also provides some utility methods for easier
 * implementation of nodes. */
class NodeOperation : public Operation {
 private:
  /* The node that this operation represents. */
  DNode node_;

 public:
  /* Populate the output results based on the node outputs and populate the input descriptors based
   * on the node inputs. */
  NodeOperation(Context &context, DNode node);

  /* Calls the evaluate method of the operation, but also measures the execution time and stores it
   * in the context's profile data. */
  void evaluate() override;

  /* Compute and set the initial reference counts of all the results of the operation. The
   * reference counts of the results are the number of operations that use those results, which is
   * computed as the number of inputs whose node is part of the schedule and is linked to the
   * output corresponding to each result. The node execution schedule is given as an input. */
  void compute_results_reference_counts(const Schedule &schedule);

 protected:
  /* Compute a node preview using the result returned from the get_preview_result method. */
  void compute_preview() override;

  /* Returns a reference to the derived node that this operation represents. */
  const DNode &node() const;

  /* Returns a reference to the node that this operation represents. */
  const bNode &bnode() const;

  /* Returns true if the output identified by the given identifier is needed and should be
   * computed, otherwise returns false. */
  bool should_compute_output(StringRef identifier);

 private:
  /* Get the result which will be previewed in the node, this is chosen as the first linked output
   * of the node, if no outputs exist, then the first allocated input will be chosen. Returns
   * nullptr if no result is viewable. */
  Result *get_preview_result();
};

}  // namespace blender::compositor
