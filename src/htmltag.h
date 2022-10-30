
#ifndef CHTML_HTMLTAG_H
#define CHTML_HTMLTAG_H

enum TagType {
  HTML,
  // HEAD,
  // TITLE,
  // BODY,
};

typedef struct Tag {
  TagType type;
} Tag;

#endif