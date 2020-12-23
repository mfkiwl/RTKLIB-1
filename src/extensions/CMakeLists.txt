add_subdirectory(glo_ifb)
add_subdirectory(tdiff_phases)

if(D_GALILEO_SAR_RLM)
  add_subdirectory(sar_rlm)
endif(D_GALILEO_SAR_RLM)
