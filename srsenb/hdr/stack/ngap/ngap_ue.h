/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */
#ifndef SRSENB_NGAP_UE_H
#define SRSENB_NGAP_UE_H

#include "ngap.h"
#include "ngap_ue_proc.h"
#include "ngap_ue_utils.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/ngap.h"
namespace srsenb {

class ngap::ue : public ngap_interface_ngap_proc
{
public:
  explicit ue(ngap* ngap_ptr_, rrc_interface_ngap_nr* rrc_ptr_, srslog::basic_logger& logger_);
  virtual ~ue();
  // TS 38.413 - Section 9.2.5.1 - Initial UE Message
  bool send_initial_ue_message(asn1::ngap_nr::rrcestablishment_cause_e cause,
                               srsran::unique_byte_buffer_t            pdu,
                               bool                                    has_tmsi,
                               uint32_t                                s_tmsi = 0);
  // TS 38.413 - Section 9.2.5.3 - Uplink NAS Transport
  bool send_ul_nas_transport(srsran::unique_byte_buffer_t pdu);
  // TS 38.413 - Section 9.2.2.2 - Initial Context Setup Response
  bool send_initial_ctxt_setup_response();
  // TS 38.413 - Section 9.2.2.3 - Initial Context Setup Failure
  bool send_initial_ctxt_setup_failure(asn1::ngap_nr::cause_c cause);
  // TS 38.413 - Section 9.2.2.1 - Initial Context Setup Request
  bool handle_initial_ctxt_setup_request(const asn1::ngap_nr::init_context_setup_request_s& msg);
  // TS 38.413 - Section 9.2.2.5 - UE Context Release Command
  bool handle_ue_ctxt_release_cmd(const asn1::ngap_nr::ue_context_release_cmd_s& msg);

  bool was_uectxtrelease_requested() const { return release_requested; }
  void ue_ctxt_setup_complete();
  void notify_rrc_reconf_complete(const bool reconf_complete_outcome);

  srsran::proc_t<ngap_ue_initial_context_setup_proc> initial_context_setup_proc;
  srsran::proc_t<ngap_ue_ue_context_release_proc>    ue_context_release_proc;

  ngap_ue_ctxt_t ctxt      = {};
  uint16_t       stream_id = 1;

private:
  // args
  ngap*                  ngap_ptr;
  rrc_interface_ngap_nr* rrc_ptr;

  // state
  bool release_requested = false;

  // logger
  srslog::basic_logger& logger;
};

} // namespace srsenb
#endif