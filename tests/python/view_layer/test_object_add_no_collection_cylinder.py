# SPDX-FileCopyrightText: 2017-2022 Blender Authors
#
# SPDX-License-Identifier: GPL-2.0-or-later

# ############################################################
# Importing - Same For All Render Layer Tests
# ############################################################

import unittest

from view_layer_common import (
    ViewLayerTesting,
    setup_extra_arguments,
)


# ############################################################
# Testing
# ############################################################

class UnitTesting(ViewLayerTesting):
    def test_object_add_cylinder(self):
        """
        See if new objects are added to the correct collection
        bpy.ops.mesh.primitive_cylinder_add()
        """
        self.do_object_add_no_collection('CYLINDER')


# ############################################################
# Main - Same For All Render Layer Tests
# ############################################################

if __name__ == '__main__':
    UnitTesting._extra_arguments = setup_extra_arguments(__file__)
    unittest.main()
