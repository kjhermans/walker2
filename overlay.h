#ifndef _WALKER2_OVERLAY_H_
#define _WALKER2_OVERLAY_H_

#include "text.h"
#include "array.h"

void overlay_setpixel
  (unsigned* buf, unsigned width, unsigned height, unsigned x, unsigned y);

void overlay
  (unsigned* buf, unsigned width, unsigned height);

void overlay_string
  (unsigned* buf, unsigned w, unsigned h, unsigned x, unsigned y, char* s);

MAKE_ARRAY_HEADER(text_object_t*, textobjlist_)

text_object_t* overlay_add_textobject
  (unsigned x, unsigned y, char* string);

#endif
