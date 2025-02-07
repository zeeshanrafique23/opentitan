// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0
//
// This module is the overall reset manager wrapper
// TODO: This module is only a draft implementation that covers most of the rstmgr
// functoinality but is incomplete

package rstmgr_pkg;

  // Power domain parameters
  parameter int PowerDomains = ${len(power_domains)};
  % for power_domain in power_domains:
  parameter int Domain${power_domain}Sel = ${loop.index};
  % endfor

  // Number of non-always-on domains
  parameter int OffDomains = PowerDomains-1;

  // positions of software controllable reset bits
% for rst in sw_rsts:
  parameter int ${rst.upper()} = ${loop.index};
% endfor

  // resets generated and broadcast
  typedef struct packed {
% for rst in output_rsts:
  % if rst.shadowed:
    logic [PowerDomains-1:0] rst_${rst.name}_shadowed_n;
  % endif
    logic [PowerDomains-1:0] rst_${rst.name}_n;
% endfor
  } rstmgr_out_t;

  // reset indication for alert handler
  typedef struct packed {
<% n_rst = 0 %>\
% for rst in output_rsts:
  % if rst.shadowed:
    lc_ctrl_pkg::lc_tx_t [PowerDomains-1:0] rst_${rst.name}_shadowed;<% n_rst += 1 %>
  % endif
    lc_ctrl_pkg::lc_tx_t [PowerDomains-1:0] rst_${rst.name};<% n_rst += 1 %>
% endfor
  } rstmgr_rst_en_t;

  parameter int NumOutputRst = ${n_rst} * PowerDomains;

  // cpu reset requests and status
  typedef struct packed {
    logic rst_cpu_n;
    logic ndmreset_req;
  } rstmgr_cpu_t;

  // exported resets
% for intf, eps in export_rsts.items():
  typedef struct packed {
  % for ep, rsts in eps.items():
    % for rst in rsts:
    logic [PowerDomains-1:0] rst_${intf}_${ep}_${rst['name']}_n;
    % endfor
  % endfor
  } rstmgr_${intf}_out_t;
% endfor

  // default value for rstmgr_ast_rsp_t (for dangling ports)
  parameter rstmgr_cpu_t RSTMGR_CPU_DEFAULT = '{
    rst_cpu_n: 1'b1,
    ndmreset_req: '0
  };

endpackage // rstmgr_pkg
