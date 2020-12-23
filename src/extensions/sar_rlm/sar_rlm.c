#include "rtklib.h"
/* Galileo SAR RLM only sent on E1-B odd pages
   successful decode requires 4 or 8 messages
   to be received in order */

void reset_rlm_state(raw_t *raw, int sat) {
  // reset message receive state
  raw->sar_rlm.part_count[sat - 1] = 0;
  raw->sar_rlm.msg_parts[sat - 1] = 0;
}

void decode_gal_inav_rlm(raw_t *raw, int sat) {

  unsigned char *raw_buffer = raw->subfrm[sat - 1] + 16 + 9; // Odd page, offset 58b
  unsigned char *msg_buffer = raw->sar_rlm.msg_buffer[sat - 1]; // parsed message buffer

  uint8_t rlm_buff_offset = 2; // Not byte aligned

  // Decode message header
  uint8_t start_bit = getbitu(raw_buffer, rlm_buff_offset, 1);
  uint8_t msg_parts =
      (getbitu(raw_buffer, rlm_buff_offset + 1, 1) == 0) ? 4 : 8;

  // Initialise decode state if first message part (or spare/no message)
  if (start_bit == 1) {
    raw->sar_rlm.part_count[sat - 1] = 0;
    raw->sar_rlm.msg_parts[sat - 1] = msg_parts;
  }

  // Check for decode state consistency

  // If this is not the first message, part count must be >0
  if (start_bit == 0) {
    if (raw->sar_rlm.part_count[sat - 1] == 0) {
      reset_rlm_state(raw, sat);
      return;
    }
    // Number of message parts expected should not change mid-message
    if (msg_parts != raw->sar_rlm.msg_parts[sat - 1]) {
      reset_rlm_state(raw, sat);
      return;
    }

    // Number of parts received should not exceed number expected
    if (raw->sar_rlm.part_count[sat - 1] > msg_parts) {
      trace(3, "RLM(%d): Message too long (%d>%d)", sat,
            raw->sar_rlm.part_count[sat - 1], msg_parts);
      reset_rlm_state(raw, sat);
      return;
    }
  }

  // Copy 20 bit message part into message buffer
  setbitu(msg_buffer, raw->sar_rlm.part_count[sat - 1] * 20, 20, getbitu(raw_buffer, 0, 20));

  // increment counter
  raw->sar_rlm.part_count[sat - 1]++;

  // If this was the last message, read out
  if (raw->sar_rlm.part_count[sat - 1] == msg_parts) {
    // Read 60-bit beacon ID with 20 bits over 3 messages
    unsigned char beaconID[8];
    setbitu(beaconID, 0, 60, getbitu(msg_buffer, 0, 60));

    unsigned char *cp = beaconID;
    trace(2, "RLM(%d) - Message From Beacon: %x", sat, beaconID);

    reset_rlm_state(raw, sat);
    return;
  }
}