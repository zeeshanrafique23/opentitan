# Copyright lowRISC contributors.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0
#
# ------------------- W A R N I N G: A U T O - G E N E R A T E D   C O D E !! -------------------#
# PLEASE DO NOT HAND-EDIT THIS FILE. IT HAS BEEN AUTO-GENERATED WITH THE FOLLOWING COMMAND:
# util/topgen.py -t hw/top_earlgrey/data/top_earlgrey.hjson
# -o hw/top_earlgrey

# IP Integration Tests
plic_all_irqs_test_lib = declare_dependency(
  link_with: static_library(
    'plic_all_irqs_test_lib',
    sources: [
      hw_ip_alert_handler_reg_h,
      'plic_all_irqs_test.c',
    ],
    dependencies: [
      sw_lib_dif_alert_handler,
      sw_lib_dif_gpio,
      sw_lib_dif_hmac,
      sw_lib_dif_i2c,
      sw_lib_dif_uart,
      sw_lib_dif_rv_plic,
      sw_lib_irq,
      sw_lib_mmio,
      sw_lib_runtime_log,
      sw_lib_testing_rv_plic_testutils,
      sw_lib_testing_test_status,
      top_earlgrey,
    ],
  ),
)
sw_tests += {
  'plic_all_irqs_test': {
    'library': plic_all_irqs_test_lib,
  }
}
