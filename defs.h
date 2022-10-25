//
// Created by lyc8503 on 2022/9/19.
//

#ifndef DNET_DEFS_H
#define DNET_DEFS_H

#include <cstdio>
#include <cstring>
#include <cassert>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define DNET_DEBUG(format, args...) printf("[DEBUG %10s:%-4d] " format "\n", __FILENAME__, __LINE__, ##args)
#define DNET_ERROR(format, args...) fprintf(stderr, "[ERR %10s:%-4d] " format "\n", __FILENAME__, __LINE__, ##args)

//#define DNET_DEBUG(format, args...) void(0);
//#define DNET_ERROR(format, args...) void(0);

#define DNET_ASSERT assert

#endif //DNET_DEFS_H
