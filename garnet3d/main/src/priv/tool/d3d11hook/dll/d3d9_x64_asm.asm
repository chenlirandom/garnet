.code
extern mProcs:QWORD
D3DPERF_BeginEvent_wrapper proc
	jmp mProcs[0*8]
D3DPERF_BeginEvent_wrapper endp
D3DPERF_EndEvent_wrapper proc
	jmp mProcs[1*8]
D3DPERF_EndEvent_wrapper endp
D3DPERF_GetStatus_wrapper proc
	jmp mProcs[2*8]
D3DPERF_GetStatus_wrapper endp
D3DPERF_QueryRepeatFrame_wrapper proc
	jmp mProcs[3*8]
D3DPERF_QueryRepeatFrame_wrapper endp
D3DPERF_SetMarker_wrapper proc
	jmp mProcs[4*8]
D3DPERF_SetMarker_wrapper endp
D3DPERF_SetOptions_wrapper proc
	jmp mProcs[5*8]
D3DPERF_SetOptions_wrapper endp
D3DPERF_SetRegion_wrapper proc
	jmp mProcs[6*8]
D3DPERF_SetRegion_wrapper endp
DebugSetLevel_wrapper proc
	jmp mProcs[7*8]
DebugSetLevel_wrapper endp
DebugSetMute_wrapper proc
	jmp mProcs[8*8]
DebugSetMute_wrapper endp
Direct3D9EnableMaximizedWindowedModeShim_wrapper proc
	jmp mProcs[9*8]
Direct3D9EnableMaximizedWindowedModeShim_wrapper endp
;;Direct3DCreate9_wrapper proc
;;	jmp mProcs[10*8]
;;Direct3DCreate9_wrapper endp
;;Direct3DCreate9Ex_wrapper proc
;;	jmp mProcs[11*8]
;;Direct3DCreate9Ex_wrapper endp
Direct3DShaderValidatorCreate9_wrapper proc
	jmp mProcs[12*8]
Direct3DShaderValidatorCreate9_wrapper endp
PSGPError_wrapper proc
	jmp mProcs[13*8]
PSGPError_wrapper endp
PSGPSampleTexture_wrapper proc
	jmp mProcs[14*8]
PSGPSampleTexture_wrapper endp
end
