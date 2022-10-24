//
// Created by lyc8503 on 2022/9/19.
//

#ifndef DNET_DEFS_H
#define DNET_DEFS_H

#include <cstdio>
#include <cstring>
#include <cassert>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define DNET_DEBUG(format, args...) printf("[DEBUG %s:%d] " format "\n", __FILENAME__, __LINE__, ##args)
#define DNET_ERROR(format, args...) fprintf(stderr, "[ERR %s:%d] " format "\n", __FILENAME__, __LINE__, ##args)

#define DNET_ASSERT assert

#endif //DNET_DEFS_H
