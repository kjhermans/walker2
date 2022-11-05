#include <string.h>
#include <stdio.h>

#include "text.h"
#include "overlay.h"

void text_object_init
  (text_object_t* o, unsigned x, unsigned y, char* t)
{
  memset(o, 0, sizeof(*o));
  o->visible = 1;
  o->x = x;
  o->y = y;
  snprintf(o->text, sizeof(o->text), "%s", t);
}

void text_object_set_text
  (text_object_t* o, char* t)
{
  snprintf(o->text, sizeof(o->text), "%s", t);
}

void text_object_set_position
  (text_object_t* o, unsigned x, unsigned y)
{
  o->x = x;
  o->y = y;
}

void text_object_set_visibility
  (text_object_t* o, int visible)
{
  o->visible = visible;
}

void text_object_render
  (text_object_t* o, unsigned* buf, unsigned w, unsigned h)
{
  if (o->visible) {
    overlay_string(buf, w, h, o->x, o->y, o->text);
  }
}
