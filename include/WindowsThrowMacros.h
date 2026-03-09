#pragma once

#define RHWND_EXCEPT( hr ) Window::HrException( __LINE__,__FILE__,(hr) )
#define RHWND_LAST_EXCEPT() Window::HrException( __LINE__,__FILE__,GetLastError() )
#define RHWND_NOGFX_EXCEPT() Window::NoGfxException( __LINE__,__FILE__ )
