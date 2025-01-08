#include <stddef.h>
#include <string.h>
#include <time.h>

#if defined(__cplusplus)
extern "C" {
#endif
const char *mg_unlist(size_t no);
const char *mg_unpack(const char *, size_t *, time_t *);
#if defined(__cplusplus)
}
#endif

static const unsigned char v1[] = {
  60,  33,  68,  79,  67,  84,  89,  80,  69,  32, 104, 116, // <!DOCTYPE ht
 109, 108,  62,  10,  60, 104, 116, 109, 108,  62,  10,  60, // ml>.<html>.<
  98, 111, 100, 121,  62,  10,  10,  60, 104,  49,  62,  83, // body>..<h1>S
 104, 111, 119,  32,  97,  32,  67, 111, 108, 111, 114,  32, // how a Color 
  80, 105,  99, 107, 101, 114,  60,  47, 104,  49,  62,  10, // Picker</h1>.
  10,  60, 102, 111, 114, 109,  32,  97,  99, 116, 105, 111, // .<form actio
 110,  61,  34,  47,  97, 112, 105,  47, 108, 101, 100,  34, // n="/api/led"
  62,  10,  32,  32,  60, 108,  97,  98, 101, 108,  32, 102, // >.  <label f
 111, 114,  61,  34, 102,  97, 118,  99, 111, 108, 111, 114, // or="favcolor
  34,  62,  83, 101, 108, 101,  99, 116,  32,  76,  69,  68, // ">Select LED
  32,  99, 111, 108, 111, 114,  58,  60,  47, 108,  97,  98, //  color:</lab
 101, 108,  62,  10,  32,  32,  60, 105, 110, 112, 117, 116, // el>.  <input
  32, 116, 121, 112, 101,  61,  34,  99, 111, 108, 111, 114, //  type="color
  34,  32, 105, 100,  61,  34,  99, 111, 108, 111, 114,  34, // " id="color"
  32, 110,  97, 109, 101,  61,  34,  99, 111, 108, 111, 114, //  name="color
  34,  32, 118,  97, 108, 117, 101,  61,  34,  35, 102, 102, // " value="#ff
 102, 102, 102, 102,  34,  62,  60,  98, 114,  62,  60,  98, // ffff"><br><b
 114,  62,  10,  32,  32,  60, 105, 110, 112, 117, 116,  32, // r>.  <input 
 116, 121, 112, 101,  61,  34, 115, 117,  98, 109, 105, 116, // type="submit
  34,  62,  10,  60,  47, 102, 111, 114, 109,  62,  10,  10, // ">.</form>..
  60,  47,  98, 111, 100, 121,  62,  10,  60,  47, 104, 116, // </body>.</ht
 109, 108,  62,  10, 0 // ml>.
};

static const struct packed_file {
  const char *name;
  const unsigned char *data;
  size_t size;
  time_t mtime;
} packed_files[] = {
  {"/web/index.html", v1, sizeof(v1), 1736339429},
  {NULL, NULL, 0, 0}
};

static int scmp(const char *a, const char *b) {
  while (*a && (*a == *b)) a++, b++;
  return *(const unsigned char *) a - *(const unsigned char *) b;
}
const char *mg_unlist(size_t no) {
  return packed_files[no].name;
}
const char *mg_unpack(const char *name, size_t *size, time_t *mtime) {
  const struct packed_file *p;
  for (p = packed_files; p->name != NULL; p++) {
    if (scmp(p->name, name) != 0) continue;
    if (size != NULL) *size = p->size - 1;
    if (mtime != NULL) *mtime = p->mtime;
    return (const char *) p->data;
  }
  return NULL;
}
