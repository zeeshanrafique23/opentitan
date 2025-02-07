// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

use crate::transport::Transport;
use anyhow::Result;
use erased_serde::Serialize;
pub use opentitantool_derive::*;
use std::any::Any;

/// The `CommandDispatch` trait should be implemented for all leaf structures
/// in the application's command hierarchy.  It can be automatically derived
/// on internal nodes in the heirarchy.  See the documentation for
/// [`opentitantool_derive`].
///
/// The `context` parameter can be used to carry information from prior levels
/// in the command hierarchy to the next level.  This is typically used to
/// implement parameters on interior nodes before the next layer of subcommands.
pub trait CommandDispatch {
    fn run(
        &self,
        context: &dyn Any,
        transport: &mut dyn Transport,
    ) -> Result<Option<Box<dyn Serialize>>>;
}
