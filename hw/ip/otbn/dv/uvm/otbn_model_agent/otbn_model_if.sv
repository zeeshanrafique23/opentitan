// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

interface otbn_model_if #(
  // Size of the instruction memory, in bytes
  parameter int ImemSizeByte = 4096,

  localparam int ImemAddrWidth = prim_util_pkg::vbits(ImemSizeByte)
) (
  input logic clk_i,
  input logic rst_ni
);

  // Inputs to DUT
  logic                     start;        // Start the operation

  // Outputs from DUT
  bit                       done;         // Operation done
  bit                       err;          // Something went wrong
  bit [31:0]                stop_pc;      // PC at end of operation
  chandle                   handle;       // Handle for DPI calls to C model

  // Wait until done goes high. Stops early on reset
  task automatic wait_done();
    while (rst_ni === 1'b1 && !done) begin
      @(posedge clk_i or negedge rst_ni);
    end
  endtask

  // Wait until done goes low. Stops early on reset
  task automatic wait_not_done();
    while (rst_ni === 1'b1 && done) begin
      @(posedge clk_i or negedge rst_ni);
    end
  endtask

  // Start model by setting start for a cycle. Waits until not in reset.
  task automatic start_model();
    wait(rst_ni);
    start = 1'b1;
    @(posedge clk_i or negedge rst_ni);
    start = 1'b0;
  endtask

  // The err signal is asserted by the model if it fails to find the DUT or if it finds a mismatch
  // in results. It should never go high.
  `ASSERT(NoModelErrs, !err)

endinterface
