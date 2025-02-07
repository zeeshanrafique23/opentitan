// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include <assert.h>
#include <stdbool.h>

#include "sw/device/lib/base/bitfield.h"
#include "sw/device/lib/base/memory.h"
#include "sw/device/lib/base/mmio.h"
#include "sw/device/lib/dif/dif_lc_ctrl.h"
#include "sw/device/lib/dif/dif_otp_ctrl.h"
#include "sw/device/lib/runtime/log.h"
#include "sw/device/lib/testing/check.h"
#include "sw/device/lib/testing/test_framework/test_main.h"

#include "hw/top_earlgrey/sw/autogen/top_earlgrey.h"

static dif_otp_ctrl_t otp;
static dif_lc_ctrl_t lc;

const test_config_t kTestConfig;
const uint8_t NUM_DEVICE_ID = 8;
const uint8_t OTP_DAI_TIMEOUT = 10;

/**
 * Busy-wait until the DAI is done with whatever operation it is doing.
 */
static void wait_for_dai(void) {
  for (int i = 0; i < OTP_DAI_TIMEOUT; i++) {
    dif_otp_ctrl_status_t status;
    CHECK(dif_otp_ctrl_get_status(&otp, &status) == kDifOtpCtrlOk);
    if (status.codes == (1 << kDifOtpCtrlStatusCodeDaiIdle)) {
      return;
    }
    LOG_INFO("Waiting for DAI...");
    usleep(10);
  }
  LOG_ERROR("OTP DAI access timeout.");
}

/**
 * Read and return 32-bit OTP data via the DAI interface.
 */
static void otp_ctrl_dai_read_32(const dif_otp_ctrl_t *otp,
                                 dif_otp_ctrl_partition_t partition,
                                 uint32_t address, uint32_t *buf) {
  dif_otp_ctrl_dai_result_t read_start_err =
      dif_otp_ctrl_dai_read_start(otp, partition, address);
  CHECK(read_start_err == kDifOtpCtrlDaiOk,
        "Failed to perform OTP DAI read start.");

  wait_for_dai();

  dif_otp_ctrl_dai_result_t read_end_err =
      dif_otp_ctrl_dai_read32_end(otp, buf);
  CHECK(read_end_err == kDifOtpCtrlDaiOk,
        "Failed to perform OTP DAI read end.");
}

/**
 * Tests that the OTP sends correct HW_CFG partition data to the receiving IPs.
 */
// TODO: needs to support other recipients besides LC_CTRL.
bool test_main(void) {
  mmio_region_t otp_reg_core =
      mmio_region_from_addr(TOP_EARLGREY_OTP_CTRL_CORE_BASE_ADDR);
  mmio_region_t otp_reg_prim =
      mmio_region_from_addr(TOP_EARLGREY_OTP_CTRL_PRIM_BASE_ADDR);
  CHECK(
      dif_otp_ctrl_init((dif_otp_ctrl_params_t){.base_addr_core = otp_reg_core,
                                                .base_addr_prim = otp_reg_prim},
                        &otp) == kDifOtpCtrlOk);

  mmio_region_t lc_reg = mmio_region_from_addr(TOP_EARLGREY_LC_CTRL_BASE_ADDR);
  CHECK_DIF_OK(dif_lc_ctrl_init(lc_reg, &lc));

  dif_otp_ctrl_config_t config = {
      .check_timeout = 100000,
      .integrity_period_mask = 0x3ffff,
      .consistency_period_mask = 0x3ffffff,
  };
  CHECK(dif_otp_ctrl_configure(&otp, config) == kDifOtpCtrlLockableOk);

  // Read out Device ID from LC_CTRL's `device_id` registers.
  dif_lc_ctrl_device_id_t lc_device_id;
  CHECK_DIF_OK(dif_lc_ctrl_get_device_id(&lc, &lc_device_id));

  // Read out Device ID from OTP DAI read, and compare the value with LC_CTRL's
  // `device_id` registers.
  // TODO: current HW CFG value is randomly genenrated from the HJSON file,
  // plan to backdoor inject.
  uint32_t otp_device_id;
  for (int i = 0; i < NUM_DEVICE_ID; i++) {
    otp_ctrl_dai_read_32(&otp, kDifOtpCtrlPartitionHwCfg, i * 4,
                         &otp_device_id);

    CHECK(otp_device_id == lc_device_id.data[i],
          "Device_ID_%0d mismatch bewtween otp_ctrl: %0h and lc_ctrl: %0h", i,
          otp_device_id, lc_device_id.data[i]);
  }
  return true;
}
