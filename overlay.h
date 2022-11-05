#ifndef _WALKER2_OVERLAY_H_
#define _WALKER2_OVERLAY_H_

void overlay_setpixel
  (unsigned* buf, unsigned width, unsigned height, unsigned x, unsigned y);

void overlay
  (unsigned* buf, unsigned width, unsigned height);

void overlay_string
  (unsigned* buf, unsigned w, unsigned h, unsigned x, unsigned y, char* s);

#endif
