#ifndef TOOLS_H
#define TOOLS_H

#define DOWN2(data, bound) (data & ~(bound - 1))
#define UP2(data, bound) ((data + bound - 1) & ~(bound - 1))

#endif // !TOOLS_H